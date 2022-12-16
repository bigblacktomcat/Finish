#ifndef SENSORSTABWIDGET_H
#define SENSORSTABWIDGET_H

#include "issoobject.h"
#include <QWidget>
#include <QComboBox>


namespace Ui {
    class SensorsTabWidget;
}


class SensorsTabWidget : public QWidget
{
        Q_OBJECT
    private:
        Ui::SensorsTabWidget *ui;

        IssoObject* m_object = nullptr;
        /// \brief  Установить обработчики событий
        void initEventHandlers();

        void displayFixedSensorTypeSettings(IssoParamId paramId);
        void updateFixedSensorTypeSettings(IssoParamId paramId);

        void getFixedSensorTypeGroupBoxValues(IssoParamId paramId,
                                              float& minValue, float& maxValue);
        void setFixedSensorTypeGroupBoxValues(IssoParamId paramId,
                                              float minValue, float maxValue);

    public:
        explicit SensorsTabWidget(QWidget *parent = 0);
        ~SensorsTabWidget();

        /// \brief  Инициализировать виджет
        void setObject(IssoObject* obj);

    private slots:
        /// \brief  Обработчик кнопки Добавить изменяемый тип датчиков
        void processAddUnfixedSensorBtnClicked();
        /// \brief  Обработчик кнопки Удалить изменяемый тип датчиков
        void processRemoveUnfixedSensorBtnClicked();
        /// \brief  Обработчик кнопки Сохранить изменения
        ///         штатных значений фиксированных датчиков
        void processSaveFixedTypeBtnClicked();
        /// \brief  Обработчик кнопки Сохранить изменения
        ///         штатных значений изменяемых датчиков
        void processSaveUnfixedTypeBtnClicked();
        /// \brief  Обработчик выбора изменяемого типа датчков в визуальном списке
        void processUnfixedSensorTypesSelectionChanged();


};

#endif // SENSORSTABWIDGET_H
