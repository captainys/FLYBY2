# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "IMPLS32.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : .\IMPLS32.lib .\WinRel\IMPLS32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"IMPLS32.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"IMPLS32.bsc" 
BSC32_SBRS= \
	.\WinRel\IWCPOLYG.SBR \
	.\WinRel\IWCCOLOR.SBR \
	.\WinRel\IWILDCAT.SBR \
	.\WinRel\IWIN95.SBR \
	.\WinRel\IEPSLINK.SBR \
	.\WinRel\IRENDLNK.SBR \
	.\WinRel\ITERRAIN.SBR \
	.\WinRel\ITWIST.SBR \
	.\WinRel\ITMPHEAP.SBR \
	.\WinRel\I3DGBBOX.SBR \
	.\WinRel\IRENDPLG.SBR \
	.\WinRel\IRENDER.SBR \
	.\WinRel\ITIFFILE.SBR \
	.\WinRel\I2DPICT.SBR \
	.\WinRel\IDEVICE.SBR \
	.\WinRel\IRENDTRI.SBR \
	.\WinRel\IEPSFILE.SBR \
	.\WinRel\ILNCROSS.SBR \
	.\WinRel\IMODEL.SBR \
	.\WinRel\I3DGOBJ.SBR \
	.\WinRel\ISCRN.SBR \
	.\WinRel\ICALC.SBR \
	.\WinRel\I3DG.SBR \
	.\WinRel\I2DG.SBR

.\WinRel\IMPLS32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO /OUT:"IMPLS32.lib"
LIB32_FLAGS=/NOLOGO /OUT:"IMPLS32.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	.\WinRel\IWCPOLYG.OBJ \
	.\WinRel\IWCCOLOR.OBJ \
	.\WinRel\IWILDCAT.OBJ \
	.\WinRel\IWIN95.OBJ \
	.\WinRel\IEPSLINK.OBJ \
	.\WinRel\IRENDLNK.OBJ \
	.\WinRel\ITERRAIN.OBJ \
	.\WinRel\ITWIST.OBJ \
	.\WinRel\ITMPHEAP.OBJ \
	.\WinRel\I3DGBBOX.OBJ \
	.\WinRel\IRENDPLG.OBJ \
	.\WinRel\IRENDER.OBJ \
	.\WinRel\ITIFFILE.OBJ \
	.\WinRel\I2DPICT.OBJ \
	.\WinRel\IDEVICE.OBJ \
	.\WinRel\IRENDTRI.OBJ \
	.\WinRel\IEPSFILE.OBJ \
	.\WinRel\ILNCROSS.OBJ \
	.\WinRel\IMODEL.OBJ \
	.\WinRel\I3DGOBJ.OBJ \
	.\WinRel\ISCRN.OBJ \
	.\WinRel\ICALC.OBJ \
	.\WinRel\I3DG.OBJ \
	.\WinRel\I2DG.OBJ

.\IMPLS32.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : .\IMPLS32.lib .\WinDebug\IMPLS32.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /W3 /GX /Z7 /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /W3 /GX /Z7 /YX /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
CPP_PROJ=/nologo /W3 /GX /Z7 /YX /O2 /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR$(INTDIR)/ /Fp$(OUTDIR)/"IMPLS32.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinDebug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"IMPLS32.bsc" 
BSC32_SBRS= \
	.\WinDebug\IWCPOLYG.SBR \
	.\WinDebug\IWCCOLOR.SBR \
	.\WinDebug\IWILDCAT.SBR \
	.\WinDebug\IWIN95.SBR \
	.\WinDebug\IEPSLINK.SBR \
	.\WinDebug\IRENDLNK.SBR \
	.\WinDebug\ITERRAIN.SBR \
	.\WinDebug\ITWIST.SBR \
	.\WinDebug\ITMPHEAP.SBR \
	.\WinDebug\I3DGBBOX.SBR \
	.\WinDebug\IRENDPLG.SBR \
	.\WinDebug\IRENDER.SBR \
	.\WinDebug\ITIFFILE.SBR \
	.\WinDebug\I2DPICT.SBR \
	.\WinDebug\IDEVICE.SBR \
	.\WinDebug\IRENDTRI.SBR \
	.\WinDebug\IEPSFILE.SBR \
	.\WinDebug\ILNCROSS.SBR \
	.\WinDebug\IMODEL.SBR \
	.\WinDebug\I3DGOBJ.SBR \
	.\WinDebug\ISCRN.SBR \
	.\WinDebug\ICALC.SBR \
	.\WinDebug\I3DG.SBR \
	.\WinDebug\I2DG.SBR

.\WinDebug\IMPLS32.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=lib.exe
# ADD BASE LIB32 /NOLOGO
# ADD LIB32 /NOLOGO /OUT:"IMPLS32.lib"
LIB32_FLAGS=/NOLOGO /OUT:"IMPLS32.lib" 
DEF_FLAGS=
DEF_FILE=
LIB32_OBJS= \
	.\WinDebug\IWCPOLYG.OBJ \
	.\WinDebug\IWCCOLOR.OBJ \
	.\WinDebug\IWILDCAT.OBJ \
	.\WinDebug\IWIN95.OBJ \
	.\WinDebug\IEPSLINK.OBJ \
	.\WinDebug\IRENDLNK.OBJ \
	.\WinDebug\ITERRAIN.OBJ \
	.\WinDebug\ITWIST.OBJ \
	.\WinDebug\ITMPHEAP.OBJ \
	.\WinDebug\I3DGBBOX.OBJ \
	.\WinDebug\IRENDPLG.OBJ \
	.\WinDebug\IRENDER.OBJ \
	.\WinDebug\ITIFFILE.OBJ \
	.\WinDebug\I2DPICT.OBJ \
	.\WinDebug\IDEVICE.OBJ \
	.\WinDebug\IRENDTRI.OBJ \
	.\WinDebug\IEPSFILE.OBJ \
	.\WinDebug\ILNCROSS.OBJ \
	.\WinDebug\IMODEL.OBJ \
	.\WinDebug\I3DGOBJ.OBJ \
	.\WinDebug\ISCRN.OBJ \
	.\WinDebug\ICALC.OBJ \
	.\WinDebug\I3DG.OBJ \
	.\WinDebug\I2DG.OBJ

.\IMPLS32.lib : $(OUTDIR)  $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\IWCPOLYG.C
DEP_IWCPO=\
	.\IWILDCAT.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IWCPOLYG.OBJ :  $(SOURCE)  $(DEP_IWCPO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IWCPOLYG.OBJ :  $(SOURCE)  $(DEP_IWCPO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IWCCOLOR.C
DEP_IWCCO=\
	.\IWILDCAT.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IWCCOLOR.OBJ :  $(SOURCE)  $(DEP_IWCCO) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IWCCOLOR.OBJ :  $(SOURCE)  $(DEP_IWCCO) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IWILDCAT.C
DEP_IWILD=\
	.\IWILDCAT.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IWILDCAT.OBJ :  $(SOURCE)  $(DEP_IWILD) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IWILDCAT.OBJ :  $(SOURCE)  $(DEP_IWILD) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\IWIN95.C
DEP_IWIN9=\
	..\IMPULSE.H\
	.\IWILDCAT.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IWIN95.OBJ :  $(SOURCE)  $(DEP_IWIN9) $(INTDIR)

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IWIN95.OBJ :  $(SOURCE)  $(DEP_IWIN9) $(INTDIR)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IEPSLINK.C
DEP_IEPSL=\
	..\IMPULSE.H\
	\SRC\IMPULSE\IEPSFILE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IEPSLINK.OBJ :  $(SOURCE)  $(DEP_IEPSL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IEPSLINK.OBJ :  $(SOURCE)  $(DEP_IEPSL) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IRENDLNK.C
DEP_IREND=\
	..\IMPULSE.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IRENDLNK.OBJ :  $(SOURCE)  $(DEP_IREND) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IRENDLNK.OBJ :  $(SOURCE)  $(DEP_IREND) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ITERRAIN.C
DEP_ITERR=\
	..\IMPULSE.H\
	\SRC\IMPULSE\I3DG.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ITERRAIN.OBJ :  $(SOURCE)  $(DEP_ITERR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ITERRAIN.OBJ :  $(SOURCE)  $(DEP_ITERR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ITWIST.C
DEP_ITWIS=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ITWIST.OBJ :  $(SOURCE)  $(DEP_ITWIS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ITWIST.OBJ :  $(SOURCE)  $(DEP_ITWIS) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ITMPHEAP.C
DEP_ITMPH=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ITMPHEAP.OBJ :  $(SOURCE)  $(DEP_ITMPH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ITMPHEAP.OBJ :  $(SOURCE)  $(DEP_ITMPH) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\I3DGBBOX.C
DEP_I3DGB=\
	..\IMPULSE.H\
	\SRC\IMPULSE\I3DG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\I3DGBBOX.OBJ :  $(SOURCE)  $(DEP_I3DGB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\I3DGBBOX.OBJ :  $(SOURCE)  $(DEP_I3DGB) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IRENDPLG.C
DEP_IRENDP=\
	..\IMPULSE.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IRENDPLG.OBJ :  $(SOURCE)  $(DEP_IRENDP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IRENDPLG.OBJ :  $(SOURCE)  $(DEP_IRENDP) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IRENDER.C
DEP_IRENDE=\
	..\IMPULSE.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IRENDER.OBJ :  $(SOURCE)  $(DEP_IRENDE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IRENDER.OBJ :  $(SOURCE)  $(DEP_IRENDE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ITIFFILE.C
DEP_ITIFF=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ITIFFILE.OBJ :  $(SOURCE)  $(DEP_ITIFF) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ITIFFILE.OBJ :  $(SOURCE)  $(DEP_ITIFF) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\I2DPICT.C
DEP_I2DPI=\
	..\IMPULSE.H\
	\SRC\IMPULSE\I3DG.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\I2DPICT.OBJ :  $(SOURCE)  $(DEP_I2DPI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\I2DPICT.OBJ :  $(SOURCE)  $(DEP_I2DPI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IDEVICE.C
DEP_IDEVI=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IDEVICE.OBJ :  $(SOURCE)  $(DEP_IDEVI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IDEVICE.OBJ :  $(SOURCE)  $(DEP_IDEVI) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IRENDTRI.C
DEP_IRENDT=\
	..\IMPULSE.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IRENDTRI.OBJ :  $(SOURCE)  $(DEP_IRENDT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IRENDTRI.OBJ :  $(SOURCE)  $(DEP_IRENDT) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IEPSFILE.C
DEP_IEPSF=\
	\SRC\IMPULSE\IEPSFILE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IEPSFILE.OBJ :  $(SOURCE)  $(DEP_IEPSF) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IEPSFILE.OBJ :  $(SOURCE)  $(DEP_IEPSF) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ILNCROSS.C
DEP_ILNCR=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ILNCROSS.OBJ :  $(SOURCE)  $(DEP_ILNCR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ILNCROSS.OBJ :  $(SOURCE)  $(DEP_ILNCR) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\IMODEL.C
DEP_IMODE=\
	..\IMPULSE.H\
	\SRC\IMPULSE\I3DG.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\IMODEL.OBJ :  $(SOURCE)  $(DEP_IMODE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\IMODEL.OBJ :  $(SOURCE)  $(DEP_IMODE) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\I3DGOBJ.C
DEP_I3DGO=\
	..\IMPULSE.H\
	\SRC\IMPULSE\I3DG.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\I3DGOBJ.OBJ :  $(SOURCE)  $(DEP_I3DGO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\I3DGOBJ.OBJ :  $(SOURCE)  $(DEP_I3DGO) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ISCRN.C
DEP_ISCRN=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ISCRN.OBJ :  $(SOURCE)  $(DEP_ISCRN) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ISCRN.OBJ :  $(SOURCE)  $(DEP_ISCRN) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\ICALC.C
DEP_ICALC=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\ICALC.OBJ :  $(SOURCE)  $(DEP_ICALC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\ICALC.OBJ :  $(SOURCE)  $(DEP_ICALC) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\I3DG.C
DEP_I3DG_=\
	..\IMPULSE.H\
	\SRC\IMPULSE\I3DG.H\
	\SRC\IMPULSE\IRENDER.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\I3DG.OBJ :  $(SOURCE)  $(DEP_I3DG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\I3DG.OBJ :  $(SOURCE)  $(DEP_I3DG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\SRC\IMPULSE\I2DG.C
DEP_I2DG_=\
	..\IMPULSE.H

!IF  "$(CFG)" == "Win32 Release"

.\WinRel\I2DG.OBJ :  $(SOURCE)  $(DEP_I2DG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Debug"

.\WinDebug\I2DG.OBJ :  $(SOURCE)  $(DEP_I2DG_) $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

!ENDIF 

# End Source File
# End Group
# End Project
################################################################################
