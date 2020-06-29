#ifndef READFILE_H
#define READFILE_H

#include <QFileDialog>
#include <QMainWindow>

class ReadFile :  public QObject
{
    Q_OBJECT

public:
    explicit ReadFile() = default;
    ~ReadFile() override = default;

Q_SIGNALS:

public Q_SLOTS:
    void startReadSlot(QMainWindow *mainWindow);

private:
    QFileDialog *fileDialog;

    QMainWindow *mainWindow;



};

#endif // READFILE_H
