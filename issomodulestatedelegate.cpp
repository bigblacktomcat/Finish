#include "issomodulestatedelegate.h"


IssoModuleStateDelegate::IssoModuleStateDelegate(QObject *parent) : QItemDelegate(parent)
{
}


void IssoModuleStateDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    // толщина рамки
    qreal frameWidth = 2.0;
    // смещение текста
    int textOffset = 10;
    // область отрисовки
    QRectF rect = option.rect;
    // получить состояние (модуля) текущего элемента списка
    IssoModuleState state = (IssoModuleState)index.data(Qt::UserRole).toInt();
    // получить текст текущего элемента списка

    QString title = index.data(Qt::DisplayRole).toString();
    // получить цвет фона согласно состоянию
    QColor backColor = IssoCommonData::backgroundByModuleState(state);
//    QColor backColor = IssoStateListItemDelegate::colorByState(state);


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
    // текст:
    //
    // цвет текста
    QColor textColor = IssoCommonData::foregroundByModuleState(state);
    painter->setPen(textColor);
//    painter->setPen(state == ALARM ? QColor(Qt::white) : QColor(Qt::black));
    // шрифт
    painter->setFont(QFont("MS Shell Dlg 2", 10));
    // вывести текст
    painter->drawText(rect.left() + textOffset, rect.top(),
                      rect.width(), rect.height(),
                      Qt::AlignVCenter | Qt::AlignLeft, title);
    painter->restore();
}
