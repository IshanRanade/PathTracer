#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QImage>
#include <QGraphicsView>
#include <QLabel>


class RenderButtonWidget : public QWidget {
    public:
        RenderButtonWidget(QWidget *parent);

    private:
        QPushButton *button;
        QVBoxLayout *layout;
};


class RenderDisplayWidget : public QWidget {
    public:
        RenderDisplayWidget(QWidget *parent);

    private:
        QVBoxLayout *layout;
        QImage *image;
        QLabel *label;
};


class GUI : public QMainWindow {
    public:
        GUI();

        void show();

        int imageWidth;
        int imageHeight;

    private:
        QWidget *mainWidget;
        QHBoxLayout *layout;
        RenderButtonWidget *renderButtonWidget;
        RenderDisplayWidget *renderDisplayWidget;

        void initLayout();
};
