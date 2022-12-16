#ifndef MODULECONFIGTABWIDGET_H
#define MODULECONFIGTABWIDGET_H

#include <QWidget>
#include <QMap>
#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include "issocommondata.h"
#include "issoobject.h"
#include "issoanalogsensorsetuppanel.h"
#include "issocheckableselectionpanel.h"


namespace Ui {
    class ModuleConfigTabWidget;
}


class ModuleConfigTabWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::ModuleConfigTabWidget *ui;

        IssoObject* m_object = nullptr;

        QMap<IssoParamId, IssoAnalogSensorSetupPanel*> m_sensorSetupPanels;
        QMap<IssoParamId, IssoCheckableSelectionPanel*> m_chainLinkPanels;
        QMap<IssoParamId, IssoCheckableSelectionPanel*> m_relayClassPanels;
        QMap<int, QComboBox*> m_buttonClassCombos;

        void setupWidget();

        void initSensorSetupPanels();
        void initChainLinkPanels();
        void initScenarioPanels();
        void initRelayClassPanels();
        void initButtonClassCombos();

        void initWidgetHandlers();

        void rearrangeWidgets();

        IssoModuleConfig* selectedModuleConfig();
        void clearModuleConfig();

        IssoModuleConfig* makeModuleConfig(const QString& cfgName);

        /// \brief  Отобразить конфигурацию встроенного (аналогового) датчика
        void displaySensorConfig(IssoParamId sensorId, IssoAnalogChainConfig* cfg);
        /// \brief  Обновить конфигурацию встроенного (аналогового) датчика
        void updateSensorConfig(IssoParamId sensorId, IssoAnalogChainConfig* cfg);

        void displayAllSensorConfigs(IssoModuleConfig* moduleCfg);
        void updateAllSensorConfigs(IssoModuleConfig* moduleCfg);

        void displayAllChainConfigLinks(IssoModuleConfig* moduleCfg);
        void updateAllChainConfigLinks(IssoModuleConfig* moduleCfg);

        void displayAllRelayClassNames(IssoModuleConfig* moduleCfg);
        void updateAllRelayClassNames(IssoModuleConfig* moduleCfg);

        void displayAllScenarioNames(IssoModuleConfig* moduleCfg);
        void updateAllScenarioNames(IssoModuleConfig* moduleCfg);

        void displayButtonClasses(IssoModuleConfig* moduleCfg);
        void updateButtonClasses(IssoModuleConfig* moduleCfg);

    protected:
        virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;


    public:
        explicit ModuleConfigTabWidget(QWidget *parent = 0);
        ~ModuleConfigTabWidget();

        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);
        /// \brief  Обновить вкладку
        void updateTab();

    private slots:
        void processAddModuleConfigBtnClicked();
        void processRemoveModuleConfigBtnClicked();
        void processModuleConfigsSelectionChanged();
        void processSaveSettingsBtnClicked();
};

#endif // MODULECONFIGTABWIDGET_H
