#ifndef ISSOCHECKABLESELECTIONPANEL_H
#define ISSOCHECKABLESELECTIONPANEL_H

#include <QWidget>

namespace Ui {
    class IssoCheckableSelectionPanel;
}

class IssoCheckableSelectionPanel : public QWidget
{
        Q_OBJECT
    private:
        Ui::IssoCheckableSelectionPanel *ui;

        void init();

    public:
        explicit IssoCheckableSelectionPanel(QWidget *parent = 0);
        ~IssoCheckableSelectionPanel();

        QString label() const;
        void setLabel(const QString& label);

        void setItemNames(const QStringList& itemNames);

        QString selectedItemName() const;
        void setSelectedItemName(const QString& itemName);

        bool checked() const;
        void setChecked(bool checked);
};

#endif // ISSOCHECKABLESELECTIONPANEL_H
