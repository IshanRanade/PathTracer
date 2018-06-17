#include <iostream>
#include <QApplication>
#include "pathtracer.h"
#include "./ui/ui.h"
#include "globals.h"


int main(int argc, char **argv) {
    QApplication app(argc, argv);
    GUI gui;
    gui.show();

    // Set the root directory once at startup so other files can reference their kernels from here
    // extern std::string KERNEL_ROOT_DIR;
    // KERNEL_ROOT_DIR = std::string("../src/");

    PathTracer pathtracer(gui.imageWidth, gui.imageHeight);
    pathtracer.getFrameBuffer();
    return app.exec();
}