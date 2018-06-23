#include "ui.h"
#include <QSize>
#include <QPixmap>
#include <QColor>
#include <QPoint>
#include <iostream>

/*
*/
RenderButtonWidget::RenderButtonWidget(QWidget *parent) :
    QWidget(parent) {

    layout = new QVBoxLayout();
    button = new QPushButton("Render");
    layout->addWidget(button);
    setLayout(layout);
}

/*
*/
RenderDisplayWidget::RenderDisplayWidget(QWidget *parent, int width, int height) :
    QWidget(parent), width(width), height(height) {

    layout = new QVBoxLayout();
    image = new QImage(width, height, QImage::Format_RGB32);

    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            image->setPixelColor(QPoint(i, j), QColor(0,255,0));
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

void RenderDisplayWidget::updateDisplay(std::vector<float> rgbData) {
    int row = -1;
    int col = 0;

    int i = 0;
    while(i < rgbData.size()) {
        if(i % width == 0) {
            row += 1;
            col = 0;
        }
        image->setPixelColor(QPoint(col, row), QColor(rgbData[i],rgbData[i+1],rgbData[i+2]));
        col++;

        i += 3;
    }

    label->setPixmap(QPixmap::fromImage(*image));    
}

/*
*/
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

void GUI::updateDisplay(std::vector<float> rgbData) {
    renderDisplayWidget->updateDisplay(rgbData);
    update();
}
