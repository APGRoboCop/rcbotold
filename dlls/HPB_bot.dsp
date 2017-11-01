# Microsoft Developer Studio Project File - Name="HPB_bot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=HPB_bot - Win32 TFC Debug metamod
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HPB_bot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HPB_bot.mak" CFG="HPB_bot - Win32 TFC Debug metamod"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HPB_bot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 MetaMod Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 MetaMod Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 TFC Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 TFC Debug metamod" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HPB_bot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Copying to DLL folder(s)
TargetPath=.\Release\HPB_bot.dll
TargetName=HPB_bot
InputPath=.\Release\HPB_bot.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\paul.murphy@ntlworld.com\half-life\rcbot\dlls\rcbot.dll" 
	copy $(TargetPath) g:\half-life\rcbot\dlls\rcbot.dll 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\HPB_bot___Win"
# PROP BASE Intermediate_Dir ".\HPB_bot___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /vd0 /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x809 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT LINK32 /profile
# Begin Custom Build
TargetPath=.\Debug\HPB_bot.dll
TargetName=HPB_bot
InputPath=.\Debug\HPB_bot.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) g:\Half-Life\rcbot\dlls 
	copy $(TargetPath) "c:\program files\steam\steamapps\paul.murphy@ntlworld.com\half-life\rcbot\dlls" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "HPB_bot___Win32_MetaMod_Debug"
# PROP BASE Intermediate_Dir "HPB_bot___Win32_MetaMod_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "HPB_bot___Win32_MetaMod_Debug"
# PROP Intermediate_Dir "HPB_bot___Win32_MetaMod_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /I "..\metamod" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "VALVE_DLL" /D "RCBOT_META_BUILD" /D "HPB_bot_EXPORTS" /D "_WIN32" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /dll /debug /machine:I386 /def:".\HPB_bot.def" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Custom Build
TargetPath=.\HPB_bot___Win32_MetaMod_Debug\HPB_bot.dll
TargetName=HPB_bot
InputPath=.\HPB_bot___Win32_MetaMod_Debug\HPB_bot.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\paul.murphy@ntlworld.com\half-life\rcbot\dlls" 
	copy                         $(TargetPath)                         g:\half-life\rcbot\dlls\  
	
# End Custom Build

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "HPB_bot___Win32_MetaMod_Release"
# PROP BASE Intermediate_Dir "HPB_bot___Win32_MetaMod_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "HPB_bot___Win32_MetaMod_Release"
# PROP Intermediate_Dir "HPB_bot___Win32_MetaMod_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "RCBOT_META_BUILD" /D "HPB_bot_EXPORTS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT LINK32 /profile
# Begin Custom Build - Copying to DLL folder(s)
TargetPath=.\HPB_bot___Win32_MetaMod_Release\HPB_bot.dll
TargetName=HPB_bot
InputPath=.\HPB_bot___Win32_MetaMod_Release\HPB_bot.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) g:\half-life\rcbot\dlls\rcbot_mm.dll 
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\paul.murphy@ntlworld.com\half-life\rcbot\dlls\rcbot_mm.dll" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "HPB_bot___Win32_TFC_Debug"
# PROP BASE Intermediate_Dir "HPB_bot___Win32_TFC_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "HPB_bot___Win32_TFC_Debug"
# PROP Intermediate_Dir "HPB_bot___Win32_TFC_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x809 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT LINK32 /profile
# Begin Custom Build
TargetPath=.\HPB_bot___Win32_TFC_Debug\HPB_bot.dll
TargetName=HPB_bot
InputPath=.\HPB_bot___Win32_TFC_Debug\HPB_bot.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) g:\Half-Life\rcbot\dlls 
	copy $(TargetPath) "c:\program files\steam\steamapps\paul.murphy@ntlworld.com\half-life\rcbot\dlls" 
	copy $(TargetPath) "c:\program files\steam\steamapps\paul.murphy@ntlworld.com\team fortress classic\rcbot\dlls" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug metamod"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "HPB_bot___Win32_TFC_Debug_metamod"
# PROP BASE Intermediate_Dir "HPB_bot___Win32_TFC_Debug_metamod"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "HPB_bot___Win32_TFC_Debug_metamod"
# PROP Intermediate_Dir "HPB_bot___Win32_TFC_Debug_metamod"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "RCBOT_META_BUILD" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /i "..\engine" /d "_DEBUG"
# ADD RSC /l 0x809 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\HPB_bot.def"
# SUBTRACT LINK32 /profile
# Begin Custom Build
TargetPath=.\HPB_bot___Win32_TFC_Debug_metamod\HPB_bot.dll
TargetName=HPB_bot
InputPath=.\HPB_bot___Win32_TFC_Debug_metamod\HPB_bot.dll
SOURCE="$(InputPath)"

"$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath) g:\Half-Life\rcbot\dlls 
	copy $(TargetPath) "c:\program files\steam\steamapps\paul.murphy@ntlworld.com\half-life\rcbot\dlls" 
	copy $(TargetPath) "c:\program files\steam\steamapps\paul.murphy@ntlworld.com\team fortress classic\rcbot\dlls" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "HPB_bot - Win32 Release"
# Name "HPB_bot - Win32 Debug"
# Name "HPB_bot - Win32 MetaMod Debug"
# Name "HPB_bot - Win32 MetaMod Release"
# Name "HPB_bot - Win32 TFC Debug"
# Name "HPB_bot - Win32 TFC Debug metamod"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Group "ga"

# PROP Default_Filter "*.cpp *.h"
# Begin Source File

SOURCE=.\bot_ga.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_ga.h
# End Source File
# Begin Source File

SOURCE=.\ga.cpp
# End Source File
# Begin Source File

SOURCE=.\ga.h
# End Source File
# Begin Source File

SOURCE=.\gannconst.h
# End Source File
# End Group
# Begin Group "nn"

# PROP Default_Filter "*.cpp *.h"
# Begin Source File

SOURCE=.\bot_nn.cpp
# End Source File
# Begin Source File

SOURCE=.\nn.h
# End Source File
# Begin Source File

SOURCE=.\perceptron.cpp
# End Source File
# Begin Source File

SOURCE=.\perceptron.h
# End Source File
# Begin Source File

SOURCE=.\som.cpp
# End Source File
# Begin Source File

SOURCE=.\som.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\bits.cpp
# End Source File
# Begin Source File

SOURCE=.\bot.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_client.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_commands.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_rep.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_visibles.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\client.cpp
# End Source File
# Begin Source File

SOURCE=.\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp

!IF  "$(CFG)" == "HPB_bot - Win32 Release"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 Debug"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug metamod"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp

!IF  "$(CFG)" == "HPB_bot - Win32 Release"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 Debug"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Release"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug metamod"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\megahal.cpp
# End Source File
# Begin Source File

SOURCE=.\netmessages.cpp
# End Source File
# Begin Source File

SOURCE=.\rcbot_meta_api.cpp

!IF  "$(CFG)" == "HPB_bot - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 Debug"

# SUBTRACT CPP /WX

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Debug"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 MetaMod Release"

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug"

# SUBTRACT BASE CPP /WX
# SUBTRACT CPP /WX

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 TFC Debug metamod"

# SUBTRACT BASE CPP /WX
# SUBTRACT CPP /WX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\waypoint.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\actionutility.h
# End Source File
# Begin Source File

SOURCE=.\bits.h
# End Source File
# Begin Source File

SOURCE=.\bot.h
# End Source File
# Begin Source File

SOURCE=.\bot_builder.h
# End Source File
# Begin Source File

SOURCE=.\bot_client.h
# End Source File
# Begin Source File

SOURCE=.\bot_commands.h
# End Source File
# Begin Source File

SOURCE=.\bot_const.h
# End Source File
# Begin Source File

SOURCE=.\bot_menu.h
# End Source File
# Begin Source File

SOURCE=.\bot_tick.h
# End Source File
# Begin Source File

SOURCE=.\bot_visibles.h
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.h
# End Source File
# Begin Source File

SOURCE=.\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\dir.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\generic_class.h
# End Source File
# Begin Source File

SOURCE=.\h_export_meta.h
# End Source File
# Begin Source File

SOURCE=.\megahal.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\usercmd.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\waypoint.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\notes.txt
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# Begin Source File

SOURCE=.\version.txt
# End Source File
# End Target
# End Project
