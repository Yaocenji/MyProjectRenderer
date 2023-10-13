#include "render_arealight.h"

namespace Render {

AreaLight::AreaLight(QObject *parent, QColor col, float stgth, QVector3D p,
                     QVector3D n)
    : Light{parent, col, stgth}, centerPoint(p), normalVec(n) {
}

void AreaLight::setCenter(float x, float y, float z) {
    centerPoint = QVector3D(x, y, z);
}
void AreaLight::setCenter(QVector3D p) {
    centerPoint = p;
}

void AreaLight::setNormal(float x, float y, float z) {
    normalVec = QVector3D(x, y, z).normalized();
}
void AreaLight::setNormal(QVector3D n) {
    normalVec = n.normalized();
}

QVector3D AreaLight::normal() {
    return normalVec.normalized();
}

} // namespace Render
