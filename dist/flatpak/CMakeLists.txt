message(STATUS "Installing icons")
install(
  FILES ../xdg/logo64.png
  DESTINATION /app/share/icons/hicolor/64x64/apps
  RENAME io.github._0xzer0x.qurancompanion.png)
install(
  FILES ../xdg/logo128.png
  DESTINATION /app/share/icons/hicolor/128x128/apps
  RENAME io.github._0xzer0x.qurancompanion.png)
install(
  FILES ../xdg/logo256.png
  DESTINATION /app/share/icons/hicolor/256x256/apps
  RENAME io.github._0xzer0x.qurancompanion.png)

message(STATUS "Installing metainfo")
install(FILES ../xdg/io.github._0xzer0x.qurancompanion.metainfo.xml
        DESTINATION /app/share/metainfo)

message(STATUS "Installing desktop file")
install(FILES ../xdg/io.github._0xzer0x.qurancompanion.desktop
        DESTINATION /app/share/applications)
