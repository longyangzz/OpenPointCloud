IF(MSVC80 OR MSVC90)
  OPTION(OSG_MSVC_GENERATE_PLUGINS_AND_WRAPPERS_MANIFESTS "Generate or not manifests files under VS8 for dynamically loaded dlls" ON)
ENDIF()

#the old construct SUBDIRS( was substituded by ADD_SUBDIRECTORY that is to be preferred according on CMake docs.
FOREACH( mylibfolder 
        DCCore
        DCView
        DCGp
        DCUtil
        DCGa
        OpenThreads
      	plugins
      	DCGui
      	DCConsole
      	TileSplit
    )

    ADD_SUBDIRECTORY(${mylibfolder})

ENDFOREACH()

