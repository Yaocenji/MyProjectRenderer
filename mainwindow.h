#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Render/render_gl_realtime_raster_renderer.h>

#include <QMainWindow>
#include <QOpenGLWidget>
#include <QVulkanWindow>
#include <assimp/Importer.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
