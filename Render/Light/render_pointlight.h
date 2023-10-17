#ifndef RENDER_POINTLIGHT_H
#define RENDER_POINTLIGHT_H

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>

#include "../render_cubemap_rendertexture.h"
#include "../render_gl_shadermanager.h"
#include "../render_rendertexture.h"
#include "render_light.h"

namespace Render {

class Model;

class PointLight : public Light {
    Q_OBJECT
public:
    explicit PointLight(QObject *parent = nullptr, QColor col = Qt::white,
                        float stgth = 1.0f, QVector3D p = QVector3D(0, 1, 0));

public:
    // 获取光空间view矩阵
    QMatrix4x4 *LightMatrix();
    // 获取光空间proj矩阵
    QMatrix4x4 *LightProjMatrix();

    // 计算点光源的最远距离
    float Distance();

public:
    void setPos(QVector3D p);

    /// 绑定光源
    void bind(QOpenGLShaderProgram *curShader, int index);

public:
    QVector3D position;

public:
    // 是否有阴影
    bool hasShadow;
    // 六个光空间 view矩阵
    QMatrix4x4 lightMatrix[6];
    // 光空间proj矩阵
    QMatrix4x4 lightProjMatrix;
    // 阴影贴图
    RenderTexture *shadowMap;
    // 渲染阴影贴图
    void RenderShadow(Model *targetModel, QOpenGLFunctions_4_5_Core *f,
                      ShaderManager *shaders);

signals:
};

} // namespace Render

#endif // RENDER_POINTLIGHT_H
