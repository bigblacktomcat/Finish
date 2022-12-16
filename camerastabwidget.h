#ifndef CAMERASTABWIDGET_H
#define CAMERASTABWIDGET_H

#include "issoobject.h"
#include "newcameradialog.h"
#include "issocommondata.h"
#include <QWidget>


namespace Ui {
    class CamerasTabWidget;
}

class CamerasTabWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::CamerasTabWidget *ui;

        IssoObject* m_object = nullptr;
        /// \brief  Установить обработчики событий
        void initEventHandlers();
        /// \brief  Заполнить визуальный список камер
        void fillCameraList(QList<IssoCamera*> cameras);

    public:
        explicit CamerasTabWidget(QWidget *parent = 0);
        ~CamerasTabWidget();
        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);

        void updateTab();


    private slots:
        void processAddCameraBtnClicked();
        void processRemoveCameraBtnClicked();
        void processSaveSettingsBtnClicked();
        void processListCameraSelectionChanged();

};

#endif // CAMERASTABWIDGET_H
