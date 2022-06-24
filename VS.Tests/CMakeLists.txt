cmake_minimum_required(VERSION 3.5)

project(test_mainWindow LANGUAGES CXX)

find_package(QT NAMES Qt5 Qt6 COMPONENTS Gui Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Test REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets SQL LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets SQL LinguistTools REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Sql)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

find_library(OpenAL_lib OpenAL32.lib
    ../libs/OpenAL
    )

find_library(ALUT_lib alut.lib
    ../libs/alut
    ../include/freealut/build/src/Release
    )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


file(COPY "../assets"
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY ${ALUT_lib}
    DESTINATION ${EXECUTABLE_OUTPUT_PATH})

link_directories("../include/freealut/build/src/Release")
link_directories("include")

add_executable(test_mainWindow test_mainWindow.cpp)
add_test(NAME test_mainWindow COMMAND test_mainWindow)

target_link_libraries(test_mainWindow PRIVATE Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(test_mainWindow PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(test_mainWindow PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_directories(test_mainWindow PRIVATE "../assets")
target_link_libraries(test_mainWindow PRIVATE ${OpenAL_lib})
target_link_libraries(test_mainWindow PRIVATE ${ALUT_lib})

