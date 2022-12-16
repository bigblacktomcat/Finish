#ifndef OBJECTSVIEWWINDOW_H
#define OBJECTSVIEWWINDOW_H

#include "issocommondata.h"
#include "issodatabase.h"
#include "issoremotelogpoller.h"
#include "mainwindow.h"
#include <QMainWindow>
#include <QMenu>


namespace Ui {
    class ObjectsViewWindow;
}

/**
 * @brief The ObjectsViewWindow class описание представления объекта
 */
class ObjectsViewWindow : public QMainWindow
{
        Q_OBJECT
    private:
        Ui::ObjectsViewWindow *ui;

        const int POLL_INTERVAL = 5000;

        const QString ACT_DISPLAY_OBJECT = tr("Показать объект");
        const QString ACT_SOUND_ON = tr("Включить звук");
        const QString ACT_SOUND_OFF = tr("Выключить звук");

        IssoAccount m_account;
        IssoAppType m_appType;
        IssoGuiMode m_guiMode;
        IssoDataBase* m_dataBase;
        MainWindow* m_appWindow = nullptr;

        IssoRemoteLogPoller m_logPoller;

        IssoSoundPlayer m_soundPlayer;

        QMenu m_contextMenu;

        void initMenu();
        void initMenuActions();
        void setMenuItemsVisible(const QModelIndex& curIndex);

        void setDataBase(IssoDataBase* db);
        void selectObjects();
        void setEventHandlers();
        void displayAppWindow(int objectId,
                              const QDate &date,
                              const QTime &timeFrom,
                              const QTime &timeTo);

        void startPollingLog();
        void stopPollingLog();

        bool checkEventIsAlarmed(const QString& stateName);
        bool checkEventIsActual(const QString& stateName);
        void updateAppWindowEvents(int objectId,
                                   const QDate &date,
                                   const QTime &timeFrom,
                                   const QTime &timeTo);
        void displayEvents(const QList<IssoEventData>& events);

        void parseMsEvents(const QMap<QString, QMap<QString, IssoEventData> > &msEvents,
                           QMap<QString, IssoParamBuffer *>& bufferMap);
        // определить, изменились ли данные объекта
        bool checkIfObjectDataChanged(const IssoEventData& event, bool& stateChanged);

        void updateAlarmSound(/*IssoModuleState state*/);
//        IssoModuleState getCommonState(const QMap<int, QString>& objectStates);
        IssoModuleState getSoundState();
        void updateView();

        QSet<int> objectIds();
        QMap<int, QString> currentStates();
        QModelIndex modelIndexByObjectId(int objectId);
        bool objectExists(int objectId);
        QString objectStateName(int objectId);

        void updateDisplayingObjectOnlineState(int objectId, const QString& objStateName);

        void turnObjectSound(const QModelIndex& objectIndex, bool soundOn);
        void turnObjectSound(int objectId, bool soundOn);


    protected:
        virtual void closeEvent(QCloseEvent *event);

    public:
        explicit ObjectsViewWindow(IssoAppType appType, IssoAccount account,
                                   IssoGuiMode guiMode, IssoDataBase* db, QWidget *parent = 0);
        virtual ~ObjectsViewWindow();

        IssoAppType appType() const;
        void setAppType(const IssoAppType &appType);

        IssoGuiMode guiMode() const;
        void setGuiMode(const IssoGuiMode &guiMode);

        IssoAccount account() const;
        void setAccount(const IssoAccount &account);

    private slots:
        void displaySelectedObject();
        void turnSelectedObjectSoundOff();
        void turnSelectedObjectSoundOn();
        void processContextMenuRequested(const QPoint& pos);
        void processBtnLoadClicked();
        void processBtnDeleteClicked();
        void processSensorEventsReceived(const QList<IssoEventData> &events);
        void processPhotoEventsReceived(const QList<IssoEventData> &events);

        void processPingEventsReceived(const QList<IssoEventData>& pingEvents);
//        void processRemotePingReceived(const QMap<int, QString>& recvStates);

};

#endif // OBJECTSVIEWWINDOW_H
