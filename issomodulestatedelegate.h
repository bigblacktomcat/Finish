#ifndef ISSOMODULESTATEDELEGATE_H
#define ISSOMODULESTATEDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QPainter>

#include "issomodule.h"


/*!
 * \brief   Класс, отвечающий за отображение элементов визаульного списка
 *          модулей согласно их состояниям
 */
class IssoModuleStateDelegate : public QItemDelegate
{
        Q_OBJECT
    private:

    public:
        IssoModuleStateDelegate(QObject *parent = nullptr);

        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // ISSOMODULESTATEDELEGATE_H
