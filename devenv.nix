{
  inputs,
  pkgs,
  lib,
  config,
  ...
}:

let
  qt6Packages = (import inputs.qt6-nixpkgs { inherit (pkgs.stdenv) system; }).qt6;
in
{
  env = {
    CMAKE_BUILD_TYPE = "Debug";
    CMAKE_GENERATOR = "Ninja Multi-Config";
    CMAKE_EXPORT_COMPILE_COMMANDS = 1;
    # WARN: Required for proper audio playback
    QT_PLUGIN_PATH = "${config.git.root}/.devenv/profile/lib/qt-6/plugins";
    LD_LIBRARY_PATH = "${config.git.root}/.devenv/profile/lib";
  };

  languages.cplusplus.enable = true;

  packages =
    (with pkgs; [
      go-task
      cmake
      ninja
      clang-tools
      crowdin-cli
      pipewire
    ])
    ++ (with qt6Packages; [
      qtbase
      qttools
      qtwayland
      qtmultimedia
      qtimageformats
      qttranslations
      wrapQtAppsHook
    ]);

  enterShell = ''
    echo "📦 Qt version: $(qmake -query QT_VERSION)"
  '';

  treefmt = {
    enable = true;
    config.programs = {
      cmake-format.enable = true;
      clang-format.enable = true;
      prettier.enable = true;
    };
  };

  git-hooks.hooks = {
    treefmt.enable = true;
  };
}
