cmake_minimum_required(VERSION 3.0.2)
include(CMakeDependentOption)

set(SHORT_NAME 3dgrapher)

#-DSWITCH_BUILD=ON
#set(-DSWITCH_BUILD=ON)

if(VITA_BUILD AND SWITCH_BUILD)
    message(FATAL_ERROR "Cannot build with -DVITA_BUILD=ON and -DSWITCH_BUILD=ON at the same time")
endif()

option(WIN_RELEASE "Disable console" OFF)

option(DRAW_FPS "Draw FPS on the top left corner of the window." OFF)
option(SYSTEM_LIBS "Use system libraries when available." ON)
cmake_dependent_option(VITA_BUILD "Build for the PlayStation Vita handheld game console." OFF "NOT MSVC" OFF)
cmake_dependent_option(SWITCH_BUILD "Build for the Nintendo Switch handheld game console." OFF "NOT MSVC; NOT VITA_BUILD" OFF)

if(VITA_BUILD AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
endif()

if(SWITCH_BUILD AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{DEVKITPRO})
        set(DEVKITPRO $ENV{DEVKITPRO})
        set(CMAKE_TOOLCHAIN_FILE "./cmake/switch.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
    endif()
endif()

project(${SHORT_NAME})

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD=1)
#SET (SWITCH_BUILD ON)


set(OPENGL_HELPERS
        src/glhelpers/shaderloader.h
        src/glhelpers/shaders.h
        src/glhelpers/shader.h
        src/glhelpers/camera.h
        src/glad/glad.h
        src/glad/glad.c
        src/glad/khrplatform.h
        src/tinyexpr.c
        src/tinyexpr.h
        )
set(IMGUI_FILES
        src/imgui/backends/imgui_impl_sdl.cpp
        src/imgui/backends/imgui_impl_opengl3.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_tables.cpp
        src/tinyexpr/tinyexpr.c
        )
set(SOURCE_FILES
        src/main.cpp
        include/glad/glad.c
        src/tinyexpr/tinyexpr.c
        ${IMGUI_FILES}
        )
if (NOT SWITCH_BUILD)
    set(SOURCE_FILES ${SOURCE_FILES} include/glad/glad.c)
    include_directories(${PROJECT_SOURCE_DIR}/include)

endif ()
if (SWITCH_BUILD)
    add_compile_definitions(NINTENDO_SWITCH=1)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src/imgui)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ldl  ")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3  ")
if (VITA_BUILD)
    include("${VITASDK}/share/vita.cmake" REQUIRED)
    set(VITA_TITLEID  "JULIUS001")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/")

if (CMAKE_VERSION VERSION_LESS 3.1)
    if (CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    endif()
else()
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_CXX_STANDARD 14)
endif()

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror=implicit-function-declaration")

    if (CMAKE_C_COMPILER_VERSION VERSION_GREATER 5.0)
        set(CMAKE_C_FLAGS "-Werror=incompatible-pointer-types -Werror=int-conversion -Wstrict-prototypes ${CMAKE_C_FLAGS}")
    endif()
endif()

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 1)
set(IS_RELEASE_VERSION FALSE)

if(IS_RELEASE_VERSION)
    set(PROJECT_VERSION_TWEAK 0)
    set(VERSION_REVISION "")
elseif (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    find_package(Git)
    if(Git_FOUND)
        execute_process(
                COMMAND git rev-list --count HEAD ^tags/v${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE TWEAK_RESULT
                OUTPUT_VARIABLE PROJECT_VERSION_TWEAK
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
                COMMAND git rev-parse --short --verify HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE VERSION_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
                COMMAND git diff-index --quiet HEAD --
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE VERSION_COMMIT_DIRTY
        )
        if(VERSION_COMMIT_DIRTY)
            set(DIRTY_POSTFIX "-dirty")
        else()
            set(DIRTY_POSTFIX "")
        endif()
        if(NOT TWEAK_RESULT EQUAL 0)
            # Happens on a shallow git clone, like Travis does. Append date to version ref.
            set(PROJECT_VERSION_TWEAK 0)
            string(TIMESTAMP TODAY "%Y%m%d")
            set(VERSION_REVISION "-${TODAY}-${VERSION_COMMIT_HASH}${DIRTY_POSTFIX}")
        else()
            set(VERSION_REVISION ".${PROJECT_VERSION_TWEAK}-${VERSION_COMMIT_HASH}${DIRTY_POSTFIX}")
        endif()
    else()
        set(PROJECT_VERSION_TWEAK 0)
        set(VERSION_REVISION " unknown development version")
    endif()
else()
    set(PROJECT_VERSION_TWEAK 0)
    set(VERSION_REVISION " unknown development version")
endif()

if(VITA_BUILD)
    string(LENGTH "${PROJECT_VERSION_MAJOR}" NUM_DIGITS)
    if(NUM_DIGITS EQUAL 1)
        set(VITA_VERSION_MAJOR "0${PROJECT_VERSION_MAJOR}")
    elseif(NUM_DIGITS EQUAL 2)
        set(VITA_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    else()
        message(FATAL_ERROR "On Vita, major version number can only have one or two digits, found ${NUM_DIGITS} digits instead")
    endif()

    string(LENGTH "${PROJECT_VERSION_MINOR}" NUM_DIGITS)
    if(NUM_DIGITS EQUAL 1)
        set(VITA_VERSION_MINOR "0${PROJECT_VERSION_MINOR}")
    elseif(NUM_DIGITS EQUAL 2)
        set(VITA_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    else()
        message(FATAL_ERROR "On Vita, minor version number can only have one or two digits, found ${NUM_DIGITS} digits instead")
    endif()

    set(VITA_VERSION "${VITA_VERSION_MAJOR}.${VITA_VERSION_MINOR}")
endif()

if (SWITCH_BUILD)
    message("hello")
    string(SUBSTRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${VERSION_REVISION}" 0 15 SWITCH_VERSION)
endif()

if(DRAW_FPS)
    add_definitions(-DDRAW_FPS)
endif()

set(MACOSX_FILES "")
if(APPLE)
    set(MACOSX_FILES ${PROJECT_SOURCE_DIR}/res/julius.icns)
endif()



function(GET_SDL_EXT_DIR result module)
    if(NOT module STREQUAL "")
        set(module "_${module}")
    endif()
    set(SDL_LOCATION ${PROJECT_SOURCE_DIR}/ext/SDL2)
    file(GLOB children
            RELATIVE ${SDL_LOCATION}
            CONFIGURE_DEPENDS
            ${SDL_LOCATION}/SDL${module}
            ${SDL_LOCATION}/SDL2${module}
            ${SDL_LOCATION}/SDL${module}-*
            ${SDL_LOCATION}/SDL2${module}-*
            )
    foreach(child ${children})
        if(IS_DIRECTORY "${SDL_LOCATION}/${child}")
            set(${result} "${SDL_LOCATION}/${child}" PARENT_SCOPE)
            break()
        endif()
    endforeach()
endfunction()

find_package(SDL2 REQUIRED)
#find_package(SDL2_mixer REQUIRED)

if (WIN_RELEASE)
    add_executable(${SHORT_NAME} WIN32 ${SOURCE_FILES})
else()
    add_executable(${SHORT_NAME} ${SOURCE_FILES})
endif ()

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if(APPLE)
    find_package(OpenGl REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} )
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} )
    #define IMGUI_IMPL_OPENGL_LOADER_CUSTOM <OpenGL/gl3.h>
    #add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<OpenGL/gl3.h>)




    # generating a macOS icns file (see https://stackoverflow.com/a/20703594)
    add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/res/julius.icns
            COMMAND mkdir -p julius.iconset
            COMMAND sips -z 16 16    julius_256.png --out julius.iconset/icon_16x16.png
            COMMAND sips -z 32 32    julius_256.png --out julius.iconset/icon_16x16@2x.png
            COMMAND sips -z 32 32    julius_256.png --out julius.iconset/icon_32x32.png
            COMMAND sips -z 64 64    julius_256.png --out julius.iconset/icon_32x32@2x.png
            COMMAND sips -z 128 128  julius_256.png --out julius.iconset/icon_128x128.png
            COMMAND sips -z 256 256  julius_256.png --out julius.iconset/icon_128x128@2x.png
            COMMAND sips -z 256 256  julius_256.png --out julius.iconset/icon_256x256.png
            COMMAND sips -z 512 512  julius_512.png --out julius.iconset/icon_256x256@2x.png
            COMMAND iconutil -c icns julius.iconset
            COMMAND rm -R julius.iconset
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/res)
    set_source_files_properties(${PROJECT_SOURCE_DIR}/res/julius.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")

    # setting variables that will populate Info.plist
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "nl.biancavanschaik.julius")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum OS X deployment version" FORCE)
    set(MACOSX_BUNDLE_ICON_FILE "julius.icns")
    set(MACOSX_BUNDLE_BUNDLE_VERSION
            "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${VERSION_REVISION}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${MACOSX_BUNDLE_BUNDLE_VERSION})

    set_target_properties(${SHORT_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/res/Info.plist.in")

    set(DIRS "")

    # if SDL2 library is a framework, we need to indicate to CMake
    # the path to its dependencies. SDL2_LIBRARY contains two parts.
    # Example: /Library/Frameworks/SDL2.framework;-framework Cocoa
    # So strip out everything after the ';'
    string(REGEX REPLACE ";.*$" "" SDL2_LIB_DIR "${SDL2_LIBRARY}")
    if(EXISTS "${SDL2_LIB_DIR}/Versions/A/Frameworks")
        set(DIRS "${DIRS};${SDL2_LIB_DIR}/Versions/A/Frameworks")
    endif()

    # if SDL2_mixer library is a framework, we need to indicate to CMake
    # the path to its dependencies (Ogg.framework etc):
    if(EXISTS "${SDL2_MIXER_LIBRARY}/Versions/A/Frameworks")
        set(DIRS "${DIRS};${SDL2_MIXER_LIBRARY}/Versions/A/Frameworks")
    endif()

    # when installing, "fixup" automatically copies librairies inside the
    # bundle and links the binary against them
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app \"\" \"${DIRS}\")
    " BUNDLE DESTINATION ${CMAKE_BINARY_DIR})
endif()

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIR})

if(SYSTEM_LIBS)
    find_package(PNG)
endif()
if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIRS})
    target_link_libraries(${SHORT_NAME} ${PNG_LIBRARIES})
else()
    #include_directories("ext/png")
    #target_sources(${SHORT_NAME} PRIVATE "${PNG_FILES}" "${ZLIB_FILES}")
endif()

if(VITA_BUILD)
    include_directories(
            $ENV{VITASDK}/arm-vita-eabi/include
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif (SWITCH_BUILD)
    include_directories(
            ${DEVKITPRO}/portlibs/switch/include/SDL2
            ${DEVKITPRO}/libnx/include
            ${DEVKITPRO}/portlibs/switch/include
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

include_directories(ext)
include_directories(src)
if(MSVC)
    include_directories(ext/dirent)
endif()

if (VITA_BUILD)
    target_link_libraries (${SHORT_NAME}
            ${SDL2_MIXER_LIBRARY}
            ${SDL2_LIBRARY}
            vorbisfile
            vorbis
            mikmod
            mpg123
            FLAC
            ogg
            vita2d
            m
            SceAppUtil_stub
            SceAudio_stub
            SceCommonDialog_stub
            SceCtrl_stub
            SceDisplay_stub
            SceGxm_stub
            SceHid_stub
            SceIofilemgr_stub
            SceSysmodule_stub
            SceTouch_stub
            ScePower_stub
            ScePgf_stub
            SceAppMgr_stub
            )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__vita__")
    # this setting enables larger heap memory sizes on Vita, up to ~330 MB
    # useful for pre-loading videos into memory
    set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")
    vita_create_self(${SHORT_NAME}.self ${SHORT_NAME} UNSAFE UNCOMPRESSED)
    vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
            VERSION ${VITA_VERSION}
            NAME ${SHORT_NAME}
            FILE res/sce_sys/icon0.png sce_sys/icon0.png
            FILE res/sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
            FILE res/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
            FILE res/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
            )
elseif(SWITCH_BUILD)
    target_link_libraries (${SHORT_NAME}
            SDL2_mixer
            SDL2
            EGL
            glad
            GLESv2
            glapi
            drm_nouveau
            vorbisidec
            modplug
            mpg123
            FLAC
            opusfile
            opus
            ogg
            m
            z
            nx
            stdc++
            )

    set(FLAGS
            -D__SWITCH__
            -Wno-write-strings
            -march=armv8-a
            -mtune=cortex-a57
            -mtp=soft
            -O2
            -fPIE
            -fno-asynchronous-unwind-tables
            -funroll-loops
            -fno-unwind-tables
            -fno-optimize-sibling-calls
            -mlittle-endian
            -ffunction-sections
            )

    foreach(flag ${FLAGS})
        set(FLAGS_GENERAL "${FLAGS_GENERAL} ${flag}")
    endforeach(flag ${FLAGS})

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS_GENERAL} --verbose")

    target_compile_options(${SHORT_NAME} PRIVATE ${FLAGS})
    add_custom_command(TARGET ${SHORT_NAME}
            POST_BUILD
            COMMAND ${DEVKITPRO}/tools/bin/nacptool --create "${SHORT_NAME}" "rsn8887" ${SWITCH_VERSION} ${SHORT_NAME}.nacp
            COMMAND ${DEVKITPRO}/tools/bin/elf2nro ${SHORT_NAME} ${SHORT_NAME}.nro --icon=${CMAKE_SOURCE_DIR}/res/switchdata/icon0.jpg --nacp=${SHORT_NAME}.nacp
            )
else()
    if (UNIX AND NOT APPLE AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang"))
        target_link_libraries(${SHORT_NAME} m ${CMAKE_DL_LIBS})
    endif()
    target_link_libraries (${SHORT_NAME} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY})
    if(NOT APPLE)
        install(TARGETS ${SHORT_NAME} RUNTIME DESTINATION bin)
    endif()

    if(UNIX AND NOT APPLE)
        install(FILES "res/julius.desktop" DESTINATION "share/applications" RENAME "com.github.bvschaik.julius.desktop")
        install(FILES "res/julius.metainfo.xml" DESTINATION "share/metainfo" RENAME "com.github.bvschaik.julius.metainfo.xml")
        install(FILES "res/julius_48.png" DESTINATION "share/icons/hicolor/48x48/apps" RENAME "com.github.bvschaik.julius.png")
        install(FILES "res/julius_256.png" DESTINATION "share/icons/hicolor/256x256/apps" RENAME "com.github.bvschaik.julius.png")
        install(FILES "res/julius_512.png" DESTINATION "share/icons/hicolor/512x512/apps" RENAME "com.github.bvschaik.julius.png")
    endif()

    # Unit tests
    if(NOT VITA_BUILD AND NOT SWITCH_BUILD)
        #enable_testing()
        #add_subdirectory(test)
    endif()
endif()
