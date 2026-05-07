# Windows Crash Debugging for Intermittent VST/Plugin Failures

This guide focuses on Windows-only, hard-to-reproduce crashes in DAWs using the sanitizer presets added to this project.

## Presets Added

- Windows ASan
- Windows RelWithDebInfo ASan
- Windows Clang ASan (requires clang-cl)
- Windows Clang UBSan (requires clang-cl)

These are isolated from your normal presets and use separate build directories.

## Build from VS Code

1. Open Command Palette.
2. Run `CMake: Select Configure Preset` and choose one of:
   - `Windows ASan`
   - `Windows RelWithDebInfo ASan`
   - `Windows Clang ASan`
   - `Windows Clang UBSan`
3. Run `CMake: Configure`.
4. Run `CMake: Build`.

You can also use terminal commands:

```powershell
cmake --preset "Windows ASan"
cmake --build build/windows-asan
```

```powershell
cmake --preset "Windows RelWithDebInfo ASan"
cmake --build build/windows-relwithdebinfo-asan
```

## Why These Flags Were Added

Sanitizer builds enable these flags through `cmake/Sanitizers.cmake` only when `DMT_ENABLE_ASAN` or `DMT_ENABLE_UBSAN` is ON.

- `/fsanitize=address`
  - MSVC AddressSanitizer runtime. Detects use-after-free, heap buffer overflow, and related memory corruption.
- `-fsanitize=address`
  - Clang equivalent of ASan.
- `-fsanitize=undefined`
  - Clang UBSan runtime for undefined behavior checks.
- `-fno-omit-frame-pointer`
  - Preserves frame pointers so crash call stacks remain useful.
- `/Zi`
  - Produces full PDB debug information.
- `/DEBUG`
  - Emits and links debug symbols into final binaries for debugger stack traces.
- `/Oy-`
  - Disables frame-pointer omission on MSVC, improving stack-walk reliability.
- `/INCREMENTAL:NO`
  - Reduces linker/debugger oddities in sanitizer-oriented sessions.

## Attach Visual Studio Debugger to a DAW

1. Start the DAW normally.
2. Open Visual Studio.
3. `Debug -> Attach to Process...`
4. Select your DAW process (for example `Ableton Live`, `Reaper`, `Cubase`, etc).
5. Ensure code type is Native.
6. Trigger plugin load/use scenario until crash reproduces.

Tip: when possible, disable plugin sandboxing or run in-process for better stack visibility.

## Enable First-Chance Exceptions

In Visual Studio:

1. Open `Debug -> Windows -> Exception Settings`.
2. Enable first-chance break for:
   - Access violation (`0xC0000005`)
   - Heap corruption (`0xC0000374`)
   - C++ exceptions (as needed)
3. Reproduce crash. Break at first throw/fault, not at final DAW termination.

## Page Heap (gflags)

Enable full page heap for the DAW executable:

```powershell
gflags /p /enable YourDAW.exe /full
```

Check status:

```powershell
gflags /p
```

Disable when done:

```powershell
gflags /p /disable YourDAW.exe
```

Page Heap often turns silent heap corruption into deterministic early crashes.

## Application Verifier

1. Launch Application Verifier.
2. Add the DAW executable.
3. Enable checks, at minimum:
   - Basics
   - Heaps
   - Handles
   - Locks
4. Re-run the DAW under debugger and reproduce.

Use with ASan for broader coverage across allocator, handle, and lock misuse.

## Interpreting AddressSanitizer Reports

Look for these report sections:

- Error type (`heap-use-after-free`, `heap-buffer-overflow`, etc)
- Faulting address
- Access type (`READ` or `WRITE`)
- Stack at access site
- Stack at allocation site
- Stack at free site

Prioritize fixes by matching:

1. Free site owner
2. Last surviving owner assumptions
3. Cross-thread handoff paths (message thread vs audio thread)

## Limitations and Tradeoffs

- MSVC `cl`:
  - ASan is available in modern toolsets.
  - UBSan is not generally available like Clang UBSan.
- clang-cl:
  - Supports both ASan and UBSan flags used here.
  - Behavior may differ from MSVC ABI/runtime edge cases in DAW/plugin hosting.
- Sanitizers in DAWs:
  - Some host/plugin combinations may be noisy or unsupported.
  - Use a reproducible host/project pair first, then expand matrix.

## Recommended Workflow

1. Start with `Windows ASan` for best compatibility with existing MSVC flow.
2. Reproduce crash in a single DAW with debugger attached and first-chance exceptions enabled.
3. If issue remains unclear, enable Page Heap and Application Verifier.
4. If UB is suspected, try `Windows Clang UBSan`.
5. Keep Release builds unchanged for shipping validation.
