#include "buildinitwidget.h"

#include <QMetaEnum>



BuildInitWidget::BuildInitWidget(int buildNum, QWidget *parent)
    : QGroupBox(parent),
      m_buildNum(buildNum)
{
    init();
}


bool BuildInitWidget::isOk() const
{
    return m_ok;
}

void BuildInitWidget::init()
{
    // заголовок
    setTitle(displayName());
    // шрифт
    QFont f = font();
    f.setPointSize(9);
    f.setBold(true);
    setFont(f);
    //
    // номер нижнего этажа
    //
    g_cbLowerFloorNum = new QComboBox(this);
    for (int i = 1; i >= -9; --i)
    {
        if (i != 0)
            g_cbLowerFloorNum->addItem(QString::number(i), QVariant(i));
    }
    g_cbLowerFloorNum->setCurrentIndex(0);
    //
    // количество этажей
    //
    g_sbFloorCount = new QSpinBox(this);
    g_sbFloorCount->setRange(1, 99);
    g_sbFloorCount->setSingleStep(1);
    g_sbFloorCount->setValue(1);
    //
    // статус загрузки графпланов
    g_lblImagesLoaded = new QLabel(this);
    //
    // компонент выбора фиксированных датчиков
    g_chkFixedSensors = new QCheckBox(tr("Выбрать фиксированные датчики"), this);
    g_chkFixedSensors->setChecked(true);
    //
    // таблица распределения модулей
    //
    // созать таблицу
    g_tvModules = new QTableView(this);
    // создать модель
    QStandardItemModel* model = new QStandardItemModel(1, 1, this);
    model->setHorizontalHeaderLabels(QStringList { tr("Количество модулей") });
    // установить модель для таблицы
    g_tvModules->setModel(model);
    // настроить отображение таблицы:
    // запретить множественное выделение
    g_tvModules->setSelectionMode(QAbstractItemView::SingleSelection);
    // запретить подсветку заголовков
    g_tvModules->verticalHeader()->setHighlightSections(false);
    g_tvModules->horizontalHeader()->setHighlightSections(false);
    // запретить кликабельность заголовков
    g_tvModules->verticalHeader()->setSectionsClickable(false);
    g_tvModules->horizontalHeader()->setSectionsClickable(false);
    // зафиксировать ширину заголовка столбца
    g_tvModules->verticalHeader()->setFixedWidth(150);
    // зафиксировать высоту заголовка строки
    g_tvModules->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    g_tvModules->verticalHeader()->setDefaultSectionSize(24);
    // растягивать по ширине таблице последнюю строку
    g_tvModules->horizontalHeader()->setStretchLastSection(true);
    // настроить стиль таблицы
    g_tvModules->setStyleSheet("QHeaderView::section {background-color: rgb(218, 218, 218);}"
                               "QSpinBox::up-button { height: 10px; }"
                               "QSpinBox::down-button { height: 10px; }");
    // установить делегата
    g_tvModules->setItemDelegate(new SpinBoxDelegate(0, 999, 1, this));
    //
    // кнопка загрузки графпланов
    //
    g_btnLoad = new QPushButton(tr("Загрузить графпланы"), this);
    //
    // добавить виджеты
    //
    // компоновщик настроек
    QFormLayout* settingsLayout = new QFormLayout();
    settingsLayout->addRow(tr("Номер нижнего этажа"), g_cbLowerFloorNum);
    settingsLayout->addRow(tr("Количество этажей"), g_sbFloorCount);
    settingsLayout->addRow(tr("Загружено графпланов"), g_lblImagesLoaded);
    // главный компоновщик
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(settingsLayout);
    layout->addWidget(g_tvModules);
    layout->addWidget(g_chkFixedSensors);
    layout->addWidget(g_btnLoad);
    setLayout(layout);
    //
    // обработчики событий виджетов
    setWidgetHandlers();
    // размеры
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumWidth(400);
    setMinimumHeight(400);
    //
    // обновить таблицу
    processFloorsChanged();
    // обновить статус загрузки графпланов
    setLoadedImageCount(0);
}


void BuildInitWidget::setWidgetHandlers()
{
    connect(g_sbFloorCount,     SIGNAL(valueChanged(int)),
            this,               SLOT(processFloorsChanged()));
    connect(g_cbLowerFloorNum,  SIGNAL(currentIndexChanged(int)),
            this,               SLOT(processFloorsChanged()));
    connect(g_btnLoad,  SIGNAL(clicked(bool)),
            this,       SLOT(processBtnLoadClicked()));
}


void BuildInitWidget::setLoadedImageCount(int count)
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(g_tvModules->model());
    // обновить статус загрузки
    g_lblImagesLoaded->setText(QString("%1 / %2")
                                        .arg(count)
                                        .arg(model->rowCount()));
    // задать признак успеха
    m_ok = (count == model->rowCount());
}


int BuildInitWidget::buildNumber()
{
    return m_buildNum;
}


QString BuildInitWidget::displayName()
{
    return tr("Здание-%1").arg(m_buildNum, 2, 10, QLatin1Char('0'));
}


int BuildInitWidget::lowerFloorNumber()
{
    return g_cbLowerFloorNum->currentData().toInt();
}


int BuildInitWidget::floorCount()
{
    return g_sbFloorCount->value();
}


QSet<IssoParamId> BuildInitWidget::checkableParams()
{
    QSet<IssoParamId> params;
    // если датчики выбраны
    if (g_chkFixedSensors->isChecked())
    {
        // добавить все фиксированные датчики во множество
        params.insert(BTN_FIRE);
        params.insert(BTN_SECURITY);
        params.insert(SENSOR_SMOKE);
        params.insert(SENSOR_CO);
        params.insert(VOLT_BATTERY);
        params.insert(VOLT_BUS);
        params.insert(VOLT_SENSORS);
        params.insert(SUPPLY_EXTERNAL);
        params.insert(SUPPLY_INTERNAL);
        params.insert(TEMP_OUT);
        params.insert(VIBRATION);
        params.insert(SENSOR_OPEN);
    }
    return params;
}


QMap<int, int> BuildInitWidget::moduleCountMap()
{
    QMap<int, int> map;
    QStandardItemModel* model = static_cast<QStandardItemModel*>(g_tvModules->model());
    // проход по всем строками таблицы
    for (int i = 0; i < model->rowCount(); ++i)
    {
        // номер этажа текущей строки таблицы
        int floorNum = model->headerData(i, Qt::Vertical, Qt::UserRole).toInt();
        // количество модулей
        int moduleCount = model->data(model->index(i, 0)).toInt();
        // добавить в отображение
        map.insert(floorNum, moduleCount);
    }
    return map;
}


QMap<QString, QString> BuildInitWidget::base64ImageMap()
{
    return m_base64Images;
}


//int BuildInitWidget::moduleCountByFloorNumber(int floorNumber)
//{
//    QStandardItemModel* model = static_cast<QStandardItemModel*>(g_tvModules->model());
//    // проход по всем строками таблицы
//    for (int i = 0; i < model->rowCount(); ++i)
//    {
//        // получить номер этажа текущей строки таблицы
//        int num = model->headerData(i, Qt::Vertical, Qt::UserRole).toInt();
//        // если номер совпал, вернуть количество модулей
//        if (num == floorNumber)
//            return model->data(model->index(i, 0)).toInt();
//    }
//    return -1;
//}


//bool BuildInitWidget::imagesLoaded() const
//{
//    foreach (QString strImage, m_base64Images.values())
//    {
//        // если хоть одно изображение не загружено, то false
//        if (strImage.isEmpty())
//            return false;
//    }
//    return true;
//}


//QString BuildInitWidget::base64ImageByFloorName(const QString &floorName)
//{
//    return m_base64Images.value(floorName);
//}


void BuildInitWidget::processFloorsChanged()
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(g_tvModules->model());
    // задать количество этажей
    model->setRowCount(floorCount());
    // очистить карту графпланов
    m_base64Images.clear();
    //
    // сформировать список вертикальных заголовков
    QStringList floorNames;
    int count = 0;
    for (int num = lowerFloorNumber(); count < floorCount(); ++num)
    {
        if (num != 0)
        {
            // имя этажа
            QString name = (num < 0 ? tr("Этаж-П%1").arg(abs(num)) :
                                      tr("Этаж-%1").arg(num, 2, 10, QLatin1Char('0')));
            // добавить имя в список
            floorNames << name;
            // задать номер этажа для заголовка текущей строки
            model->setHeaderData(count, Qt::Vertical, QVariant(num), Qt::UserRole);
            // если колчество модулей не задано, задать значение по умолчанию
            if (model->data(model->index(count, 0)).isNull())
                model->setData(model->index(count, 0), QVariant(1), Qt::EditRole);
            //
            ++count;
        }
    }
    // обновить номера этажей
    model->setVerticalHeaderLabels(floorNames);
    // обновить статус загрузки графпланов
    setLoadedImageCount(0);
}


void BuildInitWidget::processBtnLoadClicked()
{
    // вывести диалог выбора директории
    QString dirName =
            QFileDialog::getExistingDirectory(this, tr("Загрузить графпланы в формате PNG"),
                                              QDir::currentPath(),
                                              QFileDialog::ShowDirsOnly |
                                              QFileDialog::DontResolveSymlinks);
    int loadCount = 0;
    // pзагрузить изображение для каждого этажа из таблицы
    QStandardItemModel* model = static_cast<QStandardItemModel*>(g_tvModules->model());
    // проход по всем строками таблицы
    for (int i = 0; i < model->rowCount(); ++i)
    {
        // получить имя этажа
        QString floorName = model->verticalHeaderItem(i)->data(Qt::DisplayRole).toString();

//        // сформировать имя файла (выбранная_директория/имя_этажа.jpg)
//        QString fileName = QString("%1/%2.jpg").arg(dirName).arg(floorName);

        // сформировать имя файла (выбранная_директория/имя_этажа.png)
        QString fileName = QString("%1/%2.png").arg(dirName).arg(floorName);

        // изображение, преобразованное в строку
        QString base64 = QString();
        // проверить наличие файла
        if (QFile(fileName).exists())
        {
            // загрузить изображение
            QImage img = QImage(fileName);
            if (img.isNull())
                continue;
            // преобразовать в Base64-строку
            base64 = IssoCommonData::convertImageToBase64(img);
            // добавить в карту пару (имя_этажа : строка_изображения)
            m_base64Images.insert(floorName, base64);
            // инкрементировать число загруженных планов
            ++loadCount;
        }
    }
    // обновить статус загрузки графпланов
    setLoadedImageCount(loadCount);
}


