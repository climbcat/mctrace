# mctrace

A re-imagining of the classic mcstas neutron scattering simulation package.

Visual trace tool for mcstas neutron scattering instruments.


#### simcore

Some changes to the simulation core are noted.

There are two files: simlib.h and simcore.h. The first includes helper
libraries, and the second the actual simulation core macros and functions.

simlib changes

mcstas macros: references to FLAVOUR_* were disabled
MPI: MPI_MASTER macro has been out-commented. Any multi-threaded use would re-im>
monitor output functions:

mcdetector_out_0D
mcdetector_out_1D
mcdetector_out_2D
mcdetector_out_list
These functions should be re-introduced from the mccode codebase.( They
belong to in mccode-r and were deleted during port of simcore.h.)

sprintf: some helper sprintf caused a warning, disabled
particle_getvar: Function should be pulled from mcstas-generated code

