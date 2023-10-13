#include "render_model.h"

namespace Render {

#define findAndLoadTexture2MeshFromList(texChannel, texName)          \
    {                                                                 \
        tmp = findContains(texs, texName);                            \
        if (tmp != -1) {                                              \
            int tmp1 = hasTexturePath(directory + texs[tmp]);         \
            if (tmp1 != -1) {                                         \
                newmat.texChannel = textures[tmp1];                   \
            } else {                                                  \
                Texture *newTex = new Texture(directory + texs[tmp]); \
                textures.append(newTex);                              \
                newmat.texChannel = textures[textures.size() - 1];    \
            }                                                         \
        }                                                             \
    }

Model::Model(QObject *parent) : QObject{parent} {
}

Model::Model(QString path, QObject *parent) : QObject{parent} {
    loadModel(path);
}

void Model::draw(QOpenGLShaderProgram *curShader,
                 QOpenGLFunctions_4_5_Core *f) {
    for (int i = 0; i < meshes.size(); ++i) {
        meshes[i]->draw(curShader, f);
    }
}

void Model::loadModel(QString path) {
    // 加载模型
    Assimp::Importer importer;
    // 加载时处理：三角化，uv反转，计算法线、（副）切线
    const aiScene *scene = importer.ReadFile(
        path.toUtf8(), (aiProcess_Triangulate | aiProcess_FlipUVs |
                        aiProcess_GenNormals | aiProcess_CalcTangentSpace));

    // 检查模型是否错误
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        qDebug() << "错误：ASSIMP库读取模型：" << importer.GetErrorString();
        return;
    }

    // 存储模型存储路径
    QStringList tmpLst = path.split('/');
    directory = "";
    for (int i = 0; i < tmpLst.size() - 1; ++i) {
        directory += tmpLst[i] + "/";
    }

    // 从根节点开始递归处理
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh *m = processMesh(mesh, scene);

        //        qDebug() << node->mName.C_Str();
        m->modelMatrix = nodeGlobalTranformation(node);

        //        qDebug() << "ans " << m->modelMatrix << "\n";

        meshes.push_back(m);
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

float cor(float x) {
    if (abs(x) <= 1e-6)
        return 0;
    else
        return x;
}

QMatrix4x4 Model::nodeGlobalTranformation(aiNode *node) {
    QQueue<aiNode *> nodeSt;
    // 将该节点到根节点的路径push入队列
    aiNode *tarNode = node;
    while (true) {
        nodeSt.push_back(tarNode);
        if (tarNode->mParent == nullptr)
            break;
        else
            tarNode = tarNode->mParent;
    }
    QMatrix4x4 ans;
    ans.setToIdentity();

    // 去掉叶子节点，叶子节点的transform有问题
    nodeSt.pop_back();
    // 开始依次变换
    while (!nodeSt.empty()) {
        aiMatrix4x4 *rowTrans = &(nodeSt.front()->mTransformation);
        // 需要倒第三行的原因：坐标系变换
        QMatrix4x4 thisTrans(
            cor(rowTrans->a1), cor(rowTrans->a2), cor(rowTrans->a3),
            cor(rowTrans->a4), cor(rowTrans->b1), cor(rowTrans->b2),
            cor(rowTrans->b3), cor(rowTrans->b4), cor(rowTrans->c1),
            cor(rowTrans->c2), cor(rowTrans->c3), cor(rowTrans->c4),
            cor(rowTrans->d1), cor(rowTrans->d2), cor(rowTrans->d3),
            cor(rowTrans->d4));

        //        qDebug() << "trans " << thisTrans;

        ans = thisTrans * ans;
        nodeSt.pop_front();
    }
    return ans;
}

Mesh *Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    Mesh *m = new Mesh(this);
    // 先处理顶点
    // 先检查该mesh的顶点类型
    // 因为在读取文件时为网格自动补全了法线，因此显然有法线通道
    bool hasNormal = true;
    // 因为在读取文件时为网格自动补全了切线，因此显然有切线通道
    bool hasTangent = true;
    // 是否有顶点颜色？
    bool hasColor = mesh->HasVertexColors(0);
    // 有几套纹理坐标？
    unsigned int texcoordNum = mesh->GetNumUVChannels();
    // 开始向mesh中添加顶点
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex *v = new Vertex(mesh->mVertices[i].x, mesh->mVertices[i].y,
                               mesh->mVertices[i].z, texcoordNum, hasColor,
                               hasNormal, hasTangent);
        if (hasNormal) {
            v->setNorm(mesh->mNormals[i].x, mesh->mNormals[i].y,
                       mesh->mNormals[i].z);
        }
        if (hasTangent) {
            v->setTan(mesh->mTangents[i].x, mesh->mTangents[i].y,
                      mesh->mTangents[i].z);
        }
        if (hasColor) {
            v->setCol(mesh->mColors[0][i].r, mesh->mColors[0][i].g,
                      mesh->mColors[0][i].b, mesh->mColors[0][i].a);
        }
        for (unsigned int t = 0; t < texcoordNum; ++t) {
            v->setTexcoord(t, mesh->mTextureCoords[t][i].x,
                           mesh->mTextureCoords[t][i].y);
        }
        m->addVertex(v);
    }
    // 开始向mesh中添加索引（三角形）
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        m->addTriangle(mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1],
                       mesh->mFaces[i].mIndices[2]);
    }
    // 添加纹理
    if (mesh->mMaterialIndex >= 0) {
        // 找到对应mat
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        Material newMat = loadMaterialTexturesByName(material);
        m->material = newMat;
    }

    return m;
}

QVector<Texture *> Model::loadMaterialTextures(aiMaterial *mat,
                                               aiTextureType type,
                                               QString typeName) {
    QVector<Texture *> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        QOpenGLTexture *texture = new QOpenGLTexture(QImage());
        //        texture.id = TextureFromFile(str.C_Str(), directory);
        //        texture.type = typeName;
        //        texture.path = str;
        //        textures.push_back(texture);
    }
    return textures;
}

Material Model::loadMaterialTexturesByName(aiMaterial *mat) {
    Material newmat;
    QStringList texs;
    int tmp;
    // 读取所有类型纹理
    texs += getMaterialTexturesPathByType(mat, aiTextureType_DIFFUSE);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_SPECULAR);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_AMBIENT);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_EMISSIVE);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_HEIGHT);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_NORMALS);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_SHININESS);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_OPACITY);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_DISPLACEMENT);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_LIGHTMAP);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_REFLECTION);
    texs += getMaterialTexturesPathByType(mat, aiTextureType_UNKNOWN);
    // 去重？
    //    texs = texs.toSet().toList();

    // 寻找是否有diffuseOrAlbedo
    tmp = findContains(texs, "type_DOA");
    // 如果在该mat所含的tex中有符合DOA的纹理
    if (tmp != -1) {
        // 检查是否已经载入该纹理
        int tmp1 = hasTexturePath(directory + texs[tmp]);
        // 已经载入了，那么只需要指定指针
        if (tmp1 != -1) {
            newmat.diffuseOrAlbedo = textures[tmp1];
        } else {
            // 没有载入该纹理，那么需要载入该纹理，然后指定
            Texture *newTex = new Texture(directory + texs[tmp]);
            // 设置过滤模式
            newTex->texture->setMinMagFilters(
                QOpenGLTexture::LinearMipMapLinear,
                QOpenGLTexture::LinearMipMapLinear);
            // 设置重复模式
            newTex->texture->setWrapMode(QOpenGLTexture::ClampToEdge);
            // 加入当前纹理
            textures.append(newTex);
            newmat.diffuseOrAlbedo = textures[textures.size() - 1];
        }
    }
    // 寻找是否有MRAS纹理
    findAndLoadTexture2MeshFromList(mras, "type_MRAS");
    // 寻找是否有高光度+高光度tint纹理
    findAndLoadTexture2MeshFromList(specularAndTint, "type_SAT");
    // 寻找是否有光泽度+光泽度tint+清漆度+清漆度tint纹理
    findAndLoadTexture2MeshFromList(sheenAndTintWithclearCoatAndGloss,
                                    "type_STCT");
    // 寻找是否有法线+ao纹理
    findAndLoadTexture2MeshFromList(normalAO, "type_NAO");
    return newmat;
}

QStringList Model::getMaterialTexturesPathByType(aiMaterial *mat,
                                                 aiTextureType type) {
    QStringList ans;
    for (int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        ans.append(QString(str.C_Str()).replace("\\", "/"));
    }
    return ans;
}

int Model::findContains(QStringList &lst, QString str) {
    for (int i = 0; i < lst.size(); ++i)
        if (lst[i].contains(str)) return i;
    return -1;
}

int Model::hasTexturePath(QString path) {
    for (int i = 0; i < textures.size(); ++i)
        if (textures[i]->path == path) return i;

    return -1;
}

void Model::synchronizeGLObjects(QOpenGLFunctions_4_5_Core *f) {
    for (int i = 0; i < meshes.size(); ++i) {
        meshes[i]->synchronizeGLObjects(f);
    }
}

} // namespace Render
