#ifndef ISSOANALOGSENSORSETUPPANEL_H
#define ISSOANALOGSENSORSETUPPANEL_H

#include <QGroupBox>
#include <QMap>
#include "issocommondata.h"


namespace Ui {
    class IssoAnalogSensorSetupPanel;
}


class IssoAnalogSensorSetupPanel : public QGroupBox
{
        Q_OBJECT
    private:
        Ui::IssoAnalogSensorSetupPanel *ui;

        QMap<IssoState, QWidget*> m_stateWidgets;

        void init();
    public:
        explicit IssoAnalogSensorSetupPanel(QWidget *parent = 0);
        ~IssoAnalogSensorSetupPanel();

        void setStateRange(const IssoState& state, double min, double max);
        void getStateRange(const IssoState& state, double& min, double& max);

        QMap<IssoState, IssoStateRange> stateRanges();
        void setStateRanges(const QMap<IssoState, IssoStateRange>& ranges);
        void clearStateRanges();

        QSet<IssoState> displayingStates() const;
        void setDisplayingStates(const QSet<IssoState> states);
};

#endif // ISSOANALOGSENSORSETUPPANEL_H
