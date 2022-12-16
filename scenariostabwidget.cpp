#include "scenariostabwidget.h"
#include "ui_scenariostabwidget.h"

#include <QMessageBox>


ScenariosTabWidget::ScenariosTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScenariosTabWidget)
{
    ui->setupUi(this);
    setupWidget();
    initSystemButtonCombos();
    initWidgetHandlers();
}


ScenariosTabWidget::~ScenariosTabWidget()
{
    delete ui;
}


void ScenariosTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
}


void ScenariosTabWidget::updateTab()
{
    if (!m_object)
        return;
    // заполнить список имен классов
    ui->wgtRelayClasses->setItems(m_object->relayClassNames());
    // заполнить список имен сценариев
    ui->wgtScenarios->setItems(m_object->scenarioNames());
    // отобразить настроки кнопок панели управления системой
    displaySystemControlPanelButtons();
}


void ScenariosTabWidget::setupWidget()
{
    // список классов реле:
    //
    // задать заголовок
    ui->wgtRelayClasses->setTitle(tr("Классы реле"));
    // скрыть кнопку редактирования
    ui->wgtRelayClasses->setEditBtnVisible(false);
    //
    // список сценариев:
    //
    // задать заголовок
    ui->wgtScenarios->setTitle(tr("Сценарии"));
    // скрыть кнопку редактирования
    ui->wgtScenarios->setEditBtnVisible(false);
    //
    // заполнить комбобокс "Область действия"
    ui->cbScenarioScope->addItems(IssoCommonData::scenarioScopeStrings());
    //
    // список классов сценария:
    //
    // скрыть кнопку редактирования
    ui->wgtScenarioClasses->setEditBtnVisible(false);
}


void ScenariosTabWidget::initWidgetHandlers()
{
    // список классов реле:
    //
    // добавить класс реле
    connect(ui->wgtRelayClasses, SIGNAL(addBtnClicked(bool)),
            SLOT(processAddClassBtnClicked()));
    // удалить класс реле
    connect(ui->wgtRelayClasses, SIGNAL(removeBtnClicked(bool)),
            SLOT(processRemoveClassBtnClicked()));
    // отобразить настройки выбранного класса реле
    connect(ui->wgtRelayClasses, SIGNAL(selectionChanged()),
            SLOT(processRelayClassesSelectionChanged()));
    // сохранить настройки класса реле
    connect(ui->btnSaveClassSettings, SIGNAL(clicked(bool)),
            SLOT(processSaveClassSettingsBtnClicked()));
    //
    // список сценариев:
    //
    // добавить сценарий
    connect(ui->wgtScenarios, SIGNAL(addBtnClicked(bool)),
            SLOT(processAddScenarioBtnClicked()));
    // удалить сценарий
    connect(ui->wgtScenarios, SIGNAL(removeBtnClicked(bool)),
            SLOT(processRemoveScenarioBtnClicked()));
    // отобразить настройки выбранного сценария
    connect(ui->wgtScenarios, SIGNAL(selectionChanged()),
            SLOT(processScenariosSelectionChanged()));
    // сохранить настройки сценария
    connect(ui->btnSaveScenarioSettings, SIGNAL(clicked(bool)),
            SLOT(processSaveScenarioSettingsBtnClicked()));
    //
    // список классов сценария:
    //
    // добавить класс в сценарий
    connect(ui->wgtScenarioClasses, SIGNAL(addBtnClicked(bool)),
            SLOT(processAddScenarioClassBtnClicked()));
    // удалить класс из сценария
    connect(ui->wgtScenarioClasses, SIGNAL(removeBtnClicked(bool)),
            SLOT(processRemoveScenarioClassBtnClicked()));
    //
    // кнопки панели управления системой:
    connect(ui->btnSaveControlPanelSettings, SIGNAL(clicked(bool)),
            SLOT(processSaveControlPanelSettingsBtnClicked()));
}


void ScenariosTabWidget::initSystemButtonCombos()
{
    m_systemButtonCombos.insert(1, ui->cbSystemButton1);
    m_systemButtonCombos.insert(2, ui->cbSystemButton2);
    m_systemButtonCombos.insert(3, ui->cbSystemButton3);
    m_systemButtonCombos.insert(4, ui->cbSystemButton4);
    m_systemButtonCombos.insert(5, ui->cbSystemButton5);
    m_systemButtonCombos.insert(6, ui->cbSystemButton6);
    m_systemButtonCombos.insert(7, ui->cbSystemButton7);
    m_systemButtonCombos.insert(8, ui->cbSystemButton8);
}


void ScenariosTabWidget::displayRelayClass(const QString &relayClassName)
{
    if (!m_object)
        return;

    // настройки по умолчанию
    QString descr = "";
    int delay = 0;
    //
    // получить класс реле из коллекции объекта
    IssoRelayClass cls = m_object->relayClass(relayClassName);
    // если класс найден
    if (cls.isValid())
    {
        descr = cls.descr;
        delay = cls.delay;
    }
    // вывести на экран настройки класса:
    //
    // имя класса
    ui->leRelayClassName->setText(relayClassName);
    // описание
    ui->leRelayClassDescr->setText(descr);
    // задержка
    ui->sbRelayClassDelay->setValue(delay);
}


void ScenariosTabWidget::updateRelayClass(const QString &relayClassName)
{
    if (!m_object || relayClassName.isEmpty())
        return;
    // получить значения полей
    QString newClassName = ui->leRelayClassName->text();
    QString newClassDescr = ui->leRelayClassDescr->text();
    int newDelay = ui->sbRelayClassDelay->value();
    // создать новый класс реле
    IssoRelayClass newClass(newClassName, newClassDescr, newDelay);
    // заменить старый класс реле на новый
    m_object->replaceRelayClass(relayClassName, newClass);
    // изменить имя класса в визаульном списке
    ui->wgtRelayClasses->changeItemName(relayClassName, newClassName);
    // обновить список имен классов выбранного сценария
    processScenariosSelectionChanged();
}


void ScenariosTabWidget::displayScenario(const QString &scenarioName)
{
    if (!m_object)
        return;

    // настройки по умолчанию
    QString scopeStr = "";
    QStringList classNames;
    //
    // получить выбранный сценарий из коллекции объекта
    IssoScenario* scenario = m_object->scenario(scenarioName);
    // если сценарий найден
    if (scenario)
    {
        // область действия
        scopeStr = IssoCommonData::stringByScenarioScope(scenario->scope());
        // классы реле
        classNames = scenario->relayClassNames();
    }
    // вывести на экран настройки сценария:
    //
    // имя сценария
    ui->leScenarioName->setText(scenarioName);
    // область действия
    ui->cbScenarioScope->setCurrentText(scopeStr);
    // имена классов реле
    ui->wgtScenarioClasses->setItems(classNames);
}


void ScenariosTabWidget::updateScenario(const QString &scenarioName)
{
    if (!m_object)
        return;
    //
    // если сценарий не найден в коллекции объекта
    if (!m_object->scenarioExists(scenarioName))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно изменить настройки сценария, "
                                "т.к. не выбрано имя сценария"));
        return;
    }
    // получить значения полей:
    //
    // новое имя сценария
    QString newScenarioName = ui->leScenarioName->text();
    // область действия
    IssoScenarioScope newScope =
            IssoCommonData::scenarioScopeByString(ui->cbScenarioScope->currentText());
    // список имен классов
    QStringList newClassNames = ui->wgtScenarioClasses->itemNames();


//    //
//    // создать новый сценарий
//    IssoScenario newScenario(newScenarioName, newScope);
//    // задать список классов сценария
//    newScenario.setRelayClassNames(newClassNames);
//    //
//    // заменить старый сценарий на новый
//    m_object->replaceScenario(scenarioName, newScenario);


    // обновить настройки сценария
    m_object->updateScenario(scenarioName, newScenarioName, newScope, newClassNames);

    // изменить имя сценария в визаульном списке
    ui->wgtScenarios->changeItemName(scenarioName, newScenarioName);
}


IssoScenario *ScenariosTabWidget::currentScenario()
{
    if (!m_object)
        return nullptr;
    // имя выбранного сценария
    QString scenarioName = ui->wgtScenarios->selectedItemName();
    // если имя выбрано в визуальном списке, вернуть сценарий из коллекции объекта
    // иначе вернуть пустой указатель
    return (scenarioName.isEmpty() ? nullptr : m_object->scenario(scenarioName));
}


void ScenariosTabWidget::displaySystemControlPanelButtons()
{
    if (!m_object)
        return;
    //
    // обновить списки сценариев для настройки системных кнопок
    foreach (QComboBox* combo, m_systemButtonCombos)
    {
        // очистить список
        combo->clear();
        // добавить пустую строку (не выбрано)
        combo->addItem("");
        // добавить имена сценариев
        combo->addItems(m_object->scenarioNames());
    }
    // выьрать имя сценария в соответствиии с настройками объекта
    foreach (int buttonNum, m_systemButtonCombos.keys())
    {
        // получить комбобокс по номеру кнопки
        QComboBox* combo = m_systemButtonCombos.value(buttonNum);
        if (!combo)
            continue;
        // получить из объекта имя сценария, связанного с кнопкой
        QString scenarioName = m_object->buttonScenarioLink(buttonNum);
        // вывести в комбобокс имя сценария
        combo->setCurrentText(scenarioName);
    }
}


void ScenariosTabWidget::updateSystemControlPanelButtons()
{
    if (!m_object)
        return;

    foreach (int buttonNum, m_systemButtonCombos.keys())
    {
        // получить комбобокс по номеру кнопки
        QComboBox* combo = m_systemButtonCombos.value(buttonNum);
        if (!combo)
            continue;

        // получить выбранное имя сценария
        QString scenarioName = combo->currentText();
        // если имя сценария не выбрано,
        // удалить из карты соответствия объекта, иначе добавить
        if (scenarioName.isEmpty())
            m_object->removeButtonScenarioLink(buttonNum);
        else
            m_object->insertButtonScenarioLink(buttonNum, scenarioName);
    }
}


void ScenariosTabWidget::processAddClassBtnClicked()
{
    if (!m_object)
        return;
    // вывести диалог создания нового класса реле
    NewRelayClassDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    //
    // если имя класса не задано, известить и выйти
    if (dlg.relayClassName().isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить новый класс реле, "
                                "т.к. не задано наименование"));
        return;
    }
    //
    // если класс с указанным именем уже существует, известить и выйти
    if (m_object->relayClassExists(dlg.relayClassName()))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить новый класс реле, "
                                "т.к. класс с именем <%1> уже существует")
                                .arg(dlg.relayClassName()));
        return;
    }
    //
    // создать новый класс реле
    IssoRelayClass relayClass(dlg.relayClassName(), dlg.description(), dlg.delay());
    // добавить в коллекцию классов реле объекта
    m_object->insertRelayClass(relayClass);
    // добавить в визуальный список
    ui->wgtRelayClasses->addItem(dlg.relayClassName(), true);
}


void ScenariosTabWidget::processRemoveClassBtnClicked()
{
    if (!m_object)
        return;

    // получить выделенный элемент списка классов реле
    QListWidgetItem* item = ui->wgtRelayClasses->selectedItem();
    // проверить выбрано ли имя класса реле
    if (!item)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить класс реле, "
                                "т.к. не выбрано имя класса"));
        return;
    }
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                    tr("Вы действительно хотите удалить класс реле <%1>")
                        .arg(item->text())))
        return;
    // удалить класс реле из коллекции объекта
    // и из всех сценариев
    m_object->removeRelayClass(item->text());
    // удалить элемент из визуального списка
    ui->wgtRelayClasses->removeItem(item->text());

    // обновить список имен классов выбранного сценария
    processScenariosSelectionChanged();
}


void ScenariosTabWidget::processRelayClassesSelectionChanged()
{
    displayRelayClass(ui->wgtRelayClasses->selectedItemName());
}


void ScenariosTabWidget::processSaveClassSettingsBtnClicked()
{
    updateRelayClass(ui->wgtRelayClasses->selectedItemName());
}


void ScenariosTabWidget::processAddScenarioBtnClicked()
{
    if (!m_object)
        return;
    // вывести диалог создания нового сценария
    NewScenarioDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    //
    // если имя сценария не задано, известить и выйти
    if (dlg.scenarioName().isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить новый сценарий, "
                                "т.к. не задано наименование"));
        return;
    }
    //
    // если сценарий с указанным именем уже существует, известить и выйти
    if (m_object->scenarioExists(dlg.scenarioName()))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить новый сценарий, "
                                "т.к. сценарий с именем <%1> уже существует")
                                .arg(dlg.scenarioName()));
        return;
    }
    //
    // создать новый сценарий
    IssoScenario* scenario = new IssoScenario(dlg.scenarioName(), dlg.scenarioScope());
    // добавить в коллекцию сценариев объекта
    m_object->insertScenario(scenario);
    // добавить в визуальный список
    ui->wgtScenarios->addItem(dlg.scenarioName(), true);
    //
    // отобразить настроки кнопок панели управления системой
    displaySystemControlPanelButtons();
}


void ScenariosTabWidget::processRemoveScenarioBtnClicked()
{
    if (!m_object)
        return;

    // получить выделенный элемент списка сценариев
    QListWidgetItem* item = ui->wgtScenarios->selectedItem();
    // проверить выбрано ли имя сценария
    if (!item)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить сценарий, "
                                "т.к. не выбрано имя класса"));
        return;
    }
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                    tr("Вы действительно хотите удалить сценарий <%1>")
                        .arg(item->text())))
        return;
    // удалить сценарий из коллекции объекта
    // и всех конфигураций модуля (РЕАЛИЗОВАТЬ!!!)
    m_object->removeScenario(item->text());
    // удалить элемент из визуального списка
    ui->wgtScenarios->removeItem(item->text());
    //
    // отобразить настроки кнопок панели управления системой
    displaySystemControlPanelButtons();
}


void ScenariosTabWidget::processScenariosSelectionChanged()
{
    displayScenario(ui->wgtScenarios->selectedItemName());
}


void ScenariosTabWidget::processSaveScenarioSettingsBtnClicked()
{
    updateScenario(ui->wgtScenarios->selectedItemName());
}


void ScenariosTabWidget::processAddScenarioClassBtnClicked()
{
    // получить текущий сценарий
    IssoScenario* scenario = currentScenario();
    if (!scenario)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить класс реле в сценарий, "
                                "т.к. не выбрано имя сценария"));
        return;
    }
    // получить список классов реле
    QStringList classItems = m_object->relayClassNames();
    // если список классов пуст, известить и выйти
    if (classItems.isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить класс реле в сценарий, "
                                "т.к. список классов пуст"));
        return;
    }
    // показать диалог выбора имени класса
    bool ok;
    QString className = IssoCommonData::showSelectItemDialog(this,
                            tr("Добавить класс реле в сценарий"),
                            tr("Класс реле"), classItems, ok);
    if (!ok)
        return;
    // добавить имя класса в сценарий
    scenario->addRelayClassName(className);
    // добавить в визуальный список
    ui->wgtScenarioClasses->addItem(className, true);
}


void ScenariosTabWidget::processRemoveScenarioClassBtnClicked()
{
    // получить текущий сценарий
    IssoScenario* scenario = currentScenario();
    if (!scenario)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить класс реле из сценария, "
                                "т.к. не выбрано имя сценария"));
        return;
    }
    // получить имя выбранного для удаления из сценария класса
    QString className = ui->wgtScenarioClasses->selectedItemName();
    // если имя не выбрано, выйти
    if (className.isEmpty())
        return;
    // вывести диалог подтверждения
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                    tr("Вы действительно хотите удалить класс <%1> из сценария <%2>")
                        .arg(className).arg(scenario->name())))
        return;
    // удалить имя класса из сценария
    scenario->removeRelayClassName(className);
    // удалить из визуального списка
    ui->wgtScenarioClasses->removeItem(className);
}


void ScenariosTabWidget::processSaveControlPanelSettingsBtnClicked()
{
    updateSystemControlPanelButtons();
}
