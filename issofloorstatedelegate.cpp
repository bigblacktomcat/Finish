#include "issofloorstatedelegate.h"


IssoFloorStateDelegate::IssoFloorStateDelegate(QObject *parent) : QItemDelegate(parent)
{
}


void IssoFloorStateDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // область отрисовки
    QRectF rect = option.rect;
    // толщина рамки
    qreal frameWidth = 2.0;

    // размер пиктограммы
    qreal iconSize = 10.0;
    // расстояние между пиктограммами
    qreal iconShift = 3;

    // смещение текста
    int textOffset = 3 * (iconSize + iconShift) + 10;
    // получить состояние (модуля) текущего элемента списка
    IssoModuleState state = (IssoModuleState)index.data(Qt::UserRole).toInt();
    // получить текст текущего элемента списка
    QString title = index.data(Qt::DisplayRole).toString();
    // получить цвет фона согласно состоянию
    QColor backColor = QColor(230, 230, 230);
//    QColor backColor = IssoCommonData::backgroundByModuleState(state);


    // если курсор над элементом, затемнить фон
    if (option.state & QStyle::State_MouseOver)
        backColor = backColor.darker(130);

    // отобразить выделенный элемент списка
    if (option.state & QStyle::State_Selected)
    {
        // фон + рамка:
        //
        // обводка
        painter->setPen(QPen(QColor(Qt::black), frameWidth));
        // заливка
        painter->setBrush(backColor);
        // уменшить область отрисовки рамки на половину толщины обводки
        qreal delta = frameWidth / 2;
        rect.adjust(delta, delta, -delta, -delta);
    }
    // отобразить невыделенный элемент списка
    else
    {
        // фон + рамка:
        //
        // обводка того же цвета, что и фон
        painter->setPen(backColor);
        // заливка
        painter->setBrush(backColor);
    }
    // залить прямоугольник (фон + рамка)
    painter->drawRect(rect);
    //
    // пиктограммы:
    QRectF iconRect(rect.left() + iconShift,
                    rect.top() + (rect.height() - iconSize) / 2,
                    iconSize, iconSize);
    painter->setPen(QColor(Qt::black));
    // тревога
    if (index.data(Qt::UserRole + 1).toBool())
    {
        painter->setBrush(IssoCommonData::backgroundByModuleState(ALARM));
        painter->drawRect(iconRect);
    }
    // сместить пиктограмму
    iconRect.translate(iconSize + iconShift, 0);
    // неисправность:
    if (index.data(Qt::UserRole + 2).toBool())
    {
        painter->setBrush(IssoCommonData::backgroundByModuleState(FAIL));
        painter->drawRect(iconRect);
    }
    // сместить пиктограмму
    iconRect.translate(iconSize + iconShift, 0);
    // оффлайн
    if (index.data(Qt::UserRole + 3).toBool())
    {
        painter->setBrush(IssoCommonData::backgroundByModuleState(INACTIVE));
        painter->drawRect(iconRect);
    }
    //
    // текст:
    //
    // цвет текста
    QColor textColor = QColor(Qt::black);
    painter->setPen(textColor);
//    painter->setPen(state == ALARM ? QColor(Qt::white) : QColor(Qt::black));
//    // шрифт
//    painter->setFont(QFont("MS Shell Dlg 2", 10));
    // вывести текст
    painter->drawText(rect.left() + textOffset, rect.top(),
                      rect.width(), rect.height(),
                      Qt::AlignVCenter | Qt::AlignLeft, title);
    painter->restore();
}
