#ifndef MODULESTABWIDGET_H
#define MODULESTABWIDGET_H

#include "issomodulerequest.h"
#include "issoobject.h"
#include "issomoduledescriptor.h"
#include <QWidget>


namespace Ui {
    class ModulesTabWidget;
}


class ModulesTabWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::ModulesTabWidget *ui;
        IssoObject* m_object = nullptr;

        /// \brief  Установить обработчики событий
        void initEventHandlers();
        void updateFilterWidgets();
        void updateLinkedCameras();
        void updateModuleConfigs();
        void setModuleFilterHandlersEnabled(bool enabled);
        IssoModule* selectedModule();
        int selectedMultiSensorId();

    public:
        explicit ModulesTabWidget(QWidget *parent = 0);
        ~ModulesTabWidget();
        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);

        void updateTab();

    private slots:
        /// \brief  Обработчик кнопки Добавить модуль в систему
        ///         (в список нераспределенных модулей)
        void processModulesAddBtnClicked();
        /// \brief  Обработчик кнопки Удалить модуль из системы
        ///         (удалить из списка нераспределенных модулей и уничтожить)
        void processModulesRemoveBtnClicked();
        /// \brief  Обработчик кнопки Сохранить изменения
        ///         выбранного модуля
        void processBtnSaveModuleSettingsClicked();
        /// \brief  Обработчик кнопки Активировать
        void processBtnActivateModuleClicked();
        /// \brief  Обработчик кнопки Изменить настройки контроллера
        ///         (вызов диалога настройки дескриптора)
        void processBtnSetupDescriptorClicked();
        /// \brief  Обработчик выбора модуля в визуальном списке
        ///         всех модулей системы
        void processModulesSelectionChanged();
        /// \brief  Обработчик изменения здания в фильтре всех модулей системы
        void processBuildFilterIndexChanged(int index);


        /// \brief  Обработчик успешной активации модуля
        void processActivationSucceed(const QString &moduleName,
                                      const IssoModuleDescriptor &desc);
        /// \brief  Обработчик ошибки активации модуля
        void processActivationFailed(const QString &moduleName, IssoCmdId cmdId);

        /// \brief  Обновить визуальный список всех модулей системы
        void updateFilteredModulesList();
        /// \brief  Обновить визуальный список мультидатчиков выбранного модуля
        void updateMultiSensorList();

        void processBtnUpdateModuleConfigClicked();

        void processBtnMsActivateClicked();
        void processBtnMsReadConfigClicked();
        void processBtnMsWriteConfigClicked();
        void processBtnMsResetStateClicked();

    signals:
//        void activationRequested(int moduleId, const IssoModuleDescriptor& descr);
        void readDescriptorRequested(int moduleId);
        void writeDescriptorRequested(int moduleId, const IssoModuleDescriptor& descr);

        void descriptorRead(QString,IssoModuleDescriptor);
        void descriptorWritten(QString,IssoModuleDescriptor);
        void descriptorFailed(QString,IssoCmdId);

        void moduleSettingsChanged(int moduleId);

        void updateModuleConfigRequested(int moduleId);

        void multiSensorActivationRequested(int moduleId, const QList<quint8>& msIds);
        void multiSensorRegActionRequested(IssoMsRegAction action, int moduleId, quint8 msId);

    public slots:
        void processMultiSensorActivated(const QHostAddress& moduleIp, quint8 msId);
        void processMultiSensorRegActionCompleted(IssoMsRegAction action,
                                                    const QHostAddress& moduleIp,
                                                    quint8 msId, bool success);
};

#endif // MODULESTABWIDGET_H
