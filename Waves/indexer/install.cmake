install(DIRECTORY  "${${PROJECT_NAME}_INCLUDES_DIR}/" DESTINATION include
         PATTERN ".svn" EXCLUDE
         PATTERN "*.cc" EXCLUDE
         PATTERN "*.cpp" EXCLUDE
         PATTERN "*.cxx" EXCLUDE
         PATTERN "*.h")

install( TARGETS ${${PROJECT_NAME}_TO_INSTALL}
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib)
