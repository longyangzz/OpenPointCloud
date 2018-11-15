

IF(DYNAMIC_OpenPointCloud)
    ADD_DEFINITIONS(-DPYRAMID_EXPORT)
ENDIF()

SET(LIB_NAME TileSplit)
SET(HEADER_PATH ${OpenPointCloud_SOURCE_DIR}/src/${LIB_NAME})

SET(SOURCES_H
    #${HEADER_PATH}/AppSettings.h

)

QT5_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} OPTIONS "-f" )
#qt5_wrap_ui( generated_ui_list ${ui_list} )
#qt5_add_resources( generated_qrc_list ${qrc_list} )

## method 2
file( GLOB header_list *.h  *.hpp)
SET(TARGET_H ${header_list})


file( GLOB cpp_list *.cpp impl/*.cpp impl/*.cxx )
SET(TARGET_SRC
    ${cpp_list}
    ${SOURCES_H_MOC}
    ${OpenPointCloud_VERSIONINFO_RC}
)


IF(CMAKE_COMPILER_IS_GNUCXX)
    # Remove -pedantic flag as it barfs on Qt headers
   STRING(REGEX REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF()

SET(TARGET_LIBRARIES
    ${TARGET_LIBRARIES}
    Qt5::Core
	Qt5::Gui 
	Qt5::Widgets 
	DCCore
	${ACTUAL_3RDPARTY_DIR}/boost_1_60_0/lib64-msvc-14.0/libboost_thread-vc140-mt-gd-1_60.lib
	${ACTUAL_3RDPARTY_DIR}/boost_1_60_0/lib64-msvc-14.0/libboost_system-vc140-mt-gd-1_60.lib
	${ACTUAL_3RDPARTY_DIR}/boost_1_60_0/lib64-msvc-14.0/libboost_program_options-vc140-mt-gd-1_60.lib
	
)

INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR} 
	${QT_QTCORE_INCLUDE_DIR}
	${LIBLAS_INCLUDE_DIR}
	${ACTUAL_3RDPARTY_DIR}
	${ACTUAL_3RDPARTY_DIR}/boost_1_60_0
)

SETUP_LIBRARY(${LIB_NAME})

include(LiblasSupport)

#liblas support
target_link_liblas( ${TARGET_TARGETNAME} )

TARGET_LINK_LIBRARIES(
  ${TARGET_TARGETNAME} 
  )

