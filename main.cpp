#include <QApplication>
#include <QVulkanInstance>
#include <QVulkanWindow>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
