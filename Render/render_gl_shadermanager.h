#ifndef RENDER_GL_SHADERMANAGER_H
#define RENDER_GL_SHADERMANAGER_H

#include <QObject>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

namespace Render {

class ShaderManager : public QObject {
    Q_OBJECT
public:
    explicit ShaderManager(QObject *parent = nullptr);

public:
    /// 离屏渲染vertex shader
    QOpenGLShader *screenShaderVert;
    /// 离屏渲染fragment shader
    QOpenGLShader *screenShaderFrag;
    /// 离屏渲染shaderprogram
    QOpenGLShaderProgram *screenShaderProgram;

public:
    /// 顶点着色器
    QOpenGLShader *vert;

    /// 顶点颜色 片元shader
    QOpenGLShader *vert_color_frag;
    /// 显示顶点颜色 着色器程序
    QOpenGLShaderProgram *vert_color_prog;

    /// 显示基础色无光照 片元shader
    QOpenGLShader *diffuseUnlit_frag;
    /// 显示基础色无光照 shaderProgram
    QOpenGLShaderProgram *diffuseUnlit_proj;

    /// 基础色兰伯特 片元shader
    QOpenGLShader *diffuseLambert_frag;
    /// 基础色兰伯特 shaderProgram
    QOpenGLShaderProgram *diffuseLambert_proj;

    /// 迪士尼BRDF光照模型 fragmentShader
    QOpenGLShader *disneyBRDF_frag;
    QOpenGLShaderProgram *disneyBRDF_proj;

public:
    /// 点光源阴影贴图vert
    QOpenGLShader *pointShadowMapVert;
    /// 点光源阴影贴图geom
    QOpenGLShader *pointShadowMapGeom;
    /// 点光源阴影贴图frag
    QOpenGLShader *pointShadowMapFrag;
    /// 点光源阴影shader
    QOpenGLShaderProgram *pointShadowMapProgram;

    /// 点光源多pass阴影vert
    QOpenGLShader *pointShadowMapMPassVert;
    /// 点光源多pass阴影frag
    QOpenGLShader *pointShadowMapMPassFrag;
    /// 点光源多pass阴影
    QOpenGLShaderProgram *pointShadowMapMPassProgram;

signals:
};

} // namespace Render

#endif // RENDER_GL_SHADERMANAGER_H
