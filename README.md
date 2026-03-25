<a name="readme-top"></a>

<p align="center">
<span  dir="rtl">بسم الله الرحمن الرحيم</span>
</p>
<p align="center">
	<a href='README-AR.md'>العربية</a>
</p>
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/0xzer0x/quran-companion">
    <img src="resources/logo.ico" alt="Logo" width=128>
  </a>
</div>

<h2 align="center">Quran Companion</h2>

<h4 align="center">
  Free and open-source desktop Quran reader and player
</h4>

<p align="center">
  <a href="https://techforpalestine.org/learn-more"><img alt="StandWithPalestine" src="https://raw.githubusercontent.com/Safouene1/support-palestine-banner/master/StandWithPalestine.svg"></a>
  <img alt="GitHub Release" src="https://img.shields.io/github/v/release/0xzer0x/quran-companion">
  <a href="https://aur.archlinux.org/packages/quran-companion"><img alt="AUR Version" src="https://img.shields.io/aur/version/quran-companion"></a>
  <img alt="Flathub Downloads" src="https://img.shields.io/flathub/downloads/io.github._0xzer0x.qurancompanion?label=flathub">
  <img alt="GitHub Downloads" src="https://img.shields.io/github/downloads/0xzer0x/quran-companion/total">
  <img alt="GitHub Actions Workflow Status" src="https://img.shields.io/github/actions/workflow/status/0xzer0x/quran-companion/release.yml">
</p>

<p align="center">
    <a href="#key-features">Key Features</a> •
    <a href="#screenshots">Screenshots</a> •
    <a href="#installation">Download</a> •
    <a href="#credits">Credits</a> •
    <a href="https://github.com/0xzer0x/quran-companion/issues">Report Bug</a> •
    <a href="#license">License</a> 
</p>

<!-- TABLE OF CONTENTS -->

<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About the project</a>
    </li>
    <li>
      <a href="#key-features">Key Features</a>
    <li>
      <a href="#screenshots">Screenshots</a>
    </li>
    </li>
    <li>
    <a href="#installation">Installation</a>
    </li>
    <li><a href="#compilation">Compilation</a>
        <ul>
          <li><a href="#dependencies">Dependencies</a></li>
          <li><a href="#build">Build</a>
            <ul>
            <li><a href="#windows-1">Windows</a></li>
            <li><a href="#linux-1">Linux</a></li>
            </ul>
          </li>
        </ul>
    </li>
    </li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#credits">Credits</a></li>
    <li><a href="#license">License</a></li>
</ol>
</details>

<!-- ABOUT THE PROJECT -->

<a name="about-the-project"></a>

## About the project ✨

Quran Companion is a cross-platform Quran reader/player with recitation download capabilities, verse highlighting, resizable quran font, and a variety of tafsir books & translations

### Key Features

- Modern UI with dark theme support
- Read the Quran from the mushaf page or read the page verses one-by-one
- Resizable Quran page size
- Quran recitation player
- Option for downloading recitations for a variety of reciters
- Verse highlighting as its read
- A collection of translations & tafsir books for the Quran
- Quran verse search functionality

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- SCREENSHOTS -->

### Screenshots

![light.png](screenshots/light.png)

![two-page.png](screenshots/two-pages.png)

![dark.png](screenshots/dark.png)

#### Arabic UI

![ar_light.png](screenshots/ar_light.png)

![ar_dark.png](screenshots/ar_dark.png)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<a name="installation"></a>

## Installation ⚙️

#### Windows

> [!WARNING]
> Starting with version **1.5.0**, Windows releases are distributed as self-contained offline installers instead of the previous online installer. To migrate, completely remove the old installation first, then install the `setup.exe` from the desired release.

Latest release installer can be downloaded from [here][win-installer].

#### MacOS

1. Download and open the [application dmg][macos-dmg-url].
2. Drag the application into your _Applications_.

#### Linux

The application can be downloaded in any of the available packages (snap, flatpak, appimage, aur package).

<p align="left">
 <a href='https://flathub.org/apps/io.github._0xzer0x.qurancompanion'>
    <img width='240' alt='Download Flatpak' src='https://dl.flathub.org/assets/badges/flathub-badge-en.svg'/>
 </a>
 <a href='https://github.com/0xzer0x/quran-companion/releases/download/v1.5.2/Quran_Companion-1.5.2-x86_64.AppImage'>
    <img width='260' alt='Download AppImage' src='https://docs.appimage.org/_images/download-appimage-banner.svg' style='border-radius: 10px'/>
 </a>
 <a href="https://snapcraft.io/quran-companion">
  <img width='250' alt="Get it from the Snap Store" src="https://snapcraft.io/static/images/badges/en/snap-store-black.svg" />
 </a>
</p>

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<a name="compilation"></a>

## Compilation 🛠️

To build the application in your system, you'll need to install the dependencies needed for it and package names might be different for each distribution, see Dependencies below for more information. You can also install most of the Qt dependencies via their [installer](https://www.qt.io/download-qt-installer). If you were developing Qt apps before, you probably already have them.

This project uses [CMake](https://cmake.org/) build system, so you need to install it in order to build the project (on most Linux distributions it is available in the standard repositories as a package called cmake).

Also you can open and build/debug the project in a C++ IDE. For example, in Qt Creator you should be able to simply open `CMakeLists.txt` via `Open File or Project` in the menu after installing CMake into your system. [More information about CMake projects in Qt Creator](https://doc.qt.io/qtcreator/creator-project-cmake.html).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Dependencies

- GCC / MSVC
- CMake
- Qt >= 6.10
  - `qtbase`
  - `qtimageformats`
  - `qtsvg`
  - `qtmultimedia`
  - `qttools`

### Build

1. Clone the repo, make sure to pass `--depth=1` to reduce the download size

```sh
git clone --recursive --depth=1 -b main https://github.com/0xzer0x/quran-companion.git
```

#### Windows

2. Install Qt using the [online installer](https://www.qt.io/download-qt-installer), make sure to select Qt Multimedia & Qt image formats

3. Add the Qt bin directory to the system **PATH** variable, usually `C:\Qt\6.x.x\msvc_2019\bin`

4. Build (replace 6.x.x with the Qt version you installed)

```sh
mkdir build
cd build
cmake.exe -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc_2019" -DCMAKE_BUILD_TYPE=Release ..
cmake.exe --build . --config Release
```

#### Linux

2. Install dependencies (through the Qt [online installer](https://www.qt.io/download-qt-installer) or using your distro's package manager)

- **Ubuntu**

```sh
sudo apt install build-essential cmake qt6-tools-dev qt6-base-dev qt6-image-formats-plugins qt6-multimedia-dev qt6-l10n-tools qt6-translations-l10n ffmpeg
```

- **Arch linux**

```sh
sudo pacman -Sy base-devel cmake qt6-base qt6-multimedia qt6-tools qt6-imageformats qt6-translations ffmpeg
```

3. Build

**Note:** if you installed Qt through the online installer, you need to pass `-DCMAKE_PREFIX_PATH=~/Qt/6.x.x/gcc_64` to CMake in order for the linker to find the Qt libs

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
# To launch quran-companion
./quran-companion
```

4. Install (optional)

```sh
sudo cmake --install .
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ROADMAP -->

<a name="roadmap"></a>

## Roadmap 📊

See the [project board](https://github.com/users/0xzer0x/projects/1) for the current roadmap and development progress.

You can also review [open issues](https://github.com/0xzer0x/quran-companion/issues) for planned features and reported problems.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->

<a name="contributing"></a>

## Contributing 💡

Please check out the [CONTRIBUTING.md](CONTRIBUTING.md) file for guidelines on how to contribute, submit issues, or create pull requests.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CREDITS -->

<a name="credits"></a>

## Credits 📌

The following projects/services were used in development:

- [Ayat](https://quran.ksu.edu.sa/index.php)
- [Quran.com API](https://api-docs.quran.com/)
- [Every Ayah recitations](https://everyayah.com/recitations_ayat.html)
- [Mosshaf](https://mosshaf.com/)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

<a name="license"></a>

## License

Distributed under the [Waqf General Public License](https://ojuba.org/waqf-2.0:%D8%B1%D8%AE%D8%B5%D8%A9_%D9%88%D9%82%D9%81_%D8%A7%D9%84%D8%B9%D8%A7%D9%85%D8%A9). See `license-en.txt` for the English version.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->

[latest-release]: https://github.com/0xzer0x/quran-companion/releases/latest
[win-installer]: https://github.com/0xzer0x/quran-companion/releases/download/v1.5.2/Quran_Companion-1.5.2-x86_64-setup.exe
[translation-wiki]: https://github.com/0xzer0x/quran-companion/wiki/Contributing-Translations
[recitations-wiki]: https://github.com/0xzer0x/quran-companion/wiki/Contributing-Recitations
[macos-dmg-url]: https://github.com/0xzer0x/quran-companion/releases/download/v1.5.2/Quran_Companion-1.5.2-x86_64.dmg
