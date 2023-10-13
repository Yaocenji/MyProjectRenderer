#ifndef RENDER_ROUNDAREALIGHT_H
#define RENDER_ROUNDAREALIGHT_H

#include <QObject>

#include "render_arealight.h"

namespace Render {

class RoundAreaLight : public AreaLight {
    Q_OBJECT
public:
    explicit RoundAreaLight(QObject *parent = nullptr, QColor col = Qt::white,
                            float stgth = 1.0f,
                            QVector3D p = QVector3D(0, 1, 0),
                            QVector3D n = QVector3D(1, 0, 0), float r = 1.0);

public:
    void setRadius(float r);

public:
    /// 半径
    float radius;

signals:
};

} // namespace Render

#endif // RENDER_ROUNDAREALIGHT_H
