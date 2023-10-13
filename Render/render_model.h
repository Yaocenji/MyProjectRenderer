#ifndef RENDER_MODEL_H
#define RENDER_MODEL_H

#include <assimp/ai_assert.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLShaderProgram>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <QVector>
#include <assimp/Importer.hpp>

#include "render_mesh.h"

namespace Render {

class Model : public QObject {
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);
    explicit Model(QString path, QObject *parent = nullptr);

public:
    void draw(QOpenGLShaderProgram *curShader,
              QOpenGLFunctions_4_5_Core *f = globalgl::thisContext);

public:
    // 加载模型
    void loadModel(QString path);
    // 处理节点
    void processNode(aiNode *node, const aiScene *scene);
    // 计算节点的全局变换
    QMatrix4x4 nodeGlobalTranformation(aiNode *node);
    // 处理网格
    Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
    // 加载纹理
    QVector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            QString typeName);
    /**
     * 改进方式：通过贴图文件名来确定贴图的作用，因为fbx纹理格式与大多数情况并不匹配，
     * 比如Disneybrdf
     */
    Material loadMaterialTexturesByName(aiMaterial *mat);
    /**
     * 通过纹理种类读取所有的对应种类纹理并且加入vector
     */
    QStringList getMaterialTexturesPathByType(aiMaterial *mat,
                                              aiTextureType type);
    /**
     * 遍历QStringList，寻找其中可能包含某段字符串的字符串的索引
     */
    int findContains(QStringList &lst, QString str);
    /**
     * 判断一个纹理（路径字符串）是否已经被载入
     */
    int hasTexturePath(QString path);

    // 网格初始化
    void synchronizeGLObjects(
        QOpenGLFunctions_4_5_Core *f = globalgl::thisContext);

public:
    // 模型路径（主要用于定位纹理）
    QString directory;
    // 网格
    QVector<Mesh *> meshes;
    // 纹理
    QVector<Texture *> textures;

signals:
};

} // namespace Render

#endif // RENDER_MODEL_H
