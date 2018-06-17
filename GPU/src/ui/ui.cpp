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


RenderDisplayWidget::RenderDisplayWidget(QWidget *parent) :
    QWidget(parent) {

    layout = new QVBoxLayout();
    image = new QImage(1280, 720, QImage::Format_RGB32);

    for(int i = 0; i < 1280; ++i) {
        for(int j = 0; j < 720; ++j) {
            image->setPixelColor(QPoint(i, j), QColor(0.2,0.4,0.5));
        }
    }

    label = new QLabel();
    label->setPixmap(QPixmap::fromImage(*image));
    layout->addWidget(label);
    setLayout(layout);
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

    renderDisplayWidget = new RenderDisplayWidget(this);
    layout->addWidget(renderDisplayWidget);

    mainWidget->setLayout(layout);
    //mainWidget->setFixedSize(QSize(1000,800));
}
