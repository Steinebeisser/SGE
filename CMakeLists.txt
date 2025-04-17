cmake_minimum_required(VERSION 3.30)
project(SGE C)

set(CMAKE_C_STANDARD 11)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CLANG_EXECUTABLE "C:/Program Files/LLVM/bin/clang-cl.exe")
    if (!CLANG_EXECUTABLE)
        message(FATAL_ERROR "Clang cl not found, aborting, try downloading from \"https://github.com/llvm/llvm-project/releases\" or set Path correct")
    endif ()
    set(CMAKE_C_COMPILER ${CLANG_EXECUTABLE})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zi -fsanitize=address")
    set(ASAN_RUNTIME_PATH "C:/Program Files/LLVM/lib/clang/20/lib/windows")
    link_directories(${ASAN_RUNTIME_PATH})
    set(ASAN_LIBS
            clang_rt.asan_dynamic-x86_64.lib
            clang_rt.asan_dynamic_runtime_thunk-x86_64.lib
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT")
endif()



find_package(Vulkan REQUIRED)


add_library(SGE STATIC
        src/core/logging.c
        src/utils/sge_time.c
        src/utils/sge_file.c
        src/utils/sge_string.c
        src/utils/sge_utils.c
        src/core/os_specific/sge_window_win32.c
        src/core/memory_control.c
        src/core/input.c
        src/core/os_specific/sge_platform.c
        src/utils/sge_math.c
        src/renderer/sge_render.c
        src/renderer/vulkan_renderer/vulkan_renderer.c
        src/renderer/vulkan_renderer/sge_vulkan_instance.c
        src/renderer/vulkan_renderer/sge_vulkan_surface.c
        src/renderer/vulkan_renderer/sge_vulkan_device.c
        src/renderer/vulkan_renderer/sge_vulkan_swapchain.c
        src/renderer/vulkan_renderer/sge_vulkan_command.c
        src/renderer/vulkan_renderer/sge_vulkan_allocator.c
        src/renderer/vulkan_renderer/sge_vulkan_sync.c
        src/renderer/vulkan_renderer/sge_vulkan_pipeline.c
        src/renderer/vulkan_renderer/sge_vulkan_shader.c
        src/renderer/vulkan_renderer/sge_vulkan_draw.c
        src/renderer/vulkan_renderer/sge_vulkan_resize.c
        src/renderer/sge_camera.c
        src/renderer/vulkan_renderer/sge_vulkan_buffer.c
        src/renderer/vulkan_renderer/sge_vulkan_uniform.c
        src/renderer/vulkan_renderer/sge_vulkan_descriptor.c
        src/renderer/sge_region.c
        src/renderer/sge_render_file.c
        src/SGE.c
        src/renderer/sge_scene_file.c
        src/renderer/vulkan_renderer/sge_vulkan_renderables.c
        src/renderer/shaders/sge_shader_utils.c
        src/renderer/vulkan_renderer/sge_vulkan_draw.c
        src/sge_version.c
        src/renderer/vulkan_renderer/sge_vulkan_memory.c
        src/core/sge_internal_logging.c
        src/renderer/sge_internal_render.c
        src/utils/hash/sge_crc32.c
        src/utils/cpu/sge_endianess.c
        src/utils/image/sge_png.c
        src/renderer/sge_texture_file.c
)

add_library(zlib STATIC
        third_party/zlib/adler32.c
        third_party/zlib/crc32.c
        third_party/zlib/inffast.c
        third_party/zlib/inflate.c
        third_party/zlib/inftrees.c
        third_party/zlib/uncompr.c
        third_party/zlib/zutil.c
)
target_include_directories(zlib PUBLIC third_party/zlib)

target_include_directories(SGE
        PUBLIC  ${CMAKE_SOURCE_DIR}/include
        PRIVATE ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(SGE PUBLIC Vulkan::Vulkan ${ASAN_LIBS} zlib)

add_executable(TEST test.c)
target_link_libraries(TEST PUBLIC SGE ${ASAN_LIBS} zlib)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_link_options(TEST PRIVATE /DEBUG)
    add_custom_command(TARGET TEST POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ASAN_RUNTIME_PATH}/clang_rt.asan_dynamic-x86_64.dll"
            $<TARGET_FILE_DIR:TEST>
    )
endif ()

include(GNUInstallDirs)

install(TARGETS SGE
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})