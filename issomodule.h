#ifndef ISSOMODULE_H
#define ISSOMODULE_H

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <QStyleOptionGraphicsItem>
#include <QThread>
#include <QHostAddress>
#include <QGraphicsScene>
#include "issochainconfig.h"
#include "issocommondata.h"
#include "issomoduledescriptor.h"
#include "issoparambuffer.h"
#include "issosensorchain.h"
#include "htmlutils.h"



enum IssoSensorDisplayMode
{
    SENSOR_DISPLAY_ALL,
    SENSOR_DISPLAY_ALARM,
    SENSOR_HIDE_ALL
};
Q_DECLARE_METATYPE(IssoSensorDisplayMode)


/*!
 * \brief   Класс модуля МАСО
 * \details Представляет графический компонент, отображающий
 *          расположение и состояние модуля МАСО
 */
class IssoModule : public QGraphicsObject
{
        Q_OBJECT
    private:
        const int TOP_TITLE_FONT_SIZE = 10;
        const int BOTTOM_TITLE_FONT_SIZE = 14;
        // цвет фона
        const QColor SELECT_COLOR = QColor(0, 160, 225);
        // цвет надписи
        const QColor SELECT_TEXT_COLOR = QColor(Qt::white);
        //
        int m_uniqueId;             // уникальный идентификатор модуля
        int m_displayId;            // отображаемый идентификатор модуля
        bool m_activated = false;   // признак активации
        // дескриптор
        QHostAddress m_ip;
        quint16 m_port;
        QByteArray m_mac;
        QHostAddress m_serverIp;
        quint16 m_serverPort;
        QByteArray m_phone;
        bool m_phoneEnabled = false;
        //
        int m_floorNum;             // номер этажа
        int m_buildNum;             // номер здания
        // периферия:
        // камера
        bool m_cameraEnabled = false;
        int m_cameraId = 0;
        // табло
        bool m_boardEnabled = false;
        QHostAddress m_boardIp;
        quint16 m_boardPort = 0;

        IssoParamBuffer m_stateParams;

        QList<QSharedPointer<IssoParam>> m_changedParams;

        // имя конфига модуля
        QString m_moduleConfigName;
        // карта соответствия ID шлейфа графическому объекту шлейфа
        QMap<IssoParamId, QSharedPointer<IssoSensorChain>> m_chains;

        // коллекция ID мультидатчиков, находящихся в сервисном режиме
        QSet<quint8> m_serviceMultiSensorIds;
        //
        // настройки графического отображения
        QRectF m_rect;              // область, занимаемая модулем на сцене
        QColor m_fillColor;         // текущий цвет заливки
        QColor m_textColor;         // текущий цвет надписи
        QPainterPath m_shape;       // форма
        QRectF m_topTitleRect;      // область отрисовки верхнего заголовка
        QRectF m_bottomTitleRect;   // область отрисовки нижнего заголовка
        //
        IssoModuleState m_moduleState = INACTIVE; // состояние модуля
        //
        IssoSensorDisplayMode m_sensorDisplayMode = SENSOR_DISPLAY_ALARM;  // режим отображения датчиков
        //
        /// \brief  Сформировать геометрические параметры отображения модуля
        void createShape();
        /// \brief  Получить верхний заголовок (здание-этаж)
        QString topTitle() const;
        /// \brief  Получить нижний заголовок (номер модуля)
        QString bottomTitle() const;

        void insertSensorChain(IssoParamId id, IssoSensorChain* chain);
        void removeSensorChain(IssoParamId id);
        QPointF calcInitChainPosition(IssoParamId id);

        /// \brief  Задать состояние модуля
        void setModuleState(const IssoModuleState &moduleState);

        IssoSensorDisplayMode setSensorDisplayMode(IssoSensorDisplayMode mode);
        void updateSensorsVisibility(IssoSensorChain* chain);


    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
        void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    public:
        static const int DEFAULT_WIDTH = 60;
        static const int DEFAULT_HEIGHT = 50;

        IssoModule();
        /*!
         * \brief Конструктор графического объекта ИССО модуля
         * \param id            отображаемый идентификатор
         * \param x             x-координата компонента
         * \param y             y-координата компонента
         * \param width         ширина компонента
         * \param height        длина компонента
         * \param ??? activeColor   цвет активного состояния
         *                      (инициализирует константу ACTIVE_COLOR)
         * \param ??? inactiveColor цвет неактивного состояния
         *                      (инициализирует константу INACTIVE_COLOR)
         * \param ??? alarmColor    цвет тревожного состояния
         *                      (инициализирует константу ALARM_COLOR)
         */
        IssoModule(int uniqueId, int displayId, qreal x, qreal y,
                   const QHostAddress& ip, quint16 port,
                   const QByteArray& mac,
                   const QHostAddress& serverip,
                   quint16 serverPort = IssoConst::RECV_PORT,
                   qreal width = DEFAULT_WIDTH,
                   qreal height = DEFAULT_HEIGHT,
                   int floorNum = 0,
                   int buildNum = 0,
                   QGraphicsItem *parent = nullptr);
        IssoModule(const IssoModule& other);
        ~IssoModule();

        /// \brief  Загрузить данные модуля из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные модуля в поток
        void saveToStream(QDataStream &stream);


        /// \brief  Определить границы компонента
        QRectF boundingRect() const Q_DECL_OVERRIDE;
        /// \brief  Определить форму компонента, чтобы точнее обходить коллизии
        QPainterPath shape() const Q_DECL_OVERRIDE;
        /// \brief  Метод отрисовки компонента
        void paint(QPainter * painter,
                   const QStyleOptionGraphicsItem * option,
                   QWidget * widget = 0) Q_DECL_OVERRIDE;

        qreal width() const;
        qreal height() const;

        /// \brief  Получить уникальный идентификатор модуля
        int uniqueId() const;
        /// \brief  Установить уникальный идентификатор модуля
        void setUniqueId(int uniqueId);
        /// \brief  Получить отображаемый идентификатор модуля
        int displayId() const;
        /// \brief  Установить отображаемый идентификатор модуля
        void setDisplayId(int displayId);
        /// \brief  Получить номер этажа
        int floorNum() const;
        /// \brief  Установить номер этажа
        void setFloorNum(int floorNum);
        /// \brief  Получить номер здания
        int buildNum() const;
        /// \brief  Установить номер здания
        void setBuildNum(int buildNum);
        /// \brief  Получить отображаемое имя модуля
        QString displayName() const;
        /// \brief  Получить ip-адрес модуля
        QHostAddress ip() const;
        /// \brief  Установить ip-адрес модуля
        void setIp(const QHostAddress &ip);
        /// \brief  Получить порт модуля
        quint16 port() const;
        /// \brief  Установить порт модуля
        void setPort(const quint16 &port);

        /// \brief  Определить, размещен ли модуль на этаже.
        bool isLocated() const;

        /// \brief  Получить состояние модуля
        IssoModuleState moduleState() const;

        /// \brief  Получить тревожное состояние модуля
        bool alarmed() const;
        /// \brief  Задать разрешение перемещения модуля
        void setMovable(bool movable);
        /// \brief  Получить признак активации модуля
        bool isActivated() const;
        /// \brief  Задать признак активации модуля
        void setActivated(bool activated);

        /// \brief  Получить дескриптор модуля
        IssoModuleDescriptor getDescriptor() const;
        /// \brief  Задать дескриптор модуля
        void setDescriptor(const IssoModuleDescriptor& descr);

        //
        //
        // устаревшие методы
        bool getCameraEnabled() const;
        int getCameraId() const;
        bool getBoardEnabled() const;
        QHostAddress getBoardIp() const;
        quint16 getBoardPort() const;
        void setCameraEnabled(bool enabled, int cameraId);
        void setBoardEnabled(bool enabled, const QHostAddress& boardIp, quint16 boardPort);
        //
        //

        void clearStateParams();
        bool updateStateParam(const QSharedPointer<IssoParam> &paramPtr);
        bool updateMultiSensorStateParam(const QSharedPointer<IssoParam> &paramPtr);

        void setOnline();
        void setOffline();

        IssoParamBuffer stateParams() const;
        IssoSensorStateParam *stateParamById(const IssoParamId& paramId);

        void setParamBuffer(const IssoParamBuffer &params);

        QString moduleConfigName() const;
        void setModuleConfigName(const QString &moduleConfigName);


        void addNewChain(IssoParamId paramId, IssoChainConfig* chainConfig);
        void updateChain(IssoParamId paramId, IssoChainConfig* chainConfig);
        void updateChains(QMap<IssoParamId, IssoChainConfig*> chainConfigs);

        void removeAllSensorChains();

        void updateAnalogChainState(IssoSensorStateParam *param);
        void updateAddressChainState(IssoMultiSensorStateParam *param);

        QMap<IssoParamId, QSharedPointer<IssoSensorChain> > sensorChains() const;

        IssoSensorChain* sensorChain(IssoParamId id);

        void displayAllSensors();
        void displayAlarmSensors();
        void hideAllSensors();


        QList<QSharedPointer<IssoParam> > changedParams() const;
        void clearChangedParams();

        void analyzeSensorStates();

        static int paramIdToChainId(IssoParamId paramId);
        static IssoParamId chainIdToParamId(int chainId, IssoChainType chainType);

        bool hasServiceMultiSensorId(quint8 multiSensorId);
        void setMultiSensorServiceMode(quint8 multiSensorId, bool serviceMode);
        void setMultiSensorActivated(quint8 multiSensorId, bool activated);

        int msTotalCount();
        int msAlarmCount();
};

#endif // ISSOMODULE_H
