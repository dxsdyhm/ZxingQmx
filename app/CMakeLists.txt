cmake_minimum_required(VERSION 3.4.1)

set(CPP_DIR_ZXING ${PROJECT_SOURCE_DIR}/src/main/cpp/zxing/core)
set(CPP_DIR_ZXING_BIG ${PROJECT_SOURCE_DIR}/src/main/cpp/zxing/core/src/bigint)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/main/cpp DIR_SRC)

file(GLOB_RECURSE CPP_SRCS ${CPP_DIR_ZXING}/*.cpp)
file(GLOB_RECURSE CPP_SRCS_ZING_BIG ${CPP_DIR_ZXING_BIG}/*.cc)

include_directories(${PROJECT_SOURCE_DIR}/src/main/cpp/zxing/core/src/zxing)
include_directories(${PROJECT_SOURCE_DIR}/src/main/cpp/zxing/core/src/bigint)
include_directories(${PROJECT_SOURCE_DIR}/src/main/cpp/zxing/core/src)
include_directories(${PROJECT_SOURCE_DIR}/src/main/cpp)

add_library( # Sets the name of the library.
             native-lib
             SHARED
             ${CPP_SRCS_ZING_BIG}
             ${CPP_SRCS}
             ${DIR_SRC})

find_library(log-lib
              log )

target_link_libraries(native-lib
                       ${log-lib} )