#==============================================================================
# Opt-in sanitizer and debug-symbol configuration.
# Supports both target-scoped (for selective sanitization) and global (for consistency).
#==============================================================================

option(DMT_ENABLE_ASAN "Enable AddressSanitizer for all targets" OFF)
option(DMT_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for all targets" OFF)

# Apply sanitizer flags globally when enabled to ensure consistency across all compilation units
if(DMT_ENABLE_ASAN OR DMT_ENABLE_UBSAN)
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
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
        # Clang/GCC style sanitizers (including clang-cl frontend).
        set(enabled_sanitizers "")
        if(DMT_ENABLE_ASAN)
            list(APPEND enabled_sanitizers address)
        endif()
        if(DMT_ENABLE_UBSAN)
            list(APPEND enabled_sanitizers undefined)
        endif()

        if(enabled_sanitizers)
            string(JOIN "," sanitizer_arg ${enabled_sanitizers})
            add_compile_options(-fsanitize=${sanitizer_arg} -fno-omit-frame-pointer)
            add_link_options(-fsanitize=${sanitizer_arg})
        endif()

        if(WIN32)
            # Keep PDB/debugger quality high on Windows clang/clang-cl presets.
            add_compile_options(/Zi /Oy-)
            add_link_options(/DEBUG /INCREMENTAL:NO)
        else()
            add_compile_options(-g)
        endif()
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
