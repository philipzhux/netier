file(GLOB_RECURSE netier_sources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
add_library(netier_shared SHARED ${netier_sources})