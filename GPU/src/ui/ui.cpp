#include <iostream>
#include <fstream>
#include <streambuf>
#include <thread>
#include <vector>

#include "../pathtracer.h"

#include <QMainWindow>
#include <QApplication>

int main(int argc, char **argv) {
    // PathTracer pathtracer = PathTracer();
    // std::cout << pathtracer.getFrameBuffer() << std::endl;
    QApplication app (argc, argv);
    return app.exec();
}