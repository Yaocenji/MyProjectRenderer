#include "render_pointlight.h"

namespace Render {

PointLight::PointLight(QObject *parent, QColor col, float stgth, QVector3D p)
    : Light{parent, col, stgth}, position(p) {
}

void PointLight::setPos(QVector3D p) {
    position = p;
}

void PointLight::bind(QOpenGLShaderProgram *curShader, int index) {
    QString posname = "pLight[" + QString::number(index) + "].Position";
    curShader->setUniformValue(posname.toStdString().c_str(), position);

    QString casname = "pLight[" + QString::number(index) + "].ColorAndStrength";
    curShader->setUniformValue(
        casname.toStdString().c_str(),
        QVector4D(color.redF(), color.greenF(), color.blueF(), strength));
}

} // namespace Render
