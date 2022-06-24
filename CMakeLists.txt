cmake_minimum_required(VERSION 3.5)

project(Vocal_Sinth VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets SQL LinguistTools Gui Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets SQL LinguistTools Gui Test REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Sql)

# Сборка для OpenAL
# cmake .. -DOPENAL_LIBRARY="C:\Program Files (x86)\OpenAL 1.1 SDK\libs\Win64\OpenAL32.lib"
#-DOPENAL_INCLUDE_DIR="C:\Program Files (x86)\OpenAL 1.1 SDK\include"
#-DCMAKE_INSTALL_PREFIX:STRING="/usr" -DCMAKE_C_FLAGS:STRING="-march=athlon-xp -O2"

find_library(OpenAL_lib OpenAL32.lib        ./libs/OpenAL  )
find_library(ALUT_lib   alut.lib            ./libs/alut    )
# C:/Users/alex1/source/repos/Own_projects/Vocal_Sinth/libs/fftw
find_library(fftw_lib   libfftw3-3.lib libfftw3f-3.lib libfftw3l-3.lib          ./libs/fftw     )

if(NOT ${fftw_lib})
  message("fftw_lib library not found: " ${fftw_lib}) #FATAL_ERROR
endif()

set(TS_FILES Vocal_Sinth_en_US.ts)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(COPY "./VS/assets"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY "./libs/fftw/libfftw3-3.dll" "./libs/fftw/libfftw3f-3.dll" "./libs/fftw/libfftw3l-3.dll"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY "./libs/alut/alut.dll"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})

set(AUDIO
      VS.Core/Audio/audio.cpp
      VS.Core/Audio/audio.h

      VS.Core/Audio/TempoList.h
      VS.Core/Audio/TempoList.cpp
)

set(PLUGINS
    VS.Plugin/SyllableBasedPhonemizer.cpp
    VS.Plugin/SyllableBasedPhonemizer.h

)

set(SRC
      VS.Core/main.cpp

      VS.Core/project.cpp
      VS.Core/project.h

      VS.Core/variables.cpp
      VS.Core/variables.h

      VS.Core/voicebank.cpp
      VS.Core/voicebank.h

      VS.Core/wavtool.cpp
      VS.Core/wavtool.h

      VS.Core/musicMath.cpp
      VS.Core/musicMath.h

      VS.Core/PrefixMap.h
      VS.Core/PrefixMap.cpp
)

set(FORMAT
      VS.Core/Format/wave.h
#      VS.Core/Format/wave.cpp

      VS.Core/Format/ustx.h
      VS.Core/Format/ustx.cpp

      VS.Core/Format/Oto.h
      VS.Core/Format/Oto.cpp

      VS.Core/Format/VSQx.h
      VS.Core/Format/VSQx.cpp

      VS.Core/Format/midi.h
      VS.Core/Format/midi.cpp

      VS.Core/Format/ust.h
      VS.Core/Format/ust.cpp
)

set(UI
      VS.Core/ui/Indicator.cpp
      VS.Core/ui/Indicator.h

#      VS.Core/ui/partsGraphicsScene.cpp
#      VS.Core/ui/partsGraphicsScene.h

      VS.Core/ui/partsGraphicsView.cpp
      VS.Core/ui/partsGraphicsView.h

      VS.Core/ui/partsHeaderGraphicsView.cpp
      VS.Core/ui/partsHeaderGraphicsView.h

      VS.Core/ui/partsHeaderGraphicsView.cpp
      VS.Core/ui/partsHeaderGraphicsView.h

      VS.Core/ui/partHead.cpp
      VS.Core/ui/partHead.h

      VS.Core/ui/partBody.cpp
      VS.Core/ui/partBody.h

      VS.Core/ui/partsTimelineGraphicsView.cpp
      VS.Core/ui/partsTimelineGraphicsView.h

      VS.Core/ui/PRGraphicsScene.cpp
      VS.Core/ui/PRGraphicsScene.h

      VS.Core/ui/PRGraphicsView.cpp
      VS.Core/ui/PRGraphicsView.h

      VS.Core/ui/PRNotes.cpp
      VS.Core/ui/PRNotes.h

      VS.Core/ui/PRNotesGraphicsView.cpp
      VS.Core/ui/PRNotesGraphicsView.h

      VS.Core/ui/PRTimelineGraphicsView.cpp
      VS.Core/ui/PRTimelineGraphicsView.h

      VS.Core/ui/settingsTabBar.cpp
      VS.Core/ui/settingsTabBar.h

      VS.Core/ui/syllableEdit.cpp
      VS.Core/ui/syllableEdit.h

      VS.Core/ui/track.cpp
      VS.Core/ui/track.h

      VS.Core/ui/mainSettings.cpp
      VS.Core/ui/mainSettings.h

      VS.Core/ui/mainwindow.cpp
      VS.Core/ui/mainwindow.h

      VS.Core/ui/projectSettingsWindow.cpp
      VS.Core/ui/projectSettingsWindow.h

      VS.Core/ui/welcomeScreen.cpp
      VS.Core/ui/welcomeScreen.h

      VS.Core/ui/VBSettingsWindow.cpp
      VS.Core/ui/VBSettingsWindow.h

      VS.Core/ui/trackSettings.cpp
      VS.Core/ui/trackSettings.h
)

set(RENDER
      VS.Core/Render/renderEngine.h
      VS.Core/Render/renderEngine.cpp

      VS.Core/Render/renderResult.h
      VS.Core/Render/renderResult.cpp

      VS.Core/Render/renderPhrase.h
      VS.Core/Render/renderPhrase.cpp

      VS.Core/Render/resampler.h
      VS.Core/Render/resampler.cpp
)

set(UIFORM
        VS.Core/ui/mainSettings.ui
        VS.Core/ui/mainwindow.ui
        VS.Core/ui/projectSettingsWindow.ui
        VS.Core/ui/welcomeScreen.ui
        VS.Core/ui/VBSettingsWindow.ui
    )

include_directories(VS/)
include_directories(VS.Tests/)
include_directories(VS.Core/)
include_directories(VS.Core/ui/)
include_directories(VS.Core/Audio/)

add_executable(Vocal_Sinth ${AUDIO} ${FORMAT} ${SRC} ${UI} ${UIFORM} ${RENDER} ${PLUGINS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${fftw_lib})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenAL_lib})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ALUT_lib})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test)
