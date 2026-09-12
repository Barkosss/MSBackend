cmake_minimum_required(VERSION 3.31.5)
project(OrionServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BOOST_DIR "F:/tools/vcpkg/installed/x64-windows")
set(Boost_INCLUDE_DIR "${BOOST_DIR}/include")
set(Boost_LIBRARY_DIR "${BOOST_DIR}/lib")

add_executable(OrionServer
        src/main.cpp
        src/utils/config.cpp
        src/utils/rules.cpp
        src/server/router.cpp
        src/server/server.cpp
        src/handlers/healthzHandler.cpp
        src/handlers/manifestHandler.cpp
        src/handlers/downloadHandler.cpp
        src/handlers/checkHandler.cpp
        src/handlers/reloadHandler.cpp
)

set_target_properties(OrionServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

find_package(Boost REQUIRED COMPONENTS filesystem system url)
find_package(yaml-cpp REQUIRED)

target_link_libraries(OrionServer PRIVATE
        ${Boost_LIBRARIES}
        yaml-cpp::yaml-cpp
)

target_compile_definitions(OrionServer PRIVATE
        BOOST_ASIO_HAS_CO_AWAIT
        BOOST_ASIO_NO_DEPRECATED
)

target_include_directories(OrionServer PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
target_compile_definitions(OrionServer PRIVATE
        _WIN32_WINNT=0x0A00
        BOOST_ALL_DYN_LINK
)
endif()

if(UNIX AND NOT WIN32)
    target_compile_definitions(OrionServer PRIVATE
        _GNU_SOURCE
    )
endif()

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")