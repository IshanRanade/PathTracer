#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QImage>
#include <QGraphicsView>
#include <QLabel>


/*
    This class is the button that Renders the framebuffer.
*/
class RenderButtonWidget : public QWidget {
public:
    RenderButtonWidget(QWidget *parent);

private:
    QPushButton *button;
    QVBoxLayout *layout;
};


/*
    This class encapsulates the image area that displays the path traced frame buffer.
*/
class RenderDisplayWidget : public QWidget {
public:
    RenderDisplayWidget(QWidget *parent, int width, int height);

    void setDisplaySize(int width, int height);

private:
    int width;
    int height;
    QVBoxLayout *layout;
    QImage *image;
    QLabel *label;
};


/*
    This class encapsulates the entire GUI, using q QMainWindow.  It holds all the buttons
    and inputs that affect the path tracer and also an image area that displays the path traced
    final framebuffer.
*/
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
