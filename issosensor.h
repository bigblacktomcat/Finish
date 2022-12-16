#ifndef ISSOSENSOR_H
#define ISSOSENSOR_H

#include <QObject>
#include <QGraphicsObject>
#include <QTimer>
#include "issocommondata.h"


class IssoSensor : public QGraphicsObject
{
        Q_OBJECT
    private:
        const int TOP_TITLE_FONT_SIZE = 8;
        const int BOTTOM_TITLE_FONT_SIZE = 10;
        const int TIMER_FONT_SIZE = 20;

        quint8 m_id;
        int m_moduleUniqueId;
        int m_moduleDisplayId;
        quint8 m_chainId;
        IssoChainType m_chainType;
        //
        QString m_locationInfo = "";
        //
        IssoState m_state;

        bool m_activated = false;
        QTimer m_blinkTimer;
        QTimer m_moveDetectionTimer;
        quint32 m_detectionSeconds = 0;

        //
        // настройки графического отображения
        QRectF m_rect;              // область, занимаемая модулем на сцене
        QColor m_brushColor;         // текущий цвет заливки
        QColor m_penColor;         // текущий цвет надписи
        QPainterPath m_shape;       // форма
        QRectF m_topTitleRect;      // область отрисовки верхнего заголовка
        QRectF m_bottomTitleRect;   // область отрисовки нижнего заголовка

        QPainterPath m_textPath;
        QPainterPath m_iconPath;

        /// \brief  Сформировать геометрические параметры отображения камеры
        void createShape();
        /// \brief  Получить верхний заголовок (здание-этаж)
        QString topTitle() const;
        /// \brief  Получить нижний заголовок (номер модуля)
        QString bottomTitle() const;

        void startBlinking(const QColor& textColor1, const QColor& fillColor1,
                           const QColor &textColor2, const QColor &fillColor2,
                           int interval = 300);
        void stopBlinking();

        void startMoveDetectionTimer();
        void stopMoveDetectionTimer();

        QPainterPath makeIcon(qreal iconSize);
        QPainterPath makeTextPath(const QFont& font, const QString& text);

    public:
        static const int DEFAULT_SIZE = 60;

        IssoSensor();
        explicit IssoSensor(quint8 id, int moduleUniqueId, int moduleDisplayId,
                            quint8 chainId, IssoChainType type,
                            QGraphicsItem *parent = nullptr);
        ~IssoSensor();

        /// \brief  Загрузить данные датчика из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные датчика в поток
        void saveToStream(QDataStream &stream);

        /// \brief  Определить границы компонента
        QRectF boundingRect() const Q_DECL_OVERRIDE;
        /// \brief  Определить форму компонента, чтобы точнее обходить коллизии
        QPainterPath shape() const Q_DECL_OVERRIDE;
        /// \brief  Метод отрисовки компонента
        void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
                   QWidget * widget = 0) Q_DECL_OVERRIDE;

        quint8 id() const;
        int moduleUniqueId() const;
        int moduleDisplayId() const;
        quint8 chainId() const;
        IssoChainType chainType() const;

        IssoState state() const;
        void setState(IssoState state);

        QColor fillColor() const;
        QColor textColor() const;
        void setColors(const QColor &penColor, const QColor &brushColor);

        void setMovable(bool movable);


        bool activated() const;
        void setActivated(bool activated);

        QString locationInfo() const;
        void setLocationInfo(const QString &locationInfo);

};

#endif // ISSOSENSOR_H
