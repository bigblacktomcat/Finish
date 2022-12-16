#include "issocommondata.h"
#include "issoeventdelegate.h"
#include "issoeventquerymodel.h"
#include "issoparam.h"

#include <QSortFilterProxyModel>
#include <QStyleOptionViewItemV4>


IssoEventDelegate::IssoEventDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
}


QSize IssoEventDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    int hintWidth = 0;
    // значение ячейки
    QVariant cellValue = index.data();
    if (cellValue.isValid())
    {
        // текст заголовка
        QString headerText =
                index.model()->headerData(index.column(), Qt::Horizontal).toString();
        // получить текст ячейки
        QString text = cellValue.toString();
//        // измерять текст большей длины
//        text = (text.length() > headerText.length() ? text : headerText);

        if (index.column() == IssoEventQueryModel::COL_PHOTO)
            text = headerText;
        else
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


void IssoEventDelegate::paint(QPainter *painter,
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
    //
    // задать настройки текста
    QTextOption textOption;
    // запретить перенос слов
    textOption.setWrapMode(QTextOption::NoWrap);
    // настроить колонку
    switch(index.column())
    {
        // Детализация
        case IssoEventQueryModel::COL_DETAILS/*7*/:
        // Расположение
        case IssoEventQueryModel::COL_LOCATION/*9*/:
        {
            // выравнивание по левому краю
            textOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            // задать отступ текста справа
            textRect.adjust(opt.fontMetrics.averageCharWidth(), 0, 0, 0);
            break;
        }
        // Фото
        case IssoEventQueryModel::COL_PHOTO/*8*/:
        {
            // если фото есть, указать это
            cellText = (index.data().isNull() ? "" : "Есть");
            // выравнивание по центру
            textOption.setAlignment(Qt::AlignCenter);
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
    // получить значение поля comment (стоблец 6) выбранной записи
    QVariant commentValue = model->data(model->index(index.row(), IssoEventQueryModel::COL_COMMENT/*6*/));
    // пуст ли comment (необработанная тревога)
    bool alarmed = commentValue.isNull();
    // выбрана ли строка
    bool selected = opt.state & QStyle::State_Selected;
    //
    // выбрать цвет фона:
    // если событие - необработанная тревога, то красный фон, иначе белый
    QColor backColor = (alarmed ? QColor(255, 0, 0, 150) : QColor(Qt::white));
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

//void IssoEventDelegate::paint(QPainter *painter,
//                              const QStyleOptionViewItem &option,
//                              const QModelIndex &index) const
//{
////    // получить SQL-модель из индекса
////    const QSqlQueryModel* model = qobject_cast<const QSqlQueryModel*>(index.model());
////    if (model)
////    {
////        // получить выбранную запись
////        QSqlRecord rec = model->record(index.row());
////        if (!rec.isEmpty())
////        {
////            // получить значение поля comment
////            QSqlField field = rec.field("comment");
////            // если comment пуст (необработанная тревога)
////            if (field.isNull())
////            {
////                // залить фон красным цветом
////                QBrush brush = QBrush(QColor(255, 0, 0, 150));
////                painter->fillRect(option.rect, brush);
////            }
////        }
////    }

//    // получить прокси-модель из индекса
//    auto model = index.model();
//    if (!model)
//        return;

//    // получить значение поля comment (стоблец 6) выбранной записи
//    QVariant value = model->data(model->index(index.row(), 6));
//    // если comment пуст (необработанная тревога)
//    if (value.isNull())
//    {
//        // залить фон красным цветом
//        QBrush brush = QBrush(QColor(255, 0, 0, 150));
//        painter->fillRect(option.rect, brush);
//    }

//    if (index.column() == 7)
//    {
//        painter->save();
//        //
//        QString extValueStr;
//        // получить расширенное значение
//        int extValue = model->data(index).toInt();
//        // имя датчика
//        QString sensorName = model->data(model->index(index.row(), 5)).toString();
//        // если мультидатчик, то детализация встроенных датчиков
//        if (sensorName.contains(IssoCommonData::stringByParamId(MULTISENSOR)))
//        {
//            extValueStr = makeMsDetailString(extValue);
//        }
//        // иначе расширенное значение
//        else
//        {
//            extValueStr = QString::number(extValue / 10.0, 'f', 1);
//        }
//        // вывести текст
//        painter->drawText(option.rect, extValueStr);
//        painter->restore();
//    }
//    else
//        QStyledItemDelegate::paint(painter, option, index);
//}
