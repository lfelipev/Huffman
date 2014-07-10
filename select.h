#ifndef SELECT_H
#define SELECT_H

#include <QDialog>

namespace Ui {
class Select;
}

class Select : public QDialog {
    Q_OBJECT

public:
    explicit Select(QWidget *parent = 0);
    ~Select();

private:
    Ui::Select *ui;

private slots:
    void showCompressDialog();
    void showDecompressDialog();
};

#endif // SELECT_H
