cmake_minimum_required(VERSION 3.5)

project(power_carrier VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../Release")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort Sql)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FILES_PATH ".")
endif()

set(GLOBAL_DEFINED
    SHARE_PATH="${FILES_PATH}/share"
    CONFIG_INI="${FILES_PATH}/share/config.ini"
    DB_PATH="${FILES_PATH}/db"
)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)
set(PROJECT_NAMESPACE_DEFINED
    PROJECT_NAMESPACE_NAME=${PROJECT_NAME}
    PROJECT_BEGIN_NAMESPACE=namespace\ ${PROJECT_NAME}\ \{
    PROJECT_USING_NAMESPACE=using\ namespace\ ${PROJECT_NAME}
    PROJECT_END_NAMESPACE=\}
)

set(INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(UI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ui")
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS
    "${UI_PATH}/dialog"
    "${UI_PATH}/widget"
)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${INCLUDE_PATH}"
    "${INCLUDE_PATH}/dialog"
    "${INCLUDE_PATH}/widget"
    "${INCLUDE_PATH}/model"
    "${INCLUDE_PATH}/object"
)

file(GLOB_RECURSE ISU_FILES
    "${INCLUDE_PATH}/*.hpp"
    "${INCLUDE_PATH}/*.h"
    "${SRC_PATH}/*.cpp"
    "${SRC_PATH}/*.c"
    "${UI_PATH}/*.ui"
)
# file(GLOB_RECURSE SOURCES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# file(GLOB UI_FILES "${UI_PATH}/*.ui")

set(PROJECT_SOURCES
        ${ISU_FILES}
        _res.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        # safetools.h safetools.cpp
        # testruntime.hpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET power_carrier APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${GLOBAL_DEFINED}
    ${PROJECT_NAMESPACE_DEFINED}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.power_carrier)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
