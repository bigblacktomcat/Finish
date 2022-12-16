#include "issoobjectitemdelegate.h"


IssoObjectItemDelegate::IssoObjectItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}


QSize IssoObjectItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    int hintWidth = 0;
    // значение ячейки
    QVariant cellValue = index.model()->data(index, Qt::DisplayRole);
    if (cellValue.isValid())
    {
        // текст заголовка
        QString headerText =
                index.model()->headerData(index.column(), Qt::Horizontal).toString();
        // получить текст ячейки
        QString text = cellValue.toString();
        // измерять текст большей длины
        text = (text.length() > headerText.length() ? text : headerText);
        // ширина остальных столбцов =
        // средняя ширина 2х символов (отступы по краям) + ширина строки ячейки
        hintWidth = 2 * option.fontMetrics.averageCharWidth() +
                        option.fontMetrics.width(text);
    }
    else
        hintWidth = 200;
    // вернуть размер ячейки
    return QSize(hintWidth, option.fontMetrics.height());
}


void IssoObjectItemDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    // получить прокси-модель из индекса
    auto model = index.model();
    if (!model)
        return;
    //
    // ВАЖНО!!!
    // проинициализировать настройки стиля
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    //
    // настройки отображения ячейки
    QBrush cellBrush;
    QString cellText = opt.text;
    QRect textRect = opt.rect;
    QRect brushRect = opt.rect;
    //
    // сохранить
    painter->save();
    //
    // задать настройки текста
    QTextOption textOption;
    // запретить перенос слов
    textOption.setWrapMode(QTextOption::NoWrap);
    // настроить колонку
    switch(index.column())
    {
        // Наименование
        case 1:
        // Контактное лицо
        case 2:
        // Телефон
        case 3:
        // Адрес
        case 4:
        {
            // выравнивание по левому краю
            textOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            // задать отступ текста справа
            textRect.adjust(opt.fontMetrics.averageCharWidth(), 0, 0, 0);
            break;
        }
        default:
        {
            // выравнивание по центру
            textOption.setAlignment(Qt::AlignCenter);
            break;
        }
    }
    //
    // получить значение поля Состояние
    QVariant stateValue = model->data(model->index(index.row(), 5));
    // преобразовать в состояние объекта
    IssoModuleState state = IssoCommonData::moduleStateByString(stateValue.toString());
    // залить строку таблицы согласно состоянию объекта
    QColor backColor;
    switch(state)
    {
        case ALARM:
        case INACTIVE:
        {
            backColor = IssoCommonData::backgroundByModuleState(ALARM);
            break;
        }
        default:
        {
            backColor = IssoCommonData::backgroundByModuleState(state);
            break;
        }
    }
    // выбрана ли строка
    bool selected = opt.state & QStyle::State_Selected;
    // если строка выбрана, затемнить фон
    cellBrush = QBrush(selected ? backColor.darker(130) : backColor);
    // залить фон
    painter->fillRect(brushRect, cellBrush);
    // отобразить текст
    painter->drawText(textRect, cellText, textOption);
    //
    // восстановить
    painter->restore();
}

//void IssoObjectItemDelegate::paint(QPainter *painter,
//                                   const QStyleOptionViewItem &option,
//                                   const QModelIndex &index) const
//{
//    painter->save();
//    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

//    // область отрисовки
//    QRectF rect = option.rect;
//    // толщина рамки
//    qreal frameWidth = 2.0;
//    // получить текст текущего элемента списка
//    QString title = index.data(Qt::DisplayRole).toString();

//    // получить прокси-модель из индекса
//    const QSortFilterProxyModel* model =
//            static_cast<const QSortFilterProxyModel*>(index.model());
//    if (model)
//    {
//        // получить значение поля comment (стоблец 6) выбранной записи
//        QVariant stateValue = model->data(model->index(index.row(), 5));
//        // преобразовать в состояние объекта
//        IssoModuleState state = IssoCommonData::moduleStateByString(stateValue.toString());
//        // залить строку таблицы согласно состоянию объекта
//        QColor backColor, textColor;
//        switch(state)
//        {
//            case ALARM:
//            case INACTIVE:
//            {
//                backColor = IssoCommonData::backgroundByModuleState(ALARM);
//                textColor = IssoCommonData::foregroundByModuleState(ALARM);
//                break;
//            }
//            default:
//            {
//                backColor = IssoCommonData::backgroundByModuleState(state);
//                textColor = IssoCommonData::foregroundByModuleState(state);
//                break;
//            }
//        }

//        // если курсор над элементом, затемнить фон
//        if (option.state & QStyle::State_MouseOver)
//            backColor = backColor.darker(130);

//        // отобразить выделенный элемент списка
//        if (option.state & QStyle::State_Selected)
//        {
//            // фон + рамка:
//            //
//            // обводка
//            painter->setPen(QPen(QColor(Qt::black), frameWidth));
//            // заливка
//            painter->setBrush(backColor);
//            // уменшить область отрисовки рамки на половину толщины обводки
//            qreal delta = frameWidth / 2;
//            rect.adjust(delta, delta, -delta, -delta);
//        }
//        // отобразить невыделенный элемент списка
//        else
//        {
//            // фон + рамка:
//            //
//            // обводка того же цвета, что и фон
//            painter->setPen(backColor);
//            // заливка
//            painter->setBrush(backColor);
//        }
//        // залить прямоугольник (фон + рамка)
//        painter->drawRect(rect);
//        //
//        // текст:
//        //
//        // цвет текста
//        painter->setPen(textColor);
//        // шрифт
////        int fontSize = (m_guiMode == IssoGuiMode::STANDART_MONITOR ? 10 : 14);
////        painter->setFont(QFont("MS Shell Dlg 2", fontSize));
//        // вывести текст
//        painter->drawText(rect.left() + 10, rect.top(),
//                          rect.width(), rect.height(),
//                          Qt::AlignVCenter | Qt::AlignLeft, title);
//    }

//    painter->restore();
//}

