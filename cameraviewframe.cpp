#include "cameraviewframe.h"
#include "ui_cameraviewframe.h"

#include <QPixmap>
#include <QDebug>


CameraViewFrame::CameraViewFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CameraViewFrame)
{
    ui->setupUi(this);
    // убрать кнопки "свернуть" и "раскрыть"
    setWindowFlags(windowFlags() &
                   ~Qt::WindowMinimizeButtonHint &
                   ~Qt::WindowMaximizeButtonHint);
//    // убрать кнопки "свернуть" и "раскрыть"
//    // и добавить флаг "поверх всех окон"
//    setWindowFlags(windowFlags() &
//                   ~Qt::WindowMinimizeButtonHint &
//                   ~Qt::WindowMaximizeButtonHint |
//                   Qt::WindowStaysOnTopHint);
    // сместить в левый верхний угол
    move(0, 0);
}


CameraViewFrame::~CameraViewFrame()
{
    delete ui;
}

void CameraViewFrame::closeEvent(QCloseEvent *event)
{
    emit closed();
    QFrame::closeEvent(event);
}


void CameraViewFrame::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    // вывести сообщение
    ui->lblVideo->setText(tr("Устанавливается соединение с камерой..."));
}


void CameraViewFrame::displayFrame(const QImage &frame)
{
    QPixmap pixmap = QPixmap::fromImage(frame)
                            .scaled(ui->lblVideo->size(), Qt::KeepAspectRatio);
    ui->lblVideo->setPixmap(pixmap);
}


void CameraViewFrame::show(const QString& title)
{
    // установить заголовок окна
    setWindowTitle(title);
    // показать окно
    QFrame::show();
}

//void CameraViewFrame::show(const QHostAddress &ip)
//{
//    // установить заголовок окна
//    setWindowTitle(ip.toString());
//    // показать окно
//    QFrame::show();
//}
