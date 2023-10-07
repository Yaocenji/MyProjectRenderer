#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include <QObject>

namespace Render {

class Scene : public QObject {
    Q_OBJECT
public:
    explicit Scene(QObject *parent = nullptr);

signals:
};

} // namespace Render

#endif // RENDER_SCENE_H
