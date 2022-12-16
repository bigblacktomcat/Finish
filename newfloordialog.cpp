/**
\brief Реализация диалогового класса NewFloorDialog
Для нового этажа задаём идентификатор, графический план
\date  2020-08-18
**/
#include "newfloordialog.h"
#include "ui_newfloordialog.h"
#include <QFileDialog>
#include <QSet>


NewFloorDialog::NewFloorDialog(QList<int> availableNumbers, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFloorDialog)
{
    init();
    // установить заголовок
    setWindowTitle(tr("Добавить этаж"));
    // заполнить список доступных номеров этажей
    foreach (int number, availableNumbers)
    {
        QString numberText = (number < 0 ? tr("Подвал %1").arg(abs(number)) :
                                           tr("Этаж %1").arg(number));
        // добавить описание и номер этажа
        ui->cbNumber->addItem(numberText, QVariant::fromValue(number));
    }
}


NewFloorDialog::NewFloorDialog(int currentNumber, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFloorDialog)
{
    init();
    // установить заголовок
    setWindowTitle(tr("Изменить этаж"));
    // отобразить редактируемый этаж
    QString numberText = (currentNumber < 0 ? tr("Подвал %1").arg(abs(currentNumber)) :
                                              tr("Этаж %1").arg(currentNumber));
    ui->cbNumber->addItem(numberText, QVariant::fromValue(currentNumber));
    // деактивировать выпадающий список
    ui->cbNumber->setEnabled(false);
}


NewFloorDialog::~NewFloorDialog()
{
    delete ui;
}


void NewFloorDialog::init()
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // создать сцену
    ui->gvPreview->setScene(new QGraphicsScene(this));
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
}


int NewFloorDialog::number() const
{
//   return ui->sbNumber->value();
    return ui->cbNumber->currentData().toInt();
}


QPixmap NewFloorDialog::pixmap(bool &ok) const
{
    ok = m_ok;
    return m_pixmap;
}


void NewFloorDialog::on_btnSelectFloorPath_clicked()
{
    QString fileName =
            QFileDialog::getOpenFileName(this,
                                         tr("Открыть план этажа"),
                                         QDir::currentPath(),
                                         tr("Изображения (*.png *.xpm *.jpg *.jpeg)"));
    if(!fileName.isEmpty())
    {
        ui->leFileName->setText(fileName);
        ui->gvPreview->scene()->clear();
        m_pixmap = QPixmap(fileName);
        QSize viewSize = ui->gvPreview->size();
        ui->gvPreview->scene()->addPixmap(QPixmap(fileName).scaled(viewSize.width(),
                                                                   viewSize.height(),
                                                                   Qt::KeepAspectRatio));
        ui->gvPreview->show();
        m_ok = true;
    }
    else
        m_ok = false;
}
