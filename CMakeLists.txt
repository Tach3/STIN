cmake_minimum_required(VERSION 3.8)

project(project)

# full path from root directory
include(C:/Users/peter/VCPKG/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(Crow CONFIG REQUIRED)

add_executable(
    app
    main.cpp
)

target_link_libraries(app PUBLIC Crow::Crow)