#ifndef RENDER_POINTLIGHT_H
#define RENDER_POINTLIGHT_H

#include <QObject>
#include <QOpenGLShaderProgram>>

#include "render_light.h"

namespace Render {

class PointLight : public Light {
    Q_OBJECT
public:
    explicit PointLight(QObject *parent = nullptr, QColor col = Qt::white,
                        float stgth = 1.0f, QVector3D p = QVector3D(0, 1, 0));

public:
    void setPos(QVector3D p);

    /// 绑定主光源
    void bind(QOpenGLShaderProgram *curShader, int index);

public:
    QVector3D position;

signals:
};

} // namespace Render

#endif // RENDER_POINTLIGHT_H
