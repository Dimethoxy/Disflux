//==============================================================================
/* ██████╗ ██╗███╗   ███╗███████╗████████╗██╗  ██╗ ██████╗ ██╗  ██╗██╗   ██╗
 * ██╔══██╗██║████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔═══██╗╚██╗██╔╝╚██╗ ██╔╝
 * ██║  ██║██║██╔████╔██║█████╗     ██║   ███████║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██║  ██║██║██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║   ██║ ╚███╔╝  ╚████╔╝
 * ██████╔╝██║██║ ╚═╝ ██║███████╗   ██║   ██║  ██║╚██████╔╝██╔╝ ██╗   ██║
 * ╚═════╝ ╚═╝╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * Part of the Dimethoxy Library, primarily intended for Dimethoxy plugins.
 * External use is permitted but not recommended.
 * No support or compatibility guarantees are provided.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * Compile-time feature switches for the oscilloscope stack. Each section can
 * be toggled independently to isolate crash sources without removing the
 * surrounding code paths.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#ifndef DMT_OSC_ENABLE_THREAD
#define DMT_OSC_ENABLE_THREAD 1
#endif

#ifndef DMT_OSC_ENABLE_BUFFER_SYNC
#define DMT_OSC_ENABLE_BUFFER_SYNC 0
#endif

#ifndef DMT_OSC_ENABLE_IMAGE_SCROLL
#define DMT_OSC_ENABLE_IMAGE_SCROLL 1
#endif

#ifndef DMT_OSC_ENABLE_RENDERER_DRAW
#define DMT_OSC_ENABLE_RENDERER_DRAW 1
#endif

#ifndef DMT_OSC_ENABLE_PARAMETER_FEED
#define DMT_OSC_ENABLE_PARAMETER_FEED 1
#endif
