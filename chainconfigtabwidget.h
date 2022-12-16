/**
настройка конфигурации шлейфов
*/

#ifndef CHAINCONFIGTABWIDGET_H
#define CHAINCONFIGTABWIDGET_H

#include "issodigitalsensorsetuppanel.h"
#include "issoobject.h"

#include <QWidget>
#include <QButtonGroup>
#include <QMessageBox>


namespace Ui {
    class ChainConfigTabWidget;
}

class ChainConfigTabWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::ChainConfigTabWidget *ui;

        IssoObject* m_object = nullptr;
        QMap<IssoDigitalSensorId, IssoDigitalSensorSetupPanel*> m_digitalSensorSetupPanels;

        void setupWidget();
        void initWidgetHandlers();

        void initMsTypes();

        void initDigitalSensorSetupPanels();

        IssoChainConfig* selectedChainConfig();
        IssoMultiSensorConfig* selectedMultiSensorConfig();

        void displayAnalogChainConfig(IssoAnalogChainConfig* cfg);
        void displayDigitalChainConfig(IssoAddressChainConfig* cfg);
        void clearChainConfig();



        void getDigitalSensorSettings(const IssoDigitalSensorId& id, int& warningValue, int& alarmValue,
                                      IssoStateCondition& condition, bool &checked);
        void setDigitalSensorSettings(const IssoDigitalSensorId& id, int warningValue, int alarmValue,
                                      IssoStateCondition condition, bool checked);

        void displayMultiSensorConfig(IssoMultiSensorConfig* cfg);
        void updateMultiSensorConfig(IssoMultiSensorConfig* cfg);


        void selectChainSettingsWidget(IssoChainType type);

        void updateCameraList();

    public:
        explicit ChainConfigTabWidget(QWidget *parent = 0);
        ~ChainConfigTabWidget();

        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);
        void updateTab();

    private slots:
        void processAddChainConfigBtnClicked();
        void processRemoveChainConfigBtnClicked();

        void processAddMultisensorConfigBtnClicked();
        void processRemoveMultisensorConfigBtnClicked();

        void processChainConfigsSelectionChanged();
        void processMultisensorConfigsSelectionChanged();

        void processSaveAnalogChainSettingsBtnClicked();
        void processSaveAddressChainSettingsBtnClicked();

        void processMsTypeChanged(int index);

};

#endif // CHAINCONFIGTABWIDGET_H
