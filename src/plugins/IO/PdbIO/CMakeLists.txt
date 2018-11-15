

IF(DYNAMIC_OpenPointCloud)
    ADD_DEFINITIONS(-DDCCORE_LIB)
ENDIF()
file( GLOB header_list *.h  )
SET(TARGET_H ${header_list})

SET(TARGET_SRC
    PdbIO.cpp
    
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
SETUP_PLUGIN(PdbIO)