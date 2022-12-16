#include "audiomessagedialog.h"
#include "commentdialog.h"
#include "graphicplantabwidget.h"
#include "issoeventquerymodel.h"
#include "issofloorstatedelegate.h"
#include "issoobjectutils.h"
#include "newbuildingdialog.h"
#include "newfloordialog.h"
#include "passworddialog.h"
#include "selectmoduledialog.h"
#include "ui_graphicplantabwidget.h"
#include "testingwidget.h"
#include <QMessageBox>
#include <QScroller>
#include <QSortFilterProxyModel>
#include "issoexchangemanager.h"

GraphicPlanTabWidget::GraphicPlanTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphicPlanTabWidget)
{
    ui->setupUi(this);
    // сформировать контекстное меню
    initTableMenu();
}


GraphicPlanTabWidget::~GraphicPlanTabWidget()
{
    delete ui;
}


void GraphicPlanTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
    // если объект пуст, выход
    if (!m_object)
        return;

    // отключить обработчики выбора модулей
    setSelectionHandlersEnabled(false);
    // очистить визуальные списки
    clearWidgetLists();
    // заполнить визуальный список имен зданий
    foreach (IssoBuilding* build, m_object->buildings())
    {
        ui->wgtBuilds->addItem(build->displayName());
    }
    // подключить обработчики выбора модулей
    setSelectionHandlersEnabled(true);
    // выбрать 1 здание в визуальном списке
    // (запустит обработчик выбора здания,
    // который заполнит список этажей и выберет 1 этаж)
    ui->wgtBuilds->setItemSelected(0, true);
    // обновить состояния визуальных списков
    updateAllLists();
    // скрыть панель индикации (по умолчанию)
    ui->wgtIndicators->setVisible(false);
    //
    //
    // DEBUG
//    setTestEventHandlers();
    initTestPanel();
}


void GraphicPlanTabWidget::uninit()
{
    // скрыть сцену
    hideScene();
}


void GraphicPlanTabWidget::init(IssoAppType appType, IssoGuiMode mode)
{
    // инициализировать переменные,
    // ссылающиеся на визуальные компоненты
    initWidgetVars(mode);
    // инициализировать виджеты
    initWidgets();
    // задать обработчики виджетов
    initWidgetHandlers();
    //
    // задать вид окна согласно типу устройства отображения
    switch(mode)
    {
        case IssoGuiMode::STANDART_MONITOR:
        {
            ui->stwgtBottomPanel->setCurrentIndex(0);
            break;
        }
        case IssoGuiMode::TOUCH_SCREEN:
        case IssoGuiMode::TABLET:
        {
            ui->stwgtBottomPanel->setCurrentIndex(1);
            updateGuiForTouchScreen();
            break;
        }
        default:
            break;
    }
    //
    // задать вид окна согласно типу приложения
    switch(appType)
    {
        case IssoAppType::SOO:
        case IssoAppType::GSC:
        {
            // скрыть панели управления
            ui->gbModuleControls->setVisible(false);
            ui->gbSystemControls->setVisible(false);
            ui->gbTouchModuleControls->setVisible(false);
            ui->gbTouchSystemControls->setVisible(false);
//            // скрыть панель комментария
//            ui->wgtComment->setVisible(false);
            break;
        }
        default:
            break;
    }
}


void GraphicPlanTabWidget::setSuperUserMode(bool superUser)
{
    // видимость конфигурационных кнопок:
    //
    // модули
    ui->wgtModules->setButtonsVisible(superUser);
    // этажи
    ui->wgtFloors->setButtonsVisible(superUser);
    // здания
    ui->wgtBuilds->setButtonsVisible(superUser);
    // панель Тестирование системы
    ui->gbSystemTestButtons->setVisible(superUser);
    //
    // установить разрешение перемещать графические объекты
    if (m_object)
        m_object->setGraphicItemsMovable(superUser);
}


void GraphicPlanTabWidget::setDataBaseModel(QAbstractItemModel* model)
{
    if (!m_object)
        return;
    // задать модель данных
    ui->tvEvents->setModel(model);
    // задать режим выделения одной строки
    ui->tvEvents->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvEvents->setSelectionMode(QAbstractItemView::SingleSelection);
//    // подгонять ширину столбцов под размер контента
//    ui->tvEvents->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // запретить редактирование БД
    ui->tvEvents->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //
    // настроить фильтр
    //
    // заполнить список типов событий
    ui->cbFilterEventType->addItem("");
    ui->cbFilterEventType->addItems(IssoCommonData::eventSourceNames());
    // установить фильтр
    setEventFilter(QDate::currentDate(), QTime(0, 0, 0), QTime(23, 59, 59), "");
//    // деактивировать фильтр
//    setEventFilterEnabled(false);
}


void GraphicPlanTabWidget::displayAlarmModule(IssoModule *module)
{
    if (!m_object)
        return;
    // получить этаж, на котором расположен тревожный модуль
    IssoFloorPlan* floor = m_object->findFloor(module->buildNum(), module->floorNum());
    if (!floor)
        return;

    // если тревога не на текущем этаже
    if (floor != m_object->currentFloor())
    {
        // если с последней тревоги прошло > 1 минуты
        if (m_lastAlarmTime.isNull() || (m_lastAlarmTime.elapsed() > 20000))
        {
            // найти здание
            IssoBuilding* build = m_object->findBuildingByNumber(module->buildNum());
            if (!build)
                return;

            // снять обработчик выбора этажа в визуальном списке
            disconnect(ui->wgtFloors, SIGNAL(selectionChanged()), 0, 0);
            // найти здание в визуальном списке
            QListWidgetItem* buildItem = ui->wgtBuilds->findItem(build->displayName());
            // если здание найдено
            if (buildItem)
            {
                // выбрать здание в визуальном списке
                buildItem->setSelected(true);
                // найти этаж в визуальном списке
                QListWidgetItem* floorItem = ui->wgtFloors->findItem(floor->displayName());
                // если этаж найден
                if (floorItem)
                {
                    // выбрать этаж в визуальном списке
                    floorItem->setSelected(true);
                    // отобразить изменения
                    processFloorChanged();
                }
            }
            //
            // установить обработчик выбора этажа в визуальном списке
            connect(ui->wgtFloors, SIGNAL(selectionChanged()),
                    SLOT(processFloorChanged()));
        }
        else
            // иначе ничего не делаем
            return;
    }

    // если количество тревог на этаже > 1
    if (m_object->currentFloor()->alarmCount() > 1)
    {
        // множественная тревога: показать весь графплан
        ui->view->fitSceneToViewport();
    }
    else
    {
        // одиночная тревога: выбрать модуль в списке и спозиционировать на него сцену
        if (auto item = ui->wgtModules->findItem(module->displayName()))
        {
            processModuleNameDoubleClicked(item);
        }
//        // одиночная тревога: позиционировать сцену на модуль
//        ui->view->locateViewportOnModule(module);

    }
    // перезапустить таймер
    m_lastAlarmTime.restart();
}


void GraphicPlanTabWidget::processModuleSelectionChanged()
{
    if (!m_object)
        return;
    // снять обработчики изменений сцены и визуального списка
    setSelectionHandlersEnabled(false);
    // сформировать список отображаемых имен выбранных на сцене модулей
    QStringList selectedNames;
    m_object->currentFloor()->selectedModuleNames(selectedNames);
    // выделить строки визуального списка,
    // соответствующие именам выбранных модулей
    for(int i = 0; i < ui->wgtModules->itemCount(); ++i)
    {
        // текущая строка визуального списка
        QListWidgetItem* item = ui->wgtModules->item(i);
        // если строка найдена в списке имен выбранных модулей,
        // то выделить ее, иначе снять выделение
        bool selected = selectedNames.contains(item->text());
        item->setSelected(selected);
        // прокрутить список на выбранный элемент
        if (selected)
            ui->wgtModules->scrollListToItem(item);
    }
    // установить обработчики изменений сцены и визуального списка
    setSelectionHandlersEnabled(true);
    // обновить информацию о выбранном модуле
    updateSelectedModuleInfo();
    //
    // обновить список адресов мультидатчиков тестовой панели
    testUpdateSelectedModuleMsAddrs();
}


void GraphicPlanTabWidget::processModuleNameSelectionChanged()
{
    // снять обработчики изменений сцены и визуального списка
    setSelectionHandlersEnabled(false);
    // сформировать список выбранных строк визуального списка
    // выбрать модули, соответствующие именам из списка
    ui->view->selectModules(ui->wgtModules->selectedItemNames());
    // установить обработчики изменений сцены и визуального списка
    setSelectionHandlersEnabled(true);
    // обновить информацию о выбранном модуле
    updateSelectedModuleInfo();
}


void GraphicPlanTabWidget::processModuleNameDoubleClicked(QListWidgetItem *item)
{
    // выделить модуль, имя которого выбрано в визуальном списке
    ui->view->selectModules(QStringList { item->text() });
    // получить выделенный модуль
    IssoModule* selectedModule = m_object->selectedModule();
    // позиционировать сцену на модуль
    if (selectedModule)
        ui->view->locateViewportOnModule(selectedModule);
}


void GraphicPlanTabWidget::processFloorChanged()
{
    if (!m_object)
        return;
    // проверить наличие выделенных строк визуального списка
    if (!ui->wgtFloors->hasSelectedItems())
    {
        setCurrentFloorPlan(nullptr);
        return;
    }
    // найти выделенную строку визуального списка
    QListWidgetItem* item = ui->wgtFloors->selectedItem();
    if (item)
    {
        // найти план этажа по имени
        IssoFloorPlan* floor = m_object->currentBuilding()->findFloorByName(item->text());
        if (floor)
            // сделать этаж текущим (отобразить)
            setCurrentFloorPlan(floor);
    }

    // обновить кнопки панели управления системой (сценариями)
    updateSystemContolButtons();
}


void GraphicPlanTabWidget::processBuildingChanged()
{
    if (!m_object)
        return;
    // проверить наличие выделенных строк визуального списка
    if (!ui->wgtBuilds->hasSelectedItems())
    {
        // запомнить выбранное здание
        m_object->setCurrentBuilding(nullptr);
        // очистить список этажей здания
        ui->wgtFloors->clearList();
        return;
    }
    // найти выделенную строку визуального списка
    QListWidgetItem* item = ui->wgtBuilds->selectedItem();
    if (item)
    {
        // найти здание по имени
        IssoBuilding* build = m_object->findBuildingByName(item->text());
        if (build)
        {
            // запомнить выбранное здание
            m_object->setCurrentBuilding(build);
            // заполнить визуальный список этажей выбранного здания
            fillVisualFloorList(build);
            // выбрать 1 этаж в визуальном списке
            // (запустит обработчик выбора этажа,
            // который заполнит список модулей)
            ui->wgtFloors->setItemSelected(0, true);
        }
    }
}


//void GraphicPlanTabWidget::processBtnWriteCommentClicked()
//{
//    // комментарий
//    QString comment = ui->txteComment->toPlainText();
//    // запретить пустую строку в комментарии
//    if (comment.isEmpty())
//        return;
//    // если строка строка события не выбрана, выход
//    if (ui->tvEvents->selectionModel()->selectedIndexes().isEmpty())
//        return;
//    // получить индекс модели
//    QModelIndex idx = ui->tvEvents->selectionModel()->selectedIndexes().first();
//    // получить прокси-модель из индекса
//    const QSortFilterProxyModel* model = dynamic_cast<const QSortFilterProxyModel*>(idx.model());
//    if (!model)
//        return;
//    // получить значение поля id (стоблец 0) выбранной записи таблицы
//    QVariant value = model->data(model->index(idx.row(), 0));
//    // если значение id не пустое
//    if (!value.isNull())
//    {
//        // инициировать запрос записи комментария
//        emit writeEventCommentRequested(value.toInt(), comment);
//    }
//}


void GraphicPlanTabWidget::processModuleFuncBtnClicked()
{
    if (!m_object)
        return;
    // выбранный модуль
    IssoModule* module = m_object->selectedModule();
    if (!module)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно отправить запрос, т.к. модуль не выбран"));
        return;
    }
    // кнопка - источник события
    QPushButton* btn = qobject_cast<QPushButton*>(QObject::sender());
    if (!btn)
        return;
    //
    // если к кнопке привязана фиксированная функция, выполнить
    if (IssoCommonData::isFixedModuleBtnFunc(btn->text()))
    {
        // преобразовать имя функции в перечисление
        IssoFixedModuleBtnFunc func = IssoCommonData::fixedModuleBtnFuncByString(btn->text());
        switch(func)
        {
//            case IssoFixedModuleBtnFunc::CAMERA:
//            {
//                processBtnModuleCameraClicked();
//                break;
//            }
            case IssoFixedModuleBtnFunc::VOICE:
            {
                processBtnModuleVoiceClicked();
                break;
            }
            default:
                break;
        }
    }
    // иначе определить список реле и активировать
    else
    {
        // получить номер кнопки
        int buttonNum = m_moduleButtons.key(btn, -1);
        if (buttonNum == -1)
            return;
        // получить конфиг модуля
        IssoModuleConfig* cfg = m_object->moduleConfig(module->moduleConfigName());
        if (!cfg)
            return;
        // получить список Id реле, активируемых по кнопке
        QList<IssoParamId> relaysIds = cfg->getRelayIdsByButtonNumber(buttonNum);
        if (relaysIds.isEmpty())
            return;
        // получить текущее состояние реле (для этого берется первый Id реле из списка)
        IssoSensorStateParam* relayStateParam = module->stateParamById(relaysIds.first());
        bool pressed;
        if (relayStateParam)
        {
            // получить состояние кнопки
            pressed = (relayStateParam->state() == RELAY_ON);
        }
        else
        {
            pressed = false;
        }
        //
        // для каждого реле из списка
        foreach (IssoParamId relayId, relaysIds)
        {
            // инициировать запрос переключения реле
            emit turnModuleRelayRequested(module->uniqueId(), relayId, !pressed);
        }
    }
}

//void GraphicPlanTabWidget::processRelayBtnClicked()
//{
//    if (!m_object)
//        return;
//    // выбранный модуль
//    IssoModule* module = m_object->selectedModule();
//    if (!module)
//    {
//        QMessageBox::warning(this, tr("Внимание"),
//                             tr("Невозможно отправить запрос, т.к. модуль не выбран"));
//        return;
//    }
//    // кнопка - источник события
//    QPushButton* btn = qobject_cast<QPushButton*>(QObject::sender());
//    if (!btn)
//        return;
//    // найти параметр действия по кнопке, связанной с ним
//    IssoParamId paramId = m_moduleButtons.key(btn);
//    // получить параметр состояния кнопки
//    IssoSensorStateParam* btnStateParam = module->stateParamById(paramId);
//    bool pressed;
//    if (btnStateParam)
//    {
//        // получить состояние кнопки
//        pressed = (btnStateParam->state() == RELAY_ON);
//    }
//    else
//    {
//        pressed = false;
//    }
//    // 05.06.2019
//    // ВРЕМЕННО УБРАЛ ПАРОЛЬ!!!
////    // если Реле-1 или Реле-2, запросить пароль
////    if ((paramId == OUT_RELAY1) || (paramId == OUT_RELAY2))
////    {
////        // если пароль неверный, выход
////        if (!enterPassword())
////            return;
////    }
//    // инициировать запрос переключения реле
//    emit turnModuleRelayRequested(module->uniqueId(), paramId, !pressed);
//}


//void GraphicPlanTabWidget::processBtnModuleCameraClicked()
//{
//    if (!m_object)
//        return;
//    // получить выбранный модуль
//    IssoModule* module = m_object->selectedModule();
//    // если модуль не выбран, выход
//    if (!module)
//    {
//        QMessageBox::warning(this, tr("Внимание"),
//                             tr("Невозможно определить IP-адрес "
//                                "ассоциированной камеры, т.к. модуль не выбран"));
//        return;
//    }
//    // если нет ассоциированных с модулем камер, сообщить
//    if (!module->getCameraEnabled())
//    {
//        QMessageBox::warning(this, tr("Внимание"),
//                             tr("С модулем <%1> не ассоциирована ни одна IP-камера")
//                                .arg(module->displayName()));
//        return;
//    }
//    // инициировать запрос на воспроизведение видео с камеры
//    emit videoStreamRequested(module->getCameraId());
//}


void GraphicPlanTabWidget::processBtnModuleVoiceClicked()
{
    if (!m_object)
        return;
    if (!m_object->selectedModule())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно отправить голосовое сообщение, "
                                "т.к. модуль не выбран"));
        return;
    }
    //
    AudioMessageDialog dlg(this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;
    //
    // тип аудио сообщения
    switch (dlg.audioMsgType())
    {
        case VOICE:
        {
            // запросить воспроизведение записи с микрофона
            emit playVoiceRecordRequested(m_object->selectedModule()->ip(),
                                          IssoConst::AUDIO_SEND_PORT,
                                          dlg.voiceRecord());
            break;
        }
        case MODULE_REC:
        {
            // запросить воспроизведение аудиозаписи из памяти контроллера
            emit playModuleRecordRequested(m_object->selectedModule()->ip(),
                                           IssoConst::AUDIO_SEND_PORT,
                                           dlg.recordNumber());
            break;
        }
        case PC_REC:
        {
            if (!QFile(dlg.recordFileName()).exists())
            {
                QMessageBox::warning(this, tr("Внимание"),
                                     tr("Файл %1 не найден").arg(dlg.recordFileName()));
                break;
            }
            // запросить воспроизведение файла из памяти ПК
            emit playPcRecordRequested(m_object->selectedModule()->ip(),
                                       IssoConst::AUDIO_SEND_PORT,
                                       dlg.recordFileName());
            break;
        }
        default:
            break;
    }
}


void GraphicPlanTabWidget::processSystemFuncBtnClicked()
{
    if (!m_object)
        return;
    // кнопка - источник события
    QPushButton* btn = qobject_cast<QPushButton*>(QObject::sender());
    if (!btn)
        return;
    // получить номер кнопки
    int buttonNum = m_systemButtons.key(btn, -1);
    if (buttonNum == -1)
        return;
    // получить имя сценария, привязанному к кнопке
    QString scenarioName = m_object->buttonScenarioLink(buttonNum);
    if (scenarioName.isEmpty())
        return;
    // определить здание и этаж
    IssoFloorPlan* floor = m_object->currentFloor();
    if (!floor)
        return;

    // если кнопка нажата, остановить сценарий,
    // иначе запустить сценарий
    bool active = buttonPressed(btn);
    emit changeScenarioStateRequested(
                IssoScenarioInfo(scenarioName, floor->buildNum(), floor->number()),
                !active);
}


void GraphicPlanTabWidget::processBtnSosClicked()
{
    // запросить пароль
    if (!enterPassword())
        return;
    // РЕАЛИЗОВАТЬ!!!

}


void GraphicPlanTabWidget::processBtnAddBuildingClicked()
{
    if (!m_object)
        return;

    NewBuildingDialog dlg(m_object->availableBuildNumbers(), this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;
    // добавить новое здание
    QString buildName = m_object->addBuilding(dlg.name(), dlg.number());
    // если имя пустое, значит, здание не добавлено
    if (buildName.isEmpty())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить новое здание, "
                                "т.к. здание с указанным отображаемым именем "
                                "или номером уже существует"));
        return;
    }
    // добавить здание в визуальный список
    ui->wgtBuilds->addItem(buildName);
    // выбрать в визуальном списке новое здание
    QListWidgetItem* item = ui->wgtBuilds->findItem(buildName);
    if (item)
        item->setSelected(true);
}


void GraphicPlanTabWidget::processBtnRemoveBuildingClicked()
{
    if (!m_object)
        return;

    // проверить выбрано ли здание
    if (!ui->wgtBuilds->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить здание, т.к. не выбрано имя здания"));
        return;
    }
    // запросить пароль
    if (!enterPassword())
        return;
    // получить выбранный элемент визуального списка
    QListWidgetItem* item = ui->wgtBuilds->selectedItem();
    // если в списке больше одного здания, выбрать другое здание
    if (ui->wgtBuilds->itemCount() > 1)
    {
        // текущий номер строки
        int curNum = ui->wgtBuilds->list()->row(item);
        // новый номер строки
        int newNum = (curNum == 0 ? 1 : 0);
        // выбрать новую строку
        ui->wgtBuilds->setItemSelected(newNum, true);
    }
    else
    {
        // очистить выделение
        ui->wgtBuilds->clearSelection();
    }
    // удалить здание
    m_object->removeBuilding(item->text());
    // снять обработчик выбора здания
    disconnect(ui->wgtBuilds, SIGNAL(selectionChanged()),
               this,          SLOT(processBuildingChanged()));
    // удалить элемент из визуального списка и уничтожить
    ui->wgtBuilds->removeAndDeleteItem(item);
    // если остались здания, установить текущий элемент списка,
    // слетевший после удаления другого элемента
    ui->wgtBuilds->setItemSelected(0, true);
    // установить обработчик выбора здания
    connect(ui->wgtBuilds, SIGNAL(itemSelectionChanged()),
            SLOT(processBuildingChanged()));
}


void GraphicPlanTabWidget::processBtnAddFloorClicked()
{
    if (!m_object)
        return;

    // если здание не выбрано, сообщить
    if (!ui->wgtBuilds->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить этаж, т.к. не выбрано имя здания"));
        return;
    }
    // получить имя здания
    QString buildName = ui->wgtBuilds->selectedItemName();
    // вывести диалог
    NewFloorDialog dlg(m_object->availableFloorNumbers(), this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;

    // получить изображение
    bool ok;
    QPixmap pixmap = dlg.pixmap(ok);
    // если изображение не получено, выход
    if (!ok)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить этаж, т.к. не выбран файл графплана"));
        return;
    }
    // добавить этаж в текущее здание
    result = m_object->addFloorToBuilding(buildName, dlg.number(), pixmap);
    // если этаж добавлен
    if (result)
    {
        // обновить список этажей здания
        processBuildingChanged();
    }
    else
    {
        // сообщить о невозможности добавления
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить этаж, т.к. здание "
                                "уже содержит этаж с номером <%1>")
                                .arg(dlg.number()));
        return;
    }
}

/**
 * @brief GraphicPlanTabWidget::processBtnRemoveFloorClicked
 * @brief удаление этажа со схемы
 */
void GraphicPlanTabWidget::processBtnRemoveFloorClicked()
{
    if (!m_object)
        return;

    // проверить, выбрано ли здание
    if (!ui->wgtBuilds->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить этаж, т.к. не выбрано имя здания"));
        return;
    }
    // получить имя здания
    QString buildName = ui->wgtBuilds->selectedItemName();
    //
    // проверить, выбран ли этаж
    if (!ui->wgtFloors->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить этаж, т.к. не выбрано имя этажа"));
        return;
    }
    // получить выбранный элемент списка этажей
    QListWidgetItem* floorItem = ui->wgtFloors->selectedItem();
    // получить имя этажа
    QString floorName = floorItem->text();
    //
    // запросить пароль
    if (!enterPassword())
        return;
    //
    // снять обработчик выбора этажа в визуальном списке
    disconnect(ui->wgtFloors, SIGNAL(selectionChanged()), 0, 0);
    // удалить элемент визуального списка и уничтожить
    ui->wgtFloors->removeAndDeleteItem(floorItem);
    // если остались этажи, выбрать первый
    if (ui->wgtFloors->itemCount() > 0)
        ui->wgtFloors->setItemSelected(0, true);
    // иначе снять выделение
    else
        ui->wgtFloors->clearSelection();
    // отобразить изменения
    processFloorChanged();
    // удалить этаж из текущего здания
    m_object->removeFloorFromBuilding(buildName, floorName);
    //
    // установить обработчик выбора этажа в визуальном списке
    connect(ui->wgtFloors, SIGNAL(selectionChanged()),
            SLOT(processFloorChanged()));
}

/**
 * @brief GraphicPlanTabWidget::processBtnEditFloorClicked
 * Редактирование этажа, замена графплана
 */
void GraphicPlanTabWidget::processBtnEditFloorClicked()
{
    if (!m_object)
        return;

    // если здание не выбрано, сообщить
    if (!m_object->currentFloor())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно изменить этаж, т.к. не выбрано имя этажа"));
        return;
    }
    NewFloorDialog dlg(m_object->currentFloor()->number(), this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;

    // получить изображение
    bool ok;
    QPixmap pixmap = dlg.pixmap(ok);
    // если изображение не получено, выход
    if (!ok)
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно изменить графплан этажа, т.к. не выбран файл графплана"));
        return;
    }
    m_object->currentFloor()->setBackground(pixmap.toImage());
}
/**
 * @brief GraphicPlanTabWidget::processBtnAddModuleToFloorClicked
 *
 */
void GraphicPlanTabWidget::processBtnAddModuleToFloorClicked()
{
    if (!m_object)
        return;

    // если здание не выбрано, сообщить
    if (!m_object->currentFloor())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить модуль, т.к. не выбрано имя этажа"));
        return;
    }
    // передать в диалог список имен неразмещенных модулей
    SelectModuleDialog dlg(m_object->unlocatedModuleNames(), this);
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return;

    // найти выбранный модуль
    IssoModule* module = m_object->findModuleByDisplayName(dlg.selectedModuleName());
    if (!module)
        return;

    // запомнить старое имя модуля
    QString oldDisplayName = module->displayName();
    // переместить модуль на текщий этаж текущего здания
    m_object->changeModuleLocation(module,
                                   m_object->currentBuilding()->number(),
                                   m_object->currentFloor()->number());
    // обновить визуальный список модулей
    fillVisualModulesList(m_object->currentFloor());
    // сообщить об изменении имени модуля
    QMessageBox::information(this, tr("Информация"),
                             tr("Имя модуля изменено с <%1> на <%2>")
                                .arg(oldDisplayName)
                                .arg(module->displayName()));
}


void GraphicPlanTabWidget::processBtnRemoveModuleFromFloorClicked()
{
    if (!m_object)
        return;

    if (!ui->wgtModules->hasSelectedItems())
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно удалить модуль с этажа, т.к. модуль не выбран"));
        return;
    }
    IssoModule* module = m_object->selectedModule();
    // если модуль не выбран, выход
    if (!module)
        return;

    // запомнить старое имя модуля
    QString oldDisplayName = module->displayName();
    // обнулить модулю номер здания и номер этажа
    m_object->changeModuleLocation(module, 0, 0);
    // обновить визуальный список модулей
    fillVisualModulesList(m_object->currentFloor());
    // сообщить об изменении имени модуля
    QMessageBox::information(this, tr("Информация"),
                             tr("Имя модуля изменено с <%1> на <%2>")
                                .arg(oldDisplayName)
                                .arg(module->displayName()));
}

/**
 * @brief GraphicPlanTabWidget::processBtnRequestModuleInfoClicked
 * инициируем запрос состояния модуля
 * @attention Забыт опрос состояния подключенных датчиков
 */
void GraphicPlanTabWidget::processBtnRequestModuleInfoClicked()
{
    if (!m_object)
        return;

    // найти модуль
    IssoModule* module = m_object->selectedModule();
    if (module)
    {
        // инициировать запрос состояние модуля
        emit moduleInfoRequested(module->uniqueId());
        // инициируем запрос состояния MS датчиков на шлейфах
        // здесь должен стоять опрос состояния мультидатчиков

        // даем команду CMD_MULTISENSOR_EXIST_GET для всех присоединенных к moduleId ms
        //IssoExchangeManager exchange;
        //QByteArray multiSensorIds = exchange.moduleMultiSensorAddrs(module);
        // сформировать запрос установки адресов мультидатчика для опроса
        //IssoExchangeManager(module->ip(), module->port(),
        //                    module->alarmPort )
        //auto request = makeMultiSensorPollingAddrsRequest(module->ip(), module->port(), multiSensorIds);
        // асинхронно добавить запрос в очередь
        //addToUserQueueAsync(request);
  //      emit


    }
    else
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно запросить состояние модуля, т.к. модуль не выбран"));
        return;
    }
}


void GraphicPlanTabWidget::processContextMenuRequested(const QPoint &pos)
{
    // определить строку таблицы, над которой был клик мыши
    QModelIndex index = ui->tvEvents->indexAt(pos);
    if (!index.isValid())
        return;
    // задать видимость пунктов меню
    setMenuItemsVisible(index);
    // показать меню
    m_tableMenu.popup(ui->tvEvents->viewport()->mapToGlobal(pos));
}


void GraphicPlanTabWidget::editComment()
{
    // определить выбранный модельный индекс
    QModelIndex curIndex = ui->tvEvents->selectionModel()->currentIndex();
    if (!curIndex.isValid())
        return;
    // номер выбранной строки
    int selectedRow = curIndex.row();
    // ID события
    int eventId = ui->tvEvents->model()->index(selectedRow,
                                               IssoEventQueryModel::COL_ID/*0*/).data().toInt();
    // дата
    QString date = ui->tvEvents->model()->index(selectedRow,
                                                IssoEventQueryModel::COL_DATE/*1*/).data().toString();
    // время
    QString time = ui->tvEvents->model()->index(selectedRow,
                                                IssoEventQueryModel::COL_TIME/*2*/).data().toString();
    // имя модуля
    QString moduleName = ui->tvEvents->model()->index(selectedRow,
                                                      IssoEventQueryModel::COL_MODULE/*3*/).data().toString();
    // состояние
    QString state = ui->tvEvents->model()->index(selectedRow,
                                                 IssoEventQueryModel::COL_STATE/*4*/).data().toString();
    // имя датчика
    QString sensorName = ui->tvEvents->model()->index(selectedRow,
                                                      IssoEventQueryModel::COL_SENSOR/*5*/).data().toString();
    // комментарий
    QString comment = ui->tvEvents->model()->index(selectedRow,
                                                   IssoEventQueryModel::COL_COMMENT/*6*/).data().toString();
    //
    // вывести диалог
    CommentDialog dlg(date, time, moduleName, state, sensorName, comment, this);
    int result = dlg.exec();
    if (result == QDialog::Accepted)
    {
        // инициировать запрос записи комментария
        emit writeEventCommentRequested(eventId, dlg.comment());
    }
}


void GraphicPlanTabWidget::showPhoto()
{
    // определить выбранный модельный индекс
    QModelIndex curIndex = ui->tvEvents->selectionModel()->currentIndex();
    if (!curIndex.isValid())
        return;
    // дата
    QString date = ui->tvEvents->model()->
                    index(curIndex.row(),IssoEventQueryModel::COL_DATE).data().toString();
    // время
    QString time = ui->tvEvents->model()->
                    index(curIndex.row(),IssoEventQueryModel::COL_TIME).data().toString();
    // модуль
    QString module = ui->tvEvents->model()->
                    index(curIndex.row(),IssoEventQueryModel::COL_MODULE).data().toString();
    // мультидатчик
    QString sensor = ui->tvEvents->model()->
                    index(curIndex.row(),IssoEventQueryModel::COL_SENSOR).data().toString();
    // заголовок
    QString title = QString("[%1_%2]   %3 <%4>")
                        .arg(date).arg(time).arg(module).arg(sensor);
    // значение ячейки фото
    QVariant value = ui->tvEvents->model()->index(curIndex.row(),
                                                  IssoEventQueryModel::COL_PHOTO).data();
    if (!value.isNull())
    {
//        // преобразовать в JPEG
//        QImage photo = QImage::fromData(value.toByteArray(), "JPG");
        // преобразовать в JPEG
        QImage photo = IssoCommonData::convertBase64ToImage(value.toString(), "JPG");

        // отобразить
        QLabel* label = new QLabel();
        label->setPixmap(QPixmap::fromImage(photo));
        label->setWindowTitle(title);
        label->resize(photo.size());
        label->setAttribute(Qt::WA_DeleteOnClose);
        label->show();
    }
}


void GraphicPlanTabWidget::selectEvents()
{
    if (!m_object)
        return;

    // инициировать запрос выборки событий из БД
    emit selectEventsRequested(ui->deFilterDate->date(),
                               ui->teFilterTimeFrom->time(),
                               ui->teFilterTimeTo->time(),
                               ui->cbFilterEventType->currentText(),
                               m_object->id(),
                               ui->chkEventFilter->isChecked());
    // скрыть столбцы id и comment
    ui->tvEvents->horizontalHeader()->setSectionHidden(IssoEventQueryModel::COL_ID/*0*/, true);
    ui->tvEvents->horizontalHeader()->setSectionHidden(IssoEventQueryModel::COL_COMMENT/*6*/, true);

    // подогнать высоту строк под содержимое
    ui->tvEvents->resizeColumnsToContents();
    // расположение должно заполнять свободное пространство
    ui->tvEvents->horizontalHeader()->setSectionResizeMode(IssoEventQueryModel::COL_LOCATION/*9*/, QHeaderView::Stretch);
    // подогнать высоту строк под содержимое
    ui->tvEvents->resizeRowsToContents();

    // прокрутить до 1ой записи
    ui->tvEvents->scrollToTop();
//    // очистить поле ввода комментария
//    ui->txteComment->clear();
}


void GraphicPlanTabWidget::processScenarioStateChanged(IssoScenarioInfo scenarioInfo,
                                                       bool active)
{
    if (active)
    {
        insertRunningScenario(scenarioInfo);
        qDebug() << "Scenario button checked: " << scenarioInfo.complexName() << endl;
    }
    else
        removeRunningScenario(scenarioInfo.complexName());

    // обновить кнопки панели управления системой (сценариями)
    updateSystemContolButtons();
}


void GraphicPlanTabWidget::processSensorStatesChanged(const QString &moduleName)
{
    if (!m_object)
        return;
    IssoModule* module = m_object->findModuleByDisplayName(moduleName);
    if (!module)
        return;
    //
    // обновить состояния всех визуальных списков
    updateAllLists();
    // если источником события является выбранный модуль
    IssoModule* currentModule = m_object->selectedModule();
    if ((currentModule) && (currentModule->displayName() == moduleName))
        // обновить информацию о модуле
        updateSelectedModuleInfo();
    //
    // если модуль находится в тревожном состоянии
    if (module->alarmed())
    {
        // позиционировать окно отображения на тревожный модуль
        displayAlarmModule(module);
    }
}


void GraphicPlanTabWidget::processRelayStateChanged(const QString &moduleName,
                                                    IssoParamId paramId, bool pressed)
{
    if (!m_object)
        return;
    // если источником события является выбранный модуль
    IssoModule* currentModule = m_object->selectedModule();
    if ((!currentModule) || (currentModule->displayName() != moduleName))
        return;
    //
    // получить конфиг модуля
    IssoModuleConfig* cfg = m_object->moduleConfig(currentModule->moduleConfigName());
    if (!cfg)
        return;
    //
    // получить номера кнопок, связанных с реле
    auto buttonNums = cfg->getButtonNumbersByRelayId(paramId);
    // обновить состояние каждой кнопки, связанной с реле
    foreach (int buttonNum, buttonNums)
    {
        // получить кнопку по номеру
        QPushButton* button = m_moduleButtons.value(buttonNum, nullptr);
        // обновить состояния кнопок
        updateButtonStyle(button, pressed);
    }
}

//void GraphicPlanTabWidget::processRelayStateChanged(const QString &moduleName,
//                                                    IssoParamId paramId, bool pressed)
//{
//    if (!m_object)
//        return;
//    // если источником события является выбранный модуль
//    IssoModule* currentModule = m_object->selectedModule();
//    if ((currentModule) && (currentModule->displayName() == moduleName))
//    {
//        // получить кнопку, соответствующую параметру
//        QPushButton* button = m_moduleButtons.value(paramId, nullptr);
//        // обновить состояния кнопок
//        updateRelayButtonStyle(button, pressed);
//    }
//}


void GraphicPlanTabWidget::insertRunningScenario(IssoScenarioInfo info)
{
    m_runningScenarios.insert(info.complexName(), info);
}


void GraphicPlanTabWidget::removeRunningScenario(const QString &complexName)
{
    m_runningScenarios.remove(complexName);
}


IssoScenarioInfo GraphicPlanTabWidget::runningScenario(const QString &complexName)
{
    return m_runningScenarios.value(complexName, IssoScenarioInfo());
}


bool GraphicPlanTabWidget::scenarioIsRunning(const QString &complexName)
{
    return m_runningScenarios.contains(complexName);
}


void GraphicPlanTabWidget::initWidgets()
{
    // установить делегат для кастомной отрисовки элементов списка зданий и этажей
    IssoFloorStateDelegate* floorStateDelegate = new IssoFloorStateDelegate();
    //
    // список зданий:
    //
    // задать заголовок
    ui->wgtBuilds->setTitle(tr("Здания"));
    // скрыть кнопку редактирования
    ui->wgtBuilds->setEditBtnVisible(false);
    // задать делегат
    ui->wgtBuilds->list()->setItemDelegate(floorStateDelegate);
    //
    // список этажей:
    //
    // задать заголовок
    ui->wgtFloors->setTitle(tr("Этажи"));
    // показать кнопку редактирования
    ui->wgtFloors->setEditBtnVisible(true);
    // задать делегат
    ui->wgtFloors->list()->setItemDelegate(floorStateDelegate);
    //
    // список модулей:
    //
    // задать заголовок
    ui->wgtModules->setTitle(tr("Модули"));
    // скрыть кнопку редактирования
    ui->wgtModules->setEditBtnVisible(false);
    // задать делегат
    ui->wgtModules->list()->setItemDelegate(new IssoModuleStateDelegate(this));
    //
    // установить делегат для кастомной отрисовки элементов журнала событий
    ui->tvEvents->setItemDelegate(new IssoEventDelegate());
    //
    // добавить эффект тени в статус модуля
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(QColor(Qt::black));
    shadowEffect->setOffset(0, 0);
    shadowEffect->setBlurRadius(6);
    g_lblModuleState->setGraphicsEffect(shadowEffect);
    //
    // скрыть тестовую панель
    ui->gbTestButtons->setVisible(false);
}


void GraphicPlanTabWidget::initWidgetVars(IssoGuiMode mode)
{
    // инициализировать переменные, ссылающиеся на визуальные компоненты,
    // в зависимости от выбранного устройства вывода
    if (mode == IssoGuiMode::STANDART_MONITOR)
    {
        // данные выбранного модуля
        g_lblModuleName = ui->lblModuleName;
        g_lblModuleStatus = ui->lblModuleStatus;
        g_lblModuleState = ui->lblModuleState;
        // управление модулем
        m_moduleButtons.insert(1, ui->btnModule1);
        m_moduleButtons.insert(2, ui->btnModule2);
        m_moduleButtons.insert(3, ui->btnModule3);
        m_moduleButtons.insert(4, ui->btnModule4);
        m_moduleButtons.insert(5, ui->btnModule5);
        m_moduleButtons.insert(6, ui->btnModule6);
        // управление системой
        m_systemButtons.insert(1, ui->btnSystem1);
        m_systemButtons.insert(2, ui->btnSystem2);
        m_systemButtons.insert(3, ui->btnSystem3);
        m_systemButtons.insert(4, ui->btnSystem4);
        m_systemButtons.insert(5, ui->btnSystem5);
        m_systemButtons.insert(6, ui->btnSystem6);
        m_systemButtons.insert(7, ui->btnSystem7);
        m_systemButtons.insert(8, ui->btnSystem8);
        // кнопка SOS
        g_btnSos = ui->btnSos;
        // карта лейблов, отображающих состояния параметров модуля (Наблюдение)
        m_sensorLabels.insert(IssoParamId::SENSOR_SMOKE, ui->lblSensorSmoke);
        m_sensorLabels.insert(IssoParamId::SENSOR_CO, ui->lblSensorCO);
        m_sensorLabels.insert(IssoParamId::SENSOR_OPEN, ui->lblSensorOpen);
        m_sensorLabels.insert(IssoParamId::VIBRATION, ui->lblVibration);
        m_sensorLabels.insert(IssoParamId::BTN_FIRE, ui->lblBtnFire);
        m_sensorLabels.insert(IssoParamId::BTN_SECURITY, ui->lblBtnSecurity);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_1, ui->lblSensor1);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_2, ui->lblSensor2);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_3, ui->lblSensor3);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_4, ui->lblSensor4);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_5, ui->lblSensor5);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_6, ui->lblSensor6);
        m_sensorLabels.insert(IssoParamId::VOLT_SENSORS, ui->lblVoltSensors);
        m_sensorLabels.insert(IssoParamId::TEMP_OUT, ui->lblTempOut);
        m_sensorLabels.insert(IssoParamId::VOLT_BUS, ui->lblVoltBus);
        m_sensorLabels.insert(IssoParamId::VOLT_BATTERY, ui->lblVoltBattery);
        m_sensorLabels.insert(IssoParamId::SUPPLY_INTERNAL, ui->lblSupplyInternal);
        m_sensorLabels.insert(IssoParamId::SUPPLY_EXTERNAL, ui->lblSupplyExternal);
        m_sensorLabels.insert(IssoParamId::MULTISENSOR, ui->lblDigitalChain1);
    }
    else
    {
        // данные выбранного модуля
        g_lblModuleName = ui->lblTouchModuleName;
        g_lblModuleStatus = ui->lblTouchModuleStatus;
        g_lblModuleState = ui->lblTouchModuleState;
        // управление модуля
        m_moduleButtons.insert(1, ui->btnTouchModule1);
        m_moduleButtons.insert(2, ui->btnTouchModule2);
        m_moduleButtons.insert(3, ui->btnTouchModule3);
        m_moduleButtons.insert(4, ui->btnTouchModule4);
        m_moduleButtons.insert(5, ui->btnTouchModule5);
        // управление системой
        m_systemButtons.insert(1, ui->btnTouchSystem1);
        m_systemButtons.insert(2, ui->btnTouchSystem2);
        m_systemButtons.insert(3, ui->btnTouchSystem3);
        m_systemButtons.insert(4, ui->btnTouchSystem4);
        m_systemButtons.insert(5, ui->btnTouchSystem5);
        m_systemButtons.insert(6, ui->btnTouchSystem6);
        m_systemButtons.insert(7, ui->btnTouchSystem7);
        m_systemButtons.insert(8, ui->btnTouchSystem8);
        // кнопка SOS
        g_btnSos = ui->btnTouchSos;
        // карта лейблов, отображающих состояния парметров модуля (Наблюдение)
        m_sensorLabels.insert(IssoParamId::SENSOR_SMOKE, ui->lblTouchSensorSmoke);
        m_sensorLabels.insert(IssoParamId::SENSOR_CO, ui->lblTouchSensorCO);
        m_sensorLabels.insert(IssoParamId::SENSOR_OPEN, ui->lblTouchSensorOpen);
        m_sensorLabels.insert(IssoParamId::VIBRATION, ui->lblTouchVibration);
        m_sensorLabels.insert(IssoParamId::BTN_FIRE, ui->lblTouchBtnFire);
        m_sensorLabels.insert(IssoParamId::BTN_SECURITY, ui->lblTouchBtnSecurity);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_1, ui->lblTouchSensor1);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_2, ui->lblTouchSensor2);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_3, ui->lblTouchSensor3);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_4, ui->lblTouchSensor4);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_5, ui->lblTouchSensor5);
        m_sensorLabels.insert(IssoParamId::ANALOG_CHAIN_6, ui->lblTouchSensor6);
        m_sensorLabels.insert(IssoParamId::VOLT_SENSORS, ui->lblTouchVoltSensors);
        m_sensorLabels.insert(IssoParamId::TEMP_OUT, ui->lblTouchTempOut);
        m_sensorLabels.insert(IssoParamId::VOLT_BUS, ui->lblTouchVoltBus);
        m_sensorLabels.insert(IssoParamId::VOLT_BATTERY, ui->lblTouchVoltBattery);
        m_sensorLabels.insert(IssoParamId::SUPPLY_INTERNAL, ui->lblTouchSupplyInternal);
        m_sensorLabels.insert(IssoParamId::SUPPLY_EXTERNAL, ui->lblTouchSupplyExternal);
        m_sensorLabels.insert(IssoParamId::MULTISENSOR, ui->lblTouchDigitalChain1);
    }
    // добавить хинты для лейблов состояний
    foreach (IssoParamId paramId, m_sensorLabels.keys())
    {
        // получить описание параметра
        QString hint = IssoCommonData::stringByParamId(paramId);
        // задать лейблу подсказку
        m_sensorLabels.value(paramId)->setToolTip(hint);
    }
}


void GraphicPlanTabWidget::initWidgetHandlers()
{
    // выбрать здание
    connect(ui->wgtBuilds, SIGNAL(selectionChanged()),
            SLOT(processBuildingChanged()));
    // выбрать этаж
    connect(ui->wgtFloors, SIGNAL(selectionChanged()),
            SLOT(processFloorChanged()));
//    connect(ui->btnWriteComment, SIGNAL(clicked(bool)),
//            this,                SLOT(processBtnWriteCommentClicked()));
    connect(ui->btnRequestModuleInfo, SIGNAL(clicked(bool)),
            this,                     SLOT(processBtnRequestModuleInfoClicked()));
    // кнопки Управление модулем
    foreach (QPushButton* button, m_moduleButtons.values())
    {
        connect(button, SIGNAL(clicked(bool)),
                this,   SLOT(processModuleFuncBtnClicked()));
    }
    // кнопки Управление системой
    foreach (QPushButton* button, m_systemButtons.values())
    {
        connect(button, SIGNAL(clicked(bool)),
                this,   SLOT(processSystemFuncBtnClicked()));
    }
    // кнопка МЧС
    connect(g_btnSos, SIGNAL(clicked(bool)),
            this,     SLOT(processBtnSosClicked()));
    // добавить здание
    connect(ui->wgtBuilds, SIGNAL(addBtnClicked(bool)),
            SLOT(processBtnAddBuildingClicked()));
    // удалить здание
    connect(ui->wgtBuilds, SIGNAL(removeBtnClicked(bool)),
            SLOT(processBtnRemoveBuildingClicked()));
    // добавить этаж
    connect(ui->wgtFloors, SIGNAL(addBtnClicked(bool)),
            SLOT(processBtnAddFloorClicked()));
    // удалить этаж
    connect(ui->wgtFloors, SIGNAL(removeBtnClicked(bool)),
            SLOT(processBtnRemoveFloorClicked()));
    // изменить этаж
    connect(ui->wgtFloors, SIGNAL(editBtnClicked(bool)),
            SLOT(processBtnEditFloorClicked()));
    // добавить модуль на этаж
    connect(ui->wgtModules, SIGNAL(addBtnClicked(bool)),
            SLOT(processBtnAddModuleToFloorClicked()));
    // удалить модуль с этажа
    connect(ui->wgtModules, SIGNAL(removeBtnClicked(bool)),
            SLOT(processBtnRemoveModuleFromFloorClicked()));
//    // обработчик выбора строки в таблице событий
//    connect(ui->tvEvents, &QTableView::clicked,
//            [this](QModelIndex idx)
//            {
//                // получить прокси-модель из индекса
//                const QSortFilterProxyModel* model =
//                        static_cast<const QSortFilterProxyModel*>(idx.model());
//                if (!model)
//                    return;
//                // получить значение поля comment (стоблец 6) выбранной записи
//                QVariant value = model->data(model->index(idx.row(), 6));
//                // вывести в поле ввода комментария текущий (если задан)
//                QString comment = (value.isNull() ? "" : value.toString());
//                ui->txteComment->setText(comment);
//            });
    // события действия над камерой
    connect(ui->view, SIGNAL(cameraTurned(int,bool)),
            this,     SIGNAL(cameraTurned(int,bool)));
    // событие выбора действия над датчиком
    connect(ui->view, SIGNAL(sensorActionPerformed(quint8,int,quint8,IssoChainType,IssoSensorAction)),
            this,     SIGNAL(sensorActionPerformed(quint8,int,quint8,IssoChainType,IssoSensorAction)));
    // событие запроса обновления конфига объекта на сервере
    connect(ui->view, SIGNAL(objectConfigUpdateRequested()),
            this,     SIGNAL(objectConfigUpdateRequested()));
    //
    // обработчики фильтра событий
    setEventFilterHandlers();
}


void GraphicPlanTabWidget::setEventFilterHandlers()
{
    connect(ui->chkEventFilter, &QCheckBox::toggled,
            [this](bool checked)
            {
                ui->deFilterDate->setEnabled(checked);
                ui->teFilterTimeFrom->setEnabled(checked);
                ui->teFilterTimeTo->setEnabled(checked);
                ui->cbFilterEventType->setEnabled(checked);
                selectEvents();
            });
    connect(ui->deFilterDate,       SIGNAL(dateChanged(QDate)),
            this,                   SLOT(selectEvents()));
    connect(ui->teFilterTimeFrom,   SIGNAL(timeChanged(QTime)),
            this,                   SLOT(selectEvents()));
    connect(ui->teFilterTimeTo,     SIGNAL(timeChanged(QTime)),
            this,                   SLOT(selectEvents()));
    connect(ui->cbFilterEventType,  SIGNAL(currentIndexChanged(int)),
            this,                   SLOT(selectEvents()));
}


void GraphicPlanTabWidget::setSelectionHandlersEnabled(bool enabled)
{
    // нужно для первого случая,
    // когда текущий план этажа не установлен
    if (!m_object || !m_object->currentFloor())
        return;

    if (enabled)
    {
        // установить обработчики выделения
        connect(ui->view->scene(), SIGNAL(selectionChanged()),
                SLOT(processModuleSelectionChanged()));
        connect(ui->wgtModules,   SIGNAL(selectionChanged()),
                SLOT(processModuleNameSelectionChanged()));
        connect(ui->wgtModules,   SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                SLOT(processModuleNameDoubleClicked(QListWidgetItem*)));
    }
    else
    {
        // снять обработчики выделения
        disconnect(ui->view->scene(), SIGNAL(selectionChanged()), 0, 0);
        disconnect(ui->wgtModules,   SIGNAL(selectionChanged()), 0, 0);
        disconnect(ui->wgtModules, SIGNAL(itemDoubleClicked(QListWidgetItem*)), 0, 0);
    }
}


void GraphicPlanTabWidget::setCurrentFloorPlan(IssoFloorPlan *floor)
{
    if (!m_object)
        return;
    // снять обработчики выделения
    setSelectionHandlersEnabled(false);
    //
    // убрать текущий план этажа
    hideScene();
    // если новый этаж задан
    if (floor)
        // установить новый план этажа
        displayScene(floor);
    else
    {
        // запомнить текущий план этажа
        m_object->setCurrentFloor(floor);
        // заполнить визуальный список модулей
        // выбранного этажа
        fillVisualModulesList(floor);
    }
    //
    // установить обработчики выделения
    setSelectionHandlersEnabled(true);
    // обновить состояния выбранного модуля
    updateSelectedModuleInfo();
}


void GraphicPlanTabWidget::fillVisualFloorList(IssoBuilding *build)
{
    // очистить визуальный список этажей
    ui->wgtFloors->clearList();
    if (build)
    {
        // получить имена всех этажей выбранного здания
        QStringList names = build->floorNames();
        // заполнить визуальный список этажей
        ui->wgtFloors->addItems(names);
    }
    // обновить состояния этажей
    updateFloorsList();
}


void GraphicPlanTabWidget::fillVisualModulesList(IssoFloorPlan *floor)
{
    // очистить визуальный список модулей
    ui->wgtModules->clearList();
    // если этаж не задан, выход
    if (!floor)
        return;
    //
    // сформировать список состояний в заданной очередности
    QList<IssoModuleState> states = { ALARM, FAIL, ACTIVE, INACTIVE };
    //
    // вывести в визуальный список модули в соответствии со списком состояний
    foreach (IssoModuleState state, states)
    {
        // получить имена модулей, имеющих текущее состояние
        QStringList moduleNames = floor->moduleNamesByState(state);
        // добавить имена модулей в визуальный список
        foreach (QString name, moduleNames)
        {
            // создать элемент списка с именем модуля
            QListWidgetItem* item = new QListWidgetItem(name);
            // добавить к элементу состояние модуля
            item->setData(Qt::UserRole, state);
            // добавить в визуальный список
            ui->wgtModules->addItem(item);
        }
    }
}


void GraphicPlanTabWidget::clearWidgetLists()
{
    ui->wgtBuilds->clearList();
    ui->wgtFloors->clearList();
    ui->wgtModules->clearList();
}


void GraphicPlanTabWidget::updateBuildingsList()
{
    if (!m_object)
        return;
    //
    // заполнить визуальный список зданий
    for (int i = 0; i < ui->wgtBuilds->itemCount(); ++i)
    {
        // текущий элемент списка
        QListWidgetItem* item = ui->wgtBuilds->item(i);
        // найти здание по имени
        IssoBuilding* build = m_object->findBuildingByName(item->text());
        if (!build)
            continue;
        // получить набор состояний модулей здания
        QSet<IssoModuleState> states = build->moduleStates();
        // задать этажу наличие модулей в состоянии тревога
        item->setData(Qt::UserRole + 1, states.contains(ALARM));
        // задать этажу наличие модулей в состоянии неисправность
        item->setData(Qt::UserRole + 2, states.contains(FAIL));
        // задать этажу наличие модулей в состоянии оффлайн
        item->setData(Qt::UserRole + 3, states.contains(INACTIVE));
    }
}


void GraphicPlanTabWidget::updateFloorsList()
{
    if (!m_object)
        return;
    //
    // заполнить визуальный список этажей
    for (int i = 0; i < ui->wgtFloors->itemCount(); ++i)
    {
        // текущий элемент списка
        QListWidgetItem* item = ui->wgtFloors->item(i);
        // найти этаж по имени
        IssoFloorPlan* floor =
                m_object->currentBuilding()->findFloorByName(item->text());
        if (!floor)
            continue;
        // получить набор состояний модулей этажа
        QSet<IssoModuleState> states = floor->moduleStates();
        // задать этажу наличие модулей в состоянии тревога
        item->setData(Qt::UserRole + 1, states.contains(ALARM));
        // задать этажу наличие модулей в состоянии неисправность
        item->setData(Qt::UserRole + 2, states.contains(FAIL));
        // задать этажу наличие модулей в состоянии оффлайн
        item->setData(Qt::UserRole + 3, states.contains(INACTIVE));
    }
}


void GraphicPlanTabWidget::updateModulesList()
{
    if (!m_object)
        return;
    //
    // снять обработчики изменений сцены и визуального списка
    setSelectionHandlersEnabled(false);
    // запомнить выбранный элемент визуального списка
    QString selectedName = "";
    // проверить наличие выделенных строк визуального списка
    if (ui->wgtModules->hasSelectedItems())
    {
        // найти выделенную строку визуального списка
        selectedName = ui->wgtModules->selectedItemName();
    }
    // обновить визуальный список модулей
    fillVisualModulesList(m_object->currentFloor());
    // если элемент списка был выбран, установить выделение
    if (!selectedName.isEmpty())
    {
        // найти элемент для выделения
        QListWidgetItem* item = ui->wgtModules->findItem(selectedName);
        // если найден, выделить
        if (item)
            item->setSelected(true);
    }
    // установить обработчики изменений сцены и визуального списка
    setSelectionHandlersEnabled(true);
}


void GraphicPlanTabWidget::updateAllLists()
{
    // обновить состояние списка модулей
    updateModulesList();
    // обновить состояние списка этажей
    updateFloorsList();
    // обновить состояние списка зданий
    updateBuildingsList();
}


void GraphicPlanTabWidget::updateSelectedModuleInfo()
{
    if (!m_object)
        return;
    // выбранный модуль
    IssoModule* module = m_object->selectedModule();
    // если модуль не выбран, занулить поля
    if (!module)
    {
        // имя выбранного модуля
        g_lblModuleName->setText("");
        // общее состояние модуля
        g_lblModuleStatus->setText("");
        // статус модуля
        g_lblModuleState->setText("");
    }
    // иначе отобразить инфо
    else
    {
        // имя выбранного модуля
        g_lblModuleName->setText(module->displayName());
        // статус модуля
        g_lblModuleStatus->setText(tr("Исправен"));
        // состояние модуля
        g_lblModuleState->setText(IssoCommonData::stringByModuleState(module->moduleState()));
        switch(module->moduleState())
        {
            case ALARM:
            {
                g_lblModuleState->setStyleSheet("color: red");
                ((QGraphicsDropShadowEffect*)g_lblModuleState->graphicsEffect())->
                                                        setColor(QColor(Qt::gray));
                break;
            }
            case FAIL:
            {
                g_lblModuleState->setStyleSheet("color: yellow");
                ((QGraphicsDropShadowEffect*)g_lblModuleState->graphicsEffect())->
                                                        setColor(QColor(Qt::black));
                break;
            }
            case ACTIVE:
            {
                g_lblModuleState->setStyleSheet("color: green");
                ((QGraphicsDropShadowEffect*)g_lblModuleState->graphicsEffect())->
                                                        setColor(QColor(Qt::gray));
                break;
            }
            case INACTIVE:
            {
                g_lblModuleState->setStyleSheet("color: lightgray");
                ((QGraphicsDropShadowEffect*)g_lblModuleState->graphicsEffect())->
                                                        setColor(QColor(Qt::black));
                break;
            }
            default:
                break;
        }
    }
    // обновить состояния выбранного модуля
    updateSelectedModuleSensorStates();
    // обновить кнопки панели управления
    updateModuleContolButtons();
}

/**
 * @brief GraphicPlanTabWidget::updateSelectedModuleSensorStates
 * обновляем состояния сенсоров
 * @todo необходимо уточнить, какие состояния мы считаем
 * неисправностью. Ошибка начальной инициализации
 */
void GraphicPlanTabWidget::updateSelectedModuleSensorStates()
{
    if (!m_object)
        return;
    // выбранный модуль
    IssoModule* module = m_object->selectedModule();
    // определить, находится ли модуль в неактивном состоянии
    bool inactive = (!module) || (module->moduleState() == INACTIVE);
    // обновить все лейблы состояний датчиков модуля
    foreach (IssoParamId paramId, m_sensorLabels.keys())
    {
        // состояние датчика
        IssoState sensorState;
        // имя датчика
        QString sensorName;
        // @detail если модуль неактивен или датчик / шлейф отключен
        // cha модуль активен, если конфиг модуля найден,т.е. не нулевой указатель
        // затем мы должны залезть на шлейф и там проверить был ли мульти датчик активен,
        //  а теперь отвалился или изначально висит неактивированный.
        // @todo проверить есть ли начальная инициализация параметров шлейфов.
        if (inactive ||
                (!m_object->moduleParamEnabled(module->moduleConfigName(), paramId)))
        {
            sensorState = STATE_UNDEFINED;
        }
        // если датчик подключен
        else
        {
            // получить параметр состояния датчика
            IssoSensorStateParam* stateParam = module->stateParamById(paramId);
            // получить состояние датчика
            sensorState = (stateParam ? stateParam->state() : STATE_UNDEFINED);
            // получить имя типа датчика модуля
            sensorName = IssoCommonData::stringByParamId(paramId);
            // для некоторых параметров нужно выводить значения
            switch(paramId)
            {
                // Уровень напряжение на АКБ
                case VOLT_BATTERY:
                // Уровень напряжение на шине 12 В
                case VOLT_BUS:
                // Уровень напряжения датчиков
                case VOLT_SENSORS:
                // внешняя температура
                case TEMP_OUT:
                {
                    // получить параметр расширенного состояния датчика
                    IssoSensorExtStateParam* extStateParam =
                            dynamic_cast<IssoSensorExtStateParam*>(stateParam);
                    if (extStateParam)
                    {
                        // добавить к имени датчика значение (напряжение / температура)
                        if (paramId == TEMP_OUT)
                            sensorName += tr(" %1 C\u00B0").arg(extStateParam->value(), 0, 'f', 1);
                        else
                            sensorName += tr(" %1 В").arg(extStateParam->value(), 0, 'f', 1);
                    }
                    break;
                }
                case ANALOG_CHAIN_1:
                case ANALOG_CHAIN_2:
                case ANALOG_CHAIN_3:
                case ANALOG_CHAIN_4:
                case ANALOG_CHAIN_5:
                case ANALOG_CHAIN_6:
                case MULTISENSOR:
                {
                    sensorName = IssoCommonData::stringBySensorState(sensorState);
                    break;
                }
                default:
                    break;
            }
        }
        sensorName = (sensorName.isEmpty() ? tr("Нет") : sensorName);
        // обновить лейбл
        updateSensorLabelStyle(paramId, sensorState, sensorName);
    }
}



void GraphicPlanTabWidget::updateModuleContolButtons()
{
    if (!m_object)
        return;
    // выбранный модуль
    IssoModule* module = m_object->selectedModule();

    if (module)
    {
        // получить конфиг модуля
        IssoModuleConfig* cfg = m_object->moduleConfig(module->moduleConfigName());
        if (!cfg)
            return;
        // отобразить имена классов на кнопках согласно конфигу
        foreach (int buttonNum, m_moduleButtons.keys())
        {
            // получить кнопку по ее номеру
            QPushButton* btn = m_moduleButtons.value(buttonNum);
            if (!btn)
                continue;
            //
            // задать имя кнопки:
            //
            // получить имя класса, связанного с кнопкой
            QString btnClassName = cfg->buttonClassLink(buttonNum);
            // отобразить на кнопке имя класса
            btn->setText(btnClassName);
            //
            // задать состояния кнопки:
            //
            // получить Id реле, связанного с кнопкой
            auto relayIds = cfg->getRelayIdsByButtonNumber(buttonNum);
            if (relayIds.isEmpty())
                continue;
            IssoParamId relayId = relayIds.first();
            // получить параметр состояния реле по Id
            IssoSensorStateParam* stateParam = module->stateParamById(relayId);
            // получить состояние реле
            IssoState sensorState = (stateParam ? stateParam->state() : STATE_UNDEFINED);
            // состояние кнопки
            bool btnPressed = (sensorState == RELAY_ON);
            //
            // обновить стиль кнопки
            updateButtonStyle(btn, btnPressed);
        }
    }
    else
    {
        foreach (QPushButton* btn, m_moduleButtons)
        {
            // очистить надпись
            btn->setText("");
            // обновить стиль кнопки
            updateButtonStyle(btn, false);
        }
    }
}


void GraphicPlanTabWidget::updateSystemContolButtons()
{
    if (!m_object)
        return;
    // определить этаж
    IssoFloorPlan* floor = m_object->currentFloor();
    if (!floor)
        return;
    // получить номера здания и этажа
    int buildNum = floor->buildNum();
    int floorNum = floor->number();

    foreach (int buttonNum, m_systemButtons.keys())
    {
        // получить имя сценария, привязанного к кнопке
        QString scenarioName = m_object->buttonScenarioLink(buttonNum);
        // отобразить имя сценария на кнопке панели управления системой
        QPushButton* btn = systemButton(buttonNum);
        btn->setText(scenarioName);

        // сформировать имя сценария, специфичное для данного здания / этажа
        IssoScenarioInfo info(scenarioName, buildNum, floorNum);
        // определить, запущен ли данный сценарий
        bool active = scenarioIsRunning(info.complexName());
        // отобразить состояние на кнопке
        updateButtonStyle(btn, active);
    }
}


void GraphicPlanTabWidget::updateSensorLabelStyle(IssoParamId paramId,
                                                  IssoState sensorState,
                                                  const QString &sensorName)
{
    // получить лейбл, соответствующий параметру
    IssoStateLabel* label = m_sensorLabels.value(paramId, nullptr);
    // если найден, задать состояние лейбла
    if (label)
        label->setState(sensorState, sensorName);
}


void GraphicPlanTabWidget::updateButtonStyle(QPushButton *button, bool pressed)
{
    if (button)
    {
        // установить цвет фона согласно состоянию (нажата или нет)
        QString styleSheet = (pressed ? IssoConst::ACTIVE_BTN_STYLE :
                                        IssoConst::INACTIVE_BTN_STYLE)
                                + QString("min-width: %1px; min-height: %2px;"
                                          "max-width: %1px; max-height: %2px;")
                                                        .arg(button->width())
                                                        .arg(button->height());
        button->setStyleSheet(styleSheet);
    }
}


bool GraphicPlanTabWidget::buttonPressed(QPushButton *button)
{
    return (button && button->styleSheet().contains(IssoConst::ACTIVE_BTN_STYLE));
}


void GraphicPlanTabWidget::updateTab()
{
    if (m_object)
        m_object->calcTotalSummary();
    // обновить графплан этажа
    processFloorChanged();
}


void GraphicPlanTabWidget::setEventFilter(const QDate& date,
                                          const QTime& timeFrom,
                                          const QTime& timeTo,
                                          const QString& eventType)
{
    ui->deFilterDate->setDate(date);
    ui->teFilterTimeFrom->setTime(timeFrom);
    ui->teFilterTimeTo->setTime(timeTo);
    ui->cbFilterEventType->setCurrentText(eventType);
}


void GraphicPlanTabWidget::setEventFilterEnabled(bool enabled)
{
    ui->chkEventFilter->setChecked(enabled);
}


bool GraphicPlanTabWidget::indicatorsVisible()
{
    return ui->wgtIndicators->isVisible();
}


void GraphicPlanTabWidget::setIndicatiorsVisible(bool visible)
{
    ui->wgtIndicators->setVisible(visible);
}


void GraphicPlanTabWidget::displayScene(IssoFloorPlan *floor)
{
    if (!m_object || !floor)
        return;
    // запомнить текущий план этажа
    m_object->setCurrentFloor(floor);
    // установить подложку
    ui->view->setBackgroundImage(floor->background());
    // добавить на сцену модули данного этажа
    ui->view->addModules(floor->modules());
    // добавить на сцену камеры данного этажа
    ui->view->addCameras(floor->cameras());
    // установить область отображения (масштаб и смещение)
    ui->view->setVisibleRect(floor->visibleRect(), floor->scale());
    // установить обработчики событий
    connect(floor,  SIGNAL(moduleAdded(IssoModule*)),
            ui->view, SLOT(addModule(IssoModule*)));
    connect(floor,  SIGNAL(moduleRemoved(IssoModule*)),
            ui->view, SLOT(removeModule(IssoModule*)));
    connect(floor,  SIGNAL(cameraAdded(IssoCamera*)),
            ui->view, SLOT(addCamera(IssoCamera*)));
    connect(floor,  SIGNAL(cameraRemoved(IssoCamera*)),
            ui->view, SLOT(removeCamera(IssoCamera*)));
    connect(floor,  SIGNAL(backgroundChanged(QImage)),
            ui->view, SLOT(setBackgroundImage(QImage)));
    connect(ui->view, SIGNAL(visibleRectChanged(QRectF,qreal)),
            floor,  SLOT(changeVisibleRect(QRectF,qreal)));
    // заполнить визуальный список модулей
    // выбранного этажа
    fillVisualModulesList(floor);
}


void GraphicPlanTabWidget::hideScene()
{
    if (!m_object)
        return;
    // снять выделение с модулей
    ui->view->deselectAllModules();
    // снять обработчики событий
    disconnect(ui->view, SIGNAL(visibleRectChanged(QRectF,qreal)), 0, 0);
    if (m_object->currentFloor())
    {
        disconnect(m_object->currentFloor(), 0, ui->view, 0);
    }
    // удалить все камеры со сцены
    ui->view->removeAllCameras();
    // удалить все модули со сцены
    ui->view->removeAllModules();
    // очистить подложку
    ui->view->setBackgroundImage(QImage());
}


void GraphicPlanTabWidget::updateGuiForTouchScreen()
{
    // списки
    QList<QListWidget*> listWidgets
    {
        ui->wgtFloors->list(),
        ui->wgtBuilds->list(),
        ui->wgtModules->list()
    };
    // адаптировать каждый визуальный список графплана
    foreach(QListWidget* list, listWidgets)
    {
        // при добавлении новой строки в список изменять её размер
        connect(list->model(), &QAbstractItemModel::rowsInserted,
                [this, list](const QModelIndex &parent, int first, int last)
                {
                    updateListWidgetForTouch(list, parent, first, last);
                });

        // добавить прокрутку жестами
        QScroller::grabGesture(list, QScroller::TouchGesture);
    }
    ui->wgtControls->setMaximumHeight(100);
    ui->wgtEvents->setVisible(false);
}


void GraphicPlanTabWidget::updateListWidgetForTouch(QListWidget *list,
                                                  const QModelIndex &parent,
                                                  int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(last);
    for (int i = first; i <= last; ++i)
        list->item(i)->setSizeHint(QSize(0, 40));
}


bool GraphicPlanTabWidget::enterPassword()
{
    // вывести диалог ввода пароля
    return PasswordDialog::inputPassword(this, IssoConst::MODIFY_PASSWORD,
                                         tr("Введен неверный пароль! Действие отклонено"));
}


QPushButton *GraphicPlanTabWidget::systemButton(int buttonNum)
{
    return m_systemButtons.value(buttonNum, nullptr);
}


void GraphicPlanTabWidget::initTableMenu()
{
    initTableMenuActions();
    // задать способ обработки контекстного меню
    ui->tvEvents->setContextMenuPolicy(Qt::CustomContextMenu);
    // обработчик вызова контекстного меню
    connect(ui->tvEvents, SIGNAL(customContextMenuRequested(QPoint)),
            this,         SLOT(processContextMenuRequested(QPoint)));
}


void GraphicPlanTabWidget::initTableMenuActions()
{
    // сформировать пункты меню
    m_tableMenu.addAction(ACT_CHANGE_COMMENT,
                          this, &GraphicPlanTabWidget::editComment);
    m_tableMenu.addAction(ACT_SHOW_PHOTO,
                          this, &GraphicPlanTabWidget::showPhoto);
}


void GraphicPlanTabWidget::setMenuItemsVisible(const QModelIndex &curIndex)
{
    // получить модель
    auto model = curIndex.model();
    // определить наличие фотоснимка
    QString photoExistsStr =
            model->data(model->index(curIndex.row(), IssoEventQueryModel::COL_PHOTO/*8*/)).toString();
    bool photoExists = (!photoExistsStr.isEmpty());
    // задать активность пунктов
    foreach (QAction* action, m_tableMenu.actions())
    {
        if (action->text() == ACT_SHOW_PHOTO)
            action->setVisible(photoExists);
    }
}


void GraphicPlanTabWidget::initTestPanel()
{
    initTestWidgets();
    initTestEventHandlers();
}


void GraphicPlanTabWidget::initTestWidgets()
{
    auto msStates = QList<IssoState>{ STATE_NORMAL, STATE_WARNING, STATE_ALARM };
    foreach (IssoState state, msStates)
    {
        // получить строковое обозначение состояния
        QString stateStr = IssoCommonData::stringBySensorState(state);
        // преобразовать состояние в значение общего состояния мультидатчика
        quint8 stateValue = IssoMultiSensorData::stateToValue(state);
        // добавить в выпадающий список
        ui->cbTestMsState->addItem(stateStr, QVariant::fromValue(stateValue));
    }
}


void GraphicPlanTabWidget::initTestEventHandlers()
{
    // сгенерировать получение состояния "Тревога" для выбранного модуля
    connect(ui->btnAlarmSetToModule, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                IssoModuleReply* reply = testMakeAlarmReply(m_object->selectedModule());
                if (reply)
                {
//                    emit testCtrlAlarmReceived(*reply);
                    emit testCtrlReplyReceived(*reply, true);
                    delete reply;
                }
            });
    //
    // сгенерировать получение состояния "Норма" для выбранного модуля
    connect(ui->btnNormalSetToModule, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                IssoModuleReply* reply = testMakeNoAlarmsReply(m_object->selectedModule());
                if (reply)
                {
                    emit testCtrlReplyReceived(*reply, true);
                    delete reply;
                }
            });
    //
    // сгенерировать получение состояния "Тревога" для всех модулей
    connect(ui->btnAlarmSetToAll, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                // получить список распределенных модулей
                auto modules = m_object->locatedModules();
                // вывести диалог прогресса операции
                QProgressDialog* dlg = IssoCommonData::makeProgressDialog(
                                        "Подождите", "Выполняется отправка состояний модулей...",
                                        0, modules.size(), this);
                // настроить доступ к диалогу из другого потока
                connect(this, SIGNAL(progressCurrent(int)),
                        dlg,  SLOT(setValue(int)));
                connect(this, SIGNAL(progressFinished()),
                        dlg,  SLOT(close()));
                // вывести диалог прогресса
                dlg->show();
                // запустить метод перевода модулей в состояние Тревога асинхронно
                QtConcurrent::run([=]()
                {
                    // количество обработанных модулей
                    int moduleCount = 0;
                    // выполнить операцию для каждого модуля
                    foreach (IssoModule* module, modules)
                    {
                        IssoModuleReply* reply = testMakeAlarmReply(module);
                        if (reply)
                        {
//                            emit testCtrlAlarmReceived(*reply);
                            emit testCtrlReplyReceived(*reply, true);
                            delete reply;
                        }
                        // обновить прогресс
                        emit progressCurrent(++moduleCount);
                    }
                    // закрыть диалог
                    emit progressFinished();
                });
            });
    //
    // сгенерировать получение состояния "Норма" для всех модулей
    connect(ui->btnNormalSetToAll, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                // получить список распределенных модулей
                auto modules = m_object->locatedModules();
                // вывести диалог прогресса операции
                QProgressDialog* dlg = IssoCommonData::makeProgressDialog(
                                            "Подождите", "Выполняется отправка состояний модулей...",
                                            0, modules.size(), this);
                // настроить доступ к диалогу из другого потока
                connect(this, SIGNAL(progressCurrent(int)),
                        dlg,  SLOT(setValue(int)));
                connect(this, SIGNAL(progressFinished()),
                        dlg,  SLOT(close()));
                // вывести диалог прогресса
                dlg->show();
                // запустить метод перевода модулей в состояние Тревога асинхронно
                QtConcurrent::run([=]()
                {
                    // количество обработанных модулей
                    int moduleCount = 0;
                    // выполнить операцию для каждого модуля
                    foreach (IssoModule* module, m_object->locatedModules())
                    {
                        IssoModuleReply* reply = testMakeNoAlarmsReply(module);
                        if (reply)
                        {
                            emit testCtrlReplyReceived(*reply, true);
                            delete reply;
                        }
                        // обновить прогресс
                        emit progressCurrent(++moduleCount);
                    }
                    // закрыть диалог
                    emit progressFinished();
                });
            });
    //
    // сгенерировать получение нового состояния мультидатчика
    connect(ui->btnTestSetMsState, &QPushButton::clicked,
            [this]()
            {
                if (!m_object || (ui->cbTestMsAddress->currentText().isEmpty()))
                    return;
                // адрес мультидатчика
                quint8 msAddr = ui->cbTestMsAddress->currentText().toInt();
                // состояние мультидатчика
                quint8 msState = static_cast<quint8>(ui->cbTestMsState->currentData().toInt());
                //
                // детализация состояний встроенных датчиков
                quint16 msDetails = static_cast<quint16>(msState);
                IssoModuleReply* reply = testMakeMultiSensorStateReply(m_object->selectedModule(),
                                                                       msAddr, msState, msDetails);
                if (reply)
                {
//                    emit testCtrlAlarmReceived(*reply);
                    emit testCtrlReplyReceived(*reply, true);
                    delete reply;
                }
            });
    //
    //
    // сгенерировать получение состояния "Неактивен" для выбранного модуля
    connect(ui->btnInactiveSetToModule, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                IssoModule* module = m_object->selectedModule();
                if (module)
                {
                    // сформировать запрос состояния модуля
                    IssoModuleRequest request(module->ip(), module->port(), 1, CMD_INFO);
                    // инициировать событие отсутствия ответа от модуля
                    emit testCtrlReplyTimedOut(request);
                }
            });
    //
    connect(ui->btnTest, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                // время события
                QString dateTimeStr = QDateTime::currentDateTime().toString("dd.MM.yy   HH:mm");
                IssoLogMessageBuilder builder(m_object, EVENT_SENSORS, dateTimeStr);
                builder.addModuleData(m_object->selectedModule());
                //
                QString sensorName, sensorState;
                sensorName = IssoCommonData::stringByParamId(BTN_FIRE);
                sensorState = IssoCommonData::stringBySensorState(STATE_ALARM);
                builder.addSensorData(sensorName, sensorState, 0, "");
                //
                sensorName = IssoCommonData::stringByParamId(VIBRATION);
                sensorState = IssoCommonData::stringBySensorState(STATE_NORMAL);
                builder.addSensorData(sensorName, sensorState, 0, "");
                //
                QMessageBox* dlg = new QMessageBox(this);
                dlg->setWindowTitle("Информация");
                dlg->setText(builder.getMessage());
                dlg->setAttribute(Qt::WA_DeleteOnClose);
                dlg->show();
            });
    //
    connect(ui->btnTest2, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                IssoModuleReply* reply = testMakeMultiSensorReply(m_object->selectedModule());
                if (reply)
                {
                    emit testCtrlAlarmReceived(*reply);
                    delete reply;
                }
            });
    //
    connect(ui->btnTest3, &QPushButton::clicked,
            [this]()
            {
                if (!m_object)
                    return;
                QString objConfig = IssoObjectUtils::objectSchemeAsString(m_object);
                qDebug() << objConfig << endl;
            });
    //
    connect(ui->btnClearRemoteLog,  SIGNAL(clicked(bool)),
            this,                   SIGNAL(testClearRemoteLogRequested()));
}


void GraphicPlanTabWidget::testUpdateSelectedModuleMsAddrs()
{
    // очистить список адресов мультидатчиков
    ui->cbTestMsAddress->clear();
    //
    if (!m_object)
        return;
    // выбранный модуль
    IssoModule* module = m_object->selectedModule();
    if (!module)
        return;
    // получить конфиг адресного шлейфа
    auto msChainCfg = m_object->moduleAddressChainConfig(module->moduleConfigName());
    if (!msChainCfg)
        return;

    // заполнить выпадающий список адресов мультидатчиков
    foreach (quint8 id, msChainCfg->multiSensorIdList())
    {
        ui->cbTestMsAddress->addItem(QString::number(id));
    }
}


IssoModuleReply *GraphicPlanTabWidget::testMakeAlarmReply(IssoModule *module)
{
    if (!module || !m_object)
        return nullptr;
    //
    IssoParamBuffer params;
    params.insertParam(new IssoSensorStateParam(BTN_FIRE, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(BTN_SECURITY, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(SENSOR_SMOKE, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(SENSOR_CO, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_1, STATE_WARNING));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_2, STATE_WARNING));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_3, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_4, STATE_SHORT_CIRCUIT));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_5, STATE_BREAK));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_6, STATE_UNDEFINED));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_1, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_2, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_3, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_4, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_5, RELAY_OFF));
    params.insertParam(new IssoSensorExtStateParam(VOLT_BATTERY, VOLTAGE_LOW, 0.5));
    params.insertParam(new IssoSensorExtStateParam(VOLT_BUS, VOLTAGE_LOW, 0.6));
    params.insertParam(new IssoSensorExtStateParam(VOLT_SENSORS, VOLTAGE_HIGH, 10));
    params.insertParam(new IssoSensorExtStateParam(TEMP_OUT, STATE_ALARM, 84));
    params.insertParam(new IssoSensorStateParam(SUPPLY_EXTERNAL, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(SUPPLY_INTERNAL, STATE_ALARM));
    params.insertParam(new IssoSensorStateParam(VIBRATION, STATE_FAIL));
    params.insertParam(new IssoSensorStateParam(SENSOR_OPEN, STATE_ALARM));
    //
    // получить конфиг адресного шлейфа
    auto msChainConfig = m_object->moduleAddressChainConfig(module->moduleConfigName());
    if (msChainConfig)
    {
        //
        // состояния мультидатчиков
        // общее состояние:
        //      0 - норма
        //      1 - внимание
        //      2 - тревога
        //      3 - неисправность
        //      4 - обрыв / КЗ
        QMap<quint8, IssoMultiSensorData> dataMap;
        IssoMultiSensorData multiData;
        //
        // задать состояния каждого мультидатчика на адресном шлейфе
        foreach (quint8 id, msChainConfig->multiSensorIdList())
        {
            switch(id % 5)
            {
                case 0:
                {
                    multiData = IssoMultiSensorData(id, 0, 0x0000);
                    break;
                }
                case 1:
                {
                    multiData = IssoMultiSensorData(id, 1, 0x0005);
                    break;
                }
                case 2:
                {
                    multiData = IssoMultiSensorData(id, 2, 0x002A);
                    break;
                }
                case 3:
                {
                    multiData = IssoMultiSensorData(id, 3, 0x003F);
                    break;
                }

                case 4:
                {
                    multiData = IssoMultiSensorData(id, 4, 0x1555);
                    break;
                }
                default:
                    break;
            }
            dataMap.insert(multiData.multiSensorId(), multiData);
        }
        // добавить параметр в буфер
        params.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, dataMap));
    }
    //
    //
    IssoModuleCommand cmd(/*CMD_ALARM*/CMD_INFO, params);
    IssoModuleReply* reply =
            new IssoModuleReply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
    return reply;
}


IssoModuleReply *GraphicPlanTabWidget::testMakeNoAlarmsReply(IssoModule *module)
{
    if (!module)
        return nullptr;
    //
    IssoParamBuffer params;
    params.insertParam(new IssoSensorStateParam(BTN_FIRE, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(BTN_SECURITY, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(SENSOR_SMOKE, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(SENSOR_CO, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_1, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_2, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_3, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_4, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_5, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(ANALOG_CHAIN_6, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_1, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_2, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_3, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_4, RELAY_OFF));
    params.insertParam(new IssoSensorStateParam(INT_RELAY_5, RELAY_OFF));
    params.insertParam(new IssoSensorExtStateParam(VOLT_BATTERY, VOLTAGE_HIGH, 7.5));
    params.insertParam(new IssoSensorExtStateParam(VOLT_BUS, VOLTAGE_HIGH, 11));
    params.insertParam(new IssoSensorExtStateParam(VOLT_SENSORS, VOLTAGE_HIGH, 6.3));
    params.insertParam(new IssoSensorExtStateParam(TEMP_OUT, STATE_NORMAL, 35));
    params.insertParam(new IssoSensorStateParam(SUPPLY_EXTERNAL, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(SUPPLY_INTERNAL, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(VIBRATION, STATE_NORMAL));
    params.insertParam(new IssoSensorStateParam(SENSOR_OPEN, STATE_NORMAL));
    //
    // получить конфиг адресного шлейфа
    auto msChainConfig = m_object->moduleAddressChainConfig(module->moduleConfigName());
    if (msChainConfig)
    {
        //
        // состояния мультидатчиков
        // общее состояние:
        //      0 - норма
        //      1 - внимание
        //      2 - тревога
        //      3 - неисправность
        //      4 - обрыв / КЗ
        QMap<quint8, IssoMultiSensorData> dataMap;
        IssoMultiSensorData multiData(0, 0, 0);
        //
        // задать состояния каждого мультидатчика на адресном шлейфе
        foreach (quint8 id, msChainConfig->multiSensorIdList())
        {
            multiData.setMultiSensorId(id);
            dataMap.insert(multiData.multiSensorId(), multiData);
        }
        // добавить параметр в буфер
        params.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, dataMap));
    }
    //
    //
    IssoModuleCommand cmd(CMD_INFO, params);
    IssoModuleReply* reply =
            new IssoModuleReply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
    return reply;
}


IssoModuleReply *GraphicPlanTabWidget::testMakeMultiSensorStateReply(IssoModule *module,
                                                                     quint8 multiSensorId,
                                                                     quint8 stateValue,
                                                                     quint16 detailsValue)
{
    if (!module || !m_object)
        return nullptr;
    //
    IssoParamBuffer params;
    QMap<quint8, IssoMultiSensorData> dataMap;
    dataMap.insert(multiSensorId, IssoMultiSensorData(multiSensorId, stateValue, detailsValue));
    // добавить параметр в буфер
    params.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, dataMap));
    //
    IssoModuleCommand cmd(CMD_MS_INFO, params);
    IssoModuleReply* reply =
            new IssoModuleReply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
    return reply;
}


IssoModuleReply *GraphicPlanTabWidget::testMakeMultiSensorReply(IssoModule *module)
{
    if (!module)
        return nullptr;
    //
    IssoParamBuffer params;
    // состояния мультидатчиков
    QMap<quint8, IssoMultiSensorData> dataMap;
    // данные мультидатчика
    IssoMultiSensorData data(1, 2, 0x24E4);
    // значения цифровых датчиков
    QMap<IssoDigitalSensorId, quint16> rawValues;
    rawValues.insert(DIGITALSENSOR_SMOKE_O, 1050);
    rawValues.insert(DIGITALSENSOR_TEMP_A, 2310);
    rawValues.insert(DIGITALSENSOR_TEMP_D, 2530);
    rawValues.insert(DIGITALSENSOR_SMOKE_E, 1220);
    rawValues.insert(DIGITALSENSOR_FLAME_STD_DEV, 420);
    rawValues.insert(DIGITALSENSOR_CO, 1370);
    rawValues.insert(DIGITALSENSOR_VOC, 2190);

//    rawValues.insert(DIGITALSENSOR_SMOKE_OPTICAL, 0x00AA);
//    rawValues.insert(DIGITALSENSOR_TEMP_ANALOG, 0x00BB);
//    rawValues.insert(DIGITALSENSOR_TEMP_DIGITAL, 0x00CC);
//    rawValues.insert(DIGITALSENSOR_SMOKE_ELECTROCHEMICAL, 0x00DD);
//    rawValues.insert(DIGITALSENSOR_FLAME, 0x00EE);
//    rawValues.insert(DIGITALSENSOR_CO, 0x00FF);
//    rawValues.insert(DIGITALSENSOR_FLY_MATTER, 0x0011);

    // добавить параметр сырых данных мультидатчика
    params.insertParam(new IssoMultiSensorRawDataParam(MULTISENSOR_STATE_RAW, data, rawValues));
    //
    IssoModuleCommand cmd(CMD_ALARM, params);
    IssoModuleReply* reply =
            new IssoModuleReply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
    return reply;
}


void GraphicPlanTabWidget::keyPressEvent(QKeyEvent *event)
{
    // показать / скрыть тестовую панель
    if ((event->modifiers() & Qt::ControlModifier) &&
        (event->key() == Qt::Key_F11))
    {
        ui->gbTestButtons->setVisible(!ui->gbTestButtons->isVisible());
    }
//    // показать / скрыть панель индикаторов
//    if ((event->modifiers() & Qt::ControlModifier) &&
//        (event->key() == Qt::Key_F2))
//    {
//        ui->wgtIndicators->setVisible(!ui->wgtIndicators->isVisible());
//    }
    QWidget::keyPressEvent(event);
}


void GraphicPlanTabWidget::resizeEvent(QResizeEvent *event)
{
    processFloorChanged();
    QWidget::resizeEvent(event);
}
