#include "Select.h"
#include "codificar.h"
#include "decodificar.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Select w;
    w.show();

    return app.exec();
}
