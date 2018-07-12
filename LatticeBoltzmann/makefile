CC=cl
CFLAGS=-c -DSTRICT -G3 -Ow -W3 -Zp -Tp
CFLAGSMT=-c /D_WIN64 /D_AFXDLL /MD /nologo /W3 /WX- /O2 /Oi /Ot /Oy- /DWIN32 /D_WINDOWS /DNDEBUG /D_UNICODE /DUNICODE /Gm- /GS- /Gy /arch:SSE2 /fp:precise /Zc:wchar_t /Zc:forScope /Fax64\Release\ /Fox64\Release\ /Fdx64\Release\vc100.pdb /Gd /errorReport:none 
LINKER=link
GUIFLAGS=-SUBSYSTEM:windows /OUT:x64\Release\LatticeBoltzmann.exe /NOLOGO  /PDB:"x64\Release\LatticeBoltzmann.pdb" /SUBSYSTEM:WINDOWS /OPT:REF /OPT:NOICF /TLBID:1 /DYNAMICBASE /NXCOMPAT /MACHINE:X64 /ERRORREPORT:NONE /ENTRY:wWinMainCRTStartup /MANIFESTUAC:yes /MANIFESTUAC:level='asInvoker'
DLLFLAGS=-SUBSYSTEM:windows -DLL
GUILIBS=
RC=rc
RCVARS=/DWIN32 /D_WIN64  /DNDEBUG /D_UNICODE /DUNICODE /D_AFXDLL   /l0x0409 /Ix64\Release\ /nologo /fox64\Release\LatticeBoltzmann.res
MT=mt
MTSTUFF=/nologo /verbose  -manifest x64\Release\LatticeBoltzmann.exe.manifest

docks : trail x64\Release\LatticeBoltzmann.exe
	$(MT) $(MTSTUFF) -outputresource:x64\Release\LatticeBoltzmann.exe

x64\Release\LatticeBoltzmann.exe : x64\Release\MainFrm.obj x64\Release\Cell.obj x64\Release\ComputationPropPage.obj x64\Release\DisplayPropPage.obj x64\Release\InletOutletPropPage.obj x64\Release\Lattice.obj x64\Release\LatticeBoltzmann.obj x64\Release\LatticeBoltzmannDoc.obj x64\Release\LatticeBoltzmannView.obj x64\Release\MemoryBitmap.obj x64\Release\NumberEdit.obj x64\Release\Options.obj x64\Release\OptionsPropertySheet.obj x64\Release\SimulationPropPage.obj x64\Release\LatticeBoltzmann.res
    $(LINKER) $(GUIFLAGS) x64\Release\MainFrm.obj x64\Release\Cell.obj x64\Release\ComputationPropPage.obj x64\Release\DisplayPropPage.obj x64\Release\InletOutletPropPage.obj x64\Release\Lattice.obj x64\Release\LatticeBoltzmann.obj x64\Release\LatticeBoltzmannDoc.obj x64\Release\LatticeBoltzmannView.obj x64\Release\MemoryBitmap.obj x64\Release\NumberEdit.obj x64\Release\Options.obj x64\Release\OptionsPropertySheet.obj x64\Release\SimulationPropPage.obj x64\Release\LatticeBoltzmann.res $(GUILIBS)

trail :
	-@ if NOT EXIST "x64" mkdir "x64"
	-@ if NOT EXIST "x64\Release" mkdir "x64\Release"

x64\Release\MainFrm.obj : MainFrm.cpp  MainFrm.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) MainFrm.cpp

x64\Release\Cell.obj : Cell.cpp Cell.h  Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) Cell.cpp

x64\Release\ComputationPropPage.obj : ComputationPropPage.cpp  ComputationPropPage.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) ComputationPropPage.cpp

x64\Release\DisplayPropPage.obj : DisplayPropPage.cpp  DisplayPropPage.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) DisplayPropPage.cpp

x64\Release\InletOutletPropPage.obj : InletOutletPropPage.cpp  InletOutletPropPage.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) InletOutletPropPage.cpp

x64\Release\Lattice.obj :  Lattice.cpp Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) Lattice.cpp

x64\Release\LatticeBoltzmann.obj : LatticeBoltzmann.cpp  LatticeBoltzmann.h  Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) LatticeBoltzmann.cpp

x64\Release\LatticeBoltzmannDoc.obj :  LatticeBoltzmannDoc.cpp  LatticeBoltzmannDoc.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) LatticeBoltzmannDoc.cpp

x64\Release\LatticeBoltzmannView.obj : LatticeBoltzmannView.cpp  Lattice.h LatticeBoltzmannView.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) LatticeBoltzmannView.cpp

x64\Release\MemoryBitmap.obj :  MemoryBitmap.cpp MemoryBitmap.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) MemoryBitmap.cpp

x64\Release\NumberEdit.obj : NumberEdit.cpp NumberEdit.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) NumberEdit.cpp

x64\Release\Options.obj :  Options.cpp Lattice.h Options.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) Options.cpp

x64\Release\OptionsPropertySheet.obj :  OptionsPropertySheet.cpp OptionsPropertySheet.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) OptionsPropertySheet.cpp

x64\Release\SimulationPropPage.obj :  SimulationPropPage.cpp SimulationPropPage.h Lattice.h StdAfx.h resource.h 
     $(CC) $(CFLAGSMT) SimulationPropPage.cpp

x64\Release\LatticeBoltzmann.res : LatticeBoltzmann.rc resource.h res\LatticeBoltzmann.rc2 
	$(RC) $(RCVARS) LatticeBoltzmann.rc




