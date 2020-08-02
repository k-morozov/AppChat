#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "control/control.h"

int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    Control control;

    engine.rootContext()->setContextProperty("control", &control);
    engine.load(QUrl(QStringLiteral("qrc:/resources/main.qml")));

    if (engine.rootObjects().isEmpty())
      return -1;

    return app.exec();
}
