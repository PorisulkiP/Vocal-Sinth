set(UIFORM
      ui/mainSettings.ui
      ui/mainwindow.ui
      ui/projectSettingsWindow.ui
      ui/welcomeScreen.ui
)

set(AUDIO
      Audio/audio.cpp
      Audio/audio.h
)

set(SRC
      main.cpp

      project.cpp
      project.h

      variables.cpp
      variables.h
)

set(FORMAT
      Format/
)

set(UI
      ui/Indicator.cpp
      ui/Indicator.h

      ui/partsGraphicsScene.cpp
      ui/partsGraphicsScene.h

      ui/partsGraphicsView.cpp
      ui/partsGraphicsView.h

      ui/partsHeaderGraphicsView.cpp
      ui/partsHeaderGraphicsView.h

      ui/partsHeaderGraphicsView.cpp
      ui/partsHeaderGraphicsView.h

      ui/partsItem.cpp
      ui/partsItem.h

      ui/partsItemWidget.cpp
      ui/partsItemWidget.h

      ui/partsTimelineGraphicsView.cpp
      ui/partsTimelineGraphicsView.h

      ui/PRGraphicsScene.cpp
      ui/PRGraphicsScene.h

      ui/PRGraphicsView.cpp
      ui/PRGraphicsView.h

      ui/PRNotes.cpp
      ui/PRNotes.h

      ui/PRNotesGraphicsView.cpp
      ui/PRNotesGraphicsView.h

      ui/PRTimelineGraphicsView.cpp
      ui/PRTimelineGraphicsView.h

      ui/settingsTabBar.cpp
      ui/settingsTabBar.h

      ui/syllableEdit.cpp
      ui/syllableEdit.h

      ui/track.cpp
      ui/track.h
)

set(WINDOWS
      Windows/mainSettings.cpp
      Windows/mainSettings.h

      Windows/mainwindow.cpp
      Windows/mainwindow.h

      Windows/projectSettingsWindow.cpp
      Windows/projectSettingsWindow.h

      Windows/welcomeScreen.cpp
      Windows/welcomeScreen.h
)

#add_executable(Vocal_Sinth_Core ${AUDIO} ${FORMAT} ${SRC}
#                                ${UI} ${UIFORM} ${WINDOWS}) #
#target_link_directories(Vocal_Sinth_Core PRIVATE "/VS/assets")
#target_link_libraries(Vocal_Sinth_Core PRIVATE ${OpenAL_lib})
#target_link_libraries(Vocal_Sinth_Core PRIVATE ${ALUT_lib})
#target_link_libraries(Vocal_Sinth_Core PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
#target_link_libraries(Vocal_Sinth_Core PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
#target_link_libraries(Vocal_Sinth_Core PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test)


