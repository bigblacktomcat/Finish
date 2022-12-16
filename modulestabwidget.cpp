#include "descriptorsetupdialog.h"
#include "issofiltercomboitemdelegate.h"
#include "modulestabwidget.h"
#include "newmoduledialog.h"
#include "passworddialog.h"
#include "ui_modulestabwidget.h"

#include <QMessageBox>


ModulesTabWidget::ModulesTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulesTabWidget)
{
    ui->setupUi(this);
    initEventHandlers();
    // установить делегат для кастомной отрисовки элементов фильтров
    IssoFilterComboItemDelegate* filterDelegate = new IssoFilterComboItemDelegate();
    ui->cbFilterBuilding->setItemDelegate(filterDelegate);
    ui->cbFilterFloor->setItemDelegate(filterDelegate);
    //
    // задать заголовок
    ui->wgtModules->setTitle(tr("Модули"));
    // скрыть кнопку редактирования
    ui->wgtModules->setEditBtnVisible(false);
    // задать делегат
    ui->wgtModules->list()->setItemDelegate(filterDelegate);
    //
    // список мультидатчиков
    ui->listMultiSensors->setItemDelegate(filterDelegate);
    // установить валидатор ip-адреса
    QRegExpValidator* ipValidator = IssoCommonData::makeIpValidator(this);
    ui->leModuleIp->setValidator(ipValidator);
    ui->leBoardIp->setValidator(ipValidator);
    //
    // лишнее
    ui->wgtCameras->setVisible(false);
    ui->gbPeripherals->setVisible(false);
}


ModulesTabWidget::~ModulesTabWidget()
{
    delete ui;
}


void ModulesTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
    // если объект пуст, выход
    if (!m_object)
        return;

}


void ModulesTabWidget::updateTab()
{
    if (!m_object)
        return;

    // обновить список ассоциированных камер
    updateLinkedCameras();
    // заполнить компонент "Расположение"
    ui->gbModuleLocation->setBuildings(m_object->buildings());
    // обновить список конфигураций модулей
    updateModuleConfigs();
    // обновить компоненты фильтрации
    updateFilterWidgets();
}


void ModulesTabWidget::initEventHandlers()
{
    connect(ui->wgtModules, SIGNAL(addBtnClicked(bool)),
            this,           SLOT(processModulesAddBtnClicked()));
    connect(ui->wgtModules, SIGNAL(removeBtnClicked(bool)),
            this,           SLOT(processModulesRemoveBtnClicked()));
    connect(ui->wgtModules, SIGNAL(selectionChanged()),
            this,           SLOT(processModulesSelectionChanged()));

    connect(ui->btnSaveModuleSettings, SIGNAL(clicked(bool)),
            this,                      SLOT(processBtnSaveModuleSettingsClicked()));
    connect(ui->btnActivateModule, SIGNAL(clicked(bool)),
            this,                  SLOT(processBtnActivateModuleClicked()));
    connect(ui->btnSetupDescriptor, SIGNAL(clicked(bool)),
           this,                    SLOT(processBtnSetupDescriptorClicked()));    
    connect(ui->cbFilterBuilding, SIGNAL(currentIndexChanged(int)),
            this,                 SLOT(processBuildFilterIndexChanged(int)));
    connect(ui->btnUpdateModuleConfig, SIGNAL(clicked(bool)),
            this,                      SLOT(processBtnUpdateModuleConfigClicked()));
    connect(ui->btnMsActivateAll, SIGNAL(clicked(bool)),
            this,                 SLOT(processBtnMsActivateClicked()));
    connect(ui->btnMsReadConfig, SIGNAL(clicked(bool)),
            this,                SLOT(processBtnMsReadConfigClicked()));
    connect(ui->btnMsWriteConfig, SIGNAL(clicked(bool)),
            this,                 SLOT(processBtnMsWriteConfigClicked()));
    connect(ui->btnMsResetState, SIGNAL(clicked(bool)),
            this,                SLOT(processBtnMsResetStateClicked()));
}


void ModulesTabWidget::updateFilteredModulesList()
{
    if (!m_object)
        return;
    // очистить список модулей системы
    ui->wgtModules->clearList();
    // список модулей
    QList<IssoModule*> modules;
    // если фильтр не выбран, вывести все модули
    if (!ui->gbFilter->isChecked())
    {
        modules = m_object->modules();
    }
    else
    {
        // выбранные здание или этаж
        QString buildName = ui->cbFilterBuilding->currentText();
        QString floorName = ui->cbFilterFloor->currentText();
        // получить список отфильтрованных модулей
        modules = m_object->filteredModules(buildName, floorName);
    }

    // сортировать список
    std::sort(modules.begin(), modules.end(),
              [](const IssoModule* a, const IssoModule* b) -> bool
        {
            // если признак активации модулей не совпадает,
            // расположить активированный модуль после неактивированного
            if (a->isActivated() != b->isActivated())
            {
                int aValue = (a->isActivated() ? 1 : 0);
                int bValue = (b->isActivated() ? 1 : 0);
                return aValue < bValue;
            }
            // если номер здания a < b, не изменять позицию
            if (a->buildNum() < b->buildNum())
                return true;
            // иначе, если номер здания a == b, сортировать по номеру этажа
            if (a->buildNum() == b->buildNum())
            {
                // если номер здания == 0 (нераспределенный модуль),
                // сортировать по уникальному ID
                if (a->buildNum() == 0)
                    return a->uniqueId() < b->uniqueId();
                // если номер этажа a < b, не изменять позицию
                if (a->floorNum() < b->floorNum())
                    return true;
                // иначе, если номер этажа a == b,
                // сортировать по порядковому номеру модуля на этаже
                if (a->floorNum() == b->floorNum())
                    return a->displayId() < b->displayId();
            }
            // в противном случае поменять элементы местами
            return false;
        });

    // добавить в визуальный список модули с признаком активации
    foreach (IssoModule* module, modules)
    {
        // добавить элемент в список
        auto item = ui->wgtModules->addItem(module->displayName());
        // установить признак активации
        item->setData(Qt::UserRole, QVariant(module->isActivated()));
    }
    // если список не пуст, выбрать первый элемент
    if (ui->wgtModules->itemCount() > 0)
        ui->wgtModules->item(0)->setSelected(true);
}


void ModulesTabWidget::updateMultiSensorList()
{
    if (!m_object)
        return;
    // очистить визуальный список мультидатчиков
    ui->listMultiSensors->clear();
    // получить выбранный модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // получить адресный шлейф
    IssoSensorChain* multiChain = module->sensorChain(MULTISENSOR);
    if (!multiChain)
        return;
    // получить список мультидатчиков
    auto multiSensors = multiChain->sensors();
    // сортировать список
    std::sort(multiSensors.begin(), multiSensors.end(),
              [](const IssoSensor* a, const IssoSensor* b) -> bool
        {
            // если признак активации мультидатчиков не совпадает,
            // расположить активированный датчик после неактивированного
            if (a->activated() != b->activated())
            {
                int aValue = (a->activated() ? 1 : 0);
                int bValue = (b->activated() ? 1 : 0);
                return aValue < bValue;
            }
            // сортировать по id
            return (a->id() < b->id());
        });
    // добавить в визуальный список мультидатчики с признаком активации
    foreach (auto multiSensor, multiSensors)
    {
        // создать элемент списка
        QListWidgetItem* item =
                  new QListWidgetItem(IssoCommonData::makeMultiSensorName(multiSensor->id()));
//                new QListWidgetItem(tr("Мультидатчик-%1").arg(multiSensor->id()));
        // установить признак активации
        item->setData(Qt::UserRole, QVariant(multiSensor->activated()));
        // привязать ID мультидатчика
        item->setData(Qt::UserRole + 1, QVariant(multiSensor->id()));
        // добавить элемент в список
        ui->listMultiSensors->addItem(item);
    }
}


void ModulesTabWidget::processBtnUpdateModuleConfigClicked()
{
    if (!m_object)
        return;
    // получить выбранный модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // получить имя конфига модуля
    QString moduleCfgName = ui->cbModuleConfigs->currentText();
//    QString moduleCfgName = (ui->cbModuleConfigs->currentIndex() != -1 ?
//                                 ui->cbModuleConfigs->currentText() : "");
    //
    // обновить отображаемые шлейфы модуля
    m_object->applyModuleConfigToModule(module->displayName(), moduleCfgName);
    // отобразить список мультидатчиков
    updateMultiSensorList();
    // известить о необходимости записать конфигурацию в модуль
    emit updateModuleConfigRequested(module->uniqueId());
}


void ModulesTabWidget::processBtnMsActivateClicked()
{
    // получить выбранный модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // получить адресный шлейф
    IssoSensorChain* multiChain = module->sensorChain(MULTISENSOR);
    if (!multiChain)
        return;
    // получить список мультидатчиков
    auto multiSensors = multiChain->sensors();
    // сформировать список ID неактивированных мультидатчиков
    QList<quint8> ids;
    foreach (auto sensor, multiSensors)
    {
        if (!sensor->activated())
            ids << sensor->id();
    }
    // если список не пуст, запросить запуск активации мультидатчиков
    if (!ids.isEmpty())
        emit multiSensorActivationRequested(module->uniqueId(), ids);

//    // если список пуст, выход
//    if (ids.isEmpty())
//        return;
//    // запросить запуск активации мультидатчиков
    //    emit msActivationRequested(module->uniqueId(), ids);
}


void ModulesTabWidget::processBtnMsReadConfigClicked()
{
    // получить выбранный модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // получить ID выбранного мультидатчика
    int msId = selectedMultiSensorId();
    // запросить запись конфига мультидатчика
    if (msId != -1)
        emit multiSensorRegActionRequested(IssoMsRegAction::READ_THRESHOLD,
                                             module->uniqueId(), msId);
}


void ModulesTabWidget::processBtnMsWriteConfigClicked()
{
    // получить выбранный модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // получить ID выбранного мультидатчика
    int msId = selectedMultiSensorId();
    // запросить запись конфига мультидатчика
    if (msId != -1)
        emit multiSensorRegActionRequested(IssoMsRegAction::WRITE_THRESHOLD,
                                             module->uniqueId(), msId);
}


void ModulesTabWidget::processBtnMsResetStateClicked()
{
    // получить выбранный модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // получить ID выбранного мультидатчика
    int msId = selectedMultiSensorId();
    // запросить сброс состояния мультидатчика
    if (msId != -1)
        emit multiSensorRegActionRequested(IssoMsRegAction::RESET_STATE,
                                             module->uniqueId(), msId);
}


void ModulesTabWidget::processMultiSensorActivated(const QHostAddress &moduleIp, quint8 msId)
{
    if (!m_object)
        return;
    // найти модуль по IP
    IssoModule* module = m_object->findModuleByAddress(moduleIp);
    if (!module)
        return;
    // перевести мультидатчик модуля в состояние Активирован
    module->setMultiSensorActivated(msId, true);
    // отобразить изменения в визуальном списке
    updateMultiSensorList();
}


void ModulesTabWidget::processMultiSensorRegActionCompleted(IssoMsRegAction action,
                                                              const QHostAddress& moduleIp,
                                                              quint8 msId, bool success)
{
    if (!isVisible())
        return;

    if (!m_object)
        return;
    // найти модуль
    IssoModule* module = m_object->findModuleByAddress(moduleIp);
    if (!module)
        return;
    // сформировать сообщение
    QString msg;
    switch(action)
    {
        case IssoMsRegAction::READ_THRESHOLD:
        {
            msg = tr("Чтение пороговых значений %1 (модуль-%2) %3")
                        .arg(IssoCommonData::makeMultiSensorName(msId))
                        .arg(module->displayName())
                        .arg(success ? "успешно выполнено" : "не выполнено");
            break;
        }
        case IssoMsRegAction::WRITE_THRESHOLD:
        {
            msg = tr("Запись пороговых значений %1 (модуль-%2) %3")
                        .arg(IssoCommonData::makeMultiSensorName(msId))
                        .arg(module->displayName())
                        .arg(success ? "успешно выполнена" : "не выполнена");
            break;
        }
        case IssoMsRegAction::RESET_STATE:
        {
            msg = tr("Сброс состояния %1 (модуль-%2) %3")
                        .arg(IssoCommonData::makeMultiSensorName(msId))
                        .arg(module->displayName())
                        .arg(success ? "успешно выполнен" : "не выполнен");
            break;
        }
        case IssoMsRegAction::READ_RAW_DATA:
        {
            // автор ничего не придумал
            break;
        }
    }
    if (success)
        QMessageBox::information(this, tr("Результат запроса"), msg);
    else
        QMessageBox::warning(this, tr("Результат запроса"), msg);
}


void ModulesTabWidget::updateFilterWidgets()
{
    if (!m_object)
        return;

    // отключить обработчики фильтров
    setModuleFilterHandlersEnabled(false);
    //
    // запомнить выбранные пункты
    QString buildName = ui->cbFilterBuilding->currentText();
    QString floorName = ui->cbFilterFloor->currentText();
    QString moduleName = ui->wgtModules->selectedItemName();
    //
    // очистить фильтр зданий
    ui->cbFilterBuilding->clear();
    // заполнить фильтр зданий с признаком активации
    ui->cbFilterBuilding->addItem(tr("Не выбрано"), QVariant(true));
    foreach (IssoBuilding* build, m_object->buildings())
    {
        ui->cbFilterBuilding->addItem(build->displayName(), QVariant(build->activated()));
    }
    // восстановить выбранное ранее в фильтре здание
    int buildIndex = ui->cbFilterBuilding->findText(buildName, Qt::MatchExactly);
    // если номер здания найден в списке
    if (buildIndex != -1)
    {
        // выбрать здание в визуальном списке
        ui->cbFilterBuilding->setCurrentIndex(buildIndex);
    }
    //
    // заполнить фильтр этажей здания с признаком активации
    processBuildFilterIndexChanged(buildIndex);
    //
    // восстановить выбранный в фильтре этаж
    int floorIndex = ui->cbFilterFloor->findText(floorName, Qt::MatchExactly);
    // если номер этажа найден в списке
    if ((buildIndex != -1) && (floorIndex != -1))
    {
        // выбрать этаж в визуальном списке
        ui->cbFilterFloor->setCurrentIndex(floorIndex);
    }
    //
    // заполнить визуальный список фильтрованных модулей
    updateFilteredModulesList();
    //
    if ((buildIndex != -1) && (floorIndex != -1) && (!moduleName.isEmpty()))
    {
        // найти элемент списка
        if (auto item = ui->wgtModules->findItem(moduleName))
        {
            // выделить элемент
            item->setSelected(true);
            // прокрутить список до выделенного элемента
            ui->wgtModules->scrollListToItem(item);
        }
    }
    //
    // подключить обработчики фильтров
    setModuleFilterHandlersEnabled(true);
}


void ModulesTabWidget::updateLinkedCameras()
{
    if (!m_object)
        return;
//    // заполнить списки камеры для привязки к датчикам
//    ui->wgtCameras->setCameraList(m_object->cameras());
}

void ModulesTabWidget::updateModuleConfigs()
{
    if (!m_object)
        return;
    ui->cbModuleConfigs->clear();
    ui->cbModuleConfigs->addItems(m_object->moduleConfigNames());
    ui->cbModuleConfigs->setCurrentIndex(-1);
}


void ModulesTabWidget::setModuleFilterHandlersEnabled(bool enabled)
{
    if (enabled)
    {
        connect(ui->gbFilter,       SIGNAL(toggled(bool)),
                this,               SLOT(updateFilteredModulesList()));
        connect(ui->cbFilterFloor,  SIGNAL(currentIndexChanged(int)),
                this,               SLOT(updateFilteredModulesList()));
    }
    else
    {
        disconnect(ui->gbFilter,      SIGNAL(toggled(bool)), 0, 0);
        disconnect(ui->cbFilterFloor, SIGNAL(currentIndexChanged(int)), 0, 0);
    }
}


IssoModule *ModulesTabWidget::selectedModule()
{
    if (!ui->wgtModules->hasSelectedItems())
        return nullptr;
    else
        return m_object->findModuleByDisplayName(ui->wgtModules->selectedItemName());
}


int ModulesTabWidget::selectedMultiSensorId()
{
    auto selectedItems = ui->listMultiSensors->selectedItems();
    return (selectedItems.isEmpty() ? -1 :
                                      selectedItems.first()->data(Qt::UserRole + 1).toInt());
}


void ModulesTabWidget::processModulesAddBtnClicked()
{
    if (!m_object)
        return;

    // создать диалог
    NewModuleDialog dlg(this);
    // отобразить диалог
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;
    //
    // добавить модуль в систему
    QString displayName = m_object->addModule(dlg.displayId(), dlg.ip(),
                                              dlg.port(), dlg.uniqueId());
    // если отображаемое имя модуля пусто, значит модуль не добавлен
    if (displayName.isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить модуль в систему, "
                                "т.к. модуль с таким идентификатором уже существует"));
        return;
    }
    // добавить элемент в визуальный список
    ui->wgtModules->addItem(displayName);
    // обновить визуальный фильтрованный список модулей
    updateFilteredModulesList();
    // найти элемент списка
    if (auto item = ui->wgtModules->findItem(displayName))
    {
        // выделить элемент
        item->setSelected(true);
        // прокрутить список до выделенного элемента
        ui->wgtModules->scrollListToItem(item);
    }
}


void ModulesTabWidget::processModulesRemoveBtnClicked()
{
    if (!m_object)
        return;

    if (!ui->wgtModules->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить модуль из системы,"
                                "т.к. не выбрано имя модуля"));
        return;
    }
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                tr("Вы действительно хотите удалить модуль <%1>?")
                .arg(ui->wgtModules->selectedItemName())))
        return;
    // получить выбранный элемент визуального списка
    auto item = ui->wgtModules->selectedItem();
    // удалить из списка модулей системы
    m_object->removeModule(item->text());
    // удалить и уничтожить элемент визуального списка
    ui->wgtModules->removeAndDeleteItem(item);
}


void ModulesTabWidget::processBtnSaveModuleSettingsClicked()
{
    if (!m_object)
        return;
    // получить выбранные модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    // запомнить имя модуля
    QString oldName = selectedModule()->displayName();
    //
    // установить новые характеристики модулю:
    //
    // уникальный ID
    module->setUniqueId(ui->sbModuleUniqueId->value());
    // отображаемый ID
    module->setDisplayId(ui->sbModuleDisplayId->value());
    // ip-адрес
    module->setIp(QHostAddress(ui->leModuleIp->text()));
    // порт
    module->setPort(ui->sbModulePort->value());
    //
    // конфигурация модуля
    module->setModuleConfigName(ui->cbModuleConfigs->currentText());
    //
    // периферия:
    //

//    // камеры
//    module->setBoundCameras(ui->wgtCameras->boundCameras());

    // табло
    module->setBoardEnabled(ui->gbBoard->isChecked(),
                            QHostAddress(ui->leBoardIp->text()),
                            ui->sbBoardPort->value());

    // здание и этаж
    int buildNum = ui->gbModuleLocation->buildNumber();
    int floorNum = ui->gbModuleLocation->floorNumber();

    // если здание или этаж изменились
    if ((module->buildNum() != buildNum) || (module->floorNum() != floorNum))
    {
        // запомнить старое имя модуля
        QString oldDisplayName = module->displayName();
        // изменить расположение модуля
        m_object->changeModuleLocation(module, buildNum, floorNum);
        // сообщить об изменении имени модуля
        QMessageBox::information(this, tr("Информация"),
                                 tr("Имя модуля изменено с <%1> на <%2>")
                                    .arg(oldDisplayName)
                                    .arg(module->displayName()));
    }
    // обновить отображаемое имя модуля в списке
    ui->wgtModules->changeItemName(oldName, module->displayName());
    //
    // известить об изменении настроек модуля
    emit moduleSettingsChanged(module->uniqueId());
    //
    // сообщить об изменении настроек модуля
    QMessageBox::information(this, tr("Информация"),
                             tr("Настройки модуля <%1> сохранены")
                             .arg(module->displayName()));
}


void ModulesTabWidget::processModulesSelectionChanged()
{
    if (!m_object)
        return;

    int uniqeId = 0;
    int displayId = 0;
    QString ip = "";
    quint16 port = 0;
    QString moduleCfgName = "";
//    QMap<IssoParamId, int> boundCameras;
    //
    bool boardEnabled = false;
    QString boardIp = "";
    quint16 boardPort = 0;
    int buildNum = 0;
    int floorNum = 0;

    // получить выбранные модуль
    IssoModule* module = selectedModule();
    // если модуль не выбран, обнулить поля
    if (module)
    {
        // заполнить характеристики модуля:
        // уникальный ID
        uniqeId = module->uniqueId();
        // отображаемый ID
        displayId = module->displayId();
        // ip-адрес
        ip = module->ip().toString();
        // порт
        port = module->port();
        //
        // конфигурация модуля
        moduleCfgName = module->moduleConfigName();
        //
        // периферия:
        //
//        // камеры
//        boundCameras = module->boundCameras();

        // табло
        boardEnabled = module->getBoardEnabled();
        boardIp = module->getBoardIp().toString();
        boardPort = module->getBoardPort();
        // здание и этаж
        buildNum = module->buildNum();
        floorNum = module->floorNum();
    }
    //
    // вывести характеристики выбранного модуля:
    //
    // уникальный ID
    ui->sbModuleUniqueId->setValue(uniqeId);
    // отображаемый ID
    ui->sbModuleDisplayId->setValue(displayId);
    // ip-адрес
    ui->leModuleIp->setText(ip);
    // порт
    ui->sbModulePort->setValue(port);
    //
    // конфигурация модуля
    int cfgIndex = ui->cbModuleConfigs->findText(moduleCfgName);
    ui->cbModuleConfigs->setCurrentIndex(cfgIndex);

//    //
//    // камеры
//    ui->wgtCameras->setBoundCameras(boundCameras);

    //
    // табло
    ui->gbBoard->setChecked(boardEnabled);
    ui->leBoardIp->setText(boardIp);
    ui->sbBoardPort->setValue(boardPort);
    //
    // здание
    ui->gbModuleLocation->setBuildNumber(buildNum);
    // этаж
    ui->gbModuleLocation->setFloorNumber(floorNum);
    // задать активность кнопки "Активировать (модули)"
    ui->btnActivateModule->setEnabled((module) && (!module->isActivated()));
    // задать активность кнопки "Изменить настройки"
    ui->btnSetupDescriptor->setEnabled((module) && (module->isActivated()));
    // задать активность кнопки "Активировать (мультидатчики)"
    ui->btnMsActivateAll->setEnabled((module) && (module->isActivated()));
    //
    // обновить список мультидатчиков выбранного модуля
    updateMultiSensorList();
}


void ModulesTabWidget::processBuildFilterIndexChanged(int index)
{
    if (!m_object)
        return;
    // очистить список этажей
    ui->cbFilterFloor->clear();
    // добавить строку "Не выбран"
    ui->cbFilterFloor->addItem(tr("Не выбран"), QVariant(true));
    // если здание не выбрано, то этаж тоже не выбран
    if (index == -1)
    {
        ui->cbFilterFloor->setCurrentIndex(0);
        return;
    }
    // получить имя здания
    QString buildName = ui->cbFilterBuilding->itemText(index);
    // получить объект здания
    IssoBuilding* build = m_object->findBuildingByName(buildName);
    if (build)
    {
        // заполнить фильтр этажей здания с признаком активации
        foreach (IssoFloorPlan* floor, build->floors())
            ui->cbFilterFloor->addItem(floor->displayName(), QVariant(floor->activated()));
    }
}


void ModulesTabWidget::processBtnActivateModuleClicked()
{
    if (!m_object)
        return;
    // получить выбранные модуль
    IssoModule* module = selectedModule();
    if (!module)
        return;
    //
    // снять обработчики извещения о результате записи дескриптора
    disconnect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)), 0, 0);
    //
    // проверить пароль
    if (ui->leActivationPassword->text() != IssoConst::MODIFY_PASSWORD)
    {
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Введен неверный пароль! Активация отменена"));
        return;
    }
    //
    // задать обработчики извещения о результате записи дескриптора
    connect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)),
            this, SLOT(processActivationSucceed(QString,IssoModuleDescriptor)));
    connect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)),
            this, SLOT(processActivationFailed(QString,IssoCmdId)));
    //
    // запросить активацию (запись дескриптора)
    emit writeDescriptorRequested(module->uniqueId(), module->getDescriptor());
}


void ModulesTabWidget::processActivationSucceed(const QString &moduleName,
                                                const IssoModuleDescriptor &desc)
{
    Q_UNUSED(desc);

    if (!m_object)
        return;
    //
    // найти выьбанный модуль
    IssoModule* module = m_object->findModuleByDisplayName(moduleName);
    if (!module)
        return;
    QString s = tr("Модуль <%1> активирован!").arg(moduleName);
    // установить признак активированности
    module->setActivated(true);
    // отобразить сообщение об успешной активации
    QMessageBox::information(this, tr("Информация"), tr(s.toUtf8().data()));
    // обновить визуальный фильтрованный список модулей
    updateFilteredModulesList();
    //
    // снять обработчики извещения о результате записи дескриптора
    disconnect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)), 0, 0);
}


void ModulesTabWidget::processActivationFailed(const QString &moduleName, IssoCmdId cmdId)
{
    Q_UNUSED(moduleName);
    Q_UNUSED(cmdId);
    //
    // отобразить сообщение об ошибке активации
    QMessageBox::critical(this, tr("Ошибка"), tr("Модуль не активирован!"));
    //
    // снять обработчики извещения о результате записи дескриптора
    disconnect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)), 0, 0);
}


void ModulesTabWidget::processBtnSetupDescriptorClicked()
{
    if (!m_object)
        return;
    //
    // отключить все сигналы, связанные с дескриптором
    disconnect(this, SIGNAL(descriptorRead(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)), 0, 0);
    //
    // создать диалог конфигурирования дескриптора
    DescriptorSetupDialog dlg(m_object->modules());
    // выбрать модуль
    if (ui->wgtModules->hasSelectedItems())
    {
        dlg.setCurrentModule(ui->wgtModules->selectedItemName());
    }
    //
    // связать сигнал диалога с методом отправки запроса на чтение дескриптора
    connect(&dlg, SIGNAL(descriptorRequested(int)),
            this, SIGNAL(readDescriptorRequested(int)));
    // связать сигнал диалога с методом отправки запроса на запись дескриптора
    connect(&dlg, SIGNAL(descriptorChanged(int,IssoModuleDescriptor)),
            this, SIGNAL(writeDescriptorRequested(int,IssoModuleDescriptor)));

    // связать сигнал чтении дескриптора с обработчком в диалоге
    connect(this, SIGNAL(descriptorRead(QString,IssoModuleDescriptor)),
            &dlg, SLOT(processDescriptorRead(QString,IssoModuleDescriptor)));
    // связать сигнал записи дескриптора с обработчком в диалоге
    connect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)),
            &dlg, SLOT(processDescriptorWritten(QString,IssoModuleDescriptor)));
    // связать сигнал ошибки чтения / записи дескриптора с обработчком в диалоге
    connect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)),
            &dlg, SLOT(processDescriptorFailed(QString,IssoCmdId)));
    //
    // показать диалог
    dlg.exec();
    // обновить отображаемые настройки модуля
    processModulesSelectionChanged();
    //
    // отключить все сигналы, связанные с дескриптором
    disconnect(this, SIGNAL(descriptorRead(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)), 0, 0);
    disconnect(this, SIGNAL(descriptorFailed(QString,IssoCmdId)), 0, 0);
}
