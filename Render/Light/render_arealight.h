#ifndef RENDER_AREALIGHT_H
#define RENDER_AREALIGHT_H

#include <QObject>
#include <QVector3D>

#include "render_light.h"

namespace Render {

/// 面光源
class AreaLight : public Light {
    Q_OBJECT
public:
    explicit AreaLight(QObject *parent = nullptr, QColor col = Qt::white,
                       float stgth = 1.0f, QVector3D p = QVector3D(0, 1, 0),
                       QVector3D n = QVector3D(1, 0, 0));

public:
    void setCenter(float x, float y, float z);
    void setCenter(QVector3D p);
    void setNormal(float x, float y, float z);
    void setNormal(QVector3D n);

    QVector3D normal();

public:
    // 一个点和一个向量确定一个平面
    /// 中心点
    QVector3D centerPoint;
    /// 法向量
    QVector3D normalVec;

signals:
};

} // namespace Render

#endif // RENDER_AREALIGHT_H
