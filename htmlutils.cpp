#include "htmlutils.h"


const QMap<IssoState, QString> HtmlUtils::m_htmlClassNames =
{
    {STATE_NORMAL,  "normal"},
    {STATE_WARNING, "warning"},
    {STATE_ALARM,   "alarm"},
    {STATE_FAIL,    "fail"}
};


QString HtmlUtils::makeAnalogChainHint(int moduleId, int chainId, IssoState chainState)
{
    return QString(
        "<html>"
            "<head>"
                "<style type=\"text/css\">"
                    "p { "
                        "font-size: 10pt; "
                        "font-weight: bold; "
                        "line-height: 1.2; "
                    "}"
                "</style>"
            "</head>"
            "<body>"
                "<p> Модуль-%1 <br>"
                "    Аналоговый шлейф-%2 <br>"
                "    Состояние: %3 "
                "</p>"
            "</body>"
        "</html>"
        ).arg(moduleId)
         .arg(chainId)
         .arg(IssoCommonData::stringBySensorState(chainState));
}


QString HtmlUtils::makeMultiSensorHint(/*int moduleId,*/ /*int chainId,*/
                                       const QString& moduleName,
                                       quint8 multiSensorId, IssoState multiSensorState,
                                       const QString& location,
                                       const QMap<IssoDigitalSensorId, IssoState> &digitalStates)
{
    // создать содержимое таблицы: датчик - состояние
    QString rows = "";
//    foreach (IssoDigitalSensorId sensorId, digitalStates.keys())
    foreach (IssoDigitalSensorId sensorId, IssoCommonData::displayingDigitalSensorIds())
    {
        // получить состояние цифрового датчика
        IssoState sensorState = digitalStates.value(sensorId, STATE_UNDEFINED);
        // определить html-класс, соответствующий состоянию
        QString htmlClassName = m_htmlClassNames.value(sensorState);
        // добавить строку таблицы
        rows += QString("<tr>"
                            "<td> %1 </td>"
                            "<td class=\"%3\"> %2 </td>"
                        "</tr>")
                        .arg(IssoCommonData::stringByDigitalSensorId(sensorId))
                        .arg(IssoCommonData::stringBySensorState(sensorState))
                        .arg(htmlClassName);
    }
    // сформировать подсказку
    QString hint = QString(
                    "<html>"
                        "<head>"
                            "<style type=\"text/css\">"
                                "p { "
                                    "font-size: 10pt; "
                                    "font-weight: bold; "
                                    "line-height: 1.2; "
                                "}"
                                "table { "
                                    "border: 1px solid black; "
                                    "border-collapse: collapse; "
                                "}"
                                "th { "
                                    "border-collapse: collapse; "
                                    "border: 1px solid black; "
                                    "padding: 0.2em; "
                                    "background: silver; "
                                    "color: black; "
                                "}"
                                "td { "
                                    "border-collapse: collapse; "
                                    "border: 1px solid black; "
                                    "padding: 0.2em; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_NORMAL) + " {"
                                    "background: green; "
                                    "color: white; "
                                    "font-weight: bold; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_WARNING) + " {"
                                    "background: red; "
                                    "color: black; "
                                    "font-weight: bold; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_ALARM) + " {"
                                    "background: red; "
                                    "color: white; "
                                    "font-weight: bold; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_FAIL) + " {"
                                    "background: yellow; "
                                    "color: black; "
                                    "font-weight: bold; "
                                "}"
                            "</style>"
                        "</head>"
                        "<body>"
                            "<p> %1 <br>"
                            "    Модуль %2 <br>"
                            "    %3 <br>"
                            "    Состояние: %4 "
                            "</p>"
                            "<p>"
                                "<table>"
                                    "<tr>"
                                        "<th> Датчик </th>"
                                        "<th> Состояние </th>"
                                    "</tr>"
                                    "%5"
                                "</table>"
                            "</p>"
                        "</body>"
                    "</html>"
                )
                .arg(location)
                .arg(moduleName)
//                .arg(moduleId)
                .arg(QString("%1-%2")
                        .arg(IssoCommonData::stringByParamId(MULTISENSOR))
                        .arg(multiSensorId))
                .arg(IssoCommonData::stringBySensorState(multiSensorState))
                .arg(rows);
    return hint;
}






QString HtmlUtils::makeMultiSensorExtHint(/*int moduleId,*/ /*int chainId,*/
                                          const QString& moduleName,
                                          quint8 multiSensorId, IssoState multiSensorState,
                                          const QString& location,
                                          const QMap<IssoDigitalSensorId, IssoState> &digitalStates,
                                          const QMap<IssoDigitalSensorId, quint16> &digitalValues)
{
    // создать содержимое таблицы: датчик - состояние
    QString rows = "";
//    foreach (IssoDigitalSensorId sensorId, digitalStates.keys())
    foreach (IssoDigitalSensorId sensorId, IssoCommonData::displayingDigitalSensorIds())
    {
        // получить состояние цифрового датчика
        IssoState sensorState = digitalStates.value(sensorId, STATE_UNDEFINED);
        // получить значение цифрового датчика
        quint16 sensorValue = digitalValues.value(sensorId, 0);

//        // получить значение цифрового датчика
//        float sensorValue = digitalValues.value(sensorId, 0.0f);
//        // конвертировать значение датчика в строку
//        QString sensorValueStr;
//        sensorValueStr.setNum(sensorValue, 'f', 1);

        // определить html-класс, соответствующий состоянию
        QString htmlClassName = m_htmlClassNames.value(sensorState);
        // добавить строку таблицы
        rows += QString("<tr>"
                            "<td> %1 </td>"
                            "<td class=\"%3\"> %2 </td>"
                            "<td> %4 </td>"
                        "</tr>")
                        .arg(IssoCommonData::stringByDigitalSensorId(sensorId))
                        .arg(IssoCommonData::stringBySensorState(sensorState))
                        .arg(htmlClassName)
                        .arg(sensorValue);
    }
    // сформировать подсказку
    QString hint = QString(
                    "<html>"
                        "<head>"
                            "<style type=\"text/css\">"
                                "p { "
                                    "font-size: 10pt; "
                                    "font-weight: bold; "
                                    "line-height: 1.2; "
                                "}"
                                "table { "
                                    "border: 1px solid black; "
                                    "border-collapse: collapse; "
                                "}"
                                "th { "
                                    "border-collapse: collapse; "
                                    "border: 1px solid black; "
                                    "padding: 0.2em; "
                                    "background: silver; "
                                    "color: black; "
                                "}"
                                "td { "
                                    "border-collapse: collapse; "
                                    "border: 1px solid black; "
                                    "padding: 0.2em; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_NORMAL) + " {"
                                    "background: green; "
                                    "color: white; "
                                    "font-weight: bold; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_WARNING) + " {"
                                    "background: red; "
                                    "color: black; "
                                    "font-weight: bold; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_ALARM) + " {"
                                    "background: red; "
                                    "color: white; "
                                    "font-weight: bold; "
                                "}"
                                "td." + m_htmlClassNames.value(STATE_BREAK) + " {"
                                    "background: yellow; "
                                    "color: black; "
                                    "font-weight: bold; "
                                "}"
                            "</style>"
                        "</head>"
                        "<body>"
                            "<p> %1 <br>"
                            "    Модуль %2 <br>"
                            "    %3 <br>"
                            "    Состояние: %4 "
                            "</p>"
                            "<p>"
                                "<table>"
                                    "<tr>"
                                        "<th> Датчик </th>"
                                        "<th> Состояние </th>"
                                        "<th> Значение </th>"
                                    "</tr>"
                                    "%5"
                                "</table>"
                            "</p>"
                        "</body>"
                    "</html>"
                )
                .arg(location)
                .arg(moduleName)
//                .arg(moduleId)
//                 .arg(chainId)
                .arg(QString("%1-%2")
                            .arg(IssoCommonData::stringByParamId(MULTISENSOR))
                            .arg(multiSensorId))
                .arg(IssoCommonData::stringBySensorState(multiSensorState))
.arg(rows);
    return hint;
}
