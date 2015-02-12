# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /curc/tools/x_86_64/rh6/cmake/3.0.0/bin/cmake

# The command to remove a file.
RM = /curc/tools/x_86_64/rh6/cmake/3.0.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pamo8800/lighthouse_phd_work/amesos2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pamo8800/lighthouse_phd_work/amesos2

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/amesos2_solver.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/amesos2_solver.cpp.o: amesos2_solver.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pamo8800/lighthouse_phd_work/amesos2/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/main.dir/amesos2_solver.cpp.o"
	/curc/tools/x_86_64/rh6/openmpi/1.8.3/gcc/4.9.1/bin/mpicxx   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/main.dir/amesos2_solver.cpp.o -c /home/pamo8800/lighthouse_phd_work/amesos2/amesos2_solver.cpp

CMakeFiles/main.dir/amesos2_solver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/amesos2_solver.cpp.i"
	/curc/tools/x_86_64/rh6/openmpi/1.8.3/gcc/4.9.1/bin/mpicxx  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/pamo8800/lighthouse_phd_work/amesos2/amesos2_solver.cpp > CMakeFiles/main.dir/amesos2_solver.cpp.i

CMakeFiles/main.dir/amesos2_solver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/amesos2_solver.cpp.s"
	/curc/tools/x_86_64/rh6/openmpi/1.8.3/gcc/4.9.1/bin/mpicxx  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/pamo8800/lighthouse_phd_work/amesos2/amesos2_solver.cpp -o CMakeFiles/main.dir/amesos2_solver.cpp.s

CMakeFiles/main.dir/amesos2_solver.cpp.o.requires:
.PHONY : CMakeFiles/main.dir/amesos2_solver.cpp.o.requires

CMakeFiles/main.dir/amesos2_solver.cpp.o.provides: CMakeFiles/main.dir/amesos2_solver.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/amesos2_solver.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/amesos2_solver.cpp.o.provides

CMakeFiles/main.dir/amesos2_solver.cpp.o.provides.build: CMakeFiles/main.dir/amesos2_solver.cpp.o

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/amesos2_solver.cpp.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

main: CMakeFiles/main.dir/amesos2_solver.cpp.o
main: CMakeFiles/main.dir/build.make
main: /home/pamo8800/TRILINOS_MPI/lib/libmesquite.a
main: /home/pamo8800/TRILINOS_MPI/lib/libmoochothyra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libmoocho.a
main: /home/pamo8800/TRILINOS_MPI/lib/librythmos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libmoertel.a
main: /home/pamo8800/TRILINOS_MPI/lib/liblocathyra.a
main: /home/pamo8800/TRILINOS_MPI/lib/liblocaepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/liblocalapack.a
main: /home/pamo8800/TRILINOS_MPI/lib/libloca.a
main: /home/pamo8800/TRILINOS_MPI/lib/libnoxepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libnoxlapack.a
main: /home/pamo8800/TRILINOS_MPI/lib/libnox.a
main: /home/pamo8800/TRILINOS_MPI/lib/libintrepid.a
main: /home/pamo8800/TRILINOS_MPI/lib/libfei_trilinos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libfei_base.a
main: /home/pamo8800/TRILINOS_MPI/lib/libstratimikos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libstratimikosbelos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libstratimikosaztecoo.a
main: /home/pamo8800/TRILINOS_MPI/lib/libstratimikosamesos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libstratimikosml.a
main: /home/pamo8800/TRILINOS_MPI/lib/libstratimikosifpack.a
main: /home/pamo8800/TRILINOS_MPI/lib/libifpack2-adapters.a
main: /home/pamo8800/TRILINOS_MPI/lib/libifpack2.a
main: /home/pamo8800/TRILINOS_MPI/lib/libanasazitpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libModeLaplace.a
main: /home/pamo8800/TRILINOS_MPI/lib/libanasaziepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libanasazi.a
main: /home/pamo8800/TRILINOS_MPI/lib/libbelostpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libbelosepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libbelos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libml.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkomplex.a
main: /home/pamo8800/TRILINOS_MPI/lib/libifpack.a
main: /home/pamo8800/TRILINOS_MPI/lib/libpamgen_extras.a
main: /home/pamo8800/TRILINOS_MPI/lib/libpamgen.a
main: /home/pamo8800/TRILINOS_MPI/lib/libamesos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libamesos2.a
main: /home/pamo8800/TRILINOS_MPI/lib/libgaleri-xpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libgaleri.a
main: /home/pamo8800/TRILINOS_MPI/lib/libaztecoo.a
main: /home/pamo8800/TRILINOS_MPI/lib/libdpliris.a
main: /home/pamo8800/TRILINOS_MPI/lib/libisorropia.a
main: /home/pamo8800/TRILINOS_MPI/lib/liboptipack.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyratpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyraepetraext.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyraepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyracore.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyratpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyraepetraext.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyraepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libthyracore.a
main: /home/pamo8800/TRILINOS_MPI/lib/libxpetra-sup.a
main: /home/pamo8800/TRILINOS_MPI/lib/libxpetra-ext.a
main: /home/pamo8800/TRILINOS_MPI/lib/libxpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libepetraext.a
main: /home/pamo8800/TRILINOS_MPI/lib/libtpetraext.a
main: /home/pamo8800/TRILINOS_MPI/lib/libtpetrainout.a
main: /home/pamo8800/TRILINOS_MPI/lib/libtpetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libtriutils.a
main: /home/pamo8800/TRILINOS_MPI/lib/libglobipack.a
main: /home/pamo8800/TRILINOS_MPI/lib/libshards.a
main: /home/pamo8800/TRILINOS_MPI/lib/libzoltan.a
main: /home/pamo8800/TRILINOS_MPI/lib/libepetra.a
main: /home/pamo8800/TRILINOS_MPI/lib/libsacado.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosdisttsqr.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosnodetsqr.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkoslinalg.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosnodeapi.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosTPL_unused_dummy.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosdisttsqr.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosnodetsqr.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkoslinalg.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosnodeapi.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkos.a
main: /home/pamo8800/TRILINOS_MPI/lib/libkokkosTPL_unused_dummy.a
main: /home/pamo8800/TRILINOS_MPI/lib/librtop.a
main: /home/pamo8800/TRILINOS_MPI/lib/libtpi.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchosremainder.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchosnumerics.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchoscomm.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchosparameterlist.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchoscore.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchosremainder.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchosnumerics.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchoscomm.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchosparameterlist.a
main: /home/pamo8800/TRILINOS_MPI/lib/libteuchoscore.a
main: /curc/tools/x_86_64/rh6/lapack/3.5.0/gcc/4.8.2/lib/liblapack.a
main: /curc/tools/x_86_64/rh6/lapack/3.5.0/gcc/4.8.2/lib/librefblas.a
main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: main
.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/requires: CMakeFiles/main.dir/amesos2_solver.cpp.o.requires
.PHONY : CMakeFiles/main.dir/requires

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /home/pamo8800/lighthouse_phd_work/amesos2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pamo8800/lighthouse_phd_work/amesos2 /home/pamo8800/lighthouse_phd_work/amesos2 /home/pamo8800/lighthouse_phd_work/amesos2 /home/pamo8800/lighthouse_phd_work/amesos2 /home/pamo8800/lighthouse_phd_work/amesos2/CMakeFiles/main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main.dir/depend

