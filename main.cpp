#include "mainwindow.h"
#include "objectsviewwindow.h"
#include "issoaccountmanager.h"
#include "issoobject.h"
#include "issoconfigutils.h"
#include <QApplication>
#include <QMessageBox>


QMainWindow* makeApplicationWindow(IssoAppType appType, IssoDataBase* db,
                                   IssoAccount account, IssoGuiMode guiMode,
                                   int objectId
                                   /*bool savedConfig*/)
{
    QMainWindow* mainWindow = nullptr;
    //
    switch(appType)
    {
        case IssoAppType::SNK:
        {
            //
            //============= Создать новую конфигурацию (если необходимо) =============
            //
//            int objectId;
            // если пользователь выбрал создание новой конфигурации
//            if (!savedConfig)
            if (objectId == -1)
            {
                // вывести диалог создания охраняемого объекта
                objectId = IssoConfigUtils::createConfig(db);
                if (objectId == -1)
                {
                    // если объект не создан, вывести сообщение и завершить работу
                    QMessageBox::warning(nullptr, "Внимание",
                                         "Охраняемый объект не создан! Программа будет завершена");
                    return nullptr;
                }
            }
//            else
//            {
//                objectId = IssoConfigUtils::getFirstObjectId(db);
//                // если объект не найден в БД
//                if (objectId == -1)
//                {
//                    QMessageBox::warning(nullptr, "Внимание",
//                                         "Охраняемый объект не найден в БД! Программа будет завершена");
//                    return nullptr;
//                }
//            }
            //
            //================ Загрузить конфигурацию из БД ================
            //
            // загрузить охраняемый объект из БД
            IssoObject* obj = IssoConfigUtils::loadObject(db, objectId);
            //
            //================ Отобразить главное окно программы ================
            //
            // настроить окно приложения
            mainWindow = new MainWindow(appType, account, guiMode, obj, db);
            break;
        }

        case IssoAppType::SOO:
        case IssoAppType::GSC:
        {
            mainWindow = new ObjectsViewWindow(appType, account, guiMode, db);
            break;
        }

        default:
            break;
    }
    return mainWindow;
}


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(IssoConst::ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(IssoConst::APPLICATION_NAME);

    QApplication a(argc, argv);

//    //
//    // проанализировать ключи:
//    if (argc < 2)
//        exit(0);
//    // тип приложения
//    IssoAppType appType;
//    QString typeArg = argv[1];
//    if (typeArg == "-snk")
//        appType = SNK;
//    else if (typeArg == "-soo")
//        appType = SOO;
//    else if (typeArg == "-gsc")
//        appType = GSC;
//    else
//        exit(0);


//#if defined GSC_APP
//    qDebug() << "Main: Application type - GSC";
//    IssoAppType appType = IssoAppType::GSC;
//#elif defined SOO_APP
//    qDebug() << "Main: Application type - SOO";
//    IssoAppType appType = IssoAppType::SOO;
//#else
//    qDebug() << "Main: Application type - SNK";
//    IssoAppType appType = IssoAppType::SNK;
//#endif

#if defined GSC_APP
    qDebug() << "Main: Application type - GSC";
    IssoAppType appType = IssoAppType::GSC;
#elif defined SOO_APP
    qDebug() << "Main: Application type - SOO";
    IssoAppType appType = IssoAppType::SOO;
#else
    qDebug() << "Main: Application type - SNK";
    IssoAppType appType = IssoAppType::SNK;
#endif


    //
    int retVal = -1;
    //
    //================ Аутентифицировать пользователя ================
    //
    // создать объект взаимодействия с БД
    IssoDataBase* db = new IssoDataBase("data.db");
    //
    // создать менеджера учетных записей
    IssoAccountManager mgr(db);
    //
//    // загрузить имена пользователей
//    mgr.load();
    //
    // режим отображения
    IssoGuiMode guiMode;
    // аккаунт
    IssoAccount account;

//    // признак загрузки сохраненной конфигурации
//    bool savedConfig = false;

    // id выбранного объекта
    int selectedObjId = -1;

//    //
//    // создать объект взаимодействия с БД
//    IssoDataBase* db = new IssoDataBase("data.db");
//    //
    // проверить необходимость автозагрузки приложения
    if (IssoConfigUtils::needAutorun())
    {
        account = mgr.findAccount("user");
        guiMode = IssoGuiMode::STANDART_MONITOR;
//        savedConfig = true;

        // ВРЕМЕННО!!!
        selectedObjId = IssoConfigUtils::getFirstObjectId(db);
    }
    else
    {
//        // получить список конфигураций объектов
//        QStringList cfgNames = IssoConfigUtils::getObjectNames(db);
//        // имя выбранного конфига
//        QString selectedCfgName = "";

        //
        // аутентифицировать пользователя
        if (!mgr.authenticate(appType, account, guiMode, /*cfgNames,*/ selectedObjId))
        {
            delete db;
            exit(0);
        }
//        // если имя конфига не пусто, загрузить сохраненную конфигурацию
//        savedConfig = !selectedObjId.isEmpty();
    }
    //
    //================== Создать окно приложения ==================
    //
    QMainWindow* appWindow = makeApplicationWindow(appType, db, account,
                                                   guiMode, selectedObjId/*savedConfig*/);
    // если окно создано
    if (appWindow)
    {
        // показать главное окно программы
        appWindow->showMaximized();
        //
        // если приложение - СНК и режим оператора, создать файл автозапуска
        if ((appType == IssoAppType::SNK) && (!account.isSuperUser()))
            IssoConfigUtils::createAutorunFile();
        //
        // запустить обработку событий приложения
        retVal = a.exec();
        //
        // сохранить конфигурацию после завершения работы СНК
        if (appType == IssoAppType::SNK)
        {
            // получить главное окно СНК
            MainWindow* snkWindow = static_cast<MainWindow*>(appWindow);
            // обновить конфигурацию
            IssoConfigUtils::updateConfig(db, snkWindow->guardedObject());
            // удалить файл автозапуска
            IssoConfigUtils::deleteAutorunFile();
        }
        //
        // уничтожить окно
        delete appWindow;
    }
    // удалить объект взаимодействия с БД
    delete db;
    // вернуть значение
    return retVal;
}

