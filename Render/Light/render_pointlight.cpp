#include "render_pointlight.h"

#include "../render_model.h"

namespace Render {

PointLight::PointLight(QObject *parent, QColor col, float stgth, QVector3D p)
    : Light{parent, col, stgth}, position(p) {
    for (int i = 0; i < 6; ++i) {
        lightMatrix[i].setToIdentity();
    }
    lightProjMatrix.setToIdentity();
    lightProjMatrix.perspective(90.0, 1.0, 1e-5, Distance());

    shadowMap = new RenderTexture(this, false, true);
}

QMatrix4x4 *PointLight::LightMatrix() {
    lightProjMatrix.setToIdentity();
    lightProjMatrix.perspective(90.0, 1.0, 1e-5, Distance());
    // X+
    lightMatrix[0].setToIdentity();
    lightMatrix[0].lookAt(position, position + QVector3D(1.0, 0.0, 0.0),
                          QVector3D(0.0, 1.0, 0.0));
    lightMatrix[0] = lightProjMatrix * lightMatrix[0];

    // X-
    lightMatrix[1].setToIdentity();
    lightMatrix[1].lookAt(position, position + QVector3D(-1.0, 0.0, 0.0),
                          QVector3D(0.0, 1.0, 0.0));
    lightMatrix[1] = lightProjMatrix * lightMatrix[1];

    // Y+
    lightMatrix[2].setToIdentity();
    lightMatrix[2].lookAt(position, position + QVector3D(0.0, 1.0, 0.0),
                          QVector3D(-1.0, 0.0, 0.0));
    lightMatrix[2] = lightProjMatrix * lightMatrix[2];

    // Y-
    lightMatrix[3].setToIdentity();
    lightMatrix[3].lookAt(position, position + QVector3D(0.0, -1.0, 0.0),
                          QVector3D(1.0, 0.0, 0.0));
    lightMatrix[3] = lightProjMatrix * lightMatrix[3];

    // Z+
    lightMatrix[4].setToIdentity();
    lightMatrix[4].lookAt(position, position + QVector3D(0.0, 0.0, 1.0),
                          QVector3D(0.0, 1.0, 0.0));
    lightMatrix[4] = lightProjMatrix * lightMatrix[4];

    // Z-
    lightMatrix[5].setToIdentity();
    lightMatrix[5].lookAt(position, position + QVector3D(0.0, 0.0, -1.0),
                          QVector3D(0.0, 1.0, 0.0));
    lightMatrix[5] = lightProjMatrix * lightMatrix[5];

    return lightMatrix;
}

QMatrix4x4 *PointLight::LightProjMatrix() {
    lightProjMatrix.setToIdentity();
    lightProjMatrix.perspective(90.0, 1.0, 1e-5, Distance());
    return &lightProjMatrix;
}

float PointLight::Distance() {
    return 5.0 * sqrtf(strength);
}

void PointLight::setPos(QVector3D p) {
    position = p;
}

void PointLight::bind(QOpenGLShaderProgram *curShader, int index) {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    QString posname = "pLight[" + QString::number(index) + "].Position";
    curShader->setUniformValue(posname.toStdString().c_str(), position);

    QString casname = "pLight[" + QString::number(index) + "].ColorAndStrength";
    curShader->setUniformValue(
        casname.toStdString().c_str(),
        QVector4D(color.redF(), color.greenF(), color.blueF(), strength));

    // 绑定阴影纹理到纹理单元
    QString shadowname = "pLight[" + QString::number(index) + "].ShadowMap";
    QString hasshadowname = "pLight[" + QString::number(index) + "].hasShadow";

    f->glActiveTexture(GL_TEXTURE12 + index);
    f->glBindTexture(GL_TEXTURE_2D, this->shadowMap->depthTexture());
    curShader->setUniformValue(shadowname.toStdString().c_str(), 12 + index);
    curShader->setUniformValue(hasshadowname.toStdString().c_str(), true);

    //    static int last_unit = -1;
    //    if (last_unit == -1) {
    //        int availableTextureUnit = 0;
    //        bool flag = false;
    //        int maxTextureUnit = 0;
    //        QOpenGLFunctions *f =
    //        QOpenGLContext::currentContext()->functions();
    //        f->glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnit);
    //        while (availableTextureUnit < maxTextureUnit) {
    //            // 绑定到当前需要查看的纹理单元
    //            f->glActiveTexture(GL_TEXTURE0 + availableTextureUnit);
    //            // 验证该纹理单元是否有纹理
    //            GLint whichID;
    //            f->glGetIntegerv(GL_TEXTURE_BINDING_2D, &whichID);
    //            // 如果没有纹理，那么绑定并传参
    //            if (whichID == 0) {
    //                f->glBindTexture(GL_TEXTURE_2D,
    //                                 this->shadowMap->depthTexture());
    //                curShader->setUniformValue(shadowname.toStdString().c_str(),
    //                                           availableTextureUnit);
    //                flag = true;
    //                break;
    //            }
    //            // 否则下一个
    //            else {
    //                availableTextureUnit++;
    //            }
    //        }
    //        if (flag) {
    //            curShader->setUniformValue(hasshadowname.toStdString().c_str(),
    //                                       true);
    //            qDebug() << "光源阴影贴图单元：" << availableTextureUnit;
    //        } else {
    //            qDebug() << "错误：纹理空间不够传光源阴影了";
    //        }
    //        last_unit = availableTextureUnit;
    //    } else {
    //        f->glActiveTexture(GL_TEXTURE0 + last_unit);
    //    }
}

void PointLight::RenderShadow(Model *targetModel, QOpenGLFunctions_4_5_Core *f,
                              ShaderManager *shaders) {
    // 绑定目标阴影贴图
    shadowMap->clear(*f);
    shadowMap->bind(*f);

    // 准备
    f->glEnable(GL_DEPTH_TEST);
    f->glReadBuffer(GL_NONE);
    f->glDrawBuffer(GL_NONE);
    f->glViewport(0, 0, 1024, 1024);
    // 绑定shader
    shaders->pointShadowMapMPassProgram->bind();
    // 传参
    shaders->pointShadowMapMPassProgram->setUniformValueArray(
        "shadowMatrices", this->LightMatrix(), 6);
    shaders->pointShadowMapMPassProgram->setUniformValue("lightPos",
                                                         this->position);
    shaders->pointShadowMapMPassProgram->setUniformValue("far",
                                                         this->Distance());
    // 绘制六个面
    for (int pass = 0; pass < 6; ++pass) {
        f->glViewport(pass * 1024, 0, 1024, 1024);
        shaders->pointShadowMapMPassProgram->setUniformValue("pass", pass);
        targetModel->draw(shaders->pointShadowMapMPassProgram, f);
    }

    qDebug() << "阴影贴图" << f->glGetError();
}

} // namespace Render
