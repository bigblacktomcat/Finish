#ifndef ISSOEDITLISTPANEL_H
#define ISSOEDITLISTPANEL_H

#include <QListWidget>
#include <QWidget>

namespace Ui {
    class IssoEditListPanel;
}

class IssoEditListPanel : public QWidget
{
        Q_OBJECT
    private:
        Ui::IssoEditListPanel *ui;

        void init();
        void setEventHandlers();

    public:
        explicit IssoEditListPanel(QWidget *parent = 0);
        ~IssoEditListPanel();

        QString title() const;
        void setTitle(const QString& title);

        QListWidget* list();
        int itemCount() const;
        QListWidgetItem* item(int index);
        QStringList itemNames();

        QListWidgetItem *addItem(const QString& displayName, bool selected = false);
        void addItem(QListWidgetItem* item, bool selected = false);
        void addItems(const QStringList& displayNames);
        void removeItem(const QString& displayName);
        void removeAndDeleteItem(QListWidgetItem* item);

        void clearList();
        void clearSelection();

        void setItems(const QStringList& displayNames);

        QListWidgetItem* findItem(const QString& displayName);

        bool hasSelectedItems() const;
        QListWidgetItem* selectedItem();
        QString selectedItemName();
        QStringList selectedItemNames();
        void setItemSelected(int index, bool selected);

        void setButtonsVisible(bool visible);
        void setEditBtnVisible(bool visible);
        void setListItemDelegate(QAbstractItemDelegate* delegate);

        void scrollListToItem(const QListWidgetItem *item);

        void changeItemName(const QString& oldName, const QString& newName);

    signals:
        void addBtnClicked(bool);
        void removeBtnClicked(bool);
        void editBtnClicked(bool);

        void selectionChanged();
        void itemDoubleClicked(QListWidgetItem*);

};

#endif // ISSOEDITLISTPANEL_H
