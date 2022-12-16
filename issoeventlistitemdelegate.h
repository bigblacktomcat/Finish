#ifndef ISSOEVENTLISTITEMDELEGATE_H
#define ISSOEVENTLISTITEMDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QPainter>


/*!
 * \brief   Класс, отвечающий за отображение элементов визуального списка
 *          событий согласно их состояниям
 */
class IssoEventListItemDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit IssoEventListItemDelegate(QObject *parent = nullptr);

        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;

    signals:

    public slots:
};

#endif // ISSOEVENTLISTITEMDELEGATE_H
