#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QKeyEvent>
#include "pathtracer.h"
#include "ui/ui.h"
#include "globals.h"


class AppDialog : public QObject {
public:
    QApplication *app;

    AppDialog(QApplication *parentApp) :
        app(parentApp) {
    }

    bool eventFilter(QObject* obj, QEvent* event) {
        if (event->type()==QEvent::KeyPress) {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            if (key->key()==Qt::Key_Escape) {
                app->exit();
            } else {
                return QObject::eventFilter(obj, event);
            }
            return true;
        } else {
            return QObject::eventFilter(obj, event);
        }
        return false;
    }
};

class App : public QApplication {
public:
    App(int argc, char **argv) :
        QApplication(argc, argv) {
    }
};


int main(int argc, char **argv) {
    QApplication *app = new QApplication(argc, argv);

    GUI gui;
    gui.show();

    PathTracer pathtracer(gui.imageWidth, gui.imageHeight);
    pathtracer.getFrameBuffer();

    AppDialog *dialog = new AppDialog(app);
    app->installEventFilter(dialog);

    return app->exec();
}
