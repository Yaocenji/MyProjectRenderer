#include "render_mesh.h"

namespace Render {

Mesh::Mesh(QObject *parent) : QObject{parent} {
    verticesData.resize(0);
    triangleData.resize(0);

    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    vao = new QOpenGLVertexArrayObject(this);
}

/// 根据数组初始化顶点信息
void Mesh::initVertexataByPointerAndSize(float *pointer, unsigned int size) {
    // TODO
    //    if (size % 3 != 0) {
    //        qDebug() <<
    //        "错误：初始化网格顶点信息失败，传入数组大小并非3的倍数。"; return;
    //    }
    //    verticesData.resize(0);
    //    for (unsigned int i = 0; i < size / 3; ++i) {
    //        verticesData.push_back(Vertex(*(pointer + (i * 3)),
    //                                      *(pointer + (i * 3) + 1),
    //                                      *(pointer + (i * 3) + 2)));
    //    }
}

/// 添加一个顶点
void Mesh::addVertex(Vertex *v) {
    verticesData.push_back(v);
}
/// 添加一个顶点
void Mesh::addVertex(float x, float y, float z) {
    verticesData.push_back(new Vertex(x, y, z));
}
/// 添加一个三角形
void Mesh::addTriangle(unsigned int i0, unsigned int i1, unsigned int i2) {
    triangleData.push_back(new Triangle(i0, i1, i2));
}

/// 根据网格信息写入vbo数据数组
unsigned int Mesh::getVertexBufferSize() {
    if (verticesData.size() == 0)
        return 0;
    else
        return verticesData[0]->getSize() * verticesData.size();
}

/// 根据网格信息写入vbo数组
void Mesh::getVertexBuffer(float *p) {
    int cnt = 0;
    for (int i = 0; i < verticesData.size(); ++i) {
        verticesData[i]->getData(p + cnt);
        cnt += verticesData[i]->getSize();
    }
}

/// 根据网格信息获得ebo数据长度
unsigned int Mesh::getIndexBufferSize() {
    if (triangleData.size() == 0)
        return 0;
    else
        return triangleData.size() * 3;
}
/// 根据网格信息写入ebo数据数组
void Mesh::getIndexBuffer(unsigned int *p) {
    for (int i = 0; i < triangleData.size(); ++i) {
        *(p + i * 3 + 0) = triangleData[i]->index_0;
        *(p + i * 3 + 1) = triangleData[i]->index_1;
        *(p + i * 3 + 2) = triangleData[i]->index_2;
    }
}

/// 同步VBO/VAO/EBO
void Mesh::synchronizeGLObjects(QOpenGLFunctions_4_5_Core *f) {
    if (vbo->isCreated()) vbo->destroy();
    if (ebo->isCreated()) ebo->destroy();
    if (vao->isCreated()) vao->destroy();

    vbo->create();
    ebo->create();
    vao->create();

    if (getVertexBufferSize() == 0 || getIndexBufferSize() == 0) return;

    float *vertRawData = new float[getVertexBufferSize()];
    unsigned int *indRawData = new unsigned int[getIndexBufferSize()];

    getVertexBuffer(vertRawData);
    getIndexBuffer(indRawData);

    qDebug() << "synchronizeGLObjects:";
    qDebug() << "vertRawData";
    for (int i = 0; i < getVertexBufferSize(); ++i) {
        qDebug() << vertRawData[i] << " ";
    }
    qDebug() << "indRawData";
    for (int i = 0; i < getIndexBufferSize(); ++i) {
        qDebug() << indRawData[i] << " ";
    }

    vao->bind();

    vbo->bind();
    vbo->allocate(vertRawData, sizeof(float) * getVertexBufferSize());

    ebo->bind();
    ebo->allocate(indRawData, sizeof(unsigned int) * getIndexBufferSize());

    int cnt = 0;
    int stride_size = verticesData[0]->getSize();
    //  顶点位置信息
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                             sizeof(float) * stride_size, (void *)(cnt));
    f->glEnableVertexAttribArray(0);
    cnt += 3;
    // 法线信息
    if (verticesData[0]->hasNormal) {
        f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                 sizeof(float) * stride_size, (void *)(cnt));
        f->glEnableVertexAttribArray(1);
        cnt += 3;
    }
    // 顶点颜色信息
    if (verticesData[0]->hasColor) {
        f->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                                 sizeof(float) * stride_size, (void *)(cnt));
        f->glEnableVertexAttribArray(2);
        cnt += 4;
    }
    // 纹理坐标信息
    for (unsigned int i = 0; i < verticesData[0]->texcoordNumber; ++i) {
        f->glVertexAttribPointer(3 + i, 2, GL_FLOAT, GL_FALSE,
                                 sizeof(float) * stride_size, (void *)(cnt));
        f->glEnableVertexAttribArray(3 + i);
        cnt += 2;
    }

    vao->release();
    vbo->release();
    ebo->release();

    delete[] vertRawData;
    delete[] indRawData;
}

void Mesh::draw(QOpenGLFunctions_4_5_Core *f) {
    vao->bind();
    f->glDrawElements(GL_TRIANGLES, getIndexBufferSize(), GL_UNSIGNED_INT, 0);
}

} // namespace Render
