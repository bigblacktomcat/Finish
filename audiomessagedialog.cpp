#include "audiomessagedialog.h"
#include "ui_audiomessagedialog.h"



AudioMessageDialog::AudioMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioMessageDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // инициализировать виджеты
    initWidgets();
    // заполнить списки аудиозаписей
    initRecords();
    // инициализировать захват сэмплов с микрофона
    initMicCapture();
}


AudioMessageDialog::~AudioMessageDialog()
{
    // деинициализировать захват сэмплов с микрофона
    uninitMicCapture();
    delete ui;
}


void AudioMessageDialog::initWidgets()
{
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    //
    // сформировать группу радио кнопок
    m_radioButtons.addButton(ui->rbtnMicrophone, AudioMsgType::VOICE);
    m_radioButtons.addButton(ui->rbtnModuleRecord, AudioMsgType::MODULE_REC);
    m_radioButtons.addButton(ui->rbtnPcRecord, AudioMsgType::PC_REC);
    // задать обработчик выбора
    foreach (QAbstractButton* btn, m_radioButtons.buttons())
    {
        connect(btn,  SIGNAL(toggled(bool)),
                this, SLOT(processMsgTypeChanged()));
    }
    // выбрать 1ую радио кнопку
    ui->rbtnMicrophone->toggle();
    //
    // задать обработчики записи
    connect(ui->btnRecord, SIGNAL(pressed()),
            this,          SLOT(startRecord()));
    connect(ui->btnRecord, SIGNAL(released()),
            this,          SLOT(stopRecord()));
}


void AudioMessageDialog::initRecords()
{
    QStringList fileNames =
        {"Чрезвычайная ситуация.mp3",
         "Учебная пожарная тревога.mp3",
         "Сбор членов пожарного расчета.mp3",
         "Проверка системы оповещения.mp3",
         "Внимание! Пожарная тревога - 1.mp3",
         "Внимание! Пожарная тревога - 2.mp3",
         "Задымление.mp3"};

    QString fileName;
    QString recName;
    QString dir = QDir::currentPath();
    for (quint8 i = 0; i < fileNames.size(); ++i)
    {
        // имя записи
        recName = fileNames[i];
        recName.remove(".mp3");
        // путь к файлу записи
        fileName = dir + "/records/" + fileNames[i];
        // добавить в список записей модуля
        ui->cbModuleRecord->addItem(recName, QVariant::fromValue(i + 1));
        // добавить в список записей ПК
        ui->cbPcRecord->addItem(recName, QVariant::fromValue(fileName));
    }
}


void AudioMessageDialog::initMicCapture()
{
    mp_capture = new MicrophoneCapture(IssoConst::AUDIO_FORMAT_SAMPLE_RATE,
                                       IssoConst::AUDIO_FORMAT_CHANNEL_COUNT,
                                       IssoConst::AUDIO_FORMAT_SAMPLE_SIZE);
    connect(mp_capture, SIGNAL(samplesRead(QByteArray)),
            this,          SLOT(processSamplesRead(QByteArray)));
}


void AudioMessageDialog::uninitMicCapture()
{
    if (mp_capture)
    {
        disconnect(mp_capture, SIGNAL(samplesRead(QByteArray)), 0, 0);
        delete mp_capture;
    }
}


AudioMsgType AudioMessageDialog::audioMsgType() const
{
    return (AudioMsgType)m_radioButtons.checkedId();
}


quint8 AudioMessageDialog::recordNumber() const
{
    return ui->cbModuleRecord->currentData().value<quint8>();
}


QString AudioMessageDialog::recordFileName() const
{
    return ui->cbPcRecord->currentData().value<QString>();
}


QByteArray AudioMessageDialog::voiceRecord() const
{
    return m_voiceRecord;
}


void AudioMessageDialog::processSamplesRead(const QByteArray &samples)
{
    // добавить сэмплы к записи
    m_voiceRecord.append(samples);
    // вывести время записи в диалог
    QString timeStr = QString::number(m_recordTime.elapsed() / 1000.0, 'f', 2);
    ui->lblRecordTime->setText(tr("%1 сек").arg(timeStr));
}


void AudioMessageDialog::processMsgTypeChanged()
{
    // определить выбранный тип аудиозаписи
    AudioMsgType msgType = (AudioMsgType)m_radioButtons.checkedId();
    switch(msgType)
    {
        // запись с микрофона
        case VOICE:
        {
            ui->btnRecord->setEnabled(true);
            ui->cbModuleRecord->setEnabled(false);
            ui->cbPcRecord->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_voiceRecord.isEmpty());
            break;
        }
        // запись в памяти модуля
        case MODULE_REC:
        {
            ui->btnRecord->setEnabled(false);
            ui->cbModuleRecord->setEnabled(true);
            ui->cbPcRecord->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            break;
        }
        // запись в памяти ПК
        case PC_REC:
        {
            ui->btnRecord->setEnabled(false);
            ui->cbModuleRecord->setEnabled(false);
            ui->cbPcRecord->setEnabled(true);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            break;
        }
        default:
            break;
    }
}


void AudioMessageDialog::startRecord()
{
    // изменить состояние кнопки
    ui->btnRecord->setStyleSheet(IssoConst::ACTIVE_BTN_STYLE);
    ui->btnRecord->setText(tr("Остановить"));
    // очистить запись
    m_voiceRecord.clear();
    // начать захват сэмплов с микрофона
    mp_capture->start();
    // зафиксировать время начала записи
    m_recordTime.start();
}


void AudioMessageDialog::stopRecord()
{
    // изменить состояние кнопки
    ui->btnRecord->setStyleSheet(IssoConst::INACTIVE_BTN_STYLE);
    ui->btnRecord->setText(tr("Записать"));
    // остановить захват сэмплов
    mp_capture->stop();
    // разблокировать кнопку ОК, если запись не пуста
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_voiceRecord.isEmpty());
}
