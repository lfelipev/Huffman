#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

namespace Ui {
class dialog;
}

class dialog : public QWidget
{
    Q_OBJECT

public:
    explicit dialog(QWidget *parent = 0);
    ~dialog();

private:
    Ui::dialog *ui;
private slots:
    void showCompressDialog();
    void showDecompressDialog();
};

#endif // DIALOG_H
