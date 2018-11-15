PROJECT(DCViz_PLUGINS_MASTER)


SET(CMAKE_SHARED_MODULE_PREFIX ${OSG_PLUGIN_PREFIX})

SET(TARGET_DEFAULT_PREFIX "osgdb_")
SET(TARGET_DEFAULT_LABEL_PREFIX "IO")


############################################################
#
#  selfdefine loader plugins
#
ADD_SUBDIRECTORY(IO/PdbIO)



