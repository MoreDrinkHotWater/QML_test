#ifndef CAMERAFRAMEGRABBER_H
#define CAMERAFRAMEGRABBER_H

#include <QAbstractVideoSurface>

QT_BEGIN_NAMESPACE
namespace Ui { class Camera; }
QT_END_NAMESPACE

class CameraFrameGrabber :public QAbstractVideoSurface
{
    Q_OBJECT
public:
    CameraFrameGrabber(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;
    bool present(const QVideoFrame &frame) override;

signals:
    void frameAvailable(QImage frame);

public slots:

private:
    Ui::Camera *ui;

};


#endif // CAMERAFRAMEGRABBER_H
