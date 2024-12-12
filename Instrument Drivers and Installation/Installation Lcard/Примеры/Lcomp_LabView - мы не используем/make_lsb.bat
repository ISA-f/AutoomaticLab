nmake -f readplatadescr.lvm clean
nmake -f getslotpar.lvm clean
nmake -f filladcparameters.lvm clean
nmake -f io_async.lvm clean
nmake -f setparametersstream.lvm clean
nmake -f getsyncdata.lvm clean

nmake -f readplatadescr.lvm
nmake -f getslotpar.lvm
nmake -f filladcparameters.lvm
nmake -f io_async.lvm
nmake -f setparametersstream.lvm
nmake -f getsyncdata.lvm

del *.obj
del *.map
del *.manifest
del *.pdb
