

file( GLOB source_list *.cpp  )
SET(TARGET_SRC
    ${source_list}
    ${DCVIZ_VERSIONINFO_RC}
)


SETUP_APPLICATION(OnlyViewer)
