/**************************************************************************
* This file is part of the Fraqtive program
* Copyright (C) 2004-2012 Michał Męciński
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

!define SRCDIR ".."
!define BUILDDIR "..\release"

!define UNINST_KEY "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Fraqtive"

!include "MUI2.nsh"

!include "languages\fraqtive_en.nsh"

!ifdef INNER
    SetCompress off

    OutFile "$%TEMP%\innerinst.exe"
!else
    !verbose 4

    SetCompressor /SOLID lzma
    SetCompressorDictSize 32

    OutFile "fraqtive-${VERSION}-${ARCHITECTURE}.exe"
!endif

!define MULTIUSER_EXECUTIONLEVEL "Highest"
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "${UNINST_KEY}"
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME "UninstallString"
!define MULTIUSER_INSTALLMODE_INSTDIR "Fraqtive"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_KEY "${UNINST_KEY}"
!define MULTIUSER_INSTALLMODE_INSTDIR_REGISTRY_VALUENAME "InstallLocation"
!if ${ARCHITECTURE} == "win_x64"
    !define MULTIUSER_USE_PROGRAMFILES64
!endif
!ifndef INNER
    !define MULTIUSER_NOUNINSTALL
!endif
!include "include\multiuser64.nsh"

Name "$(NAME)"

!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-blue.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-blue.ico"

!define MUI_WELCOMEFINISHPAGE_BITMAP "images\wizard.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "images\wizard.bmp"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "images\header.bmp"
!define MUI_HEADERIMAGE_RIGHT

!define MUI_WELCOMEPAGE_TITLE "$(TITLE)"
!define MUI_WELCOMEPAGE_TEXT "$(WELCOME_TEXT)"
!insertmacro MUI_PAGE_WELCOME

!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "${SRCDIR}\COPYING"

!insertmacro MULTIUSER_PAGE_INSTALLMODE

!insertmacro MUI_PAGE_DIRECTORY

ShowInstDetails nevershow
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_TITLE "$(TITLE)"
!insertmacro MUI_PAGE_FINISH
  
!ifdef INNER  
    !define MUI_WELCOMEPAGE_TITLE "$(TITLE)"
    !insertmacro MUI_UNPAGE_WELCOME

    !insertmacro MUI_UNPAGE_CONFIRM

    ShowUninstDetails nevershow
    !insertmacro MUI_UNPAGE_INSTFILES

    !define MUI_FINISHPAGE_TITLE "$(TITLE)"
    !insertmacro MUI_UNPAGE_FINISH
!endif

!insertmacro MUI_LANGUAGE "English"

VIProductVersion "${BUILDVERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Michał Męciński"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Fraqtive Setup"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C) 2004-2015 Michał Męciński"
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "fraqtive-${VERSION}-${ARCHITECTURE}.exe"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Fraqtive"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}"

Function .onInit

!if ${ARCHITECTURE} == "win_x64"
    SetRegView 64
!endif

!ifdef INNER
    WriteUninstaller "$%TEMP%\uninstall.exe"
    Quit
!endif

    !insertmacro MULTIUSER_INIT

FunctionEnd

Section

    SetOutPath "$INSTDIR"

    File "${SRCDIR}\ChangeLog"
    File "${SRCDIR}\COPYING"
    File "${SRCDIR}\README"

    SetOutPath "$INSTDIR\bin"

    Delete "$INSTDIR\bin\*.*"

    File "${BUILDDIR}\fraqtive.exe"

    File "${VCRTDIR}\msvcp100.dll"
    File "${VCRTDIR}\msvcr100.dll"

    File "qt.conf"

    File "${QTDIR}\bin\Qt5Core.dll"
    File "${QTDIR}\bin\Qt5Gui.dll"
    File "${QTDIR}\bin\Qt5OpenGL.dll"
    File "${QTDIR}\bin\Qt5Widgets.dll"
    File "${QTDIR}\bin\Qt5Xml.dll"

    SetOutPath "$INSTDIR\plugins\imageformats"

    File "${QTDIR}\plugins\imageformats\qjpeg.dll"
    File "${QTDIR}\plugins\imageformats\qtiff.dll"

    SetOutPath "$INSTDIR\plugins\platforms"

    File "${QTDIR}\plugins\platforms\qwindows.dll"

    SetOutPath "$INSTDIR\bin"

    CreateShortCut "$SMPROGRAMS\Fraqtive.lnk" "$INSTDIR\bin\fraqtive.exe"
    CreateShortCut "$DESKTOP\Fraqtive.lnk" "$INSTDIR\bin\fraqtive.exe"

    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayIcon" '"$INSTDIR\bin\fraqtive.exe"'
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayName" "Fraqtive ${VERSION}${SUFFIX}"
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayVersion" "${VERSION}"
    WriteRegStr SHCTX "${UNINST_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe" /$MultiUser.InstallMode'
    WriteRegStr SHCTX "${UNINST_KEY}" "InstallLocation" "$INSTDIR"
    WriteRegStr SHCTX "${UNINST_KEY}" "Publisher" "Michał Męciński"
    WriteRegStr SHCTX "${UNINST_KEY}" "HelpLink" "http://fraqtive.mimec.org"
    WriteRegStr SHCTX "${UNINST_KEY}" "URLInfoAbout" "http://fraqtive.mimec.org"
    WriteRegStr SHCTX "${UNINST_KEY}" "URLUpdateInfo" "http://fraqtive.mimec.org/downloads"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "NoModify" 1
    WriteRegDWORD SHCTX "${UNINST_KEY}" "NoRepair" 1

!ifndef INNER
    SetOutPath "$INSTDIR"
    File "$%TEMP%\uninstall.exe"
!endif

SectionEnd

!ifdef INNER

Function un.onInit

!if ${ARCHITECTURE} == "win_x64"
    SetRegView 64
!endif

    !insertmacro MULTIUSER_UNINIT

FunctionEnd

Section "Uninstall"

    DeleteRegKey SHCTX "${UNINST_KEY}"
    
    Delete "$SMPROGRAMS\Fraqtive.lnk"
    Delete "$DESKTOP\Fraqtive.lnk"

    Delete "$INSTDIR\ChangeLog"
    Delete "$INSTDIR\COPYING"
    Delete "$INSTDIR\README"
    RMDir /r "$INSTDIR\bin"
    RMDir /r "$INSTDIR\plugins"
    Delete "$INSTDIR\uninstall.exe"
    RMDir "$INSTDIR"

SectionEnd

!endif
