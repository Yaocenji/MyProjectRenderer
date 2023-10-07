#ifndef RENDER_MESH_H
#define RENDER_MESH_H

#include <Global/globalgl.h>

#include <QColor>
#include <QDebug>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector2D>
#include <QVector3D>

namespace Render {

/// 顶点索引
typedef unsigned int VertexIndex;

/// 顶点
class Vertex {
public:
    Vertex(unsigned int tcn = 1, bool hc = false, bool hn = true)
        : hasNormal(hn), texcoordNumber(tcn), hasColor(hc) {
        pos = new QVector3D();
        norm = new QVector3D();
        texcoord.resize(tcn);
        for (unsigned int i = 0; i < tcn; ++i) {
            texcoord[i] = new QVector2D();
        }
        col = new QColor();
    }
    Vertex(float x, float y, float z, unsigned int tcn = 1, bool hc = false,
           bool hn = true)
        : hasNormal(hn), texcoordNumber(tcn), hasColor(hc) {
        pos = new QVector3D(x, y, z);
        norm = new QVector3D();
        texcoord.resize(tcn);
        for (unsigned int i = 0; i < tcn; ++i) {
            texcoord[i] = new QVector2D();
        }
        col = new QColor();
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
     * vbo格式：位置3+法线3+颜色4+纹理坐标2*n
     */
    unsigned int getSize() {
        return 3 + hasNormal * 3 + hasColor * 4 + texcoordNumber * 2;
    }
    /*
     * 按照vbo格式写出当前顶点的数据
     * vbo格式：位置3+法线3+颜色4+纹理坐标2*n
     */
    void getData(float *p) {
        int cnt = 0;
        // 位置信息
        *(p + 0) = pos->x();
        *(p + 1) = pos->y();
        *(p + 2) = pos->z();
        cnt = 2;
        // 法线信息
        if (hasNormal) {
            *(p + cnt + 1) = norm->x();
            *(p + cnt + 2) = norm->y();
            *(p + cnt + 3) = norm->z();
            cnt += 3;
        }
        // 顶点颜色信息
        if (hasColor) {
            // 位置信息
            *(p + cnt + 0) = col->redF();
            *(p + cnt + 1) = col->greenF();
            *(p + cnt + 2) = col->blueF();
            *(p + cnt + 3) = col->alphaF();
            cnt = 4;
        }
        // 纹理坐标信息
        for (unsigned int i = 0; i < texcoordNumber; ++i) {
            *(p + cnt + 1) = texcoord[i]->x();
            *(p + cnt + 2) = texcoord[i]->y();
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
    /// 顶点有几套纹理坐标
    unsigned int texcoordNumber;
    /// 顶点是否有顶点颜色信息
    bool hasColor;

public:
    QVector3D *pos;
    QVector3D *norm;
    QVector<QVector2D *> texcoord;
    QColor *col;
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
    void draw(QOpenGLFunctions_4_5_Core *f = globalgl::thisContext);

public:
    /*
     * 顶点信息，一个Vertex（QVector3D）对象表示一个顶点
     */
    QVector<Vertex *> verticesData;
    /*
     * 顶点信息，一个QVector3D对象表示一个顶点
     */
    QVector<Triangle *> triangleData;
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
