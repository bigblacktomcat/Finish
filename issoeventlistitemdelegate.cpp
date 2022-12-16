#include "issoeventlistitemdelegate.h"
#include <QColor>


IssoEventListItemDelegate::IssoEventListItemDelegate(QObject *parent) : QItemDelegate(parent)
{
}


void IssoEventListItemDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    // получить текст текущего элемента списка
    QString title = index.data(Qt::DisplayRole).toString();
    // получить флаг тревожности текущего события
    bool alarmed = index.data(Qt::UserRole).toBool();
    // установить цвет текста согласно флагу тревожности
    QColor textColor = (alarmed ? QColor(Qt::red) : QColor(Qt::black));
    // получить цвет фона согласно состоянию
    bool selected = (option.state & QStyle::State_Selected);
    QColor backColor = (selected ? option.palette.background().color().lighter(120) :
                                   option.palette.background().color());
    // если курсор над элементом, затемнить фон
    if (option.state & QStyle::State_MouseOver)
        backColor = backColor.lighter(110);
    //
    // фон + рамка:
    //
    //  цвет рамки
    QColor frameColor = (selected ? QColor(Qt::black) : backColor);
    painter->setPen(frameColor);
    // размер рамки
    QRectF r = option.rect;
    r.adjust(0, 0, -1, -1);
    // заливка
    painter->setBrush(backColor);
    // залить фон и рамку
    painter->drawRect(r);
    //
    // текст:
    //
    // цвет текста
    painter->setPen(textColor);
    // шрифт
    painter->setFont(QFont("MS Shell Dlg 2", 10, QFont::Medium));
    // вывести текст
    painter->drawText(r, title);
    painter->restore();
}
