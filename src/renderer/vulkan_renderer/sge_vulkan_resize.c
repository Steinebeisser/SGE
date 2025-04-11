//
// Created by Geisthardt on 10.03.2025.
//

#include "sge_vulkan_resize.h"

#include "sge_vulkan_swapchain.h"
#include "core/sge_internal_logging.h"
#include "renderer/sge_internal_render.h"

SGE_RESULT handle_resize_out_of_date(sge_render *render) {
        log_internal_event(LOG_LEVEL_INFO, "Handling Resize/Out of Date");
        sge_vulkan_swapchain_recreate(render);
        log_internal_event(LOG_LEVEL_INFO, "Recreated Swapchain");

        log_internal_event(LOG_LEVEL_INFO, "updating uniform buffer");
        sge_update_uniform_buffer(render, NULL);
        log_internal_event(LOG_LEVEL_INFO, "finished updating uniform buffer");

        log_internal_event(LOG_LEVEL_INFO, "Finished Handling Resize/Out of Date");
        return SGE_SUCCESS;
}
