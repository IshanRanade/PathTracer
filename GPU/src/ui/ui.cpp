#include "ui.h"
#include <QSize>
#include <QPixmap>
#include <QColor>
#include <QPoint>


RenderButtonWidget::RenderButtonWidget(QWidget *parent) :
    QWidget(parent) {

    layout = new QVBoxLayout();
    button = new QPushButton("Render");
    layout->addWidget(button);
    setLayout(layout);
}


RenderDisplayWidget::RenderDisplayWidget(QWidget *parent, int width, int height) :
    QWidget(parent), width(width), height(height) {

    layout = new QVBoxLayout();
    image = new QImage(width, height, QImage::Format_RGB32);

    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            image->setPixelColor(QPoint(i, j), QColor(20,40,50));
        }
    }

    label = new QLabel();
    label->setPixmap(QPixmap::fromImage(*image));
    layout->addWidget(label);
    setLayout(layout);
}

void RenderDisplayWidget::setDisplaySize(int width, int height) {
    this->width = width;
    this->height = height;
}


GUI::GUI() :
    QMainWindow(), imageWidth(1280), imageHeight(720) {

    initLayout();
}

void GUI::show() {
    mainWidget->show();
}

void GUI::initLayout() {
    mainWidget = new QWidget();
    mainWidget->setWindowTitle("Path Tracer");

    layout = new QHBoxLayout(this);

    // Add the various widgets
    renderButtonWidget = new RenderButtonWidget(this);
    layout->addWidget(renderButtonWidget);

    renderDisplayWidget = new RenderDisplayWidget(this, imageWidth, imageHeight);
    layout->addWidget(renderDisplayWidget);

    mainWidget->setLayout(layout);
}
