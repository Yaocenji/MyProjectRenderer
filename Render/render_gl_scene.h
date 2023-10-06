#ifndef RENDER_GL_SCENE_H
#define RENDER_GL_SCENE_H

#include <QObject>

namespace Render {

class render_gl_scene : public QObject {
    Q_OBJECT
public:
    explicit render_gl_scene(QObject *parent = nullptr);

signals:
};

} // namespace Render

#endif // RENDER_GL_SCENE_H
