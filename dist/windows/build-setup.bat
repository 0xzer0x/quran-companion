"C:\Program Files\CMake\bin\cmake.exe" -DCMAKE_BUILD_TYPE:STRING=Release -G "Ninja" -S ../..  -B ./build
CD build
ninja

CD ..
MKDIR bundle
XCOPY ".\build\quran-companion.exe" ".\bundle\quran-companion.exe"
XCOPY ".\build\assets" ".\bundle\assets\" /E
XCOPY ".\build\audio" ".\bundle\audio\" /E
CD ./bundle
MKDIR translations
XCOPY "..\build\translations\qt_ar.qm" ".\translations\qt_ar.qm"

windeployqt.exe --no-translations .\quran-companion.exe

CD ..
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" quran-companion-setup.iss