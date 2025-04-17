//
// Created by Geisthardt on 17.03.2025.
//

#ifndef SGE_H
#define SGE_H



#include <stdint.h>

#include "sge_types.h"
#include "sge_version.h"


#include "utils/sge_time.h"
#include "utils/sge_utils.h"
#include "utils/sge_math.h"
#include "utils/sge_file.h"
#include "utils/sge_string.h"

#include "utils/hash/sge_crc32.h"

#include "utils/image/sge_png.h"


#include "core/memory_control.h"
#include "core/logging.h"
#include "core/input.h"

#include "core/platform/sge_window.h"
#include "core/platform/sge_platform.h"


#include "renderer/sge_render.h"
#include "renderer/sge_render_file.h"
#include "renderer/sge_scene_file.h"
#include "renderer/sge_texture_file.h"
#include "renderer/sge_camera.h"
#include "renderer/sge_region.h"

#include "renderer/apis/sge_vulkan.h"
#include "renderer/apis/sge_opengl.h"
#include "renderer/apis/sge_directx.h"

#include "renderer/shader/sge_shader_utils.h"

#endif //SGE_H
