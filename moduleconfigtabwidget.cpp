#include "moduleconfigtabwidget.h"
#include "ui_moduleconfigtabwidget.h"

#include <QMessageBox>


ModuleConfigTabWidget::ModuleConfigTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModuleConfigTabWidget)
{
    ui->setupUi(this);
    setupWidget();
    initWidgetHandlers();
}


ModuleConfigTabWidget::~ModuleConfigTabWidget()
{
    delete ui;
}


void ModuleConfigTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
}


void ModuleConfigTabWidget::updateTab()
{
    if (!m_object)
        return;

    // обновить список конфигураций модулей
    ui->wgtModuleConfigs->setItems(m_object->moduleConfigNames());
    // обновить списки имен конфигураций шлейфов
    foreach (IssoParamId paramId, m_chainLinkPanels.keys())
    {
        // найти панель
        IssoCheckableSelectionPanel* panel = m_chainLinkPanels.value(paramId);
        if (!panel)
            continue;
        // заполнить список имен конфигураций согласно типу шлейфа
        switch(paramId)
        {
            case ANALOG_CHAIN_1:
            case ANALOG_CHAIN_2:
            case ANALOG_CHAIN_3:
            case ANALOG_CHAIN_4:
            case ANALOG_CHAIN_5:
            case ANALOG_CHAIN_6:
            {
                QStringList analogCfgNames;
                analogCfgNames << m_object->chainConfigNames(SMOKE_CHAIN);
                analogCfgNames << m_object->chainConfigNames(TEMP_CHAIN);
                analogCfgNames << m_object->chainConfigNames(ALARM_BUTTON_CHAIN);
                analogCfgNames << m_object->chainConfigNames(MOVE_DETECTOR_CHAIN);
                panel->setItemNames(analogCfgNames);
                break;
            }
            case MULTISENSOR:
            {
                panel->setItemNames(m_object->chainConfigNames(MULTICRITERIAL));
                break;
            }
            default:
                break;
        }
    }
    //
    // обновить списки классов реле
    foreach (IssoParamId paramId, m_relayClassPanels.keys())
    {
        // найти панель
        IssoCheckableSelectionPanel* panel = m_relayClassPanels.value(paramId);
        if (!panel)
            continue;
        // заполнить список классов панели
        panel->setItemNames(m_object->relayClassNames());

    }
    //
    // обновить списки сценариев автоматики
    ui->wgtAlarmScenario->setItemNames(m_object->scenarioNames());
    ui->wgtFailScenario->setItemNames(m_object->scenarioNames());
    //
    // обновить списки классов реле для настройки кнопок
    foreach (QComboBox* combo, m_buttonClassCombos)
    {
        // очистить список
        combo->clear();
        // добавить пустую строку (не выбрано)
        combo->addItem("");
        // добавить фикисрованные функции (Голос)
        combo->addItems(IssoCommonData::fixedModuleBtnFuncStrings());
        // добавить имена классов реле
        combo->addItems(m_object->relayClassNames());
    }
    //
    // отобразить данные выбранного конфига модуля
    processModuleConfigsSelectionChanged();
}


void ModuleConfigTabWidget::displaySensorConfig(IssoParamId sensorId, IssoAnalogChainConfig *cfg)
{
    if (!cfg)
        return;
    // найти панель по ID датчика
    IssoAnalogSensorSetupPanel* panel = m_sensorSetupPanels.value(sensorId, nullptr);
    if (panel)
    {
        // отобразить диапазоны состояний на панели
        panel->setStateRanges(cfg->stateRanges());
        // отобразить статус подключения
        panel->setChecked(cfg->enabled());
    }
}


void ModuleConfigTabWidget::updateSensorConfig(IssoParamId sensorId, IssoAnalogChainConfig *cfg)
{
    if (!cfg)
        return;
    // найти панель по ID датчика
    IssoAnalogSensorSetupPanel* panel = m_sensorSetupPanels.value(sensorId, nullptr);
    if (panel)
    {
        // обновить диапазоны состояний конфигурации
        cfg->setStateRanges(panel->stateRanges());
        // обновить статус подключения
        cfg->setEnabled(panel->isChecked());
    }
}


void ModuleConfigTabWidget::displayAllSensorConfigs(IssoModuleConfig *moduleCfg)
{
    foreach (IssoParamId id, moduleCfg->sensorConfigs().keys())
    {
        // получить конфиг встроенного датчика из конфига модуля
        IssoAnalogChainConfig* sensorCfg = moduleCfg->sensorConfig(id);
        // отобразить конфиг в соответствующей панели
        displaySensorConfig(id, sensorCfg);
    }
}


void ModuleConfigTabWidget::updateAllSensorConfigs(IssoModuleConfig *moduleCfg)
{
    foreach (IssoParamId id, moduleCfg->sensorConfigs().keys())
    {
        // получить конфиг встроенного датчика из конфига модуля
        IssoAnalogChainConfig* cfg = moduleCfg->sensorConfig(id);
        // отобразить конфиг в соответствующей панели
        updateSensorConfig(id, cfg);
    }
}


void ModuleConfigTabWidget::displayAllChainConfigLinks(IssoModuleConfig *moduleCfg)
{
    foreach (IssoParamId id, m_chainLinkPanels.keys())
    {
        // получить ссылку на конфиг шлейфа из конфига модуля
        IssoChainConfigLink link = moduleCfg->chainConfigLink(id);
        // найти панель по ID шлейфа
        IssoCheckableSelectionPanel* panel = m_chainLinkPanels.value(id, nullptr);
        if (panel)
        {
            // отобразить имя конфигурации шлейфа
            panel->setSelectedItemName(link.chainCfgName());
            // отобразить статус подключения
            panel->setChecked(link.enabled());
        }
    }
}


void ModuleConfigTabWidget::updateAllChainConfigLinks(IssoModuleConfig *moduleCfg)
{
    foreach (IssoParamId id, m_chainLinkPanels.keys())
    {
        // найти панель по ID шлейфа
        IssoCheckableSelectionPanel* panel = m_chainLinkPanels.value(id, nullptr);
        if (panel)
        {
            // получить имя конфига шлейфа из панели
            QString chainCfgName = panel->selectedItemName();
            // получить статус подключения
            bool enabled = panel->checked();
            // сохранить имя конфигурации шлейфа
            moduleCfg->insertChainConfigLink(id, chainCfgName, enabled);
        }
    }
}


void ModuleConfigTabWidget::displayAllRelayClassNames(IssoModuleConfig *moduleCfg)
{
    foreach (IssoParamId id, m_relayClassPanels.keys())
    {
        // получить из конфига модуля имя класса по id реле
        QString className = moduleCfg->relayClassName(id);
        // найти панель по ID реле
        IssoCheckableSelectionPanel* panel = m_relayClassPanels.value(id, nullptr);
        if (panel)
        {
            // отобразить имя класса, ассоциированное с реле
            panel->setSelectedItemName(className);
            // отобразить статус задействования связки
            panel->setChecked(!className.isEmpty());
        }
    }
}


void ModuleConfigTabWidget::updateAllRelayClassNames(IssoModuleConfig *moduleCfg)
{
    foreach (IssoParamId id, m_relayClassPanels.keys())
    {
        // найти панель по ID реле
        IssoCheckableSelectionPanel* panel = m_relayClassPanels.value(id, nullptr);
        if (panel)
        {
            // получить имя класса из панели
            QString className = panel->selectedItemName();
            // получить статус задействования связки
            bool enabled = panel->checked();
            //
            // если связка задействована,
            // добавить ее в конфиг модуля удалить ее, иначе удалить
            if (enabled)
                moduleCfg->insertRelayClassLink(id, className);
            else
                moduleCfg->removeRelayClassLink(id);
        }
    }
}


void ModuleConfigTabWidget::displayAllScenarioNames(IssoModuleConfig *moduleCfg)
{
    // для состояния Тревога
    QString scenarioName = moduleCfg->scenarioNameByModuleState(ALARM);
    // отобразить статус привязки
    ui->wgtAlarmScenario->setSelectedItemName(scenarioName);
    // отобразить статус привязки
    ui->wgtAlarmScenario->setChecked(!scenarioName.isEmpty());
    //
    // для состояния Неисправность
    scenarioName = moduleCfg->scenarioNameByModuleState(FAIL);
    // отобразить статус привязки
    ui->wgtFailScenario->setSelectedItemName(scenarioName);
    // отобразить статус привязки
    ui->wgtFailScenario->setChecked(!scenarioName.isEmpty());
}


void ModuleConfigTabWidget::updateAllScenarioNames(IssoModuleConfig *moduleCfg)
{
    // для состояния Тревога
    if (ui->wgtAlarmScenario->checked())
        moduleCfg->insertScenarioLink(ALARM, ui->wgtAlarmScenario->selectedItemName());
    else
        moduleCfg->removeScenarioLink(ALARM);
    //
    // для состояния Неисправность
    if (ui->wgtFailScenario->checked())
        moduleCfg->insertScenarioLink(FAIL, ui->wgtFailScenario->selectedItemName());
    else
        moduleCfg->removeScenarioLink(FAIL);
}


void ModuleConfigTabWidget::displayButtonClasses(IssoModuleConfig *moduleCfg)
{
    foreach (int buttonNum, m_buttonClassCombos.keys())
    {
        // получить комбобокс по номеру кнопки
        QComboBox* combo = m_buttonClassCombos.value(buttonNum);
        if (!combo)
            continue;
        // получить из конфига модуля имя класса, связанного с кнопкой
        QString className = moduleCfg->buttonClassLink(buttonNum);
        // вывести в комбобокс имя класса
        combo->setCurrentText(className);
    }
}


void ModuleConfigTabWidget::updateButtonClasses(IssoModuleConfig *moduleCfg)
{
    foreach (int buttonNum, m_buttonClassCombos.keys())
    {
        // получить комбобокс по номеру кнопки
        QComboBox* combo = m_buttonClassCombos.value(buttonNum);
        if (!combo)
            continue;

        // получить выбранное имя класса
        QString className = combo->currentText();
        // если имя класса не выбрано,
        // удалить из карты соответствия конфига молуля, иначе добавить
        if (className.isEmpty())
            moduleCfg->removeButtonClassLink(buttonNum);
        else
            moduleCfg->insertButtonClassLink(buttonNum, className);
    }
}


void ModuleConfigTabWidget::resizeEvent(QResizeEvent *event)
{
    rearrangeWidgets();
    QWidget::resizeEvent(event);
}


void ModuleConfigTabWidget::setupWidget()
{
    // список конфигураций модулей:
    //
    // задать заголовок
    ui->wgtModuleConfigs->setTitle(tr("Конфигурации модулей"));
    // скрыть кнопку редактирования
    ui->wgtModuleConfigs->setEditBtnVisible(false);
    //
    initSensorSetupPanels();
    // настроить панели настройки шлейфов
    initChainLinkPanels();
    // настроить панели привязки сценариев к событиям
    initScenarioPanels();
    // настроить панели привязки классов к реле
    initRelayClassPanels();

    initButtonClassCombos();
}


void ModuleConfigTabWidget::initWidgetHandlers()
{
    // добавить конфигурацию модуля
    connect(ui->wgtModuleConfigs, SIGNAL(addBtnClicked(bool)),
            SLOT(processAddModuleConfigBtnClicked()));
    // удалить конфигурацию модуля
    connect(ui->wgtModuleConfigs, SIGNAL(removeBtnClicked(bool)),
            SLOT(processRemoveModuleConfigBtnClicked()));

    // отобразить настройки выбранного конфига модуля
    connect(ui->wgtModuleConfigs, SIGNAL(selectionChanged()),
            SLOT(processModuleConfigsSelectionChanged()));

    // сохранить настройки модуля
    connect(ui->btnSaveSettings, SIGNAL(clicked(bool)),
            SLOT(processSaveSettingsBtnClicked()));
}


void ModuleConfigTabWidget::rearrangeWidgets()
{
//    // получить все чекбоксы виджета
//    QList<QCheckBox*> checkboxes = ui->wgtLinks->findChildren<QCheckBox*>();
//    // определить максимальную ширину чекбокса
//    int maxWidth = 0;
//    foreach (auto checkbox, checkboxes)
//        maxWidth = (checkbox->width() > maxWidth ? checkbox->width() : maxWidth);
//    // задать полученную ширину всем чекбоксам
//    foreach (auto checkbox, checkboxes)
//        checkbox->setMinimumWidth(maxWidth);
}


IssoModuleConfig *ModuleConfigTabWidget::selectedModuleConfig()
{
    if (!m_object)
        return nullptr;
    // получить выбранный элемент списка
    QListWidgetItem* selectedItem = ui->wgtModuleConfigs->selectedItem();
    // если имя конфигурации модуля выбрано, вернуть указатель на конфигурацию
    return (selectedItem ? m_object->moduleConfig(selectedItem->text()) : nullptr);
}


void ModuleConfigTabWidget::clearModuleConfig()
{
    // очистить панели настройки конфигов встроенных датчиков
    foreach (IssoAnalogSensorSetupPanel* panel, m_sensorSetupPanels.values())
    {
        panel->clearStateRanges();
        panel->setChecked(false);
    }
    // снять выделение имени на панели привязки конфигов шлейфов
    foreach (IssoCheckableSelectionPanel* panel, m_chainLinkPanels.values())
    {
        panel->setSelectedItemName("");
        panel->setChecked(false);
    }
    // снять выделение имени на панели привязки классов к реле
    foreach (IssoCheckableSelectionPanel* panel, m_relayClassPanels.values())
    {
        panel->setSelectedItemName("");
        panel->setChecked(false);
    }
    // снять выделение имени на панели привязки сценариев
    ui->wgtAlarmScenario->setSelectedItemName("");
    ui->wgtAlarmScenario->setChecked(false);
    ui->wgtFailScenario->setSelectedItemName("");
    ui->wgtFailScenario->setChecked(false);
}


IssoModuleConfig *ModuleConfigTabWidget::makeModuleConfig(const QString &cfgName)
{
    // создать конфиг модуля
    IssoModuleConfig* cfg = new IssoModuleConfig(cfgName);
    // добавить в конфиг модуля конфиги всех встроенных датчиков
    foreach (IssoParamId id, m_sensorSetupPanels.keys())
    {
        // получить панель настройки встроенного датчика
        IssoAnalogSensorSetupPanel* panel = m_sensorSetupPanels.value(id);
        // создать конфиг встроенного датчика, соответствующего панели
        //
        //  !!! SMOKE_CHAIN - переделать !!!
        //
//        IssoAnalogChainConfig* sensorCfg = new IssoAnalogChainConfig(SMOKE_CHAIN, panel->title());
        IssoAnalogChainConfig* sensorCfg = new IssoAnalogChainConfig(INTERNAL_SENSOR, panel->title());
        // задать набор настраиваемых состояний датчика
        sensorCfg->initStates(panel->displayingStates());
        // добавить конфиг встроенного датчика в конфиг модуля
        cfg->insertSensorConfig(id, sensorCfg);
    }
    // добавить в конфиг модуля имена конфигов всех шлейфов
    foreach (IssoParamId id, m_chainLinkPanels.keys())
    {
//        // получить панель привязки имени конфига шлейфа
//        IssoChainConfigLinkPanel* panel = m_chainLinkPanels.value(id);
        // добавить пустое имя конфига шлейфа
        cfg->insertChainConfigLink(id, "", false);
    }
    return cfg;
}


void ModuleConfigTabWidget::initSensorSetupPanels()
{
    // заполнить карту виджетов настройки встроенных датчиков
    m_sensorSetupPanels.insert(SENSOR_SMOKE, ui->wgtSmoke);
    m_sensorSetupPanels.insert(SENSOR_CO, ui->wgtCO);
    m_sensorSetupPanels.insert(BTN_FIRE, ui->wgtBtnFire);
    m_sensorSetupPanels.insert(BTN_SECURITY, ui->wgtBtnSecurity);
    m_sensorSetupPanels.insert(VOLT_BATTERY, ui->wgtVoltBattery);
    m_sensorSetupPanels.insert(VOLT_BUS, ui->wgtVoltBus);
    m_sensorSetupPanels.insert(VOLT_SENSORS, ui->wgtVoltSensors);
    m_sensorSetupPanels.insert(TEMP_OUT, ui->wgtTemp);
    m_sensorSetupPanels.insert(VIBRATION, ui->wgtVibration);
    m_sensorSetupPanels.insert(SENSOR_OPEN, ui->wgtSensorOpen);
    m_sensorSetupPanels.insert(SUPPLY_INTERNAL, ui->wgtSupplyInternal);
    m_sensorSetupPanels.insert(SUPPLY_EXTERNAL, ui->wgtSupplyExternal);
    // настроить панели
    foreach (IssoParamId sensorId, m_sensorSetupPanels.keys())
    {
        // найти панель по ID датчика
        IssoAnalogSensorSetupPanel* panel = m_sensorSetupPanels.value(sensorId);
        if (panel)
        {
            // задать заголовок
            panel->setTitle(IssoCommonData::stringByParamId(sensorId));
            //
            // задать набор настраиваемых состояний
            switch(sensorId)
            {
                case SENSOR_SMOKE:
                case SENSOR_CO:
                case VOLT_BATTERY:
                case VOLT_BUS:
                case VOLT_SENSORS:
                case TEMP_OUT:
                case VIBRATION:
                case SENSOR_OPEN:
                case SUPPLY_INTERNAL:
                case SUPPLY_EXTERNAL:
                {
                    // состояния нешлейфовых датчиков
                    QSet<IssoState> sensorStates
                    {
                        STATE_NORMAL,
                        STATE_WARNING,
                        STATE_ALARM
                    };
                    // норма, внимание, тревога
                    panel->setDisplayingStates(sensorStates);
                    break;
                }
                case BTN_FIRE:
                case BTN_SECURITY:
                {
                    // состояния шлейфов
                    QSet<IssoState> chainStates
                    {
                        STATE_NORMAL,
                        STATE_WARNING,
                        STATE_ALARM,
                        STATE_SHORT_CIRCUIT,
                        STATE_BREAK
                    };
                    // норма, внимание, тревога, КЗ, обрыв
                    panel->setDisplayingStates(chainStates);
                    break;
                }
                default:
                    break;
            }
        }
    }
}


void ModuleConfigTabWidget::initChainLinkPanels()
{
    // заполнить карту виджетов привязки конфигов шлейфов
    //
    // аналоговые шлейфы 1 - 6
    m_chainLinkPanels.insert(ANALOG_CHAIN_1, ui->wgtChainConfig1);
    m_chainLinkPanels.insert(ANALOG_CHAIN_2, ui->wgtChainConfig2);
    m_chainLinkPanels.insert(ANALOG_CHAIN_3, ui->wgtChainConfig3);
    m_chainLinkPanels.insert(ANALOG_CHAIN_4, ui->wgtChainConfig4);
    m_chainLinkPanels.insert(ANALOG_CHAIN_5, ui->wgtChainConfig5);
    m_chainLinkPanels.insert(ANALOG_CHAIN_6, ui->wgtChainConfig6);
    // цифровые шлейфы 7 - 8
    m_chainLinkPanels.insert(MULTISENSOR, ui->wgtChainConfig7);
//    m_chainLinkPanels.insert(MULTI_CHAIN_2, ui->wgtChainConfig8);
    //
    // настроить панели выбор конфигов
    foreach (IssoParamId sensorId, m_chainLinkPanels.keys())
    {
        // найти панель по ID датчика
        IssoCheckableSelectionPanel* panel = m_chainLinkPanels.value(sensorId);
        if (panel)
        {
            // задать заголовок
            panel->setLabel(IssoCommonData::stringByParamId(sensorId));
        }
    }
}


void ModuleConfigTabWidget::initScenarioPanels()
{
    // тревога
    ui->wgtAlarmScenario->setLabel(IssoCommonData::stringByModuleState(ALARM));
    ui->wgtAlarmScenario->setChecked(false);
    // неисправность
    ui->wgtFailScenario->setLabel(IssoCommonData::stringByModuleState(FAIL));
    ui->wgtFailScenario->setChecked(false);
}


void ModuleConfigTabWidget::initRelayClassPanels()
{
    // заполнить карту виджетов привязки имен классов к реле
    // внутренние реле
    m_relayClassPanels.insert(INT_RELAY_1, ui->wgtIntRelay1);
    m_relayClassPanels.insert(INT_RELAY_2, ui->wgtIntRelay2);
    m_relayClassPanels.insert(INT_RELAY_3, ui->wgtIntRelay3);
    m_relayClassPanels.insert(INT_RELAY_4, ui->wgtIntRelay4);
    m_relayClassPanels.insert(INT_RELAY_5, ui->wgtIntRelay5);
    // внешние реле
    m_relayClassPanels.insert(EXT_RELAY_1, ui->wgtExtRelay1);
    m_relayClassPanels.insert(EXT_RELAY_2, ui->wgtExtRelay2);
    m_relayClassPanels.insert(EXT_RELAY_3, ui->wgtExtRelay3);
    m_relayClassPanels.insert(EXT_RELAY_4, ui->wgtExtRelay4);
    m_relayClassPanels.insert(EXT_RELAY_5, ui->wgtExtRelay5);
    //
    // настроить панели выбора имен классов
    foreach (IssoParamId sensorId, m_relayClassPanels.keys())
    {
        // найти панель по ID реле
        IssoCheckableSelectionPanel* panel = m_relayClassPanels.value(sensorId);
        if (panel)
        {
            // задать заголовок
            panel->setLabel(IssoCommonData::stringByParamId(sensorId));
        }
    }
}


void ModuleConfigTabWidget::initButtonClassCombos()
{
    m_buttonClassCombos.insert(1, ui->cbButtonClass1);
    m_buttonClassCombos.insert(2, ui->cbButtonClass2);
    m_buttonClassCombos.insert(3, ui->cbButtonClass3);
    m_buttonClassCombos.insert(4, ui->cbButtonClass4);
    m_buttonClassCombos.insert(5, ui->cbButtonClass5);
    m_buttonClassCombos.insert(6, ui->cbButtonClass6);
}


void ModuleConfigTabWidget::processAddModuleConfigBtnClicked()
{
    if (!m_object)
        return;

    // вывести диалог ввода имени конфигурации модуля
    bool ok;
    QString cfgName = IssoCommonData::showInputTextDialog(this,
                                        tr("Введите имя конфигурации модуля"),
                                        tr("Имя конфигурации:"), ok);
    if (!ok)
        return;
    // если имя конфигурации не задано, выход
    if (cfgName.isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить конфигурацию модуля, "
                                "т.к. не задано имя конфигурации"));
        return;
    }
    // если имя конфигурации уже существует, выход
    if (m_object->moduleConfig(cfgName))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить конфигурацию модуля, "
                                "т.к. конфигурация с именем <%1> уже существует")
                                .arg(cfgName));
        return;
    }
    // создать и инициализировать конфигурацию модуля
    IssoModuleConfig* cfg = makeModuleConfig(cfgName);
    // добавить в коллекцию конфигов модулей объекта
    m_object->insertModuleConfig(cfg);
    // добавить в визуальный список
    ui->wgtModuleConfigs->addItem(cfgName, true);
}


void ModuleConfigTabWidget::processRemoveModuleConfigBtnClicked()
{
    if (!m_object)
        return;

    // получить выделенный элемент списка
    QListWidgetItem* item = ui->wgtModuleConfigs->selectedItem();
    // проверить выбрано ли имя конфигурации
    if (!item)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить конфигурацию модуля, "
                                "т.к. не выбрано имя конфигурации"));
        return;
    }
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                    tr("Вы действительно хотите удалить конфигурацию модуля <%1>")
                        .arg(item->text())))
        return;
    // удалить конфигурацию модуля
    m_object->removeModuleConfig(item->text());
    // обновить отображаемые шлейфы модуля
    m_object->applyModuleConfigChanges(item->text());
    // удалить элемент из визуального списка
    ui->wgtModuleConfigs->removeItem(item->text());
}


void ModuleConfigTabWidget::processModuleConfigsSelectionChanged()
{
    // получить выбранный конфиг модуля
    IssoModuleConfig* moduleCfg = selectedModuleConfig();
    if ((!m_object) || (!moduleCfg))
    {
        // очистить настройки модуля
        clearModuleConfig();
        return;
    }
    //
    // отобразить конфиги встроенных датчиков
    displayAllSensorConfigs(moduleCfg);
    //
    // отобразить имена конфигов шлейфов
    displayAllChainConfigLinks(moduleCfg);
    //
    // отобразить имена классов реле
    //
    displayAllRelayClassNames(moduleCfg);
    //
    // отобразить имена сценариев
    //
    displayAllScenarioNames(moduleCfg);
    //
    // отобразить настройки кнопок панели управления
    //
    displayButtonClasses(moduleCfg);
}


void ModuleConfigTabWidget::processSaveSettingsBtnClicked()
{
    // получить выбранный конфиг модуля
    IssoModuleConfig* moduleCfg = selectedModuleConfig();
    if ((!m_object) || (!moduleCfg))
        return;
//    // вывести диалог подтверждения
//    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение сохранения"),
//                    tr("При сохранении настроек конфигурации модуля произойдет "
//                       "сброс расстановки соответствующих шлейфов на графплане."
//                       "Вы действительно хотите сохранить настройки конфигурации <%1>?")
//                        .arg(moduleCfg->name())))
//        return;
    //
    // сохранить конфиги встроенных датчиков
    updateAllSensorConfigs(moduleCfg);
    //
    // сохранить имена конфигов шлейфов
    updateAllChainConfigLinks(moduleCfg);
    //
    // обновить имена классов реле
    //
    updateAllRelayClassNames(moduleCfg);
    //
    // обновить имена сценариев:
    //
    updateAllScenarioNames(moduleCfg);
    //
    // обновить настройки кнопок панели управления
    //
    updateButtonClasses(moduleCfg);
    //
    // применить конфиг модуля ко всем модулям, использующим его
    m_object->applyModuleConfigChanges(moduleCfg->name());
    // обновить настройки выбранного конфига модуля
    processModuleConfigsSelectionChanged();
}

