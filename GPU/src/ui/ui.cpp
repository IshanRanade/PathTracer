#include "./ui.h"


GUI::GUI() :
    QMainWindow(), imageWidth(1280), imageHeight(720) {

    initLayout();
}

void GUI::initLayout() {
    layout = new QHBoxLayout(this);
    renderButtonWidget = new RenderButtonWidget(this);
    layout->addWidget(renderButtonWidget);
    setLayout(layout);
}

RenderButtonWidget::RenderButtonWidget(QWidget *parent) :
    QWidget(parent), button(new QPushButton()) {
}
