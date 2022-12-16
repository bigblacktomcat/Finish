#ifndef ISSOOBJECTITEMDELEGATE_H
#define ISSOOBJECTITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QPainter>
#include "issocommondata.h"


class IssoObjectItemDelegate : public QStyledItemDelegate
{
        Q_OBJECT
    public:
        explicit IssoObjectItemDelegate(QObject *parent = nullptr);

        virtual QSize sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const Q_DECL_OVERRIDE;

        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // ISSOOBJECTITEMDELEGATE_H
