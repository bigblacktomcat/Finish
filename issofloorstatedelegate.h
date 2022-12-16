#ifndef ISSOFLOORSTATEDELEGATE_H
#define ISSOFLOORSTATEDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QPainter>

#include "issomodule.h"


/*!
 * \brief   Класс, отвечающий за отображение элементов визаульного списка
 *          этажей и зданий согласно их состояниям
 */
class IssoFloorStateDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit IssoFloorStateDelegate(QObject *parent = nullptr);

        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // ISSOFLOORSTATEDELEGATE_H
