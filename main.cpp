#include <QApplication>
#include "Window.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Window window;
    window.setWindowTitle("2D Physics Sandbox - SAT Collision Engine");
    window.show();

    return app.exec();
}