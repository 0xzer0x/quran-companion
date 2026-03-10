;--------------------------------
; NSIS Modern UI Installer for Quran Companion
;--------------------------------

!pragma warning error all
!include "MUI2.nsh"

;--------------------------------
; General

Unicode True

Name "Quran Companion"

; Set application version
!ifndef APPVER
  !define APPVER "1.0"
!endif

!ifndef OUTFILE
  !define OUTFILE "QuranCompanionSetup.exe"
!endif
OutFile "${OUTFILE}"

; Default installation folder: User Home
InstallDir "$PROFILE\QuranCompanion"

; Get previous install dir from registry if available
InstallDirRegKey HKCU "Software\QuranCompanion" ""

; Request standard user privileges
RequestExecutionLevel user

; Compression configuration
SetCompressor /SOLID lzma
SetCompressorDictSize 64

;--------------------------------
;Variables

Var StartMenuFolder

;--------------------------------
; Interface Settings

!define MUI_ABORTWARNING
!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "Software\QuranCompanion"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
; Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE $(license)
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_LICENSE $(license)
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Arabic"

; Set license file per language
LicenseLangString license ${LANG_ENGLISH} "license-en.txt"
LicenseLangString license ${LANG_ARABIC} "license-ar.txt"

; Reserve files
!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
; Installer Sections

Section "Quran Companion Application" SecMain

  SetOutPath "$INSTDIR"

  ; Copy executables and Qt DLLs
  File "quran-companion.exe"
  File "Qt6Core.dll"
  File "Qt6Gui.dll"
  File "Qt6Widgets.dll"
  File "Qt6Multimedia.dll"
  File "Qt6Network.dll"
  File "Qt6Sql.dll"
  File "Qt6Svg.dll"
  File "D3Dcompiler_47.dll"
  File "avcodec-61.dll"
  File "avformat-61.dll"
  File "avutil-59.dll"
  File "swresample-5.dll"
  File "swscale-8.dll"
  File "opengl32sw.dll"

  ; Copy plugin folders recursively
  SetOutPath "$INSTDIR\assets"
  File /r "assets\*.*"

  SetOutPath "$INSTDIR\bismillah"
  File /r "bismillah\*.*"

  SetOutPath "$INSTDIR\generic"
  File /r "generic\*.*"

  SetOutPath "$INSTDIR\iconengines"
  File /r "iconengines\*.*"

  SetOutPath "$INSTDIR\imageformats"
  File /r "imageformats\*.*"

  SetOutPath "$INSTDIR\multimedia"
  File /r "multimedia\*.*"

  SetOutPath "$INSTDIR\networkinformation"
  File /r "networkinformation\*.*"

  SetOutPath "$INSTDIR\platforms"
  File /r "platforms\*.*"

  SetOutPath "$INSTDIR\sqldrivers"
  File /r "sqldrivers\*.*"

  SetOutPath "$INSTDIR\styles"
  File /r "styles\*.*"

  SetOutPath "$INSTDIR\tls"
  File /r "tls\*.*"

  ; Store install path in registry
  WriteRegStr HKCU "Software\QuranCompanion" "" $INSTDIR

  ; Write to the Windows uninstall registry
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "DisplayName" "Quran Companion"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "DisplayIcon" "$INSTDIR\quran-companion.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "DisplayVersion" "${APPVER}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "Publisher" "Youssef Fathy"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "NoModify" 0
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion" "NoRepair" 1

  ; Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Create shortcuts in configured start menu folder
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Quran Companion.lnk" "$INSTDIR\quran-companion.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  ; Create Desktop shortcut
  CreateShortCut "$DESKTOP\Quran Companion.lnk" "$INSTDIR\quran-companion.exe"

SectionEnd

;--------------------------------
; Uninstaller Section

Section "Uninstall"

  ; Delete files
  Delete "$INSTDIR\quran-companion.exe"
  Delete "$INSTDIR\Qt6Core.dll"
  Delete "$INSTDIR\Qt6Gui.dll"
  Delete "$INSTDIR\Qt6Widgets.dll"
  Delete "$INSTDIR\Qt6Multimedia.dll"
  Delete "$INSTDIR\Qt6Network.dll"
  Delete "$INSTDIR\Qt6Sql.dll"
  Delete "$INSTDIR\Qt6Svg.dll"
  Delete "$INSTDIR\D3Dcompiler_47.dll"
  Delete "$INSTDIR\avcodec-61.dll"
  Delete "$INSTDIR\avformat-61.dll"
  Delete "$INSTDIR\avutil-59.dll"
  Delete "$INSTDIR\swresample-5.dll"
  Delete "$INSTDIR\swscale-8.dll"
  Delete "$INSTDIR\opengl32sw.dll"

  ; Remove folders recursively
  RMDir /r "$INSTDIR\assets"
  RMDir /r "$INSTDIR\bismillah"
  RMDir /r "$INSTDIR\generic"
  RMDir /r "$INSTDIR\iconengines"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\multimedia"
  RMDir /r "$INSTDIR\networkinformation"
  RMDir /r "$INSTDIR\platforms"
  RMDir /r "$INSTDIR\sqldrivers"
  RMDir /r "$INSTDIR\styles"
  RMDir /r "$INSTDIR\tls"

  ; Delete start menu shortcuts
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  ; Delete desktop shortcuts
  Delete "$DESKTOP\Quran Companion.lnk"

  ; Remove registry keys
  DeleteRegKey HKCU "Software\QuranCompanion"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quran Companion"

SectionEnd

;--------------------------------
; Installer Functions
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;--------------------------------
; Uninstaller Functions

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd
