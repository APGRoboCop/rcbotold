##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## MetaMod_Release_Win32
ProjectName            :=HPB_bot
ConfigurationName      :=MetaMod_Release_Win32
WorkspacePath          :=C:/Users/Ryan/Dropbox/src/rcbot/dlls
ProjectPath            :=C:/Users/Ryan/Dropbox/src/rcbot/dlls
IntermediateDirectory  :=./HPB_bot___Win32_MetaMod_Release/
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Ryan
Date                   :=20/07/2016
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/TDM-GCC-32/bin/g++.exe
SharedObjectLinkerName :=C:/TDM-GCC-32/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=./HPB_bot___Win32_MetaMod_Release/HPB_bot.dll
Preprocessors          :=$(PreprocessorSwitch)NDEBUG $(PreprocessorSwitch)WIN32 $(PreprocessorSwitch)_WINDOWS $(PreprocessorSwitch)VALVE_DLL $(PreprocessorSwitch)RCBOT_META_BUILD $(PreprocessorSwitch)HPB_bot_EXPORTS 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="HPB_bot.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/TDM-GCC-32/bin/windres.exe
LinkOptions            :=  -O0
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)../dlls $(IncludeSwitch)../ $(IncludeSwitch)../../hlsdk2015/dlls $(IncludeSwitch)../../hlsdk2015/engine $(IncludeSwitch)../../hlsdk2015/common $(IncludeSwitch)../../hlsdk2015/pm_shared $(IncludeSwitch)../../hlsdk2015/public $(IncludeSwitch)../../metamod-p-37/metamod 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)kernel32 $(LibrarySwitch)user32 $(LibrarySwitch)gdi32 $(LibrarySwitch)winspool $(LibrarySwitch)comdlg32 $(LibrarySwitch)advapi32 $(LibrarySwitch)shell32 $(LibrarySwitch)ole32 $(LibrarySwitch)oleaut32 $(LibrarySwitch)uuid 
ArLibs                 :=  "kernel32.a" "user32.a" "gdi32.a" "winspool.a" "comdlg32.a" "advapi32.a" "shell32.a" "ole32.a" "oleaut32.a" "uuid.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/TDM-GCC-32/bin/ar.exe rcu
CXX      := C:/TDM-GCC-32/bin/g++.exe
CC       := C:/TDM-GCC-32/bin/gcc.exe
CXXFLAGS :=  -g -Wall -O2 -fpermissive $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := C:/TDM-GCC-32/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
VS_Configuration:=MetaMod_Release
VS_IntDir:=./HPB_bot___Win32_MetaMod_Release/
VS_OutDir:=./HPB_bot___Win32_MetaMod_Release/
VS_Platform:=Win32
VS_ProjectDir:=/root/Dropbox/src/rcbot/dlls/
VS_ProjectName:=HPB_bot
VS_SolutionDir:=/root/Dropbox/src/rcbot/dlls/
Objects0=$(IntermediateDirectory)/bits.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_client.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_commands.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_menu.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_navigate.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_rep.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_visibles.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_weapons.cpp$(ObjectSuffix) $(IntermediateDirectory)/client.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/dir.cpp$(ObjectSuffix) $(IntermediateDirectory)/dll.cpp$(ObjectSuffix) $(IntermediateDirectory)/engine.cpp$(ObjectSuffix) $(IntermediateDirectory)/globals.cpp$(ObjectSuffix) $(IntermediateDirectory)/linkfunc.cpp$(ObjectSuffix) $(IntermediateDirectory)/megahal.cpp$(ObjectSuffix) $(IntermediateDirectory)/netmessages.cpp$(ObjectSuffix) $(IntermediateDirectory)/rcbot_meta_api.cpp$(ObjectSuffix) $(IntermediateDirectory)/util.cpp$(ObjectSuffix) $(IntermediateDirectory)/waypoint.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/bot_ga.cpp$(ObjectSuffix) $(IntermediateDirectory)/ga.cpp$(ObjectSuffix) $(IntermediateDirectory)/bot_nn.cpp$(ObjectSuffix) $(IntermediateDirectory)/perceptron.cpp$(ObjectSuffix) $(IntermediateDirectory)/som.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "C:\Users\Ryan\Dropbox\src\rcbot\dlls/.build-release"
	@echo rebuilt > "C:\Users\Ryan\Dropbox\src\rcbot\dlls/.build-release/HPB_bot"

MakeIntermediateDirs:
	@$(MakeDirCommand) "./HPB_bot___Win32_MetaMod_Release/"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./HPB_bot___Win32_MetaMod_Release/"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/bits.cpp$(ObjectSuffix): bits.cpp $(IntermediateDirectory)/bits.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bits.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bits.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bits.cpp$(DependSuffix): bits.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bits.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bits.cpp$(DependSuffix) -MM bits.cpp

$(IntermediateDirectory)/bits.cpp$(PreprocessSuffix): bits.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bits.cpp$(PreprocessSuffix)bits.cpp

$(IntermediateDirectory)/bot.cpp$(ObjectSuffix): bot.cpp $(IntermediateDirectory)/bot.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot.cpp$(DependSuffix): bot.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot.cpp$(DependSuffix) -MM bot.cpp

$(IntermediateDirectory)/bot.cpp$(PreprocessSuffix): bot.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot.cpp$(PreprocessSuffix)bot.cpp

$(IntermediateDirectory)/bot_client.cpp$(ObjectSuffix): bot_client.cpp $(IntermediateDirectory)/bot_client.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_client.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_client.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_client.cpp$(DependSuffix): bot_client.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_client.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_client.cpp$(DependSuffix) -MM bot_client.cpp

$(IntermediateDirectory)/bot_client.cpp$(PreprocessSuffix): bot_client.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_client.cpp$(PreprocessSuffix)bot_client.cpp

$(IntermediateDirectory)/bot_commands.cpp$(ObjectSuffix): bot_commands.cpp $(IntermediateDirectory)/bot_commands.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_commands.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_commands.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_commands.cpp$(DependSuffix): bot_commands.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_commands.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_commands.cpp$(DependSuffix) -MM bot_commands.cpp

$(IntermediateDirectory)/bot_commands.cpp$(PreprocessSuffix): bot_commands.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_commands.cpp$(PreprocessSuffix)bot_commands.cpp

$(IntermediateDirectory)/bot_menu.cpp$(ObjectSuffix): bot_menu.cpp $(IntermediateDirectory)/bot_menu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_menu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_menu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_menu.cpp$(DependSuffix): bot_menu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_menu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_menu.cpp$(DependSuffix) -MM bot_menu.cpp

$(IntermediateDirectory)/bot_menu.cpp$(PreprocessSuffix): bot_menu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_menu.cpp$(PreprocessSuffix)bot_menu.cpp

$(IntermediateDirectory)/bot_navigate.cpp$(ObjectSuffix): bot_navigate.cpp $(IntermediateDirectory)/bot_navigate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_navigate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_navigate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_navigate.cpp$(DependSuffix): bot_navigate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_navigate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_navigate.cpp$(DependSuffix) -MM bot_navigate.cpp

$(IntermediateDirectory)/bot_navigate.cpp$(PreprocessSuffix): bot_navigate.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_navigate.cpp$(PreprocessSuffix)bot_navigate.cpp

$(IntermediateDirectory)/bot_rep.cpp$(ObjectSuffix): bot_rep.cpp $(IntermediateDirectory)/bot_rep.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_rep.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_rep.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_rep.cpp$(DependSuffix): bot_rep.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_rep.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_rep.cpp$(DependSuffix) -MM bot_rep.cpp

$(IntermediateDirectory)/bot_rep.cpp$(PreprocessSuffix): bot_rep.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_rep.cpp$(PreprocessSuffix)bot_rep.cpp

$(IntermediateDirectory)/bot_visibles.cpp$(ObjectSuffix): bot_visibles.cpp $(IntermediateDirectory)/bot_visibles.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_visibles.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_visibles.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_visibles.cpp$(DependSuffix): bot_visibles.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_visibles.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_visibles.cpp$(DependSuffix) -MM bot_visibles.cpp

$(IntermediateDirectory)/bot_visibles.cpp$(PreprocessSuffix): bot_visibles.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_visibles.cpp$(PreprocessSuffix)bot_visibles.cpp

$(IntermediateDirectory)/bot_weapons.cpp$(ObjectSuffix): bot_weapons.cpp $(IntermediateDirectory)/bot_weapons.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_weapons.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_weapons.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_weapons.cpp$(DependSuffix): bot_weapons.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_weapons.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_weapons.cpp$(DependSuffix) -MM bot_weapons.cpp

$(IntermediateDirectory)/bot_weapons.cpp$(PreprocessSuffix): bot_weapons.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_weapons.cpp$(PreprocessSuffix)bot_weapons.cpp

$(IntermediateDirectory)/client.cpp$(ObjectSuffix): client.cpp $(IntermediateDirectory)/client.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/client.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/client.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/client.cpp$(DependSuffix): client.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/client.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/client.cpp$(DependSuffix) -MM client.cpp

$(IntermediateDirectory)/client.cpp$(PreprocessSuffix): client.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/client.cpp$(PreprocessSuffix)client.cpp

$(IntermediateDirectory)/dir.cpp$(ObjectSuffix): dir.cpp $(IntermediateDirectory)/dir.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/dir.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dir.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dir.cpp$(DependSuffix): dir.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dir.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dir.cpp$(DependSuffix) -MM dir.cpp

$(IntermediateDirectory)/dir.cpp$(PreprocessSuffix): dir.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dir.cpp$(PreprocessSuffix)dir.cpp

$(IntermediateDirectory)/dll.cpp$(ObjectSuffix): dll.cpp $(IntermediateDirectory)/dll.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/dll.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dll.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dll.cpp$(DependSuffix): dll.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dll.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/dll.cpp$(DependSuffix) -MM dll.cpp

$(IntermediateDirectory)/dll.cpp$(PreprocessSuffix): dll.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dll.cpp$(PreprocessSuffix)dll.cpp

$(IntermediateDirectory)/engine.cpp$(ObjectSuffix): engine.cpp $(IntermediateDirectory)/engine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/engine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/engine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/engine.cpp$(DependSuffix): engine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/engine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/engine.cpp$(DependSuffix) -MM engine.cpp

$(IntermediateDirectory)/engine.cpp$(PreprocessSuffix): engine.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/engine.cpp$(PreprocessSuffix)engine.cpp

$(IntermediateDirectory)/globals.cpp$(ObjectSuffix): globals.cpp $(IntermediateDirectory)/globals.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/globals.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/globals.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/globals.cpp$(DependSuffix): globals.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/globals.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/globals.cpp$(DependSuffix) -MM globals.cpp

$(IntermediateDirectory)/globals.cpp$(PreprocessSuffix): globals.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/globals.cpp$(PreprocessSuffix)globals.cpp

$(IntermediateDirectory)/linkfunc.cpp$(ObjectSuffix): linkfunc.cpp $(IntermediateDirectory)/linkfunc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/linkfunc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/linkfunc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/linkfunc.cpp$(DependSuffix): linkfunc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/linkfunc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/linkfunc.cpp$(DependSuffix) -MM linkfunc.cpp

$(IntermediateDirectory)/linkfunc.cpp$(PreprocessSuffix): linkfunc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/linkfunc.cpp$(PreprocessSuffix)linkfunc.cpp

$(IntermediateDirectory)/megahal.cpp$(ObjectSuffix): megahal.cpp $(IntermediateDirectory)/megahal.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/megahal.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/megahal.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/megahal.cpp$(DependSuffix): megahal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/megahal.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/megahal.cpp$(DependSuffix) -MM megahal.cpp

$(IntermediateDirectory)/megahal.cpp$(PreprocessSuffix): megahal.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/megahal.cpp$(PreprocessSuffix)megahal.cpp

$(IntermediateDirectory)/netmessages.cpp$(ObjectSuffix): netmessages.cpp $(IntermediateDirectory)/netmessages.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/netmessages.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/netmessages.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/netmessages.cpp$(DependSuffix): netmessages.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/netmessages.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/netmessages.cpp$(DependSuffix) -MM netmessages.cpp

$(IntermediateDirectory)/netmessages.cpp$(PreprocessSuffix): netmessages.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/netmessages.cpp$(PreprocessSuffix)netmessages.cpp

$(IntermediateDirectory)/rcbot_meta_api.cpp$(ObjectSuffix): rcbot_meta_api.cpp $(IntermediateDirectory)/rcbot_meta_api.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/rcbot_meta_api.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/rcbot_meta_api.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/rcbot_meta_api.cpp$(DependSuffix): rcbot_meta_api.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/rcbot_meta_api.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/rcbot_meta_api.cpp$(DependSuffix) -MM rcbot_meta_api.cpp

$(IntermediateDirectory)/rcbot_meta_api.cpp$(PreprocessSuffix): rcbot_meta_api.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/rcbot_meta_api.cpp$(PreprocessSuffix)rcbot_meta_api.cpp

$(IntermediateDirectory)/util.cpp$(ObjectSuffix): util.cpp $(IntermediateDirectory)/util.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/util.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/util.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/util.cpp$(DependSuffix): util.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/util.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/util.cpp$(DependSuffix) -MM util.cpp

$(IntermediateDirectory)/util.cpp$(PreprocessSuffix): util.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/util.cpp$(PreprocessSuffix)util.cpp

$(IntermediateDirectory)/waypoint.cpp$(ObjectSuffix): waypoint.cpp $(IntermediateDirectory)/waypoint.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/waypoint.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/waypoint.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/waypoint.cpp$(DependSuffix): waypoint.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/waypoint.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/waypoint.cpp$(DependSuffix) -MM waypoint.cpp

$(IntermediateDirectory)/waypoint.cpp$(PreprocessSuffix): waypoint.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/waypoint.cpp$(PreprocessSuffix)waypoint.cpp

$(IntermediateDirectory)/bot_ga.cpp$(ObjectSuffix): bot_ga.cpp $(IntermediateDirectory)/bot_ga.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_ga.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_ga.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_ga.cpp$(DependSuffix): bot_ga.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_ga.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_ga.cpp$(DependSuffix) -MM bot_ga.cpp

$(IntermediateDirectory)/bot_ga.cpp$(PreprocessSuffix): bot_ga.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_ga.cpp$(PreprocessSuffix)bot_ga.cpp

$(IntermediateDirectory)/ga.cpp$(ObjectSuffix): ga.cpp $(IntermediateDirectory)/ga.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/ga.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ga.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ga.cpp$(DependSuffix): ga.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ga.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ga.cpp$(DependSuffix) -MM ga.cpp

$(IntermediateDirectory)/ga.cpp$(PreprocessSuffix): ga.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ga.cpp$(PreprocessSuffix)ga.cpp

$(IntermediateDirectory)/bot_nn.cpp$(ObjectSuffix): bot_nn.cpp $(IntermediateDirectory)/bot_nn.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/bot_nn.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/bot_nn.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/bot_nn.cpp$(DependSuffix): bot_nn.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/bot_nn.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/bot_nn.cpp$(DependSuffix) -MM bot_nn.cpp

$(IntermediateDirectory)/bot_nn.cpp$(PreprocessSuffix): bot_nn.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/bot_nn.cpp$(PreprocessSuffix)bot_nn.cpp

$(IntermediateDirectory)/perceptron.cpp$(ObjectSuffix): perceptron.cpp $(IntermediateDirectory)/perceptron.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/perceptron.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/perceptron.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/perceptron.cpp$(DependSuffix): perceptron.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/perceptron.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/perceptron.cpp$(DependSuffix) -MM perceptron.cpp

$(IntermediateDirectory)/perceptron.cpp$(PreprocessSuffix): perceptron.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/perceptron.cpp$(PreprocessSuffix)perceptron.cpp

$(IntermediateDirectory)/som.cpp$(ObjectSuffix): som.cpp $(IntermediateDirectory)/som.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Ryan/Dropbox/src/rcbot/dlls/som.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/som.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/som.cpp$(DependSuffix): som.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/som.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/som.cpp$(DependSuffix) -MM som.cpp

$(IntermediateDirectory)/som.cpp$(PreprocessSuffix): som.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/som.cpp$(PreprocessSuffix)som.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./HPB_bot___Win32_MetaMod_Release/


