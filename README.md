# Disflux

> [!WARNING]
> Disflux is currently a work in progress. While it's functional, there are unfinished features, and some buttons or functionalities may not work as expected.
> Expect updates and improvements as we continue development!

Disflux is a wild audio effect plugin that takes your transients and smears them through time, adding serious punch and weight. If you're creating Hard Techno or Rawstyle and need your kicks to hit like a freaking train, Disflux is your new best friend. We're talking aggressive, distorted, gated kicks that smack like no tomorrow.

Using a chain of all-pass filters, Disflux spreads those high-energy transients through time, giving you that laser-like effect. Think of it as a cross between a short reverb and a falling sine wave. It injects depth and impact into your punch. Outside of kicks, it’s also perfect for crafting experimental soundscapes and other weird, next-level audio magic.

<div align="center">
  <img src="https://raw.githubusercontent.com/Dimethoxy/Disflux/refs/heads/main/PREVIEW.webp" alt="Image of the GUI" style="width: 50%;">
</div>


## 🔥 Features

- **Free & Open-Source** – No paywalls, no restrictions.
- **All-Pass Filter Chain** – Smear your transients through time with a series of all-pass filters.
- **Minimalist UI** – Minimalist vector interface.
- **Scalable UI** – Perfect for high-res screens.
- **Theming System** - A powerful theme system that lets you style the plugin your way.
- **Intuitive Workflow** – Get to your sound quickly with easy-to-navigate controls.
- **Low CPU impact** – Crush your sounds, not your processor.
- **Cross-platform** – Supports Windows, MacOS, and Linux.
- **Multi-Format Support** – Works with VST3, CLAP, LV2, and AU formats.
- **Regular Updates** – Continuous improvements and new features.
- **Privacy First** – We take your privacy seriously, no tracking or data collection.

## 🚧 Work in Progress

Disflux is currently unfinished and we're actively working on developing the following features before a first release:
   
- **Preset Menu** – Add functionality to select and save presets.
- **Settings & Customization** – The settings panel needs sanity checks. Right now, you can input cursed values that break everything.
- **Performance** – While performance is already solid, we’re working to optimize it even further.
- **Parameter Smoothing** – Aiming to reduce artifacts when automating parameters for seamless transitions.
- **Oversampling** – Implementing oversampling to minimize aliasing and improve the quality of high-frequency content.
- **Package Managers** – Integrating with Linux package managers like the AUR on Archlinux for easy installation and updates.


## 💖 Donations

If you have enjoyed using our software, please consider donating.
[Donate via PayPal](https://www.paypal.com/donate/?hosted_button_id=8SJXCUYV5ZHKG)

We are dedicated to providing high-quality audio tools to the community for free, and your support helps us to continue our work. Your donation will contribute to maintaining and improving our software, as well as supporting the development of new tools and features. We greatly appreciate your support and thank you for helping us to continue our work.

## 📜 License

The source code is licensed under the **GPLv3**. If you download the source or create builds, you must comply with that license. For more information, visit [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

## 🔐 Privacy

**Disflux** is built with privacy in mind. It **does not collect any personal data** or send any telemetry. We are committed to **never sharing or selling your data**. However, some features that require networking may affect your privacy. We want to be transparent about these and explain how they work.

### Update Checking

On certain systems, **Disflux** may attempt to connect to the internet to check for updates. This process is entirely anonymous, and **no personal information is sent** during the check.

### Disabling Update Notifications

If you prefer to disable update notifications, you can compile **Disflux** without these checks by using the following CMake flag:

```bash
-DDMT_DISABLE_UPDATE_NOTIFICATION=ON
```

Some pre-built versions, like those available from the **Arch User Repository (AUR)**, may already have this option enabled by default. This is because these builds are managed by package managers that handle updates on their own, making the update check unnecessary.

### Third-Party Tools and Telemetry

We use third-party libraries, such as [JUCE](https://github.com/juce-framework/JUCE), to help with plugin development. While we are not aware of any telemetry or data collection within these tools, we recommend you do your own research to ensure you are comfortable with their data practices.

If we discover any issues or unexpected data collection, we will address them immediately and update you accordingly.

## ⚠️ Warranty

You and your end-users use the software at your own risk.

**THE SOFTWARE IS PROVIDED “AS IS,” WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.**
