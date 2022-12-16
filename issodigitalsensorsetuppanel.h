#ifndef ISSODIGITALSENSORSETUPPANEL_H
#define ISSODIGITALSENSORSETUPPANEL_H

#include <QGroupBox>
#include "issocommondata.h"


namespace Ui {
    class IssoDigitalSensorSetupPanel;
}


class IssoDigitalSensorSetupPanel : public QGroupBox
{
        Q_OBJECT
    private:
        Ui::IssoDigitalSensorSetupPanel *ui;

    public:
        explicit IssoDigitalSensorSetupPanel(QWidget *parent = 0);
        ~IssoDigitalSensorSetupPanel();

//        int normalValue() const;
//        void setNormalValue(int value);

        int warningValue() const;
        void setWarningValue(int value);

        int alarmValue() const;
        void setAlarmValue(int value);

        IssoStateCondition condition() const;
        void setCondition(IssoStateCondition condition);

        QMap<IssoState, int> stateValues() const;
        void setStateValues(QMap<IssoState, int> stateValueMap);
};

#endif // ISSODIGITALSENSORSETUPPANEL_H
