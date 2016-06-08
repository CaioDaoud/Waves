aux_source_directory( utils UTILS )
aux_source_directory( parser PARSER )
aux_source_directory( methods METHODS )
aux_source_directory( structures STRUCTURES )
aux_source_directory( core CORE )
aux_source_directory( . SRC )
aux_source_directory( ../../libs-iw/indexer/include UTILS2 )

add_library( libQueryProcessor ${UTILS} ${UTILS2} ${SRC} ${PARSER} ${METHODS} ${STRUCTURES} ${CORE})

include ( ${${PROJECT_NAME}_SOURCE_DIR}/install.cmake )
