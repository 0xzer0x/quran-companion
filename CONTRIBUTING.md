# Contributing to Quran Companion

Thank you for taking the time to contribute to **Quran Companion**.

This document explains how to:

- Report bugs / request features
- Set up a development environment and build the app
- Make and submit changes (pull requests)
- Contribute recitations
- Contribute Quran content (tafsir & translation)

## Table of contents

- [Ways to contribute](#ways-to-contribute)
- [Before you start](#before-you-start)
- [Reporting bugs](#reporting-bugs)
- [Requesting features](#requesting-features)
- [Development setup](#development-setup)
  - [Dependencies](#dependencies)
  - [Clone](#clone)
  - [Build (Windows)](#build-windows)
  - [Build (Linux)](#build-linux)
- [Project structure](#project-structure)
- [Pull request process](#pull-request-process)
- [Internationalization](#internationalization)
- [Recitations](#recitations)
- [Quran Content](#quran-content)
- [Documentation](#documentation)
- [License](#license)

## Ways to contribute

You can contribute in many ways:

- **Bug fixes** (UI bugs, playback, downloads, search, rendering, etc.)
- **Features** (new UI/UX improvements, accessibility, performance)
- **Packaging / distribution** (AppImage, Flatpak, Snap, AUR, macOS dmg)
- **Internationalization** (UI translations)
- **Recitations** (metadata, download sources, organization)
- **Quran content** (tafsir, translation)
- **Docs** (README, wiki pages, Doxygen, screenshots)

If you’re unsure what to work on, check the issue tracker:

- Issues: <https://github.com/0xzer0x/quran-companion/issues>

## Before you start

### Discuss first for large changes

For bigger changes (new subsystems, significant UI redesign, refactors), please open an issue first so we can align on approach and avoid duplicated effort.

### Keep PRs focused

Small, focused pull requests are easier to review and merge.

## Reporting bugs

Please use the **Bug Report** issue template:

- <https://github.com/0xzer0x/quran-companion/issues/new/choose>

Include:

- App version (see **About Quran Companion**)
- Operating system + version
- Steps to reproduce
- Screenshots if applicable
- Logs if available

**Log locations:**

- Linux: `~/.config/QuranCompanion/qc.log`
- Windows: `C:\Users\<your-username>\Appdata\Local\QuranCompanion\qc.log`

Tip: attach the log **before launching a new app instance**, since logs may reset on startup.

## Requesting features

Please use the **Feature Request** issue template:

- <https://github.com/0xzer0x/quran-companion/issues/new/choose>

The template asks you to categorize the request (Book / Recitation / Application). If possible, include:

- Mockups / screenshots
- Any relevant references or sources

## Development setup

The README contains the authoritative build instructions. This section summarizes the workflow and points back to the README.

### Devenv (recommended)

[Devenv](https://devenv.sh) is the recommended approach for setting up a consistent development environment.

If you have Devenv installed, use it to enter a ready-to-use dev shell and run the project’s build tasks from there.

- For building: `task build`
- For running: `task run`

### Manual Environment

#### Dependencies

You will generally need:

- C++ compiler (GCC / MSVC)
- CMake
- Qt >= 6.10 with modules such as:
  - `qtbase`
  - `qtimageformats`
  - `qtsvg`
  - `qtmultimedia`
  - `qttools`

### Clone

```sh
git clone --recursive --depth=1 -b main https://github.com/0xzer0x/quran-companion.git
```

### Build (Windows)

See README for the most accurate commands and required Qt components.

Typical flow:

```sh
mkdir build
cd build
cmake.exe -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc_2019" -DCMAKE_BUILD_TYPE=Release ..
cmake.exe --build . --config Release
```

### Build (Linux)

See README for distro-specific packages.

Typical flow:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./quran-companion
```

## Project structure

```
.
├── assets/                           Runtime-required files/fonts/data required for the app to run.
│   ├── fonts/                        Fonts used by the application at runtime (UI / Quran page font / Side content font).
│   ├── tafasir/                      Tafsir databases/content shipped with the app.
│   ├── translations/                 Translation databases/content shipped with the app.
│   ├── betaqat.db                    Surah cards content SQLite DB (runtime dependency).
│   ├── glyphs.db                     QCF fonts glyphs SQLite DB (runtime dependency).
│   └── quran.db                      Quran text used by the application (runtime dependency).
├── bismillah/                        Basmallah MP3 per supported reciter (startup/transition audio).
├── dist/                             Distribution + packaging related files (not core app code).
│   ├── aur/                          Arch Linux AUR packaging files (PKGBUILD, install scripts, etc.).
│   ├── flatpak/                      Flatpak packaging manifest + related files.
│   ├── translations/                 I18n tooling + generated translation artifacts.
│   │   ├── out/                      Auto-generated translation outputs.
│   │   ├── qtbase/                   Qt base translation files / upstream Qt translations used for bundling.
│   │   ├── generate-template         Bash script/tool to generate base translation template.ts file.
│   │   └── template.ts               Qt translation template file for i18n integration with Crowdin.
│   ├── windows/                      Windows distribution/installer related files.
│   └── xdg/                          Linux desktop integration (XDG desktop files, icons, metainfo, etc.).
├── docs/                             Documentation tooling (Doxygen config lives here).
├── resources/                        Bundled resources compiled into the binary via Qt resource system (.qrc).
│   ├── dark/                         Dark theme assets (images/stylesheets).
│   ├── light/                        Light/Sepia theme assets (images/stylesheets).
│   ├── files.xml                     List of all supported Quran content (tafsir, translations).
│   ├── reciters.xml                  Reciters metadata used by player/download logic.
│   └── shortcuts.xml                 Keyboard shortcuts definitions shown/used by the app.
├── screenshots/                      README images only (documentation).
├── snap/                             Snapcraft packaging directory (Snap-specific only).
├── src/                              Application source code.
│   ├── components/                   Main UI components.
│   ├── dialogs/                      UI dialogs (settings/about/download dialogs/etc.).
│   ├── downloader/                   Downloading subsystem (recitations/content downloads).
│   ├── navigation/                   Navigation/routing between pages/views.
│   ├── notifiers/                    Notification/toast/system notifications.
│   ├── player/                       Audio playback logic (recitation playback, queueing, etc.).
│   ├── repository/                   Data-access layer: reading from SQLite/asset data sources, caching, etc.
│   ├── serializer/                   Serialization/deserialization utilities (XML/JSON/settings/DB mapping).
│   ├── service/                      Abstraction layer providing access to different SQLite databases.
│   ├── types/                        Shared structs/enums/DTOs used across modules.
│   ├── utils/                        Shared utility helpers.
│   ├── widgets/                      Custom Qt UI widgets.
│   └── main.cpp                      Application entry point.
├── third_party/                      Vendored/externals.
│   └── QtAwesome                     Third-party dependency (icons / font-awesome integration for Qt).
├── CHANGELOG.md                      Release notes / user-visible change history.
├── CMakeLists.txt                    CMake build configuration (top-level project config).
├── CONTRIBUTING.md                   Contribution guidelines (how to build, PRs, content contribution, etc.).
├── README-AR.md                      Arabic README.
├── README.md                         Primary README (English).
├── Taskfile.yml                      Task runner definitions (go-task) for common dev commands.
├── VERSION                           Current application version used by release automation.
├── crowdin.yml                       Crowdin configuration for translation management.
├── devenv.lock                       Devenv lock file (pins nix inputs/versions for reproducible env).
├── devenv.nix                        Devenv environment definition (Qt6, cmake/ninja, formatting, etc.).
├── devenv.yaml                       Devenv config file (entrypoint/settings for devenv).
├── license-ar.txt                    License (Arabic, authoritative per README).
├── license-en.txt                    License (English translation).
├── qurancompanion.rc                 Windows resource file (icon/version metadata for Windows builds).
└── resources.qrc                     Qt resource collection file defining what gets bundled from resources/.
```

## Pull request process

1. **Fork** the repo and create a branch: `git checkout -b your-branch-name`
2. **Make changes** with a clear scope.
3. **Build locally** and verify your change works on your platform.
4. **Write a good PR description**:
   - what changed and why
   - screenshots for UI changes
   - link the issue it fixes (e.g. “Fixes #123”)

### Commit messages

This project uses [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).

#### Format

```text
<type>(optional scope): <description>

(optional body)

(optional footer(s))
```

#### Common types

- `fix`: bug fix
- `feat`: new feature
- `refactor`: refactor that doesn’t change behavior
- `perf`: performance improvement
- `docs`: documentation only changes
- `style`: formatting only (no logic changes)
- `test`: adding/updating tests
- `build`: build system or external dependencies (CMake, packaging, etc.)
- `ci`: CI workflow changes
- `chore`: maintenance tasks (non-production code changes)

#### Examples

- `fix(player): prevent crash when switching reciters`
- `feat(search): highlight matches in results list`
- `refactor(downloader): simplify queue scheduling`
- `docs(contributing): document SQLite content format`
- `ci(release): update AppImage build step`

#### Breaking changes

If a change is breaking, indicate it by:

- adding `!` after the type/scope, e.g. `feat(api)!: ...`, and/or
- adding a footer like:

```text
BREAKING CHANGE: describe what changed and how to migrate
```

#### Referencing issues

If your commit fixes an issue, you can reference it in the footer:

```text
Refs: #123
```

## Internationalization

Internationalization is done through the localization platform [Crowdin](https://crowdin.com/), which makes it easier for people who aren't familiar with developer tools such as git and GitHub to contribute translations.

### Translation process 🌐

1. Create an account on [Crowdin](https://crowdin.com)
2. Join the **Quran Companion** Crowdin project as a translator using the [invite link](https://crowdin.com/project/quran-companion/invite?h=86a2925c5cda4f57e389f2c2651db9011947466)
3. If you’re new to Crowdin, review the [online editor documentation](https://support.crowdin.com/online-editor/)
4. Enable notifications for new translatable strings:
   - Crowdin → **Account Settings** → **Notifications** → enable **New Strings**
5. Once a language translation is completed and approved, it will be included in a release following its approval.

## Recitations

You can contribute new recitations by converting a reciter’s **full-surah** audio into **verse-by-verse MP3 files** and submitting them to the audio repository.

### Verse MP3 naming convention

Each verse must be exported as an MP3 file named in the format:

- `sssvvv.mp3`

Where:

- `sss` = **0-padded surah number** (001–114)
- `vvv` = **0-padded verse number** within the surah

Example:

- `002005.mp3` = Surah 2, Ayah 5

### Recommended tool: Audacity

I suggest using **Audacity** because it supports **labels**, which makes it easy to export verse segments.

Example workflow (Surah Al-Fatihah):

1. Download the surah recitation and open it in Audacity.

2. Select the verse segment.

3. Add a label for the selected segment:
   - Menu: `Edit -> Labels -> Add Label at Selection`
   - Shortcut: <kbd>Ctrl</kbd> + <kbd>B</kbd>
   - Name the label using the naming convention (e.g. `001005`)

4. Repeat the selection + labeling for the rest of the surah.

5. (Optional) Export the labels (useful if you want to tweak timings or re-export later):
   - `File -> Export -> Export Labels...`
   - This saves a text file.

6. Export verse MP3 files:
   - `File -> Export -> Export Multiple...`
   - Choose an output folder (recommended: a separate folder per surah that contains the MP3s and the labels text file)
   - Use these options:
     - **Format:** `MP3 Files`
     - **Bit Rate Mode:** `Average`
     - **Quality:** `64kbps`
     - **Channel Mode:** `Joint Stereo`
     - **Split files based on:** `Labels` (don’t include audio before first label)
     - **Name files:** `Using Label/Track Name`

### Submitting your recitation (GitLab)

After exporting all **114 surahs** into verse MP3 files:

1. Create a [GitLab](https://gitlab.com) account
2. Fork the `qc-audio` repository:
   - <https://gitlab.com/0xzer0x/qc-audio>
   - You may need to install `git-lfs`.
3. Clone your fork locally and add the recitation files into a **single directory** named using the convention:
   - `name_bitrate`
4. Open a GitLab [merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html) to merge your fork into the main repository

## Quran Content

> [!WARNING]
> Please make sure you have the right to distribute the content you contribute, and include source/attribution information in the PR/issue description.

You can contribute **Quran content** such as **translations** and **tafasir (interpretation)** in the form of SQLite `.db` files. Quran content databases should be contributed to the [quran-companion-extras repository](https://github.com/0xzer0x/quran-companion-extras), not the main application repository.

Create a pull request in the extras repo that adds your `.db` file in the appropriate directory (translations or tafasir), and include details about:

- Content type (translation / tafsir)
- Language
- Author/source + license/permission

### SQLite database format

- File type: **SQLite**
- File extension: `.db`

### Required schema

Your database must contain a table **or** view named `content` with the following columns:

```sql
CREATE TABLE IF NOT EXISTS "content" (
  "id"   int(4) NOT NULL,
  "sura" int(3) NOT NULL,
  "aya"  int(3) NOT NULL,
  "text" text   NOT NULL,
  PRIMARY KEY("id")
);
```

## Documentation

### Doxygen

Doxygen docs can be generated from `docs/`:

```sh
cd docs
doxygen
```

By default, output is HTML (you can change `Doxyfile` to generate other formats).

## License

By contributing, you agree that your contributions will be licensed under the project license:

- Waqf General Public License (see `license-en.txt` / `license-ar.txt` in the repository).
