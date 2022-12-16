#ifndef ISSOFLOORVIEW_H
#define ISSOFLOORVIEW_H

#include "issocamera.h"
#include "issomodule.h"
#include "issocommondata.h"
#include <QGestureEvent>
#include <QGraphicsView>
#include <QMenu>


class IssoFloorView : public QGraphicsView
{
        Q_OBJECT
    private:
        const qreal ZOOM_FACTOR_DELTA = 0.05;      // шаг масштабирования

        qreal m_scaleFactor = 0;                   // масштаб
        QImage m_background;
        QPoint m_cursorPos;

        QMenu m_contextMenu;

        /// \brief  Обработчик жестов
        bool gestureEvent(QGestureEvent *event);
        /// \brief  Обработчик жеста 2-мя пальцами
        void pinchTriggered(QPinchGesture *gesture);

        /// \brief  Добавить графический объект на сцену
        void addGraphicsItem(QGraphicsItem* item);
        /// \brief  Удалить графический объект со сцены
        void removeGraphicsItem(QGraphicsItem* item);

        void initMenu();
        void updateMenuItems(QGraphicsItem* item);
        void fillMenuItemsFor(IssoSensor* sensor);
        void fillMenuItemsFor(IssoModule* module);
        void fillMenuItemsFor(IssoCamera* camera);
        void fillMenuItemsForView();

    protected:
        virtual void contextMenuEvent(QContextMenuEvent *event);

        /// \brief Переопределенный обработчик события скролла мыши
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

        void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
        /// \brief Переопределенный метод отрисовки заднего фона сцены
        void drawBackground(QPainter *painter, const QRectF &rect);
        /// \brief Переопределенный метод обработчки событий
        virtual bool event(QEvent *event) Q_DECL_OVERRIDE;


    public:
        explicit IssoFloorView(QWidget *parent = nullptr);

        /// \brief  Выбрать (выделить) все модули,
        ///         отображаемые имена которых представлены в списке
        /// \param  displayNames    Список отображаемых имен модулей,
        ///                         которые необходимо выделить
        void selectModules(const QStringList& displayNames);
        void deselectAllModules();

        QRectF visibleRect() const;
        void setVisibleRect(const QRectF& rect, qreal scale);

        void setViewportOffset(const QPointF& offset);

        qreal getScaleFactor() const;
        void setScaleFactor(const qreal &scaleFactor);

        void zoomToCursor(qreal zoomDelta, QPoint widgetCursorPos);

        void fitSceneToViewport();
        void locateViewportOnModule(IssoModule* module);

    signals:
        void visibleRectChanged(const QRectF& rect, qreal getScaleFactor);
//        void сameraTurned(int cameraId, int buildNum, int floorNum);
        void cameraTurned(int cameraId, bool turnOn);
        void sensorActionPerformed(quint8 id, int moduleId, quint8 chainId,
                                   IssoChainType chainType, IssoSensorAction action);
        void objectConfigUpdateRequested();

    private slots:
        void processMenuTriggered(QAction* action);

    public slots:
        /// \brief  Добавить модуль на сцену
        void addModule(IssoModule* module);
        /// \brief  Удалить модуль со сцены
        void removeModule(IssoModule* module);

        /// \brief  Добавить список модулей на сцену
        void addModules(const QList<IssoModule*>& modules);
        /// \brief  Удалить все модули со сцены без уничтожения
        void removeAllModules();

        /// \brief  Добавить камеру на сцену
        void addCamera(IssoCamera* camera);
        /// \brief  Удалить камеру со сцены
        void removeCamera(IssoCamera *camera);

        /// \brief  Добавить список камер на сцену
        void addCameras(const QList<IssoCamera*>& cameras);
        /// \brief  Удалить все камеры со сцены без уничтожения
        void removeAllCameras();

        void setBackgroundImage(const QPixmap& pixmap);
        /// \brief  Установить фоновое изображение
        void setBackgroundImage(const QImage &img);
};

#endif // ISSOFLOORVIEW_H
