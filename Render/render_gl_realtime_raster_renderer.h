#ifndef RENDER_GL_REALTIME_RASTER_RENDERER_H
#define RENDER_GL_REALTIME_RASTER_RENDERER_H

#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>

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

signals:
};

} // namespace Render

#endif // RENDER_GL_REALTIME_RASTER_RENDERER_H
