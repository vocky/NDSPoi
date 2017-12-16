#project name

PROJECT(test_math)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -pthread -fpermissive -fexceptions" )
SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -std=c99 -pthread -fexceptions" )

set(SUB_MODULE_DIR "..")
set(LIB_NAME_PREFIX "lib")
set(LIB_NAME_SUFFIX ".a")
set(LIB_MODULE_NAME_PREFIX "lib")
set(LIB_MODULE_NAME_SUFFIX ".a")

#head file path

INCLUDE_DIRECTORIES(

include

)

#source directory

AUX_SOURCE_DIRECTORY(sqlexec DIR_SRCS)

#set environment variable

SET(SQL_SOURCE_FILES

${DIR_SRCS}

)

#add executable file

#ADD_EXECUTABLE(ctest ${TEST_MATH})
add_library(sqlexec SHARED ${SQL_SOURCE_FILES})

#add link library

TARGET_LINK_LIBRARIES(sqlexec sqlite3)
