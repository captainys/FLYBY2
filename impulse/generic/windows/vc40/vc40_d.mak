# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
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
!MESSAGE NMAKE /f "vc40_d.mak" CFG="vc40_d - Win32 Debug DIB"
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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

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

ALL : 

CLEAN : 
	-@erase 

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/vc40_d.pch" /YX /c 
CPP_OBJS=
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/vc40_d.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/vc40_d.pdb" /machine:I386 /nodefaultlib:"libc.lib"\
 /out:"$(OUTDIR)/vc40_d.exe" 
LINK32_OBJS=

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

ALL : 

CLEAN : 
	-@erase 

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/vc40_d.pch" /YX /c 
CPP_OBJS=
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/vc40_d.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32o.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/vc40_d.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib"\
 /out:"$(OUTDIR)/vc40_d.exe" 
LINK32_OBJS=

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

ALL : 

CLEAN : 
	-@erase 

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/vc40_d.pch" /YX /c 
CPP_OBJS=
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/vc40_d.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/vc40_d.pdb"\
 /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)/vc40_d.exe" 
LINK32_OBJS=

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

ALL : 

CLEAN : 
	-@erase 

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/vc40_d.pch" /YX /c 
CPP_OBJS=
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/vc40_d.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib impls32d.lib hotas32.lib shawk32.lib aurora32.lib winmm.lib\
 /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/vc40_d.pdb" /debug\
 /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)/vc40_d.exe" 
LINK32_OBJS=

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

# End Target
# End Project
################################################################################
