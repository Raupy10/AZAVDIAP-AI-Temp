cmake_minimum_required(VERSION 3.18)
project(AZAVDIAP CXX)

if(WIN32)
    enable_language(RC)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)



if(UNIX AND NOT APPLE)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(WAYLAND wayland-client)
    endif()
    install(
    FILES default_config.json
    DESTINATION share/soundboard
    )

    if(WAYLAND_FOUND)
        message(STATUS "Wayland +X11")
        set(GLFW_BUILD_WAYLAND ON  CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11     ON  CACHE BOOL "" FORCE)
    else()
        message(STATUS "X11 only")
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11     ON  CACHE BOOL "" FORCE)
    endif()
endif()

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG        "5.5"       
    GIT_PROGRESS   TRUE
)
FetchContent_Declare(
    json
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG        "v3.11.3"
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY "https://github.com/mackron/miniaudio.git"
    GIT_TAG        "0.11.22"
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(raylib json miniaudio)


file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
list(APPEND PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
if(WIN32)
    list(APPEND PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
endif()


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${miniaudio_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib nlohmann_json::nlohmann_json)


if(WIN32)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        gdi32
        winmm
        comdlg32
        shell32
        ole32
    )
    if(MINGW)
        target_link_options(${PROJECT_NAME} PRIVATE
            -mwindows
            -static
            -static-libgcc
            -static-libstdc++
        )
    endif()
elseif(UNIX AND NOT APPLE)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        m dl pthread GL rt X11
    )
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE
    ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)

#target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS_PATH="./Art/" )