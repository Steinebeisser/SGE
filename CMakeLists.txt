cmake_minimum_required(VERSION 3.30)
project(SGE C)

set(CMAKE_C_STANDARD 11)

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
        src/renderer/sge_scene_file.h
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

target_link_libraries(SGE PUBLIC Vulkan::Vulkan)

add_executable(TEST test.c)
target_link_libraries(TEST PUBLIC SGE)

include(GNUInstallDirs)

install(TARGETS SGE
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})