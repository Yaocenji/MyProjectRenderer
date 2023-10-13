#ifndef RENDER_GL_REALTIME_RASTER_RENDERER_H
#define RENDER_GL_REALTIME_RASTER_RENDERER_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTransform>

#include "Light/render_parallellight.h"
#include "Light/render_pointlight.h"
#include "render_camera.h"
#include "render_gl_shadermanager.h"
#include "render_mesh.h"
#include "render_model.h"
#include "render_renderimage2d.h"
#include "render_rendertexture.h"

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
    /// shaderManager
    ShaderManager *shaders;
    /// 离屏渲染RT
    RenderTexture *screenRT;
    /// 离屏渲染屏幕矩形网格
    Mesh *screenMesh;

public:
    /// 摄像机0
    Camera *camera0;
    /// 临时model矩阵
    QMatrix4x4 *model;

public:
    /// 主光源
    ParallelLight *mainLight;
    /// 点光源
    PointLight *pointLight0;
    PointLight *pointLight1;

public:
    /// CornellBox
    Model *cornellBox;

signals:
};

} // namespace Render

#endif // RENDER_GL_REALTIME_RASTER_RENDERER_H
