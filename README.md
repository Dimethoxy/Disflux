# Disflux

> [!WARNING]  
> Because of a bug Disflux is currently crashing constantly on certain Windows systems. \
> We are very sorry about this and are working hard on fixing it. \
> There will be an update fixing this soon!

Disflux is a wild audio effect plugin that takes your transients and smears them through time, adding serious punch and weight. If you're creating Hard Techno or Rawstyle and need your kicks to hit like a freaking train, Disflux is your new best friend. We're talking aggressive, distorted, gated kicks that smack like no tomorrow.

Using a chain of all-pass filters, Disflux spreads those high-energy transients through time, giving you that laser-like effect. Think of it as a cross between a short reverb and a falling sine wave. It injects depth and impact into your punch. Outside of kicks, it’s also perfect for crafting experimental soundscapes and other weird, next-level audio magic.

If it feels familiar, that's no accident. Disflux is a lovingly crafted take on an iconic effect, reimagined with a modern touch. We won’t name the original for trademark reasons, but you probably know it.

<div align="center">
  <img src="https://raw.githubusercontent.com/Dimethoxy/Disflux/refs/heads/main/resources/preview.webp" alt="Image of the GUI" style="width: 60%;">
</div>


## 🔥 Features

- **Free & Open-Source** – No paywalls, no restrictions.
- **All-Pass Filter Chain** – Smear your transients through time with a series of all-pass filters.
- **Minimalist UI** – Minimalist vector interface.
- **Scalable UI** – Perfect for high-res screens.
- **Theming System** - A powerful theme system that lets you style the plugin your way.
- **Intuitive Workflow** – Get to your sound quickly with easy-to-navigate controls.
- **Cross-platform** – Supports Windows, MacOS, and Linux.
- **Multi-Format Support** – Works with VST3, CLAP, LV2, and AU formats.
- **Regular Updates** – Continuous improvements and new features.
- **Privacy First** – We take your privacy seriously, no tracking or data collection.

## 🚧 Coming Soon

Here are some of the exciting things you can expect in future updates:
   
- **Bug Fixes** – We are actively working on fixing bugs and improving stability.
- **Preset Menu** – Add functionality to select and save presets.
- **Performance** – While performance is already solid, we’re working to optimize it even further.
- **Oversampling** – Implementing oversampling to minimize aliasing and improve the quality of high-frequency content.
- **Themes** - We already allow heavy theming, but we want to make it easier to export and share themes with the community.
- **Mobile Support** – We are planning to release Disflux for iOS and Android in the future.
- **Fedora Support** – We are working on a Fedora package for users who prefer this distribution.

## 💾 Downloads

You can download our pre-built binaries for Windows, MacOS and Linux [here](https://github.com/Dimethoxy/Disflux/releases).

## 💖 Donations

If you have enjoyed using our software, please consider donating.
[Donate via PayPal](https://www.paypal.com/donate/?hosted_button_id=8SJXCUYV5ZHKG)

We are dedicated to providing high-quality audio tools to the community for free, and your support helps us to continue our work. Your donation will contribute to maintaining and improving our software, as well as supporting the development of new tools and features. We greatly appreciate your support and thank you for helping us to continue our work.

## 🛠️ Compiling

If you want to compile Plasma from source yourself, follow these steps:

### 1. Prerequisites

**Ubuntu**
- Install build tools: `sudo apt-get install build-essential cmake ninja-build`
- Install dependencies: `sudo apt install libasound2-dev libjack-jackd2-dev ladspa-sdk libcurl4-openssl-dev libfreetype-dev libfontconfig1-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev libwebkit2gtk-4.1-dev libglu1-mesa-dev mesa-common-dev curl`

**MacOS** 
1. Install [Homebrew](https://brew.sh/)
2. Install build tools: `brew install ninja osxutils`

**Windows**
1. Install [Git](https://git-scm.com/downloads)
2. Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/).
    - During installation, make sure to select package "Desktop development with C++"  
4. Install [Chocolatey](https://chocolatey.org/install) (open **PowerShell** as Admin and follow instructions on their site)
5. Use **Chocolatey** to install required tools: `choco install cmake ninja llvm`
  
### 2. Clone the Repository
```bash
git clone https://github.com/yourusername/Disflux.git
cd Disflux
```

### 3. Configure the Build with CMake

Run CMake to configure the project. Use the appropriate preset for your platform:

**Ubuntu**
```bash
cmake --preset "Linux Release"
```

**MacOS**
```bash
cmake --preset "Mac Release" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
```

**Windows**
```bash
cmake --preset "Windows Release"
```

### 4. Build the project

After configuring with CMake, build the project using the following command:
```bash
cmake --build build --config "Release"
```

### 5. Locate the Build Artifacts

After the build process completes, you can find the compiled artifacts in the build directory under the following paths:
- **VST3:** `build/src/DisfluxPlugin_artefacts/Release/VST3/Disflux.vst3`
- **CLAP:** `build/src/DisfluxPlugin_artefacts/Release/CLAP/Disflux.clap`
- **LV2:** `build/src/DisfluxPlugin_artefacts/Release/LV2/Disflux.lv2`
- **AU:** `build/src/DisfluxPlugin_artefacts/Release/AU/Disflux.component`

You can move these to your plugin folder.


## 🔐 Privacy

**Disflux** is built with privacy in mind. It **does not collect any personal data** or send any telemetry. We are committed to **never sharing or selling your data**. It makes us sad that in today's day and age, we consider this to be a standout point, but here we are.

### Update Checking

On some systems, **Disflux** includes a lightweight and anonymous update notification system. It checks for new versions by sending a simple GET request to our server to retrieve the latest version string. The system then compares the retrieved version with the current version locally. No personal data is sent or stored during this process.

### Disabling Update Notifications

If you'd rather disable the update notification system, you can compile **Disflux** without it by adding the following CMake flag:

```bash
-DDMT_DISABLE_UPDATE_NOTIFICATION=ON
```

Some pre-built versions, like those available from the **Arch User Repository (AUR)**, may already have this option enabled by default. This is because these builds are managed by package managers that handle updates on their own, making the update check unnecessary.

### Third-Party Tools and Telemetry

We use third-party libraries, such as [JUCE](https://github.com/juce-framework/JUCE), to help with plugin development. While we are not aware of any telemetry or data collection within these tools, we recommend you do your own research to ensure you are comfortable with their data practices.

If we discover any issues or unexpected data collection, we will address them immediately and update you accordingly.

## 📜 License

The source code is licensed under the **GPLv3**. If you download the source or create builds, you must comply with that license. For more information, visit [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

## ⚠️ Warranty

You and your end-users use the software at your own risk.

**THE SOFTWARE IS PROVIDED “AS IS,” WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.**
