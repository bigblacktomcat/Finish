#ifndef FLOORPLANVIEW_H
#define FLOORPLANVIEW_H

#include <QObject>
#include <QFrame>
#include <QGraphicsView>
#include <QDebug>
#include <QList>
#include "issocamera.h"
#include "issomodule.h"


/*!
 * \brief   Класс компонента, отображающего план этажа здания.
 * \details Включает список модулей МАСО, расположенных на данном этаже.
 */
class IssoFloorPlan : public QObject
{
        Q_OBJECT
    private:
        int m_number;                       // номер этажа в здании
        int m_buildNum;                     // номер здания
        qreal m_scale;                      // масштаб
        QRectF m_visibleRect;               // область плана, отображаемая в виждете

        QString m_backgroundBase64;
        QSize m_backgroundSize;

        QPointF m_curModulePos;             // текущая позиция для добавления модуля

        QList<IssoModule*> m_modules;
        QList<IssoCamera*> m_cameras;

    public:
        IssoFloorPlan();
        explicit IssoFloorPlan(int number, int buildNum, QObject *parent = nullptr);
        ~IssoFloorPlan();


        /// \brief  Загрузить данные этажа из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные этажа в поток
        void saveToStream(QDataStream &stream);

        QPointF nextModulePosition();

        int nextAvailableModuleDisplayId();
        /// \brief  Добавить модуль на этаж
        void addModule(IssoModule* module);
        /// \brief  Удалить модуль с этажа
        bool removeModule(IssoModule* module);
        /// \brief  Удалить все модули со сцены без уничтожения
        void removeAllModules();

        /// \brief  Найти камеру по идентификатору
        IssoCamera* findCameraById(int id);
        /// \brief  Добавить камеру на этаж
        void addCamera(IssoCamera* camera);
        /// \brief  Добавить камеру с этажа
        bool removeCamera(IssoCamera* camera);
        /// \brief  Удалить все камеры со сцены без уничтожения
        void removeAllCameras();

        /// \brief  Найти модуль по отображаемому имени (должно быть уникально)
        IssoModule* findByDisplayName(const QString& displayName) const;
        /// \brief  Найти модуль по идентификатору
        IssoModule* findById(int id) const;
        /// \brief  Найти модуль по адресу
        IssoModule* findByAddress(QHostAddress addr) const;
        /// \brief  Получить номер этажа
        int number() const;
        /// \brief  Получить номер здания
        int buildNum() const;
        /// \brief  Получить отображаемое имя этажа
        QString displayName() const;

        /// \brief  Получить имена всех модулей этажа
        void moduleNames(QStringList& names);

        /// \brief  Получить список модулей, имеющих заданное состояние
        QStringList moduleNamesByState(IssoModuleState moduleStates);

        /// \brief  Получить имена всех выбранных модулей этажа
        void selectedModuleNames(QStringList& names);
        /// \brief  Сформировать список всех модулей этажа
        QList<IssoModule *> modules();
        QList<IssoCamera *> cameras() const;

        /// \brief  Получить выбранный модуль
        IssoModule* selectedModule();

        /// \brief  Получить набор состояний модулей этажа
        QSet<IssoModuleState> moduleStates() const;


        int alarmCount() const;

        bool activated() const;

        QImage background();
        void setBackground(const QImage &background);

        qreal scale() const;
        void setScale(const qreal &scale);

        QRectF visibleRect() const;
        void setVisibleRect(const QRectF &visibleRect);

        QSize backgroundSize() const;

    signals:
        void moduleAdded(IssoModule* module);
        void moduleRemoved(IssoModule* module);

        void cameraAdded(IssoCamera* camera);
        void cameraRemoved(IssoCamera* camera);

        void backgroundChanged(const QImage& background);

    public slots:
        void changeVisibleRect(const QRectF& rect, qreal scale);
};

#endif // FLOORPLANVIEW_H
