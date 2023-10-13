#include "render_roundarealight.h"

namespace Render {

RoundAreaLight::RoundAreaLight(QObject *parent, QColor col, float stgth,
                               QVector3D p, QVector3D n, float r)
    : AreaLight{parent, col, stgth, p, n}, radius(r) {
}

void RoundAreaLight::setRadius(float r) {
    radius = r;
}

} // namespace Render
