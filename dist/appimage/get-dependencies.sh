#!/bin/sh

set -eu

ARCH=$(uname -m)

echo "Installing package dependencies..."
echo "---------------------------------------------------------------"
pacman -Syu --noconfirm --needed \
	base-devel            \
	cmake                 \
	git                   \
	kvantum               \
	lxqt-qtplugin         \
	ninja                 \
	qt6-multimedia-ffmpeg \
	qt6-tools             \
	qt6ct

echo "Installing debloated packages..."
echo "---------------------------------------------------------------"
get-debloated-pkgs --add-common --prefer-nano ffmpeg-mini

echo "Building Quran Companion..."
echo "---------------------------------------------------------------"
cmake -B ./build -G Ninja \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_INSTALL_LIBDIR=lib
cmake --build ./build -j"$(nproc)"
cmake --install ./build

cat ./VERSION > ~/version
