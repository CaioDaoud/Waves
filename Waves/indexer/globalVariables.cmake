###########################################
## GENERAL CONFIGURATIONS OF THE PROJECT ##
###########################################

# Set Infoweb project libs directory
set(IW_LIBS_DIR ${${PROJECT_NAME}_SOURCE_DIR}/../libs-iw)

# Set External libs directory
set(EXT_LIBS_DIR ${${PROJECT_NAME}_SOURCE_DIR}/../libs-ext)

# Set Infoweb dependencies
#set(IW_LIBS_DEPENDENCES common)

# Set computer architecture
set(ARCHITECTURE x86_64)
#set(ARCHITECTURE amd64)
#set(JAVA_HOME /usr/lib/jvm/java-6-sun)

# Default includes directory
set(${PROJECT_NAME}_INCLUDES_DIR ${${PROJECT_NAME}_SOURCE_DIR}/src)

# All project includes
set(INCLUDES ${${PROJECT_NAME}_INCLUDES_DIR}
             ${JAVA_HOME}/include/ 
             ${JAVA_HOME}/include/linux/
             ${EXT_LIBS_DIR}/include)

# All external libs
#set(LIBS ${JAVA_HOME}/jre/lib/${ARCHITECTURE}/server
#         ${EXT_LIBS_DIR}/lib)

# Concat information (include e libs) of each project
foreach(TP_CURRENT ${IW_LIBS_DEPENDENCES})
	# Check project dependencies. If any one of  
	# ${IW_LIBS_DIR} or ${IW_LIBS_DIR}/${TP_CURRENT} directories
	# do not exists an error message will be printed and cmake will die.
        IF(NOT EXISTS "${IW_LIBS_DIR}/")
          MESSAGE(FATAL_ERROR "Sorry, could not find ${IW_LIBS_DIR}/ \n")      
        ENDIF(NOT EXISTS "${IW_LIBS_DIR}/")

        IF(NOT EXISTS "${IW_LIBS_DIR}/${TP_CURRENT}/")
          MESSAGE(FATAL_ERROR "Sorry, could not find ${IW_LIBS_DIR}/${TP_CURRENT}/ \n")      
        ENDIF(NOT EXISTS "${IW_LIBS_DIR}/${TP_CURRENT}/")

	IF(EXISTS ${IW_LIBS_DIR}/${TP_CURRENT}/include/thrift)
		list ( APPEND INCLUDES ${IW_LIBS_DIR}/${TP_CURRENT}/include/thrift)
	ENDIF(EXISTS ${IW_LIBS_DIR}/${TP_CURRENT}/include/thrift)
    
	# Include configuration for all projects
	list ( APPEND INCLUDES ${IW_LIBS_DIR}/${TP_CURRENT}/include )
	list ( APPEND LIBS ${IW_LIBS_DIR}/${TP_CURRENT}/lib )
endforeach(TP_CURRENT)


###########################################
##        INTALLATION POLICIES           ##
###########################################

# Ex: cmake .. -DINSTALL_DIRECTORY=/usr/local

# Set installation directory of the INSTALL target
if ( NOT DEFINED INSTALL_DIRECTORY )
   set (INSTALL_DIRECTORY ${IW_LIBS_DIR}/${PROJECT_NAME}/)   
endif ( NOT DEFINED INSTALL_DIRECTORY )

set (CMAKE_INSTALL_PREFIX ${INSTALL_DIRECTORY})


###########################################
##         COMPILATION POLICIES          ##
###########################################

# Ex: cmake .. -DCMAKE_BUILD_TYPE=Release

# Set compilation mode
if ( NOT CMAKE_BUILD_TYPE )
set (CMAKE_BUILD_TYPE debug)
endif ( NOT CMAKE_BUILD_TYPE )

# Set library type: static or shared
if ( NOT DEFINED BUILD_SHARED_LIBS )
set (BUILD_SHARED_LIBS ON)
endif ( NOT DEFINED BUILD_SHARED_LIBS )

# Set installation flags
set (CMAKE_C_FLAGS "-Wall   -O2 -I /usr/local/lib -Wno-deprecated -pthread ")
set (CMAKE_CXX_FLAGS "-Wall   -O2 -I /usr/local/lib -lm -Wno-deprecated -pthread ")
set (CMAKE_C_FLAGS_DEBUG "   -O2 -g -I /usr/local/lib  -Wno-deprecated -pthread ")
set (CMAKE_CXX_FLAGS_DEBUG "  -O2 -g -I /usr/local/lib -Wno-deprecated -pthread ")
set (CMAKE_C_FLAGS_RELEASE "-Wall    -O2 -DNDEBUG -DDISABLE_LOG_DEBUG -I /usr/local/lib  -Wno-deprecated -pthread ")
set (CMAKE_CXX_FLAGS_RELEASE "-Wall    -O2 -DNDEBUG  -DDISABLE_LOG_DEBUG -I /usr/local/lib  -Wno-deprecated -pthread ")
set (CMAKE_C_FLAGS_RELEASETUNE "-Wall   -O2 -DNDEBUG -DDISABLE_LOG_DEBUG -march=native -mtune=native -I /usr/local/lib  -Wno-deprecated -pthread ")
set (CMAKE_CXX_FLAGS_RELEASETUNE "-Wall   -O2 -DNDEBUG -DDISABLE_LOG_DEBUG -march=native -mtune=native -I /usr/local/lib  -Wno-deprecated -pthread ")

# Add an "profile" build type
SET(CMAKE_CXX_FLAGS_PROFILE "-pg" CACHE STRING
    "Flags used by the C++ compiler during profiled builds."
    FORCE)
SET(CMAKE_C_FLAGS_PROFILE "-pg" CACHE STRING
    "Flags used by the C compiler during profiled builds."
    FORCE)
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PROFILE
    CMAKE_C_FLAGS_PROFILE)


###########################################
##         OTHER CONFIGURATIONS          ##
###########################################

# Custom target for doxygen
# Use: make doc
ADD_CUSTOM_COMMAND(OUTPUT doc/${PROJECT_NAME}.doxyfile COMMAND doxygen ${${PROJECT_NAME}_SOURCE_DIR}/doc/${PROJECT_NAME}.doxyfile)
ADD_CUSTOM_TARGET(doc DEPENDS doc/${PROJECT_NAME}.doxyfile)
