#ifndef RENDER_GL_PIPELINE_H
#define RENDER_GL_PIPELINE_H

#include <QObject>

class render_gl_pipeline : public QObject
{
    Q_OBJECT
public:
    explicit render_gl_pipeline(QObject *parent = nullptr);

signals:

};

#endif // RENDER_GL_PIPELINE_H
