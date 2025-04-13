Generate semantic Git commit messages in the format `type(scope): message`.  
Use the following **types**:
- `feat`: New feature or major enhancement  
- `fix`: Bug fix or behavior correction 
- `mod`: Modifies existing behavior without adding new features (for non-bug changes that alter functionality)
- `refactor`: Code change that improves structure without changing behavior  
- `style`: Code formatting, naming, or whitespace-only changes  
- `docs`: Changes to documentation or comments  
- `test`: Adding or updating tests  
- `chore`: Maintenance, build system, versioning, or tooling updates  
- `perf`: Performance improvements  
- `revert`: Reverts a previous commit  
- `release`: Release-related changes, such as version bumps or changelog updates  
- `security`: Security-related fixes or updates  
- `ui`: UI-related changes or improvements (e.g., layout, buttons, visuals)  
- `behavior`: Changes that modify the behavior of an existing feature  

Use a relevant **scope** based on the folder or subsystem involved:
- `app`: General things for the app
- `dsp`: General DSP layer (`dsp/`)
- `gui`: General GUI layer (`gui/`)
- `model`: Parameter model and layout (`model/`)
- `theme`: Visual styling and theming (`theme/`)
- `configuration`: Option/config setup and loading (`configuration/`)
- `utility`: General-purpose helpers, math, and tools (`utility/`)
- `version`: Versioning and networking (`version/`)
- `test`: Test scripts or test data (`test/`)
- `build`: CMake or build system changes (`CMakeLists.txt`, etc.)
- `ci`: Continuous integration configuration (`.github/`, `.gitlab-ci.yml`)
- `security`: Vulnerability fixes or security updates
- `localization`: Localization updates or translations