#ifndef SENSORVALUESCHARTTABWIDGET_H
#define SENSORVALUESCHARTTABWIDGET_H

#include "issoobject.h"

#include <QWidget>
#include <QtCharts>


namespace Ui {
    class SensorValuesChartTabWidget;
}

/**
 * @brief The SensorValuesChartTabWidget class данные графиков величин на мультидатчиках
 */
class SensorValuesChartTabWidget : public QWidget
{
        Q_OBJECT

    private:
        Ui::SensorValuesChartTabWidget *ui;

        IssoObject* m_object = nullptr;
        QChartView* m_chartView;
        QMap<IssoDigitalSensorId, QSharedPointer<QList<quint16>>> m_rawValues;
        qreal m_pollInterval = 0;
        QTimer m_pollTimer;

        QCategoryAxis* m_axisStates;

        void init();
        void initChart();
        /// \brief  Сформировать визуальный список встроенных датчиков мультидатчика
        void initSensorList();
        /// \brief  Установить обработчики событий
        void initEventHandlers();
        /// \brief  Заполнить визуальный список модулей
        void fillModuleList(const QList<IssoModule*>& modules);

        bool checkModuleNamesChanged(const QList<IssoModule*>& modules);

        void addSensorData(IssoDigitalSensorId sensorId, quint16 rawValue);
        void clearAllSensorsData();
        QList<quint16> sensorData(IssoDigitalSensorId sensorId);

        void addSeries(IssoDigitalSensorId sensorId);
        void removeAllSeries();
        QLineSeries *findSeries(IssoDigitalSensorId sensorId);
        void addSeriesValue(IssoDigitalSensorId sensorId, quint16 value);
        void addSeriesValue(QXYSeries* series, qreal time, quint16 value);
        void clearAllSeriesValues();

        void initAxes();
        void attachAxes(QXYSeries* series);
        void resetAxesRanges();
        void updateAxisStates(IssoDigitalSensorId sensorId);
        void ensureSensorValueVisible(qreal time, quint16 value);

        void startMeasuring();
        void stopMeasuring();

        void requestMsRegAction(IssoMsRegAction action);

        //
        void testSensorDataReceived();

    public:
        explicit SensorValuesChartTabWidget(QWidget *parent = 0);
        ~SensorValuesChartTabWidget();

        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);
        void updateTab();
        void handleSensorData(const QMap<IssoDigitalSensorId, quint16> &sensorValues);

        int selectedModuleId() const;
        int selectedMultiSensorId() const;
        IssoDigitalSensorId selectedDigitalSensorId() const;
        bool isMeasuring() const;

    signals:
//        void multiSensorRawInfoRequested(int moduleId, quint8 multiSensorId);
        void multiSensorRegActionRequested(IssoMsRegAction action, int moduleId, quint8 msId);

    private slots:
        void processModuleNameSelectionChanged(const QString& moduleName);
        void processSensorNameSelectionChanged(int);
        void processBtnStartStopClicked();
        void processBtnBeepClicked();
        void processBtnCalibrateClicked();
        void processPollTimerTimeout();

};

#endif // SENSORVALUESCHARTTABWIDGET_H
