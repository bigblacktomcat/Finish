#ifndef DESCRIPTORSETUPDIALOG_H
#define DESCRIPTORSETUPDIALOG_H

#include "issomodule.h"
#include "issomoduledescriptor.h"

#include <QDialog>
#include <QHostAddress>


namespace Ui {
    class DescriptorSetupDialog;
}


class DescriptorSetupDialog : public QDialog
{
        Q_OBJECT
    private:
        Ui::DescriptorSetupDialog *ui;

        QList<IssoModule*> m_modules;

        void init();

        /// \brief  Найти модуль по отображаемому имени
        IssoModule* findModuleByDisplayName(const QString& displayName);
        QStringList moduleNames() const;
        void initWidgetEventHandlers();

        void setDescriptorToControls(const IssoModuleDescriptor& descr);
        IssoModuleDescriptor getDescriptorFromControls();

    public:
        explicit DescriptorSetupDialog(QList<IssoModule*> modules, QWidget *parent = 0);
        ~DescriptorSetupDialog();

        void setCurrentModule(const QString& displayName);

    signals:
        void descriptorRequested(int moduleId);
        void descriptorChanged(int moduleId, const IssoModuleDescriptor& descr);

    private slots:
        void processCbModulesCurrentIndexChanged(const QString& moduleName);
        void processBtnReadDescriptorClicked();
        void processBtnWriteDescriptorClicked();
        void processDescriptorRead(const QString& moduleDisplayName,
                                       const IssoModuleDescriptor& descr);
        void processDescriptorWritten(const QString& moduleDisplayName,
                                      const IssoModuleDescriptor& descr);
        void processDescriptorFailed(const QString& moduleDisplayName,
                                     IssoCmdId cmdId);
};

#endif // DESCRIPTORSETUPDIALOG_H
