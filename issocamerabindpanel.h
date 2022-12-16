#ifndef ISSOCAMERABINDPANEL_H
#define ISSOCAMERABINDPANEL_H

#include <QComboBox>
#include <QGroupBox>
#include "issocamera.h"
#include "issocommondata.h"

namespace Ui {
    class IssoCameraBindPanel;
}

class IssoCameraBindPanel : public QGroupBox
{
        Q_OBJECT

    private:
        Ui::IssoCameraBindPanel *ui;

        QMap<IssoParamId, QComboBox*> m_combos;

        void initCombos();

    public:
        explicit IssoCameraBindPanel(QWidget *parent = 0);
        ~IssoCameraBindPanel();

        void setCameraList(QList<IssoCamera*> cameras);

        QMap<IssoParamId, int> boundCameras();
        void setBoundCameras(const QMap<IssoParamId, int>& boundCameras);
};

#endif // ISSOCAMERABINDPANEL_H
