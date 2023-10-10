#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <Global/globalgl.h>

#include <QColor>
#include <QDebug>
#include <QImage>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

namespace Render {

/// 顶点索引
typedef unsigned int VertexIndex;

/// 顶点
class Vertex {
public:
    Vertex(unsigned int tcn = 1, bool hc = false, bool hn = true,
           bool ht = true)
        : hasNormal(hn), texcoordNumber(tcn), hasColor(hc), hasTangent(ht) {
        pos = new QVector3D();
        norm = new QVector3D();
        texcoord.resize(tcn);
        for (unsigned int i = 0; i < tcn; ++i) {
            texcoord[i] = new QVector2D();
        }
        col = new QColor();
        tan = new QVector3D();
    }
    Vertex(float x, float y, float z, unsigned int tcn = 1, bool hc = false,
           bool hn = true, bool ht = true)
        : hasNormal(hn), texcoordNumber(tcn), hasColor(hc), hasTangent(ht) {
        pos = new QVector3D(x, y, z);
        norm = new QVector3D();
        texcoord.resize(tcn);
        for (unsigned int i = 0; i < tcn; ++i) {
            texcoord[i] = new QVector2D();
        }
        col = new QColor();
        tan = new QVector3D();
    }
    ~Vertex() {
        delete pos;
        delete norm;
        delete col;
        for (unsigned int i = 0; i < texcoordNumber; ++i) {
            delete texcoord[i];
        }
        texcoord.resize(0);
    }

public:
    /*
     * 得到当前vbo格式数据长度
     * vbo格式：位置3+法线3+颜色4+切线3+纹理坐标2*n
     */
    unsigned int getSize() {
        return 3 + hasNormal * 3 + hasColor * 4 + hasTangent * 3 +
               texcoordNumber * 2;
    }
    /*
     * 按照vbo格式写出当前顶点的数据
     * vbo格式：位置3+法线3+切线3+颜色4+纹理坐标2*n
     */
    void getData(float *p) {
        int cnt = 0;
        // 位置信息
        *(p + 0) = pos->x();
        *(p + 1) = pos->y();
        *(p + 2) = pos->z();
        cnt = 3;
        // 法线信息
        if (hasNormal) {
            *(p + cnt + 0) = norm->x();
            *(p + cnt + 1) = norm->y();
            *(p + cnt + 2) = norm->z();
            cnt += 3;
        }
        // 切线信息
        if (hasTangent) {
            *(p + cnt + 0) = tan->x();
            *(p + cnt + 1) = tan->y();
            *(p + cnt + 2) = tan->z();
            cnt += 3;
        }
        // 顶点颜色信息
        if (hasColor) {
            // 位置信息
            *(p + cnt + 0) = col->redF();
            *(p + cnt + 1) = col->greenF();
            *(p + cnt + 2) = col->blueF();
            *(p + cnt + 3) = col->alphaF();
            cnt += 4;
        }
        // 纹理坐标信息
        for (unsigned int i = 0; i < texcoordNumber; ++i) {
            *(p + cnt + 0) = texcoord[i]->x();
            *(p + cnt + 1) = texcoord[i]->y();
            cnt += 2;
        }
    }
    /*
     * 设置顶点位置
     */
    void setPos(float x, float y, float z) {
        pos->setX(x);
        pos->setY(y);
        pos->setZ(z);
    }
    /*
     * 设置顶点法线
     */
    void setNorm(float x, float y, float z) {
        if (!hasNormal) {
            qDebug() << "错误：该顶点不应包含法线信息";
            return;
        }
        norm->setX(x);
        norm->setY(y);
        norm->setZ(z);
        norm->normalize();
    }
    /*
     * 设置顶点切线
     */
    void setTan(float x, float y, float z) {
        if (!hasTangent) {
            qDebug() << "错误：该顶点不应包含法线信息";
            return;
        }
        tan->setX(x);
        tan->setY(y);
        tan->setZ(z);
        tan->normalize();
    }
    /*
     * 设置顶点颜色
     */
    void setCol(float r, float g, float b, float a = 1.0) {
        if (!hasColor) {
            qDebug() << "错误：该顶点不应包含颜色信息";
            return;
        }
        col->setRedF(r);
        col->setGreenF(g);
        col->setBlueF(b);
        col->setAlphaF(a);
    }
    /*
     * 设置顶点纹理坐标
     */
    void setTexcoord(unsigned int texCoordIndex, float x, float y) {
        if (texCoordIndex >= texcoordNumber) {
            qDebug() << "错误：纹理坐标序号大于该顶点纹理坐标数";
            return;
        }
        texcoord[texCoordIndex]->setX(x);
        texcoord[texCoordIndex]->setY(y);
    }

public:
    /// 顶点是否有法线信息
    bool hasNormal;
    /// 顶点是否有切线信息
    bool hasTangent;
    /// 顶点有几套纹理坐标
    unsigned int texcoordNumber;
    /// 顶点是否有顶点颜色信息
    bool hasColor;

public:
    QVector3D *pos;
    QVector3D *norm;
    QVector<QVector2D *> texcoord;
    QColor *col;
    QVector3D *tan;
};

/// 三角形
class Triangle {
public:
    Triangle() = default;
    Triangle(VertexIndex i0, VertexIndex i1, VertexIndex i2)
        : index_0(i0), index_1(i1), index_2(i2) {}

public:
    VertexIndex index_0;
    VertexIndex index_1;
    VertexIndex index_2;
};

/// 纹理
class Texture {
public:
    Texture(QString p, QOpenGLTexture::MipMapGeneration genMipMaps =
                           QOpenGLTexture::GenerateMipMaps) {
        texture = new QOpenGLTexture(QImage(p), genMipMaps);
        path = p;
    }

public:
    QOpenGLTexture *texture;
    QString path;
};

/// 材质
class Material {
public:
    Material() {
        diffuseOrAlbedo = nullptr;
        mras = nullptr;
        specularAndTint = nullptr;
        sheenAndTintWithclearCoatAndTint = nullptr;
        normalAO = nullptr;
        Luminance = nullptr;
        Transparency = nullptr;
        tex0 = nullptr;
        tex1 = nullptr;
        tex2 = nullptr;
        tex3 = nullptr;

        // 材质默认值
        diffuseOrAlbedoVal = QVector4D(0.8, 0.8, 0.8, 1.0);
        mrasVal = QVector4D(0.2, 0.8, 0.0, 0.1);
        specularAndTintVal = QVector4D(0.0, 0.0, 0.0, 0.0);
        normalAOVal = QVector4D(0.0, 1.0, 0.0, 0.0);
        LuminanceVal = QVector4D(1.0, 1.0, 1.0, 0.0);
        TransparencyVal = QVector4D(1.0, 1.0, 1.0, 0.0);
        tex0Val = QVector4D(0.0, 0.0, 0.0, 0.0);
        tex1Val = QVector4D(0.0, 0.0, 0.0, 0.0);
        tex2Val = QVector4D(0.0, 0.0, 0.0, 0.0);
        tex3Val = QVector4D(0.0, 0.0, 0.0, 0.0);
    }

public:
    // 一个属性为纹理或定值

    /*
     * 包含Disney BRDF PBR纹理通道
     * 也包含一些其他的常用通道
     */
    /*
     * Disney BRDF 纹理
     */
    // 扩散/漫反射/基础色/次表面
    Texture *diffuseOrAlbedo;
    QVector4D diffuseOrAlbedoVal;

    //    // 金属度纹理
    //    Texture *metallic;
    //    // 粗糙度纹理
    //    Texture *roughtness;
    //    // 各向异性纹理
    //    Texture *anisotropic;
    //    // 次表面散射强度纹理
    //    Texture *subsurface;

    // 金属度m+粗糙度r+各向异性a+次表面散射s 四合一纹理
    Texture *mras;
    QVector4D mrasVal;

    // 高光强度与高光颜色强度二合一纹理：rgb:specular;w:specularTint
    Texture *specularAndTint;
    QVector4D specularAndTintVal;
    // 光泽度+光泽染色+清漆强度+清漆染色 四合一纹理
    Texture *sheenAndTintWithclearCoatAndTint;
    QVector4D sheenAndTintWithclearCoatAndTintVal;

    /*
     * TODO: Disney BSDF(2015) 扩展的参数纹理
     */

    /*
     * 其他常用纹理
     */
    //    // 环境光遮蔽（在使用了路径追踪/RTRT/实时ao下不会使用）
    //    Texture *ambientOcclution;
    //    // 法线纹理
    //    Texture *normal;
    // 法线+环境光遮蔽二合一（环境光遮蔽占据w通道）
    Texture *normalAO;
    QVector4D normalAOVal;

    // 自发光颜色+自发光强度 w通道为发光强度s的表达式 w=s/(s+1)
    Texture *Luminance;
    QVector4D LuminanceVal;

    // 透明度颜色+透明度
    Texture *Transparency;
    QVector4D TransparencyVal;

    /*
     * 自定义纹理
     */
    Texture *tex0;
    QVector4D tex0Val;

    Texture *tex1;
    QVector4D tex1Val;

    Texture *tex2;
    QVector4D tex2Val;

    Texture *tex3;
    QVector4D tex3Val;
};

class Mesh : public QObject {
    Q_OBJECT
public:
    explicit Mesh(QObject *parent = nullptr);

    /// 根据数组初始化顶点信息
    void initVertexataByPointerAndSize(float *pointer, unsigned int size);
    /// 添加一个顶点
    void addVertex(Vertex *v);
    /// 添加一个顶点
    void addVertex(float x, float y, float z);
    /// 添加一个三角形
    void addTriangle(unsigned int i0, unsigned int i1, unsigned int i2);

    /// 根据网格信息获得vbo数据长度
    unsigned int getVertexBufferSize();
    /// 根据网格信息写入vbo数据数组
    void getVertexBuffer(float *p);

    /// 根据网格信息获得ebo数据长度
    unsigned int getIndexBufferSize();
    /// 根据网格信息写入ebo数据数组
    void getIndexBuffer(unsigned int *p);

    /// 同步vbo、vao、ebo
    void synchronizeGLObjects(
        QOpenGLFunctions_4_5_Core *f = globalgl::thisContext);

    /// 绘制
    void draw(QOpenGLShaderProgram *curShader,
              QOpenGLFunctions_4_5_Core *f = globalgl::thisContext);

public:
    /*
     * 顶点信息，一个Vertex（QVector3D）对象表示一个顶点
     */
    QVector<Vertex *> verticesData;
    /*
     * 索引信息，一个QVector3D对象表示一个三角形
     */
    QVector<Triangle *> triangleData;
    /*
     * 材质信息
     */
    Material material;
    /*
     * VBO/VAO/EBO
     */
    QOpenGLBuffer *vbo;
    QOpenGLBuffer *ebo;
    QOpenGLVertexArrayObject *vao;

signals:
};

} // namespace Render

#endif // RENDER_MESH_H
