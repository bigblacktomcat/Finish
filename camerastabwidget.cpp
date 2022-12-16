#include "camerastabwidget.h"
#include "ui_camerastabwidget.h"

#include <QButtonGroup>
#include <QMessageBox>


CamerasTabWidget::CamerasTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CamerasTabWidget)
{
    ui->setupUi(this);
    initEventHandlers();
    // установить валидатор ip-адреса
    QRegExpValidator* ipValidator = IssoCommonData::makeIpValidator(this);
    ui->leCameraIp->setValidator(ipValidator);
    // настроить список камер:
    // задать заголовок
    ui->wgtCameras->setTitle(tr("Камеры"));
    // скрыть кнопку редактирования
    ui->wgtCameras->setEditBtnVisible(false);
    // сформировать группу радиокнопок
    QButtonGroup* radioGroup = new QButtonGroup(this);
    radioGroup->addButton(ui->rbtnSendOff, CAMERA_SEND_OFF);
    radioGroup->addButton(ui->rbtnSendPhoto, CAMERA_SEND_PHOTO);
    radioGroup->addButton(ui->rbtnSendVideo, CAMERA_SEND_VIDEO);
    // выбрать первый пункт
    radioGroup->button(CAMERA_SEND_OFF)->setChecked(true);
}


CamerasTabWidget::~CamerasTabWidget()
{
    delete ui;
}


void CamerasTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
}


void CamerasTabWidget::updateTab()
{
    if (!m_object)
        return;
    // заполнить список камер
    fillCameraList(m_object->cameras());
    // передать в компонент расположения камеры список зданий
    ui->gbCameraLocation->setBuildings(m_object->buildings());
}


void CamerasTabWidget::processAddCameraBtnClicked()
{
    if (!m_object)
        return;

    NewCameraDialog dlg(m_object->availableCameraIds(),
                        m_object->buildings(),
                        this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;

    // добавить камеру в объект
    IssoCamera* camera = m_object->addCamera(dlg.id(), dlg.ip(),
                                             dlg.port(), dlg.password(),
                                             dlg.buildNumber(), dlg.floorNumber());
    // если указатель пустой, значит камера не добавлена
    if (!camera)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить камеру в систему, "
                                "т.к. камера с идентификатором <%1> уже существует")
                                .arg(dlg.id()));
        return;
    }
    // обновить список камер
    fillCameraList(m_object->cameras());
    // выбрать добавленный элемент
    if (auto item = ui->wgtCameras->findItem(camera->displayName()))
    {
        item->setSelected(true);
    }
}


void CamerasTabWidget::processRemoveCameraBtnClicked()
{
    if (!m_object)
        return;

    if (!ui->wgtCameras->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить камеру из системы,"
                                "т.к. не выбрано имя камеры"));
        return;
    }
    // получить выбранный элемент визуального списка
    auto item = ui->wgtCameras->selectedItem();
    // удалить камеру из объекта
    if (!m_object->removeCamera(item->data(Qt::UserRole).toInt()))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Не удалось удалить камеру из системы"));
        return;
    }
    // удалить и уничтожить элемент визуального списка
    ui->wgtCameras->removeAndDeleteItem(item);
}


void CamerasTabWidget::processSaveSettingsBtnClicked()
{
    if (!m_object)
        return;
    // если камера не выбрана, выход
    if (!ui->wgtCameras->hasSelectedItems())
        return;

    // получить ID выбранной камеры
    int id = ui->wgtCameras->selectedItem()->data(Qt::UserRole).toInt();
    // найти камеру по ID
    IssoCamera* camera = m_object->findCameraById(id);
    if (!camera)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Выбранная камера не найдена в списке камер объекта"));
        return;
    }
    // сохранить изменения:
    //
    // ip-адрес
    camera->setIp(QHostAddress(ui->leCameraIp->text()));
    // порт
    camera->setPort(ui->sbCameraPort->value());
    // пароль
    camera->setPassword(ui->lePassword->text());
    // режим отображения камеры
    auto mode = static_cast<IssoCameraSendMode>(ui->rbtnSendOff->group()->checkedId());
    camera->setSendMode(mode);
    //
    // здание и этаж
    int buildNum = ui->gbCameraLocation->buildNumber();
    int floorNum = ui->gbCameraLocation->floorNumber();
    // переместить камеру на новый этаж
    if (!m_object->changeCameraLocation(camera, buildNum, floorNum))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Не удалось изменить расположение камеры <%1>")
                                .arg(camera->displayName()));
        return;
    }
    // сообщить об изменении настроек камеры
    QMessageBox::information(this, tr("Информация"),
                             tr("Настройки камеры <%1> сохранены")
                                .arg(camera->displayName()));
}


void CamerasTabWidget::processListCameraSelectionChanged()
{
    if (!m_object)
        return;

    IssoCamera* camera = nullptr;
    // если камера выбрана
    if (ui->wgtCameras->hasSelectedItems())
    {
        // получить ID камеры
        int id = ui->wgtCameras->selectedItem()->data(Qt::UserRole).toInt();
        // найти камеру по ID
        camera = m_object->findCameraById(id);
    }
    //
    // сформировать данные для отображения:
    //
    // ip-адрес
    QString ip = (camera ? camera->ip().toString() : "");
    // порт
    quint16 port = (camera ? camera->port() : 0);
    // пароль
    QString password = (camera ? camera->password() : "");
    // здание
    int buildNum = (camera ? camera->buildNum() : 0);
    // этаж
    int floorNum = (camera ? camera->floorNum() : 0);
    // режим отображения камеры
    int displayModeId = (camera ? camera->sendMode() : 0);
    //
    // вывести характеристики выбранной камеры:
    //
    // ip-адрес
    ui->leCameraIp->setText(ip);
    // порт
    ui->sbCameraPort->setValue(port);
    // пароль
    ui->lePassword->setText(password);
    // здание
    ui->gbCameraLocation->setBuildNumber(buildNum);
    // этаж
    ui->gbCameraLocation->setFloorNumber(floorNum);
    //
    // задать возможность редактирования настроек
    ui->gbCameraSettings->setEnabled(camera);
    //
    // режим отображения камеры
    ui->rbtnSendOff->group()->button(displayModeId)->setChecked(true);
}


void CamerasTabWidget::initEventHandlers()
{
    connect(ui->wgtCameras, SIGNAL(addBtnClicked(bool)),
            this,           SLOT(processAddCameraBtnClicked()));
    connect(ui->wgtCameras, SIGNAL(removeBtnClicked(bool)),
            this,           SLOT(processRemoveCameraBtnClicked()));
    connect(ui->wgtCameras, SIGNAL(selectionChanged()),
            this,           SLOT(processListCameraSelectionChanged()));
    connect(ui->btnSaveSettings, SIGNAL(clicked(bool)),
            this,                SLOT(processSaveSettingsBtnClicked()));
}


void CamerasTabWidget::fillCameraList(QList<IssoCamera *> cameras)
{
    // очистить список камер
    ui->wgtCameras->clearList();
    // заполнить список камер
    foreach (IssoCamera* camera, cameras)
    {
        // добавить камеру в визуальный список
        // и привязать ID камеры
        ui->wgtCameras->addItem(camera->displayName())
                            ->setData(Qt::UserRole, QVariant(camera->id()));
    }
}

