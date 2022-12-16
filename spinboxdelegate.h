#ifndef SPINBOXDELEGATE_H
#define SPINBOXDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QDebug>


class SpinBoxDelegate : public QStyledItemDelegate
{
        Q_OBJECT
    private:
        int m_min;
        int m_max;
        int m_value;
    public:
        explicit SpinBoxDelegate(int min, int max, int value, QObject *parent = nullptr);
        QWidget *createEditor(QWidget *parent,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
                          const QModelIndex &index) const override;
        void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const override;
    signals:

    public slots:
};

#endif // SPINBOXDELEGATE_H
