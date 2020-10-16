#include "Lineproperty_dialog.h"
#include "ui_Lineproperty_dialog.h"

LinePropertyDialog::LinePropertyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinePropertyDialog)
{
    ui->setupUi(this);
}

LinePropertyDialog::~LinePropertyDialog()
{
    delete ui;
}

void LinePropertyDialog::on_okButton_pressed()
{
    float width_var = ui->widthEdit->text().toFloat();

    emit send_LineProperty(width_var);

    close();
}
