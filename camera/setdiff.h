#ifndef SETDIFF_H
#define SETDIFF_H

#include <QWidget>

namespace Ui {
class setDiff;
}

class setDiff : public QWidget
{
    Q_OBJECT

public:
    explicit setDiff(QWidget *parent = nullptr);
    ~setDiff();

signals:
    void send_Diff(float diff);

private slots:
    void on_OK_clicked();

private:
    Ui::setDiff *ui;
};

#endif // SETDIFF_H
