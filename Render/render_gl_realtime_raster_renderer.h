#ifndef RENDER_GL_REALTIME_RASTER_RENDERER_H
#define RENDER_GL_REALTIME_RASTER_RENDERER_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

#include "render_camera.h"
#include "render_mesh.h"
#include "render_renderimage2d.h"
#include "render_rendertexture.h"
#include "render_scene.h"

namespace Render {

class GL_Realtime_Raster_Renderer : public QOpenGLWidget,
                                    QOpenGLFunctions_4_5_Core {
    Q_OBJECT
public:
    explicit GL_Realtime_Raster_Renderer(QWidget *parent = nullptr);

public:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

public:
    /// 离屏渲染RT
    RenderTexture *screenRT;
    /// 离屏渲染vertex shader
    QOpenGLShader *screenShaderVert;
    /// 离屏渲染fragment shader
    QOpenGLShader *screenShaderFrag;
    /// 离屏渲染shaderprogram
    QOpenGLShaderProgram *screenShaderProgram;
    /// 离屏渲染屏幕矩形网格
    Mesh *screenMesh;

signals:
};

} // namespace Render

#endif // RENDER_GL_REALTIME_RASTER_RENDERER_H
