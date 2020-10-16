#include "ExtrudeProperty_dialog.h"
#include "ui_ExtrudeProperty_dialog.h"

ExtrudePropertryDialog::ExtrudePropertryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtrudePropertrDialog)
{
    ui->setupUi(this);
}

ExtrudePropertryDialog::~ExtrudePropertryDialog()
{
    delete ui;
}

void ExtrudePropertryDialog::on_okButton_pressed()
{
    float width_var = ui->widthEdit->text().toFloat();
    float up_var = ui->upEdit->text().toFloat();
    float down_var = ui->downEdit->text().toFloat();

    emit send_ExtrudeProperty(width_var, up_var, down_var);

    close();
}
