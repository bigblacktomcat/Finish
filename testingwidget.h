#ifndef TESTINGWIDGET_H
#define TESTINGWIDGET_H

#include "issoobject.h"
#include "issoexchangemanager.h"
#include "issomodulescontroller.h"
#include "issocore.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

namespace Ui {
    class TestingWidget;
}

/**
 * @brief The TestingWidget class
 * @param QMap<IssoParamId, QComboBox*> m_intSensorStates карта соответствия ID встроенных датчиков и компонентов, отображающих их состояния
 * @param QMap<IssoParamId, QDoubleSpinBox*> m_intSensorValues карта соответствия ID встроенных датчиков и компонентов, отображающих их значения
 * @param QMap<IssoDigitalSensorId, QComboBox*> m_msSensorStates карта соответствия ID цифровых датчиков МД и компонентов, отображающих их состояния
 * @param QMap<IssoDigitalSensorId, QSpinBox*> m_msSensorValues карта соответствия ID цифровых датчиков МД и компонентов, отображающих их значения
 */
class TestingWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::TestingWidget *ui;

        const QString ALL_ITEMS = tr("Все");

        IssoObject* m_object;
        //
        // карта соответствия ID встроенных датчиков и компонентов, отображающих их состояния
        QMap<IssoParamId, QComboBox*> m_intSensorStates;
        // карта соответствия ID встроенных датчиков и компонентов, отображающих их значения
        QMap<IssoParamId, QDoubleSpinBox*> m_intSensorValues;
        //
        // карта соответствия ID цифровых датчиков МД и компонентов, отображающих их состояния
        QMap<IssoDigitalSensorId, QComboBox*> m_msSensorStates;
        // карта соответствия ID цифровых датчиков МД и компонентов, отображающих их значения
        QMap<IssoDigitalSensorId, QSpinBox*> m_msSensorValues;

        /**
         * @brief selectedModuleNames получить список имен выбранных модулей
        */
        QStringList selectedModuleNames();
        /**
         * @brief selectedMsAddresses() получить список адресов выбранных МД
         */

        QList<quint8> selectedMsAddresses(const QString &moduleCfgName);


        void initExchangerHandlers(IssoExchangeManager* exchanger);
        void initControllerHandlers(IssoModulesController* controller);
        void updateModuleList();

        void initControls();
        void initIntSensorControls();
        void initMsSensorControls();
        void initRelayControls();
        void initControlHandlers();


        IssoState msSensorState(IssoDigitalSensorId id);
        QMap<IssoDigitalSensorId, IssoState> msSensorStates();

        IssoParamBuffer makeInfoParams();
        IssoParamBuffer makeMasgParams(QList<quint8> msAddrs);

    public:
        explicit TestingWidget(IssoCore* core,
//                               IssoObject* object,
//                               IssoExchangeManager* exchanger,
//                               IssoModulesController* controller,
                               QWidget *parent = 0);
        ~TestingWidget();

    signals:
        void replyEventEmulated(IssoModuleReply reply, bool ok);

    private slots:
        void processModuleNameSelectionChanged(const QString& moduleName);
        void processBtnEmulateInfoClicked();
        void processBtnEmulateMasgClicked();
};

#endif // TESTINGWIDGET_H
