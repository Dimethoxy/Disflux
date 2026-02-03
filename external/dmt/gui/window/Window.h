//==============================================================================
// ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
// ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
// ██   ██ ██ ██ ████ ██ █████      ██    ███████ ██    ██   ███     ████
// ██   ██ ██ ██  ██  ██ ██         ██    ██   ██ ██    ██  ██ ██     ██
// ██████  ██ ██      ██ ███████    ██    ██   ██  ██████  ██   ██    ██
//
// Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
//
// This file is part of the Dimethoxy Library, a collection of essential
// classes used across various Dimethoxy projects.
// These files are primarily designed for internal use within our repositories.
//
// License:
// This code is licensed under the GPLv3 license. You are permitted to use and
// modify this code under the terms of this license.
// You must adhere GPLv3 license for any project using this code or parts of it.
// Your are not allowed to use this code in any closed-source project.
//
// Description:
// Aggregates all Dimethoxy window-related headers for convenient inclusion.
// Provides a single entry point for window, editor, alerts, compositor, header,
// popover, and tooltip classes.
//
// Authors:
// Lunix-420 (Primary Author)
//==============================================================================

#pragma once

//==============================================================================
/**
 * @brief Aggregates all Dimethoxy window-related headers for convenient
 *         inclusion.
 *
 * @details
 * This header includes all core window modules (editor, alerts, compositor,
 * header, popover, tooltip) in a single file. Use this header to ensure
 * consistent access to all window features across the codebase. Designed for
 * real-time safety and deterministic resource management.
 */
//==============================================================================

#include "./AbstractEditor.h"
#include "./Alerts.h"
#include "./Compositor.h"
#include "./Header.h"
#include "./Popover.h"
#include "./Tooltip.h"

//==============================================================================