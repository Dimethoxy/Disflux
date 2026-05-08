#==============================================================================
# Opt-in sanitizer and debug-symbol configuration.
# Supports both target-scoped (for selective sanitization) and global (for consistency).
#==============================================================================

option(DMT_ENABLE_ASAN "Enable AddressSanitizer for all targets" OFF)
option(DMT_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for all targets" OFF)

# Apply sanitizer flags globally when enabled to ensure consistency across all compilation units
# Windows MSVC only - no sanitizers on Mac or Linux
if(DMT_ENABLE_ASAN OR DMT_ENABLE_UBSAN)
    if(MSVC)
        if(DMT_ENABLE_UBSAN)
            message(WARNING "DMT_ENABLE_UBSAN is not supported with MSVC cl; ignoring UBSan")
        endif()

        if(DMT_ENABLE_ASAN)
            add_compile_options(/fsanitize=address)
            add_link_options(/fsanitize=address)
            # Disable incremental linking with ASAN
            add_link_options(/INCREMENTAL:NO)
        endif()

        # Symbol and stack quality for debugger + sanitizer crash reports.
        add_compile_options(/Zi /Oy-)
        add_link_options(/DEBUG)
    else()
        message(WARNING "Sanitizers are only supported on Windows with MSVC; ignoring DMT_ENABLE_ASAN and DMT_ENABLE_UBSAN")
    endif()
endif()

add_compile_definitions($<IF:$<BOOL:${DMT_ENABLE_ASAN}>,DMT_SANITIZERS_ENABLED=1,>)

function(dmt_target_enable_sanitizers target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "dmt_target_enable_sanitizers: unknown target '${target_name}'")
    endif()

    # Sanitizers are now applied globally, so this function is a no-op for backwards compatibility
    if(DMT_ENABLE_ASAN)
        target_compile_definitions(${target_name} PRIVATE DMT_SANITIZERS_ENABLED=1)
    endif()
endfunction()
