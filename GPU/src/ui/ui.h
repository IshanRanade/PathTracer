#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>


class RenderButtonWidget : public QWidget {
    public:
        RenderButtonWidget(QWidget *parent);

    private:
        QPushButton *button;
};

class GUI : public QMainWindow {
    public:
        GUI();

        int imageWidth;
        int imageHeight;

    private:
        QHBoxLayout *layout;
        RenderButtonWidget *renderButtonWidget;

        void initLayout();
};

