#-------------------------------------------------
#
# Project created by QtCreator 2018-09-27T10:53:46
#
#-------------------------------------------------

QT       += core gui network multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts


#----- OpenCV -----
INCLUDEPATH += $$PWD/opencv-343/include

LIBS += -L$$PWD/opencv-343/lib/ -llibopencv_core343
LIBS += -L$$PWD/opencv-343/lib/ -llibopencv_highgui343
LIBS += -L$$PWD/opencv-343/lib/ -llibopencv_imgcodecs343
LIBS += -L$$PWD/opencv-343/lib/ -llibopencv_imgproc343
LIBS += -L$$PWD/opencv-343/lib/ -llibopencv_calib3d343
LIBS += -L$$PWD/opencv-343/lib/ -llibopencv_videoio343

#INCLUDEPATH += C:\openCV_343_build_x86\install\include\

#LIBS += C:\openCV_343_build_x86\bin\libopencv_core343.dll
#LIBS += C:\openCV_343_build_x86\bin\libopencv_highgui343.dll
#LIBS += C:\openCV_343_build_x86\bin\libopencv_imgcodecs343.dll
#LIBS += C:\openCV_343_build_x86\bin\libopencv_imgproc343.dll
#LIBS += C:\openCV_343_build_x86\bin\libopencv_calib3d343.dll
#LIBS += C:\openCV_343_build_x86\bin\libopencv_videoio343.dll


#----- SPEEX -----
INCLUDEPATH += $$PWD/speex-1.2.0/include
#DEPENDPATH += $$PWD/speex-1.2.0/include

#win32: LIBS += -L$$PWD/speex-1.2.0/libspeex/ -llibspeex.dll
win32: LIBS += -L$$PWD/speex-1.2.0/libspeex/ -llibspeex-1


#----- MPG123 -----
INCLUDEPATH += $$PWD/mpg123-1.25.10-x86/include
#DEPENDPATH += $$PWD/mpg123-1.25.10-x86/include

#win32: LIBS += -L$$PWD/mpg123-1.25.10-x86/lib/ -llibmpg123.dll
win32: LIBS += -L$$PWD/mpg123-1.25.10-x86/lib/ -llibmpg123-0


#TARGET = GraphicsSceneTest
TEMPLATE = app

CONFIG += c++11

APP_TYPE = SNK
#APP_TYPE = GSC
#APP_TYPE = SOO
TARGET = $${APP_TYPE}

equals(APP_TYPE, GSC) {
    message("Project: ГСЦ")
    DEFINES += GSC_APP
} else:equals(APP_TYPE, SOO) {
    message("Project: СОО")
    DEFINES += SOO_APP
} else {
    message("Project: СНК")
    DEFINES += SNK_APP
#    DEFINES += QT_NO_DEBUG_OUTPUT
}

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}


SOURCES += main.cpp\
    mainwindow.cpp \
    issomodule.cpp \
    issobuilding.cpp \
    issofloorplan.cpp \
    issomodulereply.cpp \
    issomodulerequest.cpp \
    issocommondata.cpp \
    issomodulecommand.cpp \
    issomodulepacket.cpp \
    issoalarmreceiver.cpp \
    issorequestsender.cpp \
    issoexchangemanager.cpp \
    issomodulescontroller.cpp \
    issoeventlistitemdelegate.cpp \
    newbuildingdialog.cpp \
    newfloordialog.cpp \
    newmoduledialog.cpp \
    descriptorsetupdialog.cpp \
    selectmoduledialog.cpp \
    issosensortype.cpp \
    newsensortypedialog.cpp \
    issomoduledescriptor.cpp \
    issoremotelogger.cpp \
    issocore.cpp \
    issolocallogger.cpp \
    issoaccountmanager.cpp \
    issologindialog.cpp \
    issofloorview.cpp \
    issofiltercomboitemdelegate.cpp \
    passworddialog.cpp \
    ipcameracapture.cpp \
    cameraviewframe.cpp \
    audiomessagedialog.cpp \
    issoaudiocontroller.cpp \
    speexencoder.cpp \
    mp3filereader.cpp \
    microphonecapture.cpp \
    issodatabase.cpp \
    issoeventdelegate.cpp \
    buildinitwidget.cpp \
    spinboxdelegate.cpp \
    addguardedobjectdialog.cpp \
    issoobjectcard.cpp \
    issoobject.cpp \
    issoconfigutils.cpp \
    objectsviewwindow.cpp \
    issoparam.cpp \
    issoparambuffer.cpp \
    issologmessagebuilder.cpp \
    issocamera.cpp \
    newcameradialog.cpp \
    locationgroupbox.cpp \
    issostatelabel.cpp \
    camerastabwidget.cpp \
    sensorstabwidget.cpp \
    modulestabwidget.cpp \
    graphicplantabwidget.cpp \
    issomodulestatedelegate.cpp \
    issofloorstatedelegate.cpp \
    issosensor.cpp \
    issosensorchain.cpp \
    issoeditlistpanel.cpp \
    moduleconfigtabwidget.cpp \
    chainconfigtabwidget.cpp \
    issoanalogsensorsetuppanel.cpp \
    issodigitalsensorsetuppanel.cpp \
    issochainconfig.cpp \
    issomultisensorconfig.cpp \
    issodigitalsensorconfig.cpp \
    issomoduleconfig.cpp \
    newchainconfigdialog.cpp \
    htmlutils.cpp \
    issoremotelogpoller.cpp \
    issoobjectitemdelegate.cpp \
    issosoundplayer.cpp \
    scenariostabwidget.cpp \
    newrelayclassdialog.cpp \
    issoscenario.cpp \
    newscenariodialog.cpp \
    issocheckableselectionpanel.cpp \
    issoscenarioqueue.cpp \
    issoscenarioexecutor.cpp \
    issoscenariorequest.cpp \
    issocamerabindpanel.cpp \
    issoeventquerymodel.cpp \
    issoobjectquerymodel.cpp \
    commentdialog.cpp \
    testingwidget.cpp \
    issomsutils.cpp \
    issoobjectutils.cpp \
    issovideoframeprocessor.cpp \
    objectconfigsetupdialog.cpp \
    sensorvaluescharttabwidget.cpp

HEADERS  += mainwindow.h \
    issomodule.h \
    issobuilding.h \
    issofloorplan.h \
    issomodulereply.h \
    issomodulerequest.h \
    issocommondata.h \
    issomodulecommand.h \
    issomodulepacket.h \
    issoalarmreceiver.h \
    issorequestsender.h \
    issoexchangemanager.h \
    issomodulescontroller.h \
    issoeventlistitemdelegate.h \
    newbuildingdialog.h \
    newfloordialog.h \
    newmoduledialog.h \
    descriptorsetupdialog.h \
    selectmoduledialog.h \
    issosensortype.h \
    newsensortypedialog.h \
    issomoduledescriptor.h \
    issoremotelogger.h \
    issocore.h \
    issolocallogger.h \
    issoaccountmanager.h \
    issologindialog.h \
    issofloorview.h \
    issofiltercomboitemdelegate.h \
    passworddialog.h \
    ipcameracapture.h \
    cameraviewframe.h \
    audiomessagedialog.h \
    issoaudiocontroller.h \
    speexencoder.h \
    mp3filereader.h \
    microphonecapture.h \
    issodatabase.h \
    issoeventdelegate.h \
    buildinitwidget.h \
    spinboxdelegate.h \
    addguardedobjectdialog.h \
    issoobjectcard.h \
    issoobject.h \
    issoconfigutils.h \
    objectsviewwindow.h \
    issoparam.h \
    issoparambuffer.h \
    issologmessagebuilder.h \
    issocamera.h \
    newcameradialog.h \
    locationgroupbox.h \
    issostatelabel.h \
    camerastabwidget.h \
    sensorstabwidget.h \
    modulestabwidget.h \
    graphicplantabwidget.h \
    issomodulestatedelegate.h \
    issofloorstatedelegate.h \
    issosensor.h \
    issosensorchain.h \
    issoeditlistpanel.h \
    moduleconfigtabwidget.h \
    chainconfigtabwidget.h \
    issoanalogsensorsetuppanel.h \
    issodigitalsensorsetuppanel.h \
    issochainconfig.h \
    issomultisensorconfig.h \
    issodigitalsensorconfig.h \
    issomoduleconfig.h \
    newchainconfigdialog.h \
    htmlutils.h \
    issoremotelogpoller.h \
    issoobjectitemdelegate.h \
    issosoundplayer.h \
    scenariostabwidget.h \
    newrelayclassdialog.h \
    issoscenario.h \
    newscenariodialog.h \
    issocheckableselectionpanel.h \
    issoscenarioqueue.h \
    issoscenarioexecutor.h \
    issoscenariorequest.h \
    issocamerabindpanel.h \
    issoeventquerymodel.h \
    issoobjectquerymodel.h \
    issoregisters.h \
    commentdialog.h \
    testingwidget.h \
    issomsutils.h \
    issoobjectutils.h \
    issovideoframeprocessor.h \
    objectconfigsetupdialog.h \
    sensorvaluescharttabwidget.h

FORMS += \
    form.ui \
    newbuildingdialog.ui \
    newfloordialog.ui \
    newmoduledialog.ui \
    descriptorsetupdialog.ui \
    selectmoduledialog.ui \
    newsensortypedialog.ui \
    issologindialog.ui \
    passworddialog.ui \
    cameraviewframe.ui \
    audiomessagedialog.ui \
    addguardedobjectdialog.ui \
    objectsviewwindow.ui \
    newcameradialog.ui \
    locationgroupbox.ui \
    camerastabwidget.ui \
    sensorstabwidget.ui \
    modulestabwidget.ui \
    graphicplantabwidget.ui \
    issoeditlistpanel.ui \
    moduleconfigtabwidget.ui \
    chainconfigtabwidget.ui \
    issoanalogsensorsetuppanel.ui \
    issodigitalsensorsetuppanel.ui \
    newchainconfigdialog.ui \
    scenariostabwidget.ui \
    newrelayclassdialog.ui \
    newscenariodialog.ui \
    issocheckableselectionpanel.ui \
    issocamerabindpanel.ui \
    commentdialog.ui \
    testingwidget.ui \
    objectconfigsetupdialog.ui \
    sensorvaluescharttabwidget.ui

RESOURCES += \
    resource.qrc
