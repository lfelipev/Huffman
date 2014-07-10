#include "Select.h"
#include "ui_select.h"
#include "codificar.h"
#include "decodificar.h"

Select::Select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Select) {
    ui->setupUi(this);

    /** Botão comprimir >> Menu Compressão **/
    connect(ui->compressButton, SIGNAL(clicked()), this, SLOT(showCompressDialog()));

    /** Botão descomprimir >> Menu Descompressão **/
    connect(ui->decompressButton, SIGNAL(clicked()), this, SLOT(showDecompressDialog()));
}

Select::~Select() {
    delete ui;
}

void Select::showCompressDialog() {
    QDialog parent;
    Codificar *co;
    co = new Codificar(&parent);
    co->setFixedSize(co->size());
    co->setWindowTitle("Compressão");
    co->exec();
}

void Select::showDecompressDialog() {
    QDialog parent;
    Decodificar *de;
    de = new Decodificar(&parent);
    de->setFixedSize(de->size());
    de->setWindowTitle("Descompressão");
    de->exec();
}
