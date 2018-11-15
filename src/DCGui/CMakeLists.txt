CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#添加c++预处理器定义
ADD_DEFINITIONS(-DDCGUI_LIB)

#变量定义部分(工程名字、目标头文件、目标源文件、项目依赖库)
#***********************************************************************

#定义工程项目名字
SET(LIB_NAME DCGui)

SET(TARGET_H
    dcguiDLL.h
    Iwindow.h
)

#定义目标源文件cpp
SET(TARGET_SRC
    ConfigParser.cpp
    IWindow.cpp
)

#针对qt宏，对文件进行处理
QT4_WRAP_CPP(DCGui_HEADERS_MOC Iwindow.h)

#定义依赖库名称表，区分debug和release
IF (CMAKE_BUILD_TYPE MATCHES "Release")
    SET(TARGET_LIBRARIES
		    qtmain.lib
				QtCore4.lib
				QtGui4.lib
				QtSql4.lib
		)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(TARGET_LIBRARIES
		    qtmaind.lib
				QtCored4.lib
				QtGuid4.lib
				QtSqld4.lib
		)
ENDIF()

SET(TARGET_LIBRARIES
		    qtmaind.lib
				QtCored4.lib
				QtGuid4.lib
				QtSqld4.lib
)

#包含路径设置（设定头文件包含路径、lib库包含路径）
#***********************************************************************

#在cmake文件中，路径采用正反斜杠都可以，反斜杠需要转义使用
#不管是正反斜杠在生成的vs项目中都是正斜杠
#包含目录中的路径可以加引号（34行）也可以不加引号（28行）

#头文件包含路径
INCLUDE_DIRECTORIES(D:\\Library\\Qt\\4.8.6\\x86\\include
                    D:\\Library\\Qt\\4.8.6\\x86\\include/QtCore
                    D:\\Library\\Qt\\4.8.6\\x86\\include/QtGui
                    D:\\Library\\Qt\\4.8.6\\x86\\include/QtSql)

#包含库目录路径
LINK_DIRECTORIES("D:\\Library\\Qt\\4.8.6\\x86\\lib")

#正式创建项目
#***********************************************************************

#该命令用来匹配TARGET_H下的所有匹配文件
#FILE(GLOB TARGET_H *.h)

#创建库项目根据指定的参数 SHARED--动态库；TARGET_SRC--需要添加编译的cpp文件列表,并指定所属文件夹
ADD_LIBRARY(${LIB_NAME} SHARED ${TARGET_SRC} ${TARGET_H} ${DCGui_HEADERS_MOC})
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES FOLDER "DCLibrary")

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES RELEASE_OUTPUT_NAME "${LIB_NAME}${CMAKE_RELEASE_POSTFIX}")
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_OUTPUT_NAME "${LIB_NAME}${CMAKE_DEBUG_POSTFIX}")

#添加依赖的输入库
#TARGET_LINK_LIBRARIES(${TRGTNAME} optimized "${LINKLIB}${CMAKE_RELEASE_POSTFIX}" debug "${LINKLIB}${CMAKE_DEBUG_POSTFIX}")
TARGET_LINK_LIBRARIES(${LIB_NAME} ${TARGET_LIBRARIES})

#添加安装支持
INSTALL (TARGETS ${LIB_NAME} DESTINATION ${INSTALL_BINDIR})
INSTALL (FILES ${TARGET_H} DESTINATION ${INSTALL_INCDIR}/${LIB_NAME})