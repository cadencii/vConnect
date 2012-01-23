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
    synthesis/UtauPhonemeAnalyzer.cpp \
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
    gui/TranscribeWidget.cpp \
    io/VorbisFile.cpp \
    io/StandFile.cpp \
    math/WorldSynthesis.cpp \
    gui/TranscriberWindow.cpp \
    gui/HorizontalPianoView.cpp \
    gui/MappingView.cpp

HEADERS  += gui/ConverterWindow.h \
    io/UtauLibrary.h \
    synthesis/ConverterSetting.h \
    utility/Utility.h \
    synthesis/Converter.h \
    synthesis/UtauPhonemeAnalyzer.h \
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
    gui/TranscribeWidget.h \
    io/VorbisFile.h \
    io/StandFile.h \
    math/WorldSynthesis.h \
    gui/TranscriberWindow.h \
    gui/HorizontalPianoView.h \
    gui/MappingView.h

FORMS    += \
    gui/ConverterWindow.ui \
    gui/TranscribeWidget.ui \
    gui/TranscriberWindow.ui
