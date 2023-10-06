#ifndef RENDER_GL_MESH_H
#define RENDER_GL_MESH_H

#include <QObject>

namespace Render {

class render_gl_mesh : public QObject {
    Q_OBJECT
public:
    explicit render_gl_mesh(QObject *parent = nullptr);

signals:
};

} // namespace Render

#endif // RENDER_GL_MESH_H
