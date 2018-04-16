aux_source_directory(. APP_SRC_LIST)
add_executable(sqlgen ${APP_SRC_LIST})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


message("sqlgen main.cpp sqlgen.cpp")
message(${PROJECT_SOURCE_DIR})
message(${PROJECT_BINARY_DIR})



