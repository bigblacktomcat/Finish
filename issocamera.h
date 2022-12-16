#ifndef ISSOCAMERA_H
#define ISSOCAMERA_H

#include <QObject>
#include <QGraphicsObject>
#include <QHostAddress>
#include <QDataStream>


enum IssoCameraSendMode
{
    CAMERA_SEND_OFF,
    CAMERA_SEND_PHOTO,
    CAMERA_SEND_VIDEO
};
Q_DECLARE_METATYPE(IssoCameraSendMode)



class IssoCamera : public QGraphicsObject
{
        Q_OBJECT

    public:
        IssoCamera();
        explicit IssoCamera(int id, const QHostAddress& ip, quint16 port,
                            const QString& password,
                            int buildNum = 0, int floorNum = 0,
                            QGraphicsItem *parent = nullptr);

        /// \brief  Загрузить данные камеры из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные камеры в поток
        void saveToStream(QDataStream &stream);


        /// \brief  Определить границы компонента
        QRectF boundingRect() const Q_DECL_OVERRIDE;
        /// \brief  Определить форму компонента, чтобы точнее обходить коллизии
        QPainterPath shape() const Q_DECL_OVERRIDE;
        /// \brief  Метод отрисовки компонента
        void paint(QPainter * painter,
                   const QStyleOptionGraphicsItem * option,
                   QWidget * widget = 0) Q_DECL_OVERRIDE;

        bool online() const;
        void setOnline(bool online);

        int id() const;

        QHostAddress ip() const;
        void setIp(const QHostAddress &ip);

        quint16 port() const;
        void setPort(const quint16 &port);

        int buildNum() const;
        void setBuildNum(int buildNum);

        int floorNum() const;
        void setFloorNum(int floorNum);

        QString displayName() const;

        QString password() const;
        void setPassword(const QString &password);

        void setMovable(bool movable);

        IssoCameraSendMode sendMode() const;
        void setSendMode(const IssoCameraSendMode &sendMode);

    private:
        // размер графического представления
        const int DEFAULT_WIDTH = 60;
        const int DEFAULT_HEIGHT = 30;
        // размер шрифта заголовка
        const int TITLE_FONT_SIZE = 10;
        // цвет камеры
        const QColor ONLINE_COLOR = QColor(Qt::blue);
        const QColor OFFLINE_COLOR = QColor(30, 30, 30);

        int m_id;
        QHostAddress m_ip;
        quint16 m_port;
        QString m_password;
        int m_buildNum;
        int m_floorNum;

        IssoCameraSendMode m_sendMode = IssoCameraSendMode::CAMERA_SEND_OFF;

        bool m_online;

        //
        // настройки графического отображения
        QRectF m_rect;              // область, занимаемая модулем на сцене
        QRectF m_titleRect;         // область отрисовки заголовка
        QPainterPath m_shape;       // форма
        QColor m_fillColor;         // текущий цвет заливки

        /// \brief  Сформировать геометрические параметры отображения камеры
        void createShape();
};

#endif // ISSOCAMERA_H
