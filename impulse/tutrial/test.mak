# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=vc40_d - Win32 Debug DIB
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの vc40_d - Win32 Debug DIB を設定します。
!ENDIF 

!IF "$(CFG)" != "vc40_d - Win32 Release OpenGL" && "$(CFG)" !=\
 "vc40_d - Win32 Debug OpenGL" && "$(CFG)" != "vc40_d - Win32 Release DIB" &&\
 "$(CFG)" != "vc40_d - Win32 Debug DIB"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛ 'CFG' を定義することによって
!MESSAGE NMAKE 実行時にﾋﾞﾙﾄﾞ ﾓｰﾄﾞを指定できます。例えば:
!MESSAGE 
!MESSAGE NMAKE /f "test.mak" CFG="vc40_d - Win32 Debug DIB"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "vc40_d - Win32 Release OpenGL" ("Win32 (x86) Application" 用)
!MESSAGE "vc40_d - Win32 Debug OpenGL" ("Win32 (x86) Application" 用)
!MESSAGE "vc40_d - Win32 Release DIB" ("Win32 (x86) Application" 用)
!MESSAGE "vc40_d - Win32 Debug DIB" ("Win32 (x86) Application" 用)
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
# PROP Target_Last_Scanned "vc40_d - Win32 Debug DIB"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "vc40_d - Win32 Release OpenGL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release OpenGL"
# PROP BASE Intermediate_Dir "Release OpenGL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\test.exe"

CLEAN : 
	-@erase "$(INTDIR)\wiretxt.obj"
	-@erase "$(OUTDIR)\test.exe"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/test.pch" /YX /c 
CPP_OBJS=.\.
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/test.pdb" /machine:I386 /out:"$(OUTDIR)/test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\wiretxt.obj"

"$(OUTDIR)\test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vc40_d - Win32 Debug OpenGL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug OpenGL"
# PROP BASE Intermediate_Dir "Debug OpenGL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\test.exe"

CLEAN : 
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wiretxt.obj"
	-@erase "$(OUTDIR)\test.exe"
	-@erase "$(OUTDIR)\test.ilk"
	-@erase "$(OUTDIR)\test.pdb"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/test.pch" /YX /c 
CPP_OBJS=.\.
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/test.pdb" /debug /machine:I386 /out:"$(OUTDIR)/test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\wiretxt.obj"

"$(OUTDIR)\test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vc40_d - Win32 Release DIB"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release DIB"
# PROP BASE Intermediate_Dir "Release DIB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\test.exe"

CLEAN : 
	-@erase "$(INTDIR)\wiretxt.obj"
	-@erase "$(OUTDIR)\test.exe"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/test.pch" /YX /c 
CPP_OBJS=.\.
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/test.pdb"\
 /machine:I386 /out:"$(OUTDIR)/test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\wiretxt.obj"

"$(OUTDIR)\test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vc40_d - Win32 Debug DIB"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug DIB"
# PROP BASE Intermediate_Dir "Debug DIB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\test.exe"

CLEAN : 
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\wiretxt.obj"
	-@erase "$(OUTDIR)\test.exe"
	-@erase "$(OUTDIR)\test.ilk"
	-@erase "$(OUTDIR)\test.pdb"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/test.pch" /YX /c 
CPP_OBJS=.\.
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/test.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\wiretxt.obj"

"$(OUTDIR)\test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "vc40_d - Win32 Release OpenGL"
# Name "vc40_d - Win32 Debug OpenGL"
# Name "vc40_d - Win32 Release DIB"
# Name "vc40_d - Win32 Debug DIB"

!IF  "$(CFG)" == "vc40_d - Win32 Release OpenGL"

!ELSEIF  "$(CFG)" == "vc40_d - Win32 Debug OpenGL"

!ELSEIF  "$(CFG)" == "vc40_d - Win32 Release DIB"

!ELSEIF  "$(CFG)" == "vc40_d - Win32 Debug DIB"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\wiretxt.c
DEP_CPP_WIRET=\
	{$(INCLUDE)}"\impulse.h"\
	

!IF  "$(CFG)" == "vc40_d - Win32 Release OpenGL"


"$(INTDIR)\wiretxt.obj" : $(SOURCE) $(DEP_CPP_WIRET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "vc40_d - Win32 Debug OpenGL"


"$(INTDIR)\wiretxt.obj" : $(SOURCE) $(DEP_CPP_WIRET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "vc40_d - Win32 Release DIB"


"$(INTDIR)\wiretxt.obj" : $(SOURCE) $(DEP_CPP_WIRET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "vc40_d - Win32 Debug DIB"


"$(INTDIR)\wiretxt.obj" : $(SOURCE) $(DEP_CPP_WIRET) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
