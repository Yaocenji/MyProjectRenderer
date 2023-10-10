#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    //    Assimp::Importer importer;
    resize(900, 900);
    Render::GL_Realtime_Raster_Renderer *renderer0 =
        new Render::GL_Realtime_Raster_Renderer(this);
    setCentralWidget(renderer0);
    renderer0->update();
}

MainWindow::~MainWindow() {
}
