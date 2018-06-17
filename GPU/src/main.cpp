#include <iostream>
#include <QApplication>
#include "pathtracer.h"
#include "./ui/ui.h"


int main(int argc, char **argv) {
    QApplication app(argc, argv);
    GUI gui;
    gui.show();
    return app.exec();
}