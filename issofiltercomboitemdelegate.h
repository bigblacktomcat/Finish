#ifndef ISSOFILTERCOMBOITEMDELEGATE_H
#define ISSOFILTERCOMBOITEMDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QPainter>


class IssoFilterComboItemDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit IssoFilterComboItemDelegate(QObject *parent = nullptr);

        virtual void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // ISSOFILTERCOMBOITEMDELEGATE_H
