#ifndef ISSOEVENTDELEGATE_H
#define ISSOEVENTDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QStyledItemDelegate>


/*!
 * \brief   Класс, отвечающий за отображение элементов таблицы
 *          событий согласно их состояниям
 */
class IssoEventDelegate : public QStyledItemDelegate
{
        Q_OBJECT
    public:
        explicit IssoEventDelegate(QWidget *parent = nullptr);


        virtual QSize sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const Q_DECL_OVERRIDE;


        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // ISSOEVENTDELEGATE_H
