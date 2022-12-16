#ifndef GRAPHICPLANTABWIDGET_H
#define GRAPHICPLANTABWIDGET_H

#include "issofloorview.h"
#include "issomodulereply.h"
#include "issoobject.h"
#include "issostatelabel.h"
#include "issoeventdelegate.h"
#include "issologmessagebuilder.h"
#include "issomodulestatedelegate.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QListWidget>
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>


namespace Ui {
    class GraphicPlanTabWidget;
}


namespace
{
    const QString ACT_CHANGE_COMMENT =
            QT_TRANSLATE_NOOP("GraphicPlanTabWidget", "Изменить комментарий");
    const QString ACT_SHOW_PHOTO =
            QT_TRANSLATE_NOOP("GraphicPlanTabWidget", "Показать фотоснимок");
}


class GraphicPlanTabWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::GraphicPlanTabWidget *ui;
        // охраняемый объект
        IssoObject* m_object = nullptr;
        // время поступления последней тревоги
        QTime m_lastAlarmTime;
        // лейблы состояния выбранного модуля
        QLabel* g_lblModuleName;
        QLabel* g_lblModuleStatus;
        QLabel* g_lblModuleState;
        // коллекция кнопок управления модулем
        QMap<int, QPushButton*> m_moduleButtons;
        // коллекция кнопок управления системой
        QMap<int, QPushButton*> m_systemButtons;

        // кнопка SOS
        QPushButton* g_btnSos;

        // карта состояний датчиков модуля и лейблов, отображающих их состояния
        QMap<IssoParamId, IssoStateLabel*> m_sensorLabels;

        // контекстное меню таблицы
        QMenu m_tableMenu;

        // коллекция исполняемых сценариев
        QMap<QString, IssoScenarioInfo> m_runningScenarios;

        void insertRunningScenario(IssoScenarioInfo info);
        void removeRunningScenario(const QString& complexName);
        IssoScenarioInfo runningScenario(const QString& complexName);
        bool scenarioIsRunning(const QString& complexName);


        /// \brief  Инициализировать виджеты
        void initWidgets();
        /// \brief  Инициализировать переменные для работы с виджетами
        void initWidgetVars(IssoGuiMode mode);
        /// \brief  Установить обработчики событий
        void initWidgetHandlers();
        /// \brief  Инициализировать обработчики виджетов фильтра событий
        void setEventFilterHandlers();
        /// \brief  Установить /снять обработчики выделения модулей
        void setSelectionHandlersEnabled(bool enabled);
        /// \brief      Установить отображаемый план этажа
        /// \details    Выводит на экран указанный план этажа (делает его текущим)
        ///             и заполняет визуальный список модулей именами модулей,
        ///             расположенных на данном этаже
        void setCurrentFloorPlan(IssoFloorPlan* floor);
        /// \brief  Отобразить в визуальном списке этажей только те,
        ///         которые относятся к выбранному зданию
        void fillVisualFloorList(IssoBuilding* build);
        /// \brief  Отобразить в визуальном списке модулей только те,
        ///         которые расположены на выбранном этаже
        void fillVisualModulesList(IssoFloorPlan* floor);
        /// \brief  Очистить все визуальные списки
        void clearWidgetLists();
        /// \brief  Обновить состояния зданий в визуальном списке
        void updateBuildingsList();
        /// \brief  Обновить состояния этажей в визуальном списке
        void updateFloorsList();
        /// \brief  Обновить состояния модулей в визуальном списке
        void updateModulesList();
        /// \brief  Отобразить состояния всех датчиков выбранного модуля,
        ///         либо установить им неактивное состояние, если модуль не выбран
        void updateSelectedModuleSensorStates();

        void updateModuleContolButtons();
        void updateSystemContolButtons();

        /// \brief  Установить стиль лейбла, отображающего состояние датчика, согласно заданному состоянию
        void updateSensorLabelStyle(IssoParamId paramId, IssoState sensorState, const QString &sensorName);
        /// \brief  Установить стиль кнопки, переключающей состояние реле, согласно заданному состоянию
        void updateButtonStyle(QPushButton *button, bool pressed);

        bool buttonPressed(QPushButton *button);

        /// \brief  Отобразить графплан заданного этажа
        void displayScene(IssoFloorPlan* floor);
        /// \brief  Скрыть графплан текущего этажа
        void hideScene();
        /// \brief  Подготовить ГПИ к работе на сенсорном экране
        void updateGuiForTouchScreen();
        /// \brief  Подготовить визуальный список к работе на сенсорном экране
        void updateListWidgetForTouch(QListWidget* list, const QModelIndex &parent,
                                      int first, int last);
        /// \brief  Вывести диалог ввода пароля
        bool enterPassword();

        QPushButton* systemButton(int buttonNum);


        void initTableMenu();
        void initTableMenuActions();
        void setMenuItemsVisible(const QModelIndex& curIndex);

        // DEBUG
        void initTestPanel();
        void initTestWidgets();
        void initTestEventHandlers();
        void testUpdateSelectedModuleMsAddrs();
        IssoModuleReply* testMakeAlarmReply(IssoModule *module);
        IssoModuleReply* testMakeNoAlarmsReply(IssoModule *module);
        IssoModuleReply* testMakeMultiSensorStateReply(IssoModule *module,
                                                       quint8 multiSensorId,
                                                       quint8 stateValue,
                                                       quint16 detailsValue);
        IssoModuleReply* testMakeMultiSensorReply(IssoModule *module);

    protected:
        virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
        virtual void resizeEvent(QResizeEvent *event);

    public:
        explicit GraphicPlanTabWidget(QWidget *parent = 0);
        ~GraphicPlanTabWidget();

        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);
        /// \brief  Деинициализировать виджет
        void uninit();
        /// \brief  Задать режим ГПИ (стандартный монитор, сенсорный экран, планшет)
        void init(IssoAppType appType, IssoGuiMode mode);
        /// \brief  Задать режим пользователя (админ, пользователь)
        void setSuperUserMode(bool superUser);
        /// \brief  Задать модель БД
        void setDataBaseModel(QAbstractItemModel *model);
        /// \brief  Позиционировать сцену графплана на тревожный модуль
        void displayAlarmModule(IssoModule* module);
        /// \brief  Обновить графплан этажа
        void updateTab();

        /// \brief  Обновить состояния всех визуальных списков
        void updateAllLists();
        /// \brief      Отобразить информацию о выбранном модуле
        /// \details    Отображает имя, состояние и статус выбранного модуля,
        ///             а также состояния всех его датчиков и активных действий
        ///             (нажатых кнопках). Если модуль не выбран, устанавливает
        ///             всё в неактивное состояние
        void updateSelectedModuleInfo();

        void setEventFilter(const QDate &date, const QTime &timeFrom, const QTime &timeTo, const QString &eventType);
        void setEventFilterEnabled(bool enabled);

        bool indicatorsVisible();
        void setIndicatiorsVisible(bool visible);

    signals:
        void writeEventCommentRequested(int eventId, const QString &comment);
        void turnModuleRelayRequested(int moduleId, const IssoParamId& relayId, bool turnOn);
        void turnBroadcastRelayRequested(const IssoParamId& relayId, bool turnOn);
//        void videoStreamRequested(int cameraId);

        void playVoiceRecordRequested(const QHostAddress& ip, quint16 port,
                                      const QByteArray& voiceRecord);
        void playModuleRecordRequested(const QHostAddress& ip, quint16 port,
                                       quint8 recordNum);
        void playPcRecordRequested(const QHostAddress& ip, quint16 port,
                                   const QString& fileName);
        void moduleInfoRequested(int moduleId);
        void selectEventsRequested(QDate date, QTime timeFrom, QTime timeTo,
                                   const QString eventType, int objectId, bool filtered);
        void changeScenarioStateRequested(IssoScenarioInfo scenarioInfo, bool active);

//        void cameraActionPerformed(int cameraId, int buildNum, int floorNum);
        void cameraTurned(int cameraId, bool turnOn);
        void sensorActionPerformed(quint8 id, int moduleId, quint8 chainId,
                                   IssoChainType chainType, IssoSensorAction action);
        void objectConfigUpdateRequested();

        void progressCurrent(int count);
        void progressFinished();
        //
        // DEBUG
        //
        void testCtrlReplyReceived(IssoModuleReply reply, bool ok);
        void testCtrlAlarmReceived(IssoModuleReply reply);
        void testCtrlReplyTimedOut(const IssoModuleRequest& request);
        void testClearRemoteLogRequested();

    private slots:
        /// \brief      Обработчик выделения модуля на сцене
        /// \details    Выделяет в визуальном списке имена выбранных модулей
        void processModuleSelectionChanged();
        /// \brief      Обработчик выбора имени модуля в визуальном списке
        /// \details    Выделяет модули, имена которых выбраны в визуальном списке
        void processModuleNameSelectionChanged();

        void processModuleNameDoubleClicked(QListWidgetItem* item);

        /// \brief      Обработчик выбора имени этажа в визуальном списке
        /// \details    Делает текущим план этажа, имя которого выбрано в визуальном списке
        void processFloorChanged();
        /// \brief      Обработчик выбора имени здания в визуальном списке
        /// \details    Делает текущим здание, имя которого выбрано в визуальном списке,
        ///             и обновляет визуальный список этажей
        void processBuildingChanged();
        //
//        /// \brief  Обработчик кнопки Комментарий
//        void processBtnWriteCommentClicked();
        //
        //  Действия по кнопкам панели Управление модулем:
        //
        /// \brief  Обработчик кнопок панели управления модулем
        void processModuleFuncBtnClicked();
//        /// \brief  Обработчик кнопки Камера (модуль)
//        void processBtnModuleCameraClicked();
        /// \brief  Обработчик кнопки Голос (модуль)
        void processBtnModuleVoiceClicked();
        //
        //  Действия по кнопкам панели Управление системой:
        //
        /// \brief  Обработчик кнопок панели управления модулем
        void processSystemFuncBtnClicked();
        //
        /// \brief  Обработчик кнопки МЧС
        void processBtnSosClicked();
        //
        //
        //  Редактирование зданий:
        //
        /// \brief  Обработчик кнопки Добавить здание
        void processBtnAddBuildingClicked();
        /// \brief  Обработчик кнопки Удалить здание
        void processBtnRemoveBuildingClicked();
        //
        //  Редактирование этажей:
        //
        /// \brief  Обработчик кнопки Добавить этаж
        void processBtnAddFloorClicked();
        /// \brief  Обработчик кнопки Удалить этаж
        void processBtnRemoveFloorClicked();
        /// \brief  Обработчик кнопки Изменить этаж
        void processBtnEditFloorClicked();
        //
        //  Редактирование списка модулей этажа:
        //
        /// \brief  Обработчик кнопки Добавить модуль на этаж
        ///         (из списка нераспределенных модулей)
        void processBtnAddModuleToFloorClicked();
        /// \brief  Обработчик кнопки Удалить модуль с этажа
        ///         (переместить в список нераспределенных модулей)
        void processBtnRemoveModuleFromFloorClicked();
        //
        //
        /// \brief  Обработчик кнопки Запросить состояние
        ///         (вкладка Графплан в режиме Админа)
        void processBtnRequestModuleInfoClicked();


        void processContextMenuRequested(const QPoint& pos);
        void editComment();
        void showPhoto();

    public slots:
        //
        //  События контроллера модулей:
        //
        /// \brief  Обработчик события изменения набора тревожных параметров (датчиков) модуля
        void processSensorStatesChanged(const QString& moduleName/*,
                                        const QSet<IssoParamId>& changedParamIds*/);
        /// \brief  Обработчик события изменения состояния реле (кнопки) модуля
        void processRelayStateChanged(const QString& moduleName,
                                      IssoParamId paramId, bool pressed);
        //
        //  Работа с БД
        //
        /// \brief  Обновить выборку событий согласно текущему фильтру
        void selectEvents();
        //
        void processScenarioStateChanged(IssoScenarioInfo scenarioInfo, bool active);
};

#endif // GRAPHICPLANTABWIDGET_H
