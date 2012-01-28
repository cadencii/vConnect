#-------------------------------------------------
#
# Project created by QtCreator 2012-01-12T20:36:43
#
#-------------------------------------------------

QT       += core gui

TARGET = StandConverter
TEMPLATE = app

LIBS += \
    -lfftw3 \
    -logg_static \
    -lvorbis_static \
    -lvorbisfile_static

SOURCES += \
    main.cpp \
    gui/ConverterWindow.cpp \
    io/UtauLibrary.cpp \
    synthesis/ConverterSetting.cpp \
    utility/Utility.cpp \
    synthesis/Converter.cpp \
    io/WaveFile.cpp \
    math/FFTSet.cpp \
    math/Dio.cpp \
    math/World.cpp \
    math/Star.cpp \
    math/Platinum.cpp \
    math/MatlabFunctions.cpp \
    math/MinimumPhaseSpectrum.cpp \
    math/MFCCSet.cpp \
    gui/WorldWidget.cpp \
    io/VorbisFile.cpp \
    io/StandFile.cpp \
    math/WorldSynthesis.cpp \
    gui/TranscriberWindow.cpp \
    gui/HorizontalPianoView.cpp \
    gui/MappingView.cpp \
    math/SmootMatching.cpp \
    math/MathSet.cpp \
    synthesis/Transcriber.cpp \
    synthesis/TranscriberSetting.cpp \
    synthesis/ConverterElement.cpp \
    gui/TranscriberWidget.cpp \
    synthesis/TranscriberElement.cpp \
    io/StandLibrary.cpp

HEADERS  += gui/ConverterWindow.h \
    io/UtauLibrary.h \
    synthesis/ConverterSetting.h \
    utility/Utility.h \
    synthesis/Converter.h \
    io/WaveFile.h \
    math/FFTSet.h \
    math/Dio.h \
    math/World.h \
    math/Star.h \
    math/Platinum.h \
    math/MatlabFunctions.h \
    math/MinimumPhaseSpectrum.h \
    math/MFCCSet.h \
    gui/WorldWidget.h \
    io/VorbisFile.h \
    io/StandFile.h \
    math/WorldSynthesis.h \
    gui/TranscriberWindow.h \
    gui/HorizontalPianoView.h \
    gui/MappingView.h \
    math/SmootMatching.h \
    math/MathSet.h \
    synthesis/Transcriber.h \
    synthesis/TranscriberSetting.h \
    synthesis/ConverterElement.h \
    gui/TranscriberWidget.h \
    synthesis/TranscriberElement.h \
    io/StandLibrary.h

FORMS    += \
    gui/ConverterWindow.ui \
    gui/TranscriberWindow.ui \
    gui/TranscriberWidget.ui
