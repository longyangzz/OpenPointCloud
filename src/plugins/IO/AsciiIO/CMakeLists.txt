

IF(DYNAMIC_OpenPointCloud)
    ADD_DEFINITIONS(-DDCCORE_LIB)
ENDIF()

file( GLOB ui_list *.ui impl/*.ui)
qt5_wrap_ui( generated_ui_list ${ui_list} )

file( GLOB header_list *.h  )
SET(TARGET_H 
	${header_list}
	${generated_ui_list}
)

file( GLOB cpp_list *.cpp  *.def)
SET(TARGET_SRC
    ${cpp_list}
    
)

INCLUDE_DIRECTORIES(
		${QT_INCLUDE_DIR} 
		${QT_QTCORE_INCLUDE_DIR}
		${ACTUAL_3RDPARTY_DIR}/Eigen3
	)

SET(TARGET_COMMON_LIBRARIES
    Qt5::Core
    DCCore
    DCGp
)


#### end var setup  ###
SETUP_PLUGIN(AsciiIO)