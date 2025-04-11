cmake_minimum_required(VERSION 3.30)
project(SGE C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_COMPILER "C:/Program Files/LLVM/bin/clang-cl.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files/LLVM/bin/clang-cl.exe")

# Use /Zi for PDB generation and AddressSanitizer flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zi -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi -fsanitize=address")

# Specify ASan runtime path and libraries
set(ASAN_RUNTIME_PATH "C:\\Program Files\\LLVM\\lib\\clang\\20\\lib\\windows")
link_directories(${ASAN_RUNTIME_PATH})
set(ASAN_LIBS
        clang_rt.asan_dynamic-x86_64.lib
        clang_rt.asan_dynamic_runtime_thunk-x86_64.lib
)
# Ensure we're using /MT to avoid Debug CRT issues
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MT")



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
        src/utils/hash/crc32.c
)


target_include_directories(SGE
        PUBLIC  ${CMAKE_SOURCE_DIR}/include
        PRIVATE ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(SGE PUBLIC Vulkan::Vulkan ${ASAN_LIBS})

add_executable(TEST test.c)
target_link_libraries(TEST PUBLIC SGE ${ASAN_LIBS})
target_link_options(TEST PRIVATE /DEBUG)

add_custom_command(TARGET TEST POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASAN_RUNTIME_PATH}/clang_rt.asan_dynamic-x86_64.dll"
        $<TARGET_FILE_DIR:TEST>
)

include(GNUInstallDirs)

install(TARGETS SGE
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})