cmake_minimum_required(VERSION 3.10.0)
project(Server VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

# ========= Set Variables ===========
set(PROJECT_HEADERS_DIR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Headers)
set(PROJECT_SRC_DIR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Sources)

# =========== Include Directories =========
include_directories(${PROJECT_HEADERS_DIR_PATH})

# =========== Project Headers =========
set(PROJECT_HEADERS
  ${PROJECT_HEADERS_DIR_PATH}/dbHelper.hpp
)

# =========== Project Sources =========
set(PROJECT_SOURCES
  ${PROJECT_SRC_DIR_PATH}/dbHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(Server ${PROJECT_SOURCES} ${PROJECT_HEADERS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)

find_package(asio CONFIG REQUIRED)
target_link_libraries(Server PRIVATE asio::asio)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Server PRIVATE nlohmann_json::nlohmann_json)

find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_link_libraries(Server PRIVATE unofficial::sqlite3::sqlite3)

