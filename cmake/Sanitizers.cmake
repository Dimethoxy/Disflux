#==============================================================================
# Opt-in sanitizer and debug-symbol configuration.
# This module is intentionally target-scoped so normal builds are untouched.
#==============================================================================

option(DMT_ENABLE_ASAN "Enable AddressSanitizer for selected targets" OFF)
option(DMT_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer for selected targets" OFF)

function(dmt_target_enable_sanitizers target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "dmt_target_enable_sanitizers: unknown target '${target_name}'")
    endif()

    if(NOT (DMT_ENABLE_ASAN OR DMT_ENABLE_UBSAN))
        return()
    endif()

    target_compile_definitions(${target_name} PRIVATE DMT_SANITIZERS_ENABLED=1)

    # MSVC cl.exe: supports ASan on modern toolsets, but not UBSan.
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(DMT_ENABLE_UBSAN)
            message(WARNING "DMT_ENABLE_UBSAN is not supported with MSVC cl; ignoring UBSan for ${target_name}")
        endif()

        if(DMT_ENABLE_ASAN)
            target_compile_options(${target_name} PRIVATE /fsanitize=address)
            target_link_options(${target_name} PRIVATE /fsanitize=address)
        endif()

        # Symbol and stack quality for debugger + sanitizer crash reports.
        target_compile_options(${target_name} PRIVATE /Zi /Oy-)
        target_link_options(${target_name} PRIVATE /DEBUG /INCREMENTAL:NO)
        return()
    endif()

    # Clang/GCC style sanitizers (including clang-cl frontend).
    set(enabled_sanitizers "")
    if(DMT_ENABLE_ASAN)
        list(APPEND enabled_sanitizers address)
    endif()
    if(DMT_ENABLE_UBSAN)
        list(APPEND enabled_sanitizers undefined)
    endif()

    if(NOT enabled_sanitizers)
        return()
    endif()

    string(JOIN "," sanitizer_arg ${enabled_sanitizers})

    target_compile_options(${target_name}
        PRIVATE
            -fsanitize=${sanitizer_arg}
            -fno-omit-frame-pointer
    )

    target_link_options(${target_name}
        PRIVATE
            -fsanitize=${sanitizer_arg}
    )

    if(WIN32)
        # Keep PDB/debugger quality high on Windows clang/clang-cl presets.
        target_compile_options(${target_name} PRIVATE /Zi /Oy-)
        target_link_options(${target_name} PRIVATE /DEBUG /INCREMENTAL:NO)
    else()
        target_compile_options(${target_name} PRIVATE -g)
    endif()
endfunction()
