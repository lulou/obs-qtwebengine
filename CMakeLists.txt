cmake_minimum_required (VERSION 3.8)
project (obs-qtwebengine LANGUAGES C CXX VERSION 0.5.2)

#hack
#SET (CMAKE_POSITION_INDEPENDENT_CODE ON)
SET (CMAKE_CXX_FLAGS "-fPIC")

OPTION(USE_FPKG_QT "Use find_package for qt")
OPTION(USE_FPKG_OBS "Use find_package for obs")

SET(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type")
SET(CMAKE_INSTALL_PREFIX "/usr" CACHE STRING "Installation prefix, usually obs prefix")

IF(USE_FPKG_QT)
 find_package(Qt5Widgets CONFIG REQUIRED)
 find_package(Qt5Network CONFIG REQUIRED)
 find_package(Qt5WebEngine CONFIG REQUIRED)
 SET(QT_LIBRARIES Qt5::Widgets Qt5::Network Qt5::WebEngine)
ELSE(USE_FPKG_QT)
 SET(QT_PREFIX "/usr" CACHE STRING "QT install prefix")
 SET(QT_LIB_DIR "${QT_PREFIX}/lib")
 SET(QT_INCLUDE_DIR "${QT_PREFIX}/include/qt" "${QT_PREFIX}/include/qt/QtWidgets"
                    "${QT_PREFIX}/include/qt/QtWebEngineCore" "${QT_PREFIX}/include/qt/QtWebEngineWidgets"
		    "${QT_PREFIX}/include/qt/QtGui")
 SET(QT_LIBRARIES Qt5Core Qt5Gui Qt5WebEngineCore Qt5WebEngine Qt5WebEngineWidgets Qt5Widgets)
ENDIF(USE_FPKG_QT)

SET(OBS_LIBRARIES)
IF(USE_FPKG_OBS)
 find_package(obs CONFIG REQUIRED)
 SET(OBS_LIBRARIES ${OBS})
ELSE(USE_FPKG_OBS)
 SET(OBS_PREFIX "/usr" CACHE STRING "OBS install prefix")
 SET(OBS_LIB_DIR "${OBS_PREFIX}/lib")
 SET(OBS_INCLUDE_DIR "${OBS_PREFIX}/include/obs")
 SET(OBS_LIBRARIES obs)
ENDIF(USE_FPKG_OBS)

#set(PLUGIN_DIRECTORY "${CMAKE_BINARY_DIR}/build/obs-qtwebengine")
#set(PLUGIN_BIN_DIRECTORY "${PLUGIN_DIRECTORY}/bin/${BITS}bit")
set(PLUGIN_DATA_DIRECTORY "${PROJECT_SOURCE_DIR}/data")

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_BIN_DIRECTORY})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_BIN_DIRECTORY})

SET(SRC_FILES_PLUGIN
 src/obsqtwebengine-main.cpp
 src/obsqtwebengine-source.cpp
 src/obsqtwebengine-manager.cpp
)
SET(HEADER_FILES_PLUGIN
 src/obsqtwebengine-source.hpp
 src/obsqtwebengine-manager.hpp
)

SET(SRC_FILES_EXE
 src/obsqtwebengine-renderer.cpp
)

SET(HEADER_FILES_EXE
)

include_directories(src ${OBS_INCLUDE_DIR} ${QT_INCLUDE_DIR})
link_directories(${OBS_LIB_DIR} ${QT_LIB_DIR})

add_library(obsqtwebengine_shared OBJECT ${SRC_FILES_PLUGIN} ${HEADER_FILES_PLUGIN})
target_link_libraries(obsqtwebengine_shared ${OBS_LIBRARIES} rt)

add_library(obs-qtwebengine MODULE $<TARGET_OBJECTS:obsqtwebengine_shared>)
target_link_libraries(obs-qtwebengine ${OBS_LIBRARIES} rt)
set_target_properties(obs-qtwebengine PROPERTIES PREFIX "")

add_executable(obsqtwebengine ${SRC_FILES_EXE} $<TARGET_OBJECTS:obsqtwebengine_shared>)
target_link_libraries(obsqtwebengine ${OBS_LIBRARIES} ${QT_LIBRARIES} pthread rt -static-libstdc++)
target_compile_features(obsqtwebengine PUBLIC cxx_std_11 cxx_auto_type cxx_constexpr cxx_deleted_functions cxx_nullptr cxx_override cxx_range_for)

install(TARGETS obs-qtwebengine obsqtwebengine DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/obs-plugins)
install(DIRECTORY ${PLUGIN_DATA_DIRECTORY}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/obs/obs-plugins/obs-qtwebengine)
