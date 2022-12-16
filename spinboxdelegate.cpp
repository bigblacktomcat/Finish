#include "spinboxdelegate.h"


SpinBoxDelegate::SpinBoxDelegate(int min, int max, int value, QObject *parent)
    : QStyledItemDelegate(parent),
      m_min(min),
      m_max(max),
      m_value(value)
{
}


QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QSpinBox* editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setRange(m_min, m_max);
    return editor;
}


void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
//    int value = index.model()->data(index, Qt::EditRole).toInt();
    //
    QVariant v = index.model()->data(index, Qt::EditRole);
    int value = (v.isNull() ? m_value : v.toInt());
    //
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}


void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    model->setData(index, spinBox->value(), Qt::EditRole);

//    model->setData(index, spinBox->value(), Qt::DisplayRole);
}


void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
