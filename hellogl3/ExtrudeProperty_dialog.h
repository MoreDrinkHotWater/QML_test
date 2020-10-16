#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class ExtrudePropertrDialog;
}

class ExtrudePropertryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtrudePropertryDialog(QWidget *parent = nullptr);
    ~ExtrudePropertryDialog();

signals:
    void send_ExtrudeProperty(float width_var, float up_var, float down_var);

private slots:
    void on_okButton_pressed();

private:
    Ui::ExtrudePropertrDialog *ui;
};

#endif // DIALOG_H
