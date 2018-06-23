#include <iostream>
#include <QtWidgets>
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QKeyEvent>
#include "pathtracer.h"
#include "ui/ui.h"
#include "globals.h"


/*
    This class attaches to a QApplication and listens to keyboard inputs.
*/
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

/*
    This class is the main app and implements an MVC.
    It contains a reference to the GUI, the PathTracer, and the QApplication.
*/
class App {
public:
    GUI *gui;
    PathTracer *pathtracer;
    QApplication *app;

    App(int argc, char **argv) {
        app = new QApplication(argc, argv);
        gui = new GUI();
        pathtracer = new PathTracer(gui->imageWidth, gui->imageHeight);
        
        AppDialog *dialog = new AppDialog(app);
        app->installEventFilter(dialog);

        gui->show();
        

        // TEMP
        // Render the scene immediately and display it
        renderAndDisplayScene();

    }

    int startApp() {
        return app->exec();
    }

    void renderAndDisplayScene() {
        std::vector<float> frameBufferData = pathtracer->getFrameBuffer();
        gui->updateDisplay(frameBufferData);
    }
};
