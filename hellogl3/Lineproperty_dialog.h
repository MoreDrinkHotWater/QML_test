#ifndef LINEPROPERTYDIALOG_H
#define LINEPROPERTYDIALOG_H

#include <QDialog>

namespace Ui {
class LinePropertyDialog;
}

class LinePropertyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinePropertyDialog(QWidget *parent = nullptr);
    ~LinePropertyDialog();

signals:
    void send_LineProperty(float width_var);

private slots:
    void on_okButton_pressed();

private:
    Ui::LinePropertyDialog *ui;
};

#endif // LINEPROPERTYDIALOG_H
