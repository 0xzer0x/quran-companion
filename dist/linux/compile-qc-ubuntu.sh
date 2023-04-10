#!/bin/bash

mkdir build
cd build

echo;echo;
echo '#---------------------[Downloading Dependencies]---------------------#'
echo;echo;

sudo apt install build-essential cmake qt6-tools-dev qt6-base-dev qt6-image-formats-plugins qt6-multimedia-dev qt6-l10n-tools qt6-translations-l10n

echo;echo;
echo '#---------------------[Building]---------------------#'
echo;echo;

cmake ../../../;
make;

echo;echo;
echo '#---------------------[Copying Files]---------------------#'
echo;echo;

cd ..
mkdir bundle
cp -r build/quran-companion bundle/
cp -r build/assets bundle/
cp -r build/audio bundle/
cp -r build/translations bundle/

echo;echo;
echo '#---------------------[Done]---------------------#'
echo;echo;
