Name:           dimethoxy-disflux-snapshot
Version:        %{git_hash}
Release:        1%{?dist}
Summary:        Unstable build of the latest commit of Disflux from Dimethoxy's GitHub repository
License:        GPL-3.0-or-later
URL:            https://github.com/Dimethoxy/Disflux
Source0:        %{name}-%{git_hash}.tar.gz
Conflicts:      dimethoxy-disflux
Requires:       curl

%description
Unstable build of the latest commit of Disflux from Dimethoxy's GitHub repository.

%prep
# Extract the tarball containing the pre-built artifacts
%setup -q

%install
# Create the base installation directories
install -d %{buildroot}/usr/lib/vst3
install -d %{buildroot}/usr/lib/lv2
install -d %{buildroot}/usr/lib/clap
install -d %{buildroot}/usr/share/licenses/%{name}

# Copy the plugin format directories directly from the extracted source directory
cp -r disflux-snapshot-linux-ubuntu/VST3/* %{buildroot}/usr/lib/vst3/
cp -r disflux-snapshot-linux-ubuntu/LV2/* %{buildroot}/usr/lib/lv2/
cp -r disflux-snapshot-linux-ubuntu/CLAP/* %{buildroot}/usr/lib/clap/

# Install the license file
if [ -f "LICENSE" ]; then
    install -Dm644 LICENSE %{buildroot}/usr/share/licenses/%{name}/LICENSE
else
    curl -L "https://raw.githubusercontent.com/Dimethoxy/Disflux/refs/heads/main/LICENSE" -o %{buildroot}/usr/share/licenses/%{name}/LICENSE
fi

%files
/usr/lib/vst3/Disflux
/usr/lib/lv2/Disflux
/usr/lib/clap/Disflux
/usr/share/licenses/%{name}/LICENSE