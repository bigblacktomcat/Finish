#ifndef BUILDINITWIDGET_H
#define BUILDINITWIDGET_H

#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableView>
#include <QFormLayout>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QImage>
#include <spinboxdelegate.h>
#include <QLabel>
#include <QCheckBox>
#include "issocommondata.h"


class BuildInitWidget : public QGroupBox
{
        Q_OBJECT
    private:
        int m_buildNum;
        bool m_ok = false;
        QMap<QString, QString> m_base64Images;

        QComboBox* g_cbLowerFloorNum;
        QSpinBox* g_sbFloorCount;
        QLabel* g_lblImagesLoaded;
        QTableView* g_tvModules;
        QPushButton* g_btnLoad;
        QCheckBox* g_chkFixedSensors;

        void init();
        void setWidgetHandlers();
        void setLoadedImageCount(int count);

    public:
        explicit BuildInitWidget(int buildNum, QWidget *parent = nullptr);

        int buildNumber();
        QString displayName();
        int lowerFloorNumber();
        int floorCount();
        QSet<IssoParamId> checkableParams();

        QMap<int, int> moduleCountMap();
        QMap<QString, QString> base64ImageMap();
        bool isOk() const;

//        int moduleCountByFloorNumber(int floorNumber);
//        bool imagesLoaded() const;
//        QString base64ImageByFloorName(const QString& floorName);

    signals:

    private slots:
        void processFloorsChanged();
        void processBtnLoadClicked();
};

#endif // BUILDINITWIDGET_H
