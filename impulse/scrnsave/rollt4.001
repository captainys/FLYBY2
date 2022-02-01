# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=vc40g - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの vc40g - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "vc40g - Win32 Release" && "$(CFG)" != "vc40g - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛ 'CFG' を定義することによって
!MESSAGE NMAKE 実行時にﾋﾞﾙﾄﾞ ﾓｰﾄﾞを指定できます。例えば:
!MESSAGE 
!MESSAGE NMAKE /f "rollt4.MAK" CFG="vc40g - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "vc40g - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "vc40g - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "vc40g - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vc40g - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\rollt4.exe"

CLEAN : 
	-@erase ".\rollt4.exe"
	-@erase ".\rollt4.obj"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "BISCSV" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "BISCSV" /Fp"$(INTDIR)/rollt4.pch" /YX /c 
CPP_OBJS=
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rollt4.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32s.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib scrnsave.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32s.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 scrnsave.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/rollt4.pdb" /machine:I386 /nodefaultlib:"libc.lib"\
 /out:"$(OUTDIR)/rollt4.exe" 
LINK32_OBJS= \
	"$(INTDIR)/rollt4.obj"

"$(OUTDIR)\rollt4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vc40g - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\rollt4.exe"

CLEAN : 
	-@erase ".\rollt4.exe"
	-@erase ".\rollt4.obj"
	-@erase ".\rollt4.ilk"
	-@erase ".\rollt4.pdb"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "BISCSV" /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "BISCSV" /Fp"$(INTDIR)/rollt4.pch" /YX /c 
CPP_OBJS=
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/rollt4.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32s.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib scrnsave.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32s.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 scrnsave.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/rollt4.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib"\
 /out:"$(OUTDIR)/rollt4.exe" 
LINK32_OBJS= \
	"$(INTDIR)/rollt4.obj"

"$(OUTDIR)\rollt4.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "vc40g - Win32 Release"
# Name "vc40g - Win32 Debug"

!IF  "$(CFG)" == "vc40g - Win32 Release"

!ELSEIF  "$(CFG)" == "vc40g - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\rollt4.c
DEP_CPP_ROLLT=\
	{$(INCLUDE)}"\impulse.h"\
	{$(INCLUDE)}"\hotas.h"\
	{$(INCLUDE)}"\skyhawk.h"\
	{$(INCLUDE)}"\aurora.h"\
	

"$(INTDIR)\rollt4.obj" : $(SOURCE) $(DEP_CPP_ROLLT) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
