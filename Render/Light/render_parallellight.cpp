#include "render_parallellight.h"

namespace Render {

ParallelLight::ParallelLight(QObject *parent, QVector3D dir, QColor col,
                             float stgth)
    : Light{parent, col, stgth}, direction(dir) {
}

void ParallelLight::setDirection(QVector3D dir) {
    if (dir.length() == 0)
        return;
    else
        direction = dir.normalized();
}

QVector3D ParallelLight::getDirection() {
    return direction.normalized();
}

void ParallelLight::bind(QOpenGLShaderProgram *curShader) {
    curShader->setUniformValue("mLight.Direction", direction);
    curShader->setUniformValue(
        "mLight.ColorAndStrength",
        QVector4D(color.redF(), color.greenF(), color.blueF(), strength));
    curShader->setUniformValue("useMainLight", true);
}

} // namespace Render
