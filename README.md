<a name="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/0xzer0x/quran-companion">
    <img src="images/app-icon.ico" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Quran Companion</h3>

  <p align="center">
    cross-platform Qt-based Quran reader/listener
    <br />
    <br />
    <a href="https://github.com/0xzer0x/quran-companion/releases/latest"><img src="https://img.shields.io/github/v/release/0xzer0x/quran-companion"></a> <img src="https://img.shields.io/github/v/tag/0xzer0x/quran-companion">
    <br />
    <a href="https://github.com/0xzer0x/quran-companion#usage">View Demo</a>
    ·
    <a href="https://github.com/0xzer0x/quran-companion/issues">Report Bug</a>
    ·
    <a href="https://github.com/0xzer0x/quran-companion/issues">Request Feature</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About the project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#features">Features</a>
    </li>
    <li>
    <a href="#installation">Installation</a>
    </li>
    <li><a href="#compilation">Compilation</a>
        <ul>
          <li><a href="#dependencies">Dependencies</a></li>
          <li><a href="#build">Build</a>
            <ul>
            <li><a href="#windows">Windows</a></li>
            <li><a href="#linux">Linux</a></li>
            </ul>
          </li>
        </ul>
    </li>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>

  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## About the project

Quran Companion is a cross-platform Quran reader/listener with recitation download capabilities, verse highlighting, resizable quran font, and a variety of tafsir books & translations

### Features

- Modern UI with dark theme support
- Read the Quran from the mushaf page or read the page verses one-by-one
- Resizable Quran page size
- Quran recitation player
- Option for downloading recitations for a variety of reciters
- Verse highlighting as its read
- A collection of translations & tafsir books for the Quran
- Quran verse search functionality

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Built With

- [![qt-badge][qt-badge]][qt-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Installation

Currently, only windows setup packages are provided in the [release page][latest-release].

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Compilation

To build the application in your system, you'll need to install the dependencies needed for it and package names might be different for each distribution, see Dependencies below for more information. You can also install most of the Qt dependencies via their [installer](https://www.qt.io/download-qt-installer). If you were developing Qt apps before, you probably already have them.

This project uses [CMake](https://cmake.org/) build system, so you need to install it in order to build the project (on most Linux distributions it is available in the standard repositories as a package called cmake).

Also you can open and build/debug the project in a C++ IDE. For example, in Qt Creator you should be able to simply open `CMakeLists.txt` via `Open File or Project` in the menu after installing CMake into your system. More information about CMake projects in Qt Creator.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Dependencies

- GCC

- Qt >= 6.4

  - Qt Multimedia

- CMake >= 3.22

- Openssl1.1.1

- Inno setup for windows (Optional)

### Build

1. Clone the repo, make sure to pass `--depth=1` to reduce the download time

```sh
  git clone --depth=1 -b main https://github.com/0xzer0x/quran-companion.git
```

#### Windows

2. Install Qt using the [online installer](https://www.qt.io/download-qt-installer), make sure to select Qt Multimedia & Qt image formats

3. Add the Qt bin directory to the system **PATH** variable, usually `C:\Qt\6.X.X\mingw_64\bin`

4. Navigate to the _dist/windows_ dir & run the `build-setup.bat` script. This should create a _Bundle_ folder that contains the full application. If you installed Inno setup, a setup file will also be created.

#### Linux

For Ubuntu, a script is provided in _dist/linux_ to preform the following steps. navigate to that directory & run the script, the final application is at the created _bundle_ directory.

2. Install dependencies

```sh
  sudo apt install build-essential cmake qt6-tools-dev qt6-base-dev qt6-image-formats-plugins qt6-multimedia-dev qt6-l10n-tools qt6-translations-l10n
```

3. Build

```sh
  cd dist/linux
  mkdir build
  cd build
  cmake ../../../
  make
```

4. Copy final application files to bundle directory

```sh
  cd ..
  mkdir bundle
  cp -r build/quran-companion bundle/
  cp -r build/assets bundle/
  cp -r build/translations bundle/
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>
<!-- USAGE EXAMPLES -->

## Usage

TO-BE-FILLED

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ROADMAP -->

## Roadmap

- [ ] Add bookmarked verses tab
- [ ] Add daily verse functionality
- [ ] Add more tafsir books

See the [open issues](https://github.com/0xzer0x/quran-companion/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

## License

Distributed under the LGPL License. See `license.txt` for more information.

App icon by InkscapeBoy on [openclipart.org](https://openclipart.org/detail/221028/muslim-icon-quran)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

0xzer0x - y.essam2256@nu.edu.eg

Project Link: [https://github.com/0xzer0x/quran-companion](https://github.com/0xzer0x/quran-companion)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->

## Acknowledgments

- [Ayat](https://quran.ksu.edu.sa/index.php)
- [Quran.com API](https://quran.api-docs.io/)
- [Every Ayah recitations](https://everyayah.com/recitations_ayat.html)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->

[latest-release]: https://github.com/0xzer0x/quran-companion/releases/latest
[qt-badge]: https://shields.io/badge/Qt-Framework-green?logo=qt&style=for-the-badge
[qt-url]: https://qt.io
