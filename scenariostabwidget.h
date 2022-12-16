#ifndef SCENARIOSTABWIDGET_H
#define SCENARIOSTABWIDGET_H

#include "issoobject.h"
#include "issoscenario.h"
#include "newrelayclassdialog.h"
#include "newscenariodialog.h"

#include <QComboBox>
#include <QWidget>

namespace Ui {
    class ScenariosTabWidget;
}

class ScenariosTabWidget : public QWidget
{
        Q_OBJECT        
    private:
        Ui::ScenariosTabWidget *ui;

        IssoObject* m_object = nullptr;

        QMap<int, QComboBox*> m_systemButtonCombos;

        void setupWidget();
        void initWidgetHandlers();
        void initSystemButtonCombos();

        /// \brief  Отобразить настройки класса реле
        void displayRelayClass(const QString& relayClassName);
        /// \brief  Обновить настройки класса реле
        void updateRelayClass(const QString& relayClassName);

        /// \brief  Отобразить настройки сценария
        void displayScenario(const QString& scenarioName);
        /// \brief  Обновить настройки сценария
        void updateScenario(const QString& scenarioName);
        //
        IssoScenario* currentScenario();

        void displaySystemControlPanelButtons();
        void updateSystemControlPanelButtons();

    public:
        explicit ScenariosTabWidget(QWidget *parent = 0);
        ~ScenariosTabWidget();

        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);

        void updateTab();

    private slots:
        void processAddClassBtnClicked();
        void processRemoveClassBtnClicked();
        void processRelayClassesSelectionChanged();
        void processSaveClassSettingsBtnClicked();

        void processAddScenarioBtnClicked();
        void processRemoveScenarioBtnClicked();
        void processScenariosSelectionChanged();
        void processSaveScenarioSettingsBtnClicked();

        void processAddScenarioClassBtnClicked();
        void processRemoveScenarioClassBtnClicked();

        void processSaveControlPanelSettingsBtnClicked();
};

#endif // SCENARIOSTABWIDGET_H
