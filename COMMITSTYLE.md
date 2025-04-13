Generate semantic Git commit messages in the format `type(scope): message`.  
Use the following **types**:
- `feat`: New feature or major enhancement  
- `fix`: Bug fix or behavior correction  
- `refactor`: Code change that improves structure without changing behavior  
- `style`: Code formatting, naming, or whitespace-only changes  
- `docs`: Changes to documentation or comments  
- `test`: Adding or updating tests  
- `chore`: Maintenance, build system, versioning, or tooling updates  
- `perf`: Performance improvements  
- `revert`: Reverts a previous commit
Use a relevant **scope** based on the folder or subsystem involved. Suggested scopes include:
- `dsp`: General DSP layer
- `dsp.effect`: Effect processors (e.g., distortion, lowpass)
- `dsp.synth`: Synth and oscillator code
- `dsp.envelope`: Envelopes and modulation sources
- `gui`: General GUI layer
- `gui.component`: Individual display or slider components
- `gui.panel`: UI panels (OscillatorPanel, PitchPanel, etc.)
- `gui.preset`: Preset management system
- `gui.widget`: Shared UI widgets (sliders, buttons, etc.)
- `gui.window`: Plugin window and popover infrastructure
- `model`: Parameter model and layout
- `theme`: Visual styling and theming
- `configuration`: Option/config setup and loading
- `utility`: General-purpose helpers, math, and tools
- `version`: Versioning and networking
- `test`: Test scripts or test data
- `build`: CMake or build system changes
The **message** should be short, imperative (e.g., "add", "fix", "remove", "refactor"), and focused on the what/why of the change, t the how.
Example commits:
- `feat(dsp.effect): add new asymmetric waveshaper to Distortion`
- `fix(gui.panel): fix layout bug in OscillatorPanel on macOS`
- `refactor(model): simplify ParameterLayout construction`
- `style(utility): reformat Math.h to match code style guide`