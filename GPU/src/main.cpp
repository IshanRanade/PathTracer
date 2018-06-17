#include <iostream>
#include <QApplication>
#include <QWidget>
#include "pathtracer.h"
#include "./ui/ui.h"
#include "globals.h"


int main(int argc, char **argv) {
    QApplication app(argc, argv);

    GUI gui;
    gui.show();

    PathTracer pathtracer(gui.imageWidth, gui.imageHeight);
    pathtracer.getFrameBuffer();
    
    return app.exec();
}
