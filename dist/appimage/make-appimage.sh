#!/bin/sh

set -eu

ARCH=$(uname -m)
VERSION=${VERSION:-$(cat ./VERSION)}
export ARCH VERSION
export OUTPATH=./dist
export OUTNAME="Quran_Companion-$VERSION-$ARCH.AppImage"
export ADD_HOOKS="self-updater.hook"
export UPINFO="gh-releases-zsync|${GITHUB_REPOSITORY%/*}|${GITHUB_REPOSITORY#*/}|latest|*$ARCH.AppImage.zsync"
export DESKTOP=./dist/xdg/io.github._0xzer0x.qurancompanion.desktop
export ICON="${TMPDIR:-/tmp}/io.github._0xzer0x.qurancompanion.png"
export DEPLOY_PULSE=1
export DEPLOY_PIPEWIRE=1
export STARTUPWMCLASS=quran-companion

# quick-sharun wants the icon name to match Icon= from the desktop entry
cp -v ./dist/xdg/logo256.png "$ICON"

# Deploy dependencies, /usr/share/quran-companion holds the databases, fonts
# and recitations the application loads relative to its own binary
quick-sharun \
	/usr/bin/quran-companion \
	/usr/share/quran-companion

# desktop integration files in their FHS locations, the application itself
# does not read them at runtime but the AUR package unpacks them from here
mkdir -p ./AppDir/share/applications ./AppDir/share/metainfo \
	./AppDir/share/icons/hicolor/256x256/apps
cp -v "$DESKTOP" ./AppDir/share/applications
cp -v ./dist/xdg/io.github._0xzer0x.qurancompanion.metainfo.xml \
	./AppDir/share/metainfo/io.github._0xzer0x.qurancompanion.appdata.xml
cp -v ./dist/xdg/logo256.png \
	./AppDir/share/icons/hicolor/256x256/apps/io.github._0xzer0x.qurancompanion.png

# DirManager resolves its databases, fonts and recitations from
# <binary dir>/../share/quran-companion. The real binary lives in shared/bin
# (bin/quran-companion is a sharun hardlink), so shared/ needs a share dir
ln -sfn ../share ./AppDir/shared/share

# Turn AppDir into AppImage
quick-sharun --make-appimage

# Test the app for 12 seconds, if the test fails due to the app
# having issues running in the CI use --simple-test instead
quick-sharun --test ./dist/*.AppImage
