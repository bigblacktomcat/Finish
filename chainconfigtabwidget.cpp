#include "chainconfigtabwidget.h"
#include "newchainconfigdialog.h"
#include "ui_chainconfigtabwidget.h"
#include "issomsutils.h"



ChainConfigTabWidget::ChainConfigTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChainConfigTabWidget)
{
    ui->setupUi(this);
    setupWidget();
    initWidgetHandlers();
}


ChainConfigTabWidget::~ChainConfigTabWidget()
{
    delete ui;
}


void ChainConfigTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
    // если объект пуст, выход
    if (!m_object)
        return;
}


void ChainConfigTabWidget::updateTab()
{
    // обновить список конфигураций шлейфов
    ui->wgtChainConfigs->clearList();
    ui->wgtChainConfigs->addItems(m_object->chainConfigNames());
    ui->wgtChainConfigs->setItemSelected(0, true);
    // обновить список камер
    updateCameraList();
}


void ChainConfigTabWidget::setupWidget()
{
    // убрать заголовок настроек аналогового шлейфа
    ui->wgtAnalogChainSetup->setTitle("");
    // убрать галочку
    ui->wgtAnalogChainSetup->setCheckable(false);
    //
    // сформировать группу радиокнопок
    QButtonGroup* radioGroup = new QButtonGroup(this);
    radioGroup->addButton(ui->rbtnSmoke, SMOKE_CHAIN);
    radioGroup->addButton(ui->rbtnMulticriterial, MULTICRITERIAL);
    radioGroup->addButton(ui->rbtnAlarmButton, ALARM_BUTTON_CHAIN);
    radioGroup->addButton(ui->rbtnMoveDetector, MOVE_DETECTOR_CHAIN);
    radioGroup->addButton(ui->rbtnTemp, TEMP_CHAIN);
    // выбрать тип
//    ui->rbtnSmoke->setChecked(true);
    //
    // список конфигураций шлейфов:
    //
    // задать заголовок
    ui->wgtChainConfigs->setTitle(tr("Конфигурации шлейфов"));
    // скрыть кнопку редактирования
    ui->wgtChainConfigs->setEditBtnVisible(false);
    //
    // список мультидатчиков:
    //
    // задать заголовок
    ui->wgtMultisensorConfigs->setTitle(tr("Мультидатчики"));
    // скрыть кнопку редактирования
    ui->wgtMultisensorConfigs->setEditBtnVisible(false);
    //
    // инициализировать панели настройки встроенных датчиков мультидатчика
    initDigitalSensorSetupPanels();

    initMsTypes();
}


void ChainConfigTabWidget::initWidgetHandlers()
{
    // добавить конфигурацию шлейфа
    connect(ui->wgtChainConfigs, SIGNAL(addBtnClicked(bool)),
            SLOT(processAddChainConfigBtnClicked()));
    // удалить конфигурацию шлейфа
    connect(ui->wgtChainConfigs, SIGNAL(removeBtnClicked(bool)),
            SLOT(processRemoveChainConfigBtnClicked()));

    // добавить конфигурацию мультидатчика
    connect(ui->wgtMultisensorConfigs, SIGNAL(addBtnClicked(bool)),
            SLOT(processAddMultisensorConfigBtnClicked()));
    // удалить конфигурацию мультидатчика
    connect(ui->wgtMultisensorConfigs, SIGNAL(removeBtnClicked(bool)),
            SLOT(processRemoveMultisensorConfigBtnClicked()));

    // отобразить настройки выбранного конфига шлейфа
    connect(ui->wgtChainConfigs, SIGNAL(selectionChanged()),
            SLOT(processChainConfigsSelectionChanged()));

    // отобразить настройки выбранного конфига мультидатчика
    connect(ui->wgtMultisensorConfigs, SIGNAL(selectionChanged()),
            SLOT(processMultisensorConfigsSelectionChanged()));

    // сохранить настройки аналогового шлейфа
    connect(ui->btnSaveAnalogChainSettings, SIGNAL(clicked(bool)),
            SLOT(processSaveAnalogChainSettingsBtnClicked()));
    // сохранить настройки цифрового шлейфа
    connect(ui->btnSaveDigitalChainSettings, SIGNAL(clicked(bool)),
            SLOT(processSaveAddressChainSettingsBtnClicked()));

    // переключить тип шлейфа
    connect(ui->rbtnSmoke->group(), QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
            [=](int id, bool checked)
            {
//                if (checked)
//                    ui->stwgtChainSettings->setCurrentIndex(id);
                if (!checked)
                    return;
                if (id == MULTICRITERIAL)
                    ui->stwgtChainSettings->setCurrentWidget(ui->pageDigitalChainSettings);
                else
                    ui->stwgtChainSettings->setCurrentWidget(ui->pageAnalogChainSettings);
    });
}


void ChainConfigTabWidget::initMsTypes()
{
    // заполнить список типов МД
    ui->cbMsType->addItems(IssoMsUtils::msTypes.values());
    // обработчики
    connect(ui->cbMsType, SIGNAL(currentIndexChanged(int)),
            this,         SLOT(processMsTypeChanged(int)));
}


void ChainConfigTabWidget::initDigitalSensorSetupPanels()
{
    // заполнить карту виджетов настройки мультидатчиков
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_SMOKE_O, ui->wgtSmokeOptical);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_TEMP_A, ui->wgtTempAnalog);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_TEMP_D, ui->wgtTempDigital);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_SMOKE_E, ui->wgtSmokeElectrochemical);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_FLAME_MIN_VAL, ui->wgtFlameMinValue);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_FLAME_STD_DEV, ui->wgtFlameStdDev);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_CO, ui->wgtCO);
    m_digitalSensorSetupPanels.insert(DIGITALSENSOR_VOC, ui->wgtFlyMatter);
    // настроить панели
    foreach (IssoDigitalSensorId id, m_digitalSensorSetupPanels.keys())
    {
        // найти панель по ID датчика
        IssoDigitalSensorSetupPanel* panel = m_digitalSensorSetupPanels.value(id);
        if (panel)
        {
            // задать заголовок
            panel->setTitle(IssoCommonData::stringByDigitalSensorId(id));
        }
    }
    // настроить поведение панелей
    //
    // при расширенные настройки Дым(о) доступны, если Дым(о) включен
    connect(ui->wgtSmokeOptical,   SIGNAL(toggled(bool)),
            ui->gbSmokeOpticalExt, SLOT(setEnabled(bool)));
    // влючение / отключение панелей Пламя (мин)и Пламя (сред) происходит одновременно
    connect(ui->wgtFlameMinValue, SIGNAL(clicked(bool)),
            ui->wgtFlameStdDev,   SLOT(setChecked(bool)));
    connect(ui->wgtFlameStdDev,   SIGNAL(clicked(bool)),
            ui->wgtFlameMinValue, SLOT(setChecked(bool)));
}


IssoChainConfig *ChainConfigTabWidget::selectedChainConfig()
{
    if (!m_object)
        return nullptr;
    // получить выбранный элемент списка
    QListWidgetItem* selectedItem = ui->wgtChainConfigs->selectedItem();
    // если имя конфигурации шлейфа выбрано, вернуть указатель на конфигурацию
    return (selectedItem ? m_object->chainConfig(selectedItem->text()) : nullptr);

//    if (!selectedItem)
//        return nullptr;
//    // получить имя выбранной конфигурации шлейфа
//    QString chainCfgName = ui->wgtChainConfigs->list()->selectedItems().first()->text();
//    // найти конфигурацию в коллекции объекта
//    return m_object->chainConfig(chainCfgName);
}


IssoMultiSensorConfig *ChainConfigTabWidget::selectedMultiSensorConfig()
{
    if (!m_object)
        return nullptr;
    // если имя конфигурации мультидатчика не выбрано, выход
    if (!ui->wgtMultisensorConfigs->hasSelectedItems())
//    if (ui->wgtMultisensorConfigs->list()->selectedItems().isEmpty())
        return nullptr;
    // найти выбранную конфигурацию шлейфа
    IssoChainConfig* chainCfg = selectedChainConfig();
    // если конфиг не найден, либо тип конфига не цифровой, выход
    if ((!chainCfg) || (chainCfg->type() != MULTICRITERIAL))
        return nullptr;
    // привести к типу конфига адресного шлейфа
    IssoAddressChainConfig* addrChainCfg = dynamic_cast<IssoAddressChainConfig*>(chainCfg);
    if (!addrChainCfg)
        return nullptr;
    // получить имя выбранной конфигурации мультидатчика
    QString multisensorCfgName =
            ui->wgtMultisensorConfigs->selectedItemName();
//            ui->wgtMultisensorConfigs->list()->selectedItems().first()->text();
    // найти конфиг мультидатчика по имени
    return addrChainCfg->multiSensorConfig(multisensorCfgName);
}


void ChainConfigTabWidget::displayAnalogChainConfig(IssoAnalogChainConfig *cfg)
{
    // если конфиг выбран
    if (cfg)
    {
        // вывести количество датчиков аналогового шлейфа
        ui->sbSensorCount->setValue(cfg->sensorCount());
        // вывести диапазоны состояний аналогового шлейфа
        ui->wgtAnalogChainSetup->setStateRanges(cfg->stateRanges());
    }
}


void ChainConfigTabWidget::displayDigitalChainConfig(IssoAddressChainConfig *cfg)
{
    // если конфиг выбран
    if (cfg)
    {
        // вывести все конфиги мультидатчиков данного цифрового шлейфа
        ui->wgtMultisensorConfigs->setItems(cfg->multiSensorConfigNames());

        // выбрать первый элемент
        if (ui->wgtMultisensorConfigs->itemCount() > 0)
            ui->wgtMultisensorConfigs->item(0)->setSelected(true);
    }
}


void ChainConfigTabWidget::clearChainConfig()
{
    switch(ui->stwgtChainSettings->currentIndex())
    {
        // настройки аналогового шлейфа
        case 0:
        {
            // обнулить диапазоны значений состояний
            ui->wgtAnalogChainSetup->clearStateRanges();
            break;
        }
        // настройки цифрового шлейфа
        case 1:
        {
            // очистить список конфигов мультидатчиков
            ui->wgtMultisensorConfigs->clearList();
            break;
        }
        default:
            break;
    }
}



void ChainConfigTabWidget::selectChainSettingsWidget(IssoChainType type)
{
    // отобразить тип шлейфа
    ui->rbtnSmoke->group()->button(type)->setChecked(true);
}


void ChainConfigTabWidget::updateCameraList()
{
    if (!m_object)
        return;
    // очистить список ассоциированных камер
    ui->cbCamera->clear();
    // заполнить список ассоциированных камер
    foreach (IssoCamera* camera, m_object->cameras())
    {
        // добавить имя камеры и ID
        ui->cbCamera->addItem(camera->displayName(), QVariant(camera->id()));
    }
}


void ChainConfigTabWidget::getDigitalSensorSettings(const IssoDigitalSensorId &id,
//                                                    int &normalValue,
                                                    int &warningValue,
                                                    int &alarmValue,
                                                    IssoStateCondition &condition,
                                                    bool &checked)
{
    if (!m_digitalSensorSetupPanels.contains(id))
        return;
    // получить панель
    IssoDigitalSensorSetupPanel* panel = m_digitalSensorSetupPanels.value(id);
    // получить пороговые значения состояний
//    normalValue = panel->normalValue();
    warningValue = panel->warningValue();
    alarmValue = panel->alarmValue();
    // задать условие сработки
    condition = panel->condition();
    // статус подключения
    checked = panel->isChecked();
}


void ChainConfigTabWidget::setDigitalSensorSettings(const IssoDigitalSensorId& id,
//                                                    int normalValue,
                                                    int warningValue,
                                                    int alarmValue,
                                                    IssoStateCondition condition,
                                                    bool checked)
{
    if (!m_digitalSensorSetupPanels.contains(id))
        return;
    // получить панель
    IssoDigitalSensorSetupPanel* panel = m_digitalSensorSetupPanels.value(id);
    // задать пороговые значения состояний
//    panel->setNormalValue(normalValue);
    panel->setWarningValue(warningValue);
    panel->setAlarmValue(alarmValue);
    // задать условие сработки
    panel->setCondition(condition);
    // задать статус подключения
    panel->setChecked(checked);
}


void ChainConfigTabWidget::displayMultiSensorConfig(IssoMultiSensorConfig *cfg)
{
    if (!cfg)
        return;
//    // отобразить ID мультидатчика
//    ui->sbMultiSensorId->setValue(cfg->id());
    // отобразить настройки каждого цифрового датчика
    foreach (/*IssoDigitalSensorConfig**/auto digitalSensorCfg, cfg->digitalSensorConfigs()/*.values()*/)
    {
        // получить ID цифрового датчика
        IssoDigitalSensorId id = digitalSensorCfg->id();
        // получить состояния цифрового датчика
//        int normalValue = digitalSensorCfg.stateValue(STATE_NORMAL);
        int warningValue = digitalSensorCfg->stateValue(STATE_WARNING);
        int alarmValue = digitalSensorCfg->stateValue(STATE_ALARM);
        // получить условие сработки цифрового датчика
        IssoStateCondition condition = digitalSensorCfg->condition();
        // получить статус подключения датчика
        bool checked = digitalSensorCfg->enabled();
        // отобразить ID, значения состояний цифрового датчика, условие сработки и статус подключения
        setDigitalSensorSettings(id, /*normalValue,*/ warningValue, alarmValue, condition, checked);
    }
    //
    // панели Пламя (сред) и Пламя (мин) имеют одинаковый статус подключения
    auto panelFlameStdDev = m_digitalSensorSetupPanels.value(DIGITALSENSOR_FLAME_STD_DEV);
    auto panelFlameMinVal = m_digitalSensorSetupPanels.value(DIGITALSENSOR_FLAME_MIN_VAL);
    panelFlameMinVal->setEnabled(panelFlameStdDev->isEnabled());
    //
    // отобразить расширенные значения Дым(о)
    ui->sbSmokeOptSens->setValue(cfg->smokeOptSens());
    ui->sbSmokeOptBright->setValue(cfg->smokeOptBright());
    //
    // отобразить ID камеры
    int index = ui->cbCamera->findData(cfg->cameraId());
    ui->cbCamera->setCurrentIndex(index);
    ui->chkCameraEnabled->setChecked(index != -1);
    //
    // отобразить описание месторасположения
    ui->leLocationDesc->setText(cfg->locationDesc());
    //
    // отобразить тип МД
    ui->cbMsType->setCurrentIndex(cfg->getMsType());
}


void ChainConfigTabWidget::updateMultiSensorConfig(IssoMultiSensorConfig *cfg)
{
    if (!cfg)
        return;
    // сохранить настройки каждого цифрового датчика
    foreach (IssoDigitalSensorId id, cfg->digitalSensorConfigs().keys())
    {
        // состояния цифрового датчика
        int /*normalValue,*/ warningValue, alarmValue;
        // условие сработки цифрового датчика
        IssoStateCondition condition;
        // статус подключения датчика
        bool checked;
        // получить ID, значения состояний цифрового датчика, условие сработки и статус подключения
        getDigitalSensorSettings(id, /*normalValue,*/ warningValue, alarmValue, condition, checked);
        //

//        // создать новый конфиг цифрового датчика
//        IssoDigitalSensorConfig newDigitalSensorCfg(id);
//        // задать значения состояний
//        newDigitalSensorCfg.setStateValues(/*normalValue,*/ warningValue, alarmValue);
//        // задать условие сработки
//        newDigitalSensorCfg.setCondition(condition);
//        // задать статус подключения
//        newDigitalSensorCfg.setEnabled(checked);
//        //
//        // обновить конфиг встроенного датчика в коллекции мультидатчика
//        cfg->insertDigitalSensorConfig(newDigitalSensorCfg);

        // получить конфиг встроенного датчика
        auto digitalCfg = cfg->digitalSensorConfig(id);
        // обновить конфиг встроенного датчика
        if (digitalCfg)
        {
            // задать значения состояний
            digitalCfg->setStateValues(warningValue, alarmValue);
            // задать условие сработки
            digitalCfg->setCondition(condition);
            // задать статус подключения
            digitalCfg->setEnabled(checked);
        }
    }
    //
    // обновить расширенные значения Дым(о)
    cfg->setSmokeOptSens(ui->sbSmokeOptSens->value());
    cfg->setSmokeOptBright(ui->sbSmokeOptBright->value());
    //
    // обновить ID камеры
    int cameraId = -1;
    if (ui->chkCameraEnabled->isChecked() && (ui->cbCamera->currentIndex() != -1))
    {
        QVariant data = ui->cbCamera->currentData();
        if (data.isValid())
            cameraId = data.toInt();
    }
    cfg->setCameraId(cameraId);
    //
    // обновить описание месторасположения
    cfg->setLocationDesc(ui->leLocationDesc->text());
    //
    // обновить тип МД
    IssoMsType msType = static_cast<IssoMsType>(ui->cbMsType->currentIndex());
    cfg->setMsType(msType);
}


void ChainConfigTabWidget::processAddChainConfigBtnClicked()
{
    if (!m_object)
        return;

    // вывести диалог создания конфигурации шлейфа
    NewChainConfigDialog dlg(this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;
    // если имя конфигурации не задано, выход
    if (dlg.name().isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить конфигурацию шлейфа, "
                                "т.к. не задано имя конфигурации"));
        return;
    }
    // если имя конфигурации уже существует, выход
    if (m_object->chainConfig(dlg.name()))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить конфигурацию шлейфа, "
                                "т.к. конфигурация с именем <%1> уже существует")
                                .arg(dlg.name()));
        return;
    }
    // создать конфигурацию
    IssoChainConfig* cfg = IssoChainConfigFactory::make(dlg.chainType(), dlg.name(),
                                                        dlg.sensorCount(), true);
    // если конфиг неадресного шлейфа
    auto analogCfg = dynamic_cast<IssoAnalogChainConfig*>(cfg);
    if (analogCfg)
    {
        // задать коллекцию состояний шлейфа
        analogCfg->initStates(ui->wgtAnalogChainSetup->displayingStates());
    }

//    // если конфиг неадресного шлейфа
//    switch (cfg->type())
//    {
//        case SMOKE_CHAIN:
//        case TEMP_CHAIN:
//        case ALARM_BUTTON_CHAIN:
//        case MOVE_DETECTOR_CHAIN:
//        {
//            IssoAnalogChainConfig* analogCfg = dynamic_cast<IssoAnalogChainConfig*>(cfg);
//            if (analogCfg)
//            {
//                // задать коллекцию состояний шлейфа
//                analogCfg->initStates(ui->wgtAnalogChainSetup->displayingStates());
//            }
//            break;
//        }
//        default:
//            break;
//    }
    //
    // добавить в коллекцию конфигураций объекта
    m_object->insertChainConfig(cfg);
    // добавить в визуальный список
    QListWidgetItem* item = new QListWidgetItem(cfg->name());
    ui->wgtChainConfigs->addItem(item); //list()->addItem(item);
    // выбрать ноую конфигурацию в списке
    item->setSelected(true);
}


void ChainConfigTabWidget::processRemoveChainConfigBtnClicked()
{
    if (!m_object)
        return;

    // получить выделенный элемент списка
    QListWidgetItem* item = ui->wgtChainConfigs->selectedItem();
    // проверить выбрано ли имя конфигурации
    if (!item)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить конфигурацию шлейфа, "
                                "т.к. не выбрано имя конфигурации"));
        return;
    }
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                tr("Вы действительно хотите удалить конфигурацию шлейфа <%1>?")
                    .arg(item->text())))
        return;
    // удалить конфигурацию шлейфа
    m_object->removeChainConfig(item->text());

    // применить изменения конфига шлейфа
    m_object->applyChainConfigChanges(item->text());

    // удалить элемент из визуального списка
    ui->wgtChainConfigs->removeItem(item->text());
}


void ChainConfigTabWidget::processAddMultisensorConfigBtnClicked()
{
    if (!m_object)
        return;

    // получить выбранную конфигурацию шлейфа
    IssoChainConfig* chainCfg = selectedChainConfig();
    // привести к типу конфига адресного шлейфа
    IssoAddressChainConfig* addrChainCfg = dynamic_cast<IssoAddressChainConfig*>(chainCfg);
    // если конфиг не найден, либо тип конфига не цифровой, выход
    if (!addrChainCfg)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить конфигурацию мультидатчика, "
                                "т.к. не выбрана конфигурация адресного шлейфа"));
        return;
    }
    // рассчитать количество свободных ID мультидатчиков
    int freeIdCount = (int)IssoAddressChainConfig::MAX_MULTISENSOR_ID -
                                    addrChainCfg->multiSensorConfigCount();
    // убедиться, что лимит количества не достигнут
    if (freeIdCount <= 0)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить конфигурацию мультидатчика, "
                                "т.к. достигнут лимит количества мультидатчиков на шлейфе"));
        return;
    }
    // запросить количество добавляемых конфигов мультидатчиков
    bool ok;
    int multiCfgCount = IssoCommonData::showInputNumberDialog(this,
                                          tr("Введите количество мультидатчиков"),
                                          tr("Количество мультидатчиков:"), ok,
                                          1, freeIdCount, 1);
    if (!ok)
        return;
    // добавить конфиги мультидатчиков
    addrChainCfg->addNewMultiSensorConfigs(multiCfgCount);
    // обновить список конфигов мультидатчиков
    ui->wgtMultisensorConfigs->setItems(addrChainCfg->multiSensorConfigNames());
    //
    // применить изменения конфига шлейфа (на графплане)
    m_object->applyChainConfigChanges(addrChainCfg->name());
}


void ChainConfigTabWidget::processRemoveMultisensorConfigBtnClicked()
{
    if (!m_object)
        return;

    // проверить выбрано ли имя конфигурации
    QListWidgetItem* item = ui->wgtMultisensorConfigs->selectedItem();
    if (!item)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить конфигурацию, т.к. не выбрано имя конфигурации"));
        return;
    }
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                tr("Вы действительно хотите удалить конфигурацию мультидатчика <%1>?")
                    .arg(item->text())))
        return;
    // получить выбранную конфигурацию цифрового шлейфа
    IssoChainConfig* chainCfg = selectedChainConfig();
    // если конфиг не найден, либо тип конфига не цифровой, выход
    if ((!chainCfg) || (chainCfg->type() != MULTICRITERIAL))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить конфигурацию мультидатчика, "
                                "т.к. не выбрана конфигурация цифрового шлейфа"));
        return;
    }
    // привести к типу цифрового конфига
    IssoAddressChainConfig* addrChainCfg = dynamic_cast<IssoAddressChainConfig*>(chainCfg);
    if (!addrChainCfg)
        return;
    //
    // удалить конфигурацию мультидатчика
    addrChainCfg->removeMultiSensorConfig(item->text());
    // удалить элемент из визуального списка
    ui->wgtMultisensorConfigs->removeItem(item->text());
    //
    // применить изменения конфига шлейфа
    m_object->applyChainConfigChanges(addrChainCfg->name());
}


void ChainConfigTabWidget::processChainConfigsSelectionChanged()
{
    // получить выбранный конфиг шлейфа
    IssoChainConfig* chainCfg = selectedChainConfig();
    if ((!m_object) || (!chainCfg))
    {
        // очистить настройки шлейфа
        clearChainConfig();
        return;
    }
    // отобразить панель настроек, соответствующую типу шлефа
    selectChainSettingsWidget(chainCfg->type());
    // вывести настройки выбранного конфига
    switch(chainCfg->type())
    {
        case SMOKE_CHAIN:
        case TEMP_CHAIN:
        case ALARM_BUTTON_CHAIN:
        case MOVE_DETECTOR_CHAIN:
        {
            displayAnalogChainConfig(dynamic_cast<IssoAnalogChainConfig*>(chainCfg));
            break;
        }
        case MULTICRITERIAL:
        {
            displayDigitalChainConfig(dynamic_cast<IssoAddressChainConfig*>(chainCfg));
            break;
        }
        default:
            break;
    }
}


void ChainConfigTabWidget::processMultisensorConfigsSelectionChanged()
{
    // получить выбранный конфиг мультидатчика
    IssoMultiSensorConfig* cfg = selectedMultiSensorConfig();
    if (cfg)
    {
        displayMultiSensorConfig(cfg);
    }
    else
    {
        IssoMultiSensorConfig* defaultCfg = new IssoMultiSensorConfig();
        displayMultiSensorConfig(defaultCfg);
        delete defaultCfg;
    }
}


void ChainConfigTabWidget::processSaveAnalogChainSettingsBtnClicked()
{
    // получить выбранный конфиг шлейфа
    IssoChainConfig* chainCfg = selectedChainConfig();
    // если конфиг не выбран или не является аналоговым, выход
    if ((!chainCfg) /*|| (chainCfg->type() != ANALOG)*/)
        return;
    // привести к аналоговому типу
    IssoAnalogChainConfig* analogCfg = dynamic_cast<IssoAnalogChainConfig*>(chainCfg);
    if (!analogCfg)
        return;
    // задать конфигу аналогового шлейфа новые настройки
    analogCfg->setSensorCount(ui->sbSensorCount->value());
    analogCfg->setStateRanges(ui->wgtAnalogChainSetup->stateRanges());
    // применить отображение конфига шлейфа на графплане
    m_object->applyChainConfigChanges(analogCfg->name());
}


void ChainConfigTabWidget::processSaveAddressChainSettingsBtnClicked()
{
    // получить выбранный конфиг мультидатчика
    IssoMultiSensorConfig* multiSensorCfg = selectedMultiSensorConfig();
    if (multiSensorCfg)
    {
        updateMultiSensorConfig(multiSensorCfg);
        //
        // ВРЕМЕННО
        // применить изменения конфига шлейфа
        m_object->applyChainConfigChanges(selectedChainConfig()->name());
    }
}


void ChainConfigTabWidget::processMsTypeChanged(int index)
{
    using namespace IssoMsUtils;
    // индекс соответствует типу ИПМ
    IssoMsType msType = static_cast<IssoMsType>(index);
    // получить каналы ИПМ
    auto ids = channelIds(msType);
    // задать видимость панелей
    foreach (auto channelId, m_digitalSensorSetupPanels.keys())
    {
        // панель настройки
        auto panel = m_digitalSensorSetupPanels.value(channelId);
        // задать видимость
        panel->setEnabled(ids.contains(channelId));
    }
    // панели Пламя (сред) и Пламя (мин) имеют одинаковое состояние
    auto panelFlameStdDev = m_digitalSensorSetupPanels.value(DIGITALSENSOR_FLAME_STD_DEV);
    auto panelFlameMinVal = m_digitalSensorSetupPanels.value(DIGITALSENSOR_FLAME_MIN_VAL);
    panelFlameMinVal->setEnabled(panelFlameStdDev->isEnabled());
}
