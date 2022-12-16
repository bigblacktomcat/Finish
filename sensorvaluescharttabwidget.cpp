#include "sensorvaluescharttabwidget.h"
#include "ui_sensorvaluescharttabwidget.h"



SensorValuesChartTabWidget::SensorValuesChartTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorValuesChartTabWidget)
{
    ui->setupUi(this);
    init();
}

SensorValuesChartTabWidget::~SensorValuesChartTabWidget()
{
    delete ui;
}


void SensorValuesChartTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
}


void SensorValuesChartTabWidget::updateTab()
{
    if (!m_object)
        return;
    // получить список модулей объекта
    auto modules = m_object->modules();

    // если список модулей изменился,
    // заполнить его
    if (checkModuleNamesChanged(modules))
        fillModuleList(modules);
//    // если список модулей не изменился, выход
//    if (!checkModuleNamesChanged(modules))
//        return;
//    // заполнить список модулей
//    fillModuleList(modules);

    // обновить ось состояний
    updateAxisStates(selectedDigitalSensorId());
}


void SensorValuesChartTabWidget::handleSensorData(const QMap<IssoDigitalSensorId, quint16> &sensorValues)
{
    foreach (auto sensorId, sensorValues.keys())
    {
        quint16 value = sensorValues.value(sensorId);
        addSensorData(sensorId, value);
        addSeriesValue(sensorId, value);
    }
}


void SensorValuesChartTabWidget::processModuleNameSelectionChanged(const QString &moduleName)
{
    ui->cbMsAddress->clear();
    if (!m_object)
        return;
    // найти модуль по имени
    IssoModule* module = m_object->findModuleByDisplayName(moduleName);
    if (!module)
        return;
    // получить конфиг адресного шлейфа
    auto msChainCfg = m_object->moduleAddressChainConfig(module->moduleConfigName());
    if (!msChainCfg)
        return;
    // заполнить выпадающий список адресов мультидатчиков
    foreach (quint8 id, msChainCfg->multiSensorIdList())
        ui->cbMsAddress->addItem(QString::number(id), id);
}


void SensorValuesChartTabWidget::processSensorNameSelectionChanged(int)
{
//    // получить данные выбранного имени датчика
//    QVariant sensorIdVar = ui->cbSensorName->currentData();
//    if (!sensorIdVar.isValid())
//        return;
//    // получить ID датчика
//    auto sensorId = static_cast<IssoDigitalSensorId>(sensorIdVar.toInt());

    // получить ID датчика
    auto sensorId = selectedDigitalSensorId();
    if (sensorId == DIGITALSENSOR_UNDEFINED)
        return;

    // очистить компонент
    removeAllSeries();
    // в зависимости от выбранного типа датчика, добавить графики
    switch(sensorId)
    {
        case DIGITALSENSOR_SMOKE_O:
        {
            addSeries(DIGITALSENSOR_SMOKE_O_LED_ON);
            addSeries(DIGITALSENSOR_SMOKE_O_LED_OFF);
            addSeries(DIGITALSENSOR_SMOKE_O);
            break;
        }
        case DIGITALSENSOR_TEMP_A:
        case DIGITALSENSOR_TEMP_D:
        case DIGITALSENSOR_SMOKE_E:
        case DIGITALSENSOR_CO:
        case DIGITALSENSOR_VOC:
        {
            addSeries(sensorId);
            break;
        }
        case DIGITALSENSOR_FLAME_STD_DEV:
        {
            addSeries(DIGITALSENSOR_FLAME_1);
            addSeries(DIGITALSENSOR_FLAME_2);
            addSeries(DIGITALSENSOR_FLAME_3);
            addSeries(DIGITALSENSOR_FLAME_4);
            addSeries(DIGITALSENSOR_FLAME_5);
            addSeries(DIGITALSENSOR_FLAME_6);
            addSeries(DIGITALSENSOR_FLAME_MIN_VAL);
            addSeries(DIGITALSENSOR_FLAME_STD_DEV);
            break;
        }
        default:
            break;
    }
    //
    // обновить ось состояний
    updateAxisStates(sensorId);
}


void SensorValuesChartTabWidget::processBtnStartStopClicked()
{
    if (ui->btnStartStop->isChecked())
    {
        ui->btnStartStop->setText("Закончить измерение");
        startMeasuring();
    }
    else
    {
        ui->btnStartStop->setText("Начать измерение");
        stopMeasuring();
    }
}


void SensorValuesChartTabWidget::processBtnBeepClicked()
{
    requestMsRegAction(IssoMsRegAction::BEEP);
}


void SensorValuesChartTabWidget::processBtnCalibrateClicked()
{
    requestMsRegAction(IssoMsRegAction::CALIBRATE);
}


void SensorValuesChartTabWidget::processPollTimerTimeout()
{
    requestMsRegAction(IssoMsRegAction::READ_RAW_DATA);

//    // тест
//    testSensorDataReceived();
}


void SensorValuesChartTabWidget::testSensorDataReceived()
{
    QMap<IssoDigitalSensorId, quint16> data;
    //
    // сгенерировать данные для каждого встроенного датчика МД
    foreach (auto id, IssoCommonData::displayingDigitalSensorIds())
    {
        data.insert(id, QRandomGenerator::global()->bounded(
                                            std::numeric_limits<quint16>::max()));
    }
    // обработать данные
    handleSensorData(data);
}


void SensorValuesChartTabWidget::startMeasuring()
{
    // заблокировать виджеты настройки измерений
    ui->wgtMeasureSetup->setEnabled(false);
    // сбросить диапазоны осей
    resetAxesRanges();
    // очистить данные всех датчиков
    clearAllSensorsData();
    // очистить значения графиков
    clearAllSeriesValues();
    // запомнить интервал
    m_pollInterval = ui->sbPollInterval->value() / 1000.0;
    // запустить таймер
    m_pollTimer.start(ui->sbPollInterval->value());
}


void SensorValuesChartTabWidget::stopMeasuring()
{
    // остановить таймер
    m_pollTimer.stop();
    // разблокировать виджеты настройки измерений
    ui->wgtMeasureSetup->setEnabled(true);
}


void SensorValuesChartTabWidget::requestMsRegAction(IssoMsRegAction action)
{
    // ID модуля
    int moduleId = selectedModuleId();
    // адрес мультдатчика
    int msAddr = selectedMultiSensorId();
    // если выбран модуль и мультидатчика, послать запрос
    if ((moduleId != -1) && (msAddr != -1))
        emit multiSensorRegActionRequested(action, moduleId, msAddr);
}


int SensorValuesChartTabWidget::selectedModuleId() const
{
    QVariant idVar = ui->cbModuleName->currentData();
    return (idVar.isValid() ? idVar.toInt() : -1);
}


int SensorValuesChartTabWidget::selectedMultiSensorId() const
{
    QVariant idVar = ui->cbMsAddress->currentData();
    return (idVar.isValid() ? idVar.toInt() : -1);
}


IssoDigitalSensorId SensorValuesChartTabWidget::selectedDigitalSensorId() const
{
    // получить данные выбранного имени датчика
    QVariant sensorIdVar = ui->cbSensorName->currentData();
    if (!sensorIdVar.isValid())
        return DIGITALSENSOR_UNDEFINED;
    // вернуть ID датчика
    return static_cast<IssoDigitalSensorId>(sensorIdVar.toInt());
}


bool SensorValuesChartTabWidget::isMeasuring() const
{
    return m_pollTimer.isActive();
}


void SensorValuesChartTabWidget::init()
{
    initEventHandlers();
    initChart();
    initSensorList();
}


void SensorValuesChartTabWidget::initChart()
{
    // создать чарт
    QChart *chart = new QChart();
    // настроить легенду
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setFont(QFont(QFont().family(), 10));
    // добавить чарт в компонент отображения
    ui->wgtChartView->setRenderHint(QPainter::Antialiasing);
    ui->wgtChartView->setChart(chart);
    // проинициализировать оси
    initAxes();
    // сбросить диапазоны осей
    resetAxesRanges();
}


void SensorValuesChartTabWidget::initEventHandlers()
{
    connect(ui->cbModuleName, SIGNAL(currentIndexChanged(QString)),
            this,             SLOT(processModuleNameSelectionChanged(QString)));
    connect(ui->btnStartStop, SIGNAL(clicked(bool)),
            this,             SLOT(processBtnStartStopClicked()));
    connect(ui->btnBeep, SIGNAL(clicked(bool)),
            this,        SLOT(processBtnBeepClicked()));
    connect(ui->btnCalibrate, SIGNAL(clicked(bool)),
            this,             SLOT(processBtnCalibrateClicked()));
    connect(ui->cbSensorName, SIGNAL(currentIndexChanged(int)),
            this,             SLOT(processSensorNameSelectionChanged(int)));
    connect(&m_pollTimer, SIGNAL(timeout()),
            this,         SLOT(processPollTimerTimeout()));
}


void SensorValuesChartTabWidget::fillModuleList(const QList<IssoModule *> &modules)
{
    // очистить списк имен модулей
    ui->cbModuleName->clear();
    // заполнить список имен модулей
    foreach (auto module, modules)
        ui->cbModuleName->addItem(module->displayName(), module->uniqueId());
}


bool SensorValuesChartTabWidget::checkModuleNamesChanged(const QList<IssoModule*>& modules)
{
    // сравнить количество
    if (ui->cbModuleName->count() != modules.length())
        return true;
    // сравнить содержимое
    foreach (auto module, modules)
    {
        // если имя модуля отсутствует в списке, вернуть true (список изменен)
        if (ui->cbModuleName->findText(module->displayName()) == -1)
            return true;
    }
    // вернуть false(список не изменен)
    return false;
}



void SensorValuesChartTabWidget::addSensorData(IssoDigitalSensorId sensorId, quint16 rawValue)
{
    // получить указатель на список значений датчика
    auto listPointer = m_rawValues.value(sensorId);
    // если список не существует
    if (listPointer.isNull())
    {
        // создать список
        listPointer = QSharedPointer<QList<quint16>>(new QList<quint16>());
        // добавить в коллекцию
        m_rawValues.insert(sensorId, listPointer);
    }
    // добавить данные в список
    listPointer.data()->append(rawValue);
}


void SensorValuesChartTabWidget::clearAllSensorsData()
{
    m_rawValues.clear();
}


QList<quint16> SensorValuesChartTabWidget::sensorData(IssoDigitalSensorId sensorId)
{
    // получить указатель на список значений датчика
    auto listPointer = m_rawValues.value(sensorId);
    // если указатель нулевой, вернуть пустой список,
    // иначе вернуть список значений датчика
    return (listPointer.isNull() ? QList<quint16>() :
                                   *(listPointer.data()));
}


void SensorValuesChartTabWidget::addSeries(IssoDigitalSensorId sensorId)
{
    // создать тренд
    QLineSeries *series = new QLineSeries();
    // задать имя тренда
    series->setName(IssoCommonData::stringByDigitalSensorId(sensorId));
    // добавить в компонент
    ui->wgtChartView->chart()->addSeries(series);
    //
    // получить данные датчика
    auto values = sensorData(sensorId);
    // добавить все значения
    qreal time = 0;
    foreach (auto value, values)
    {
        // добавить значение в тренд
        addSeriesValue(series, time, value);
        // инкрементировать время следующего значения
        time += m_pollInterval;
    }
    // привязать оси к тренду
    attachAxes(series);
}


void SensorValuesChartTabWidget::removeAllSeries()
{
    ui->wgtChartView->chart()->removeAllSeries();
}


QLineSeries * SensorValuesChartTabWidget::findSeries(IssoDigitalSensorId sensorId)
{
    foreach (auto series, ui->wgtChartView->chart()->series())
    {
        if (series->name() == IssoCommonData::stringByDigitalSensorId(sensorId))
            return dynamic_cast<QLineSeries*>(series);
    }
    return nullptr;
}


void SensorValuesChartTabWidget::addSeriesValue(IssoDigitalSensorId sensorId, quint16 value)
{
    // получить тренд датчика
    auto series = findSeries(sensorId);
    if (!series)
       return;

    // получить время текущего значения
    qreal time = (!series->points().isEmpty()
                                ? series->points().last().x() + m_pollInterval
                                : 0);
    // добавить значение в тренд
    addSeriesValue(series, time, value);
}


void SensorValuesChartTabWidget::addSeriesValue(QXYSeries *series, qreal time, quint16 value)
{
    // добавить значение
    series->append(time, value);
//    qDebug() << QString("time = %1, value = %2").arg(time).arg(value) << endl;
    // убедиться, что значение датчика видимо
    ensureSensorValueVisible(time, value);
}


void SensorValuesChartTabWidget::clearAllSeriesValues()
{
    foreach (auto series, ui->wgtChartView->chart()->series())
    {
        auto trend = dynamic_cast<QLineSeries*>(series);
        if (trend)
            trend->clear();
    }
}


void SensorValuesChartTabWidget::initAxes()
{
    auto font = QFont(QFont().family(), 10);
    // ось X
    QValueAxis *axisX = new QValueAxis;
    axisX->setGridLineVisible(true);
    axisX->setTitleText(tr("Время, сек"));
    axisX->setLabelsFont(font);
    ui->wgtChartView->chart()->setAxisX(axisX);
    // ось Y
    QValueAxis *axisY = new QValueAxis;
    axisY->setGridLineVisible(true);
    axisY->setTitleText(tr("Показатель датчика"));
    axisY->setLabelsFont(font);
    ui->wgtChartView->chart()->setAxisY(axisY);


    //
    m_axisStates = new QCategoryAxis();
    m_axisStates->setGridLineVisible(true);
    m_axisStates->setGridLinePen(QPen(QBrush(QColor(Qt::red)), 1, Qt::DashLine));
    m_axisStates->setLabelsFont(font);
    m_axisStates->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    ui->wgtChartView->chart()->addAxis(m_axisStates, Qt::AlignRight);
}


void SensorValuesChartTabWidget::attachAxes(QXYSeries *series)
{
//    // ось X
//    if (QValueAxis *axisX = dynamic_cast<QValueAxis*>(ui->wgtChartView->chart()->axisX()))
//    {
//        // привязать ось X к тренду
//        series->attachAxis(axisX);
//    }
//    // ось Y
//    if (QValueAxis *axisY = dynamic_cast<QValueAxis*>(ui->wgtChartView->chart()->axisY()))
//    {
//        // привязать ось Y к тренду
//        series->attachAxis(axisY);
//    }

//    if (m_axisStates)
//    {
//        series->attachAxis(m_axisStates);
//    }

    foreach (auto axis, ui->wgtChartView->chart()->axes())
    {
        series->attachAxis(axis);
    }
}


void SensorValuesChartTabWidget::resetAxesRanges()
{
    // ось X
    if (QValueAxis *axisX = dynamic_cast<QValueAxis*>(ui->wgtChartView->chart()->axisX()))
    {
        // задать диапазон
        axisX->setRange(0, 10);
        // задать число насечек
        axisX->setTickCount(11);
    }
    // ось Y
    if (QValueAxis *axisY = dynamic_cast<QValueAxis*>(ui->wgtChartView->chart()->axisY()))
    {
        // задать диапазон
        axisY->setRange(0, 4000);
        // задать число насечек
        axisY->setTickCount(5);
    }
}


void SensorValuesChartTabWidget::updateAxisStates(IssoDigitalSensorId sensorId)
{
    if (!m_object || !m_axisStates)
        return;
    // удалить все пороговые значения с оси
    foreach (auto label, m_axisStates->categoriesLabels())
        m_axisStates->remove(label);
    // получить id выбранного модуля
    int moduleId = selectedModuleId();
    // получить id выбранного мультидатчика
    int msId = selectedMultiSensorId();
    if ((moduleId == -1) || (msId == -1))
        return;
    // найти конфиг мультидатчика выбранного модуля
    auto msCfg = m_object->findMultiSensorCfgByModuleId(moduleId, msId);
    if (!msCfg)
        return;
    // найти конфиг встроенного датчика МД
    auto sensorCfg = msCfg->digitalSensorConfig(sensorId);
    if (!sensorCfg)
        return;

    // задать пороги состояний
    foreach (auto state, QList<IssoState>() << STATE_WARNING
                                            << STATE_ALARM )
    {
        m_axisStates->append(IssoCommonData::stringBySensorState(state),
                             sensorCfg->stateValue(state));

        qDebug() << QString("[%1]: %2 = %3")
                    .arg(IssoCommonData::stringByDigitalSensorId(sensorId))
                    .arg(IssoCommonData::stringBySensorState(state))
                    .arg(sensorCfg->stateValue(state));
    }
}


void SensorValuesChartTabWidget::ensureSensorValueVisible(qreal time, quint16 value)
{
    // ось X
    if (QValueAxis *axisX = dynamic_cast<QValueAxis*>(ui->wgtChartView->chart()->axisX()))
    {
        // если достигнут максимум по оси X, увеличить его
        if (time >= axisX->max())
        {
            // число отображаемых значений, кратных 10
            int markCount = (time + 10) / 10;
            // максимальное значение
            int maxValue = markCount * 10;
            // задать максимальное значение оси X
            axisX->setMax(maxValue);
        }
    }
    // ось Y
    if (QValueAxis *axisY = dynamic_cast<QValueAxis*>(ui->wgtChartView->chart()->axisY()))
    {
        // если достигнут максимум по оси Y, увеличить его
        if (value >= axisY->max())
        {
            // число отображаемых значений, кратных 1000
            int markCount = (value + 1000) / 1000;
            // максимальное значение
            int maxValue = markCount * 1000;
            // задать максимальное значение оси Y
            axisY->setMax(maxValue);
            // задать число насечек оси Y
            axisY->setTickCount(markCount + 1);
        }
    }
}


void SensorValuesChartTabWidget::initSensorList()
{
    // заполнить список имен встроенных датчиков МД
    foreach (auto id, IssoCommonData::displayingDigitalSensorIds())
        ui->cbSensorName->addItem(IssoCommonData::stringByDigitalSensorId(id), id);
}
