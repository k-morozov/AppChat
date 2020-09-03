#include <QApplication>
#include "control/control.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    Control control(argc, argv);

    return a.exec();
}
