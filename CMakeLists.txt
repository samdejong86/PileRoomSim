#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(PileRoomSim)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
option(PORTABLE "Make a static build")

if(PORTABLE AND WITH_GEANT4_UIVIS)
  message("Cannot have portable build with visulization turned on. Visulization will be disabled")
  set(WITH_GEANT4_UIVIS OFF)
endif()

if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

if(PORTABLE)
  find_package(Geant4 REQUIRED static)
  set(Geant4_USES_QT5 0)
endif()


#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
if(PORTABLE)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  SET(BUILD_SHARED_LIBRARIES OFF)
  SET(CMAKE_EXE_LINKER_FLAGS "-static")

endif()

add_executable(PileRoomSim PileRoomSim.cc ${sources} ${headers} ${TOOLS_FORTRAN_OBJECTS})
target_link_libraries(PileRoomSim ${Geant4_LIBRARIES} ${HBOOK_LIBRARIES})


if(PORTABLE)
  target_link_libraries(PileRoomSim -static-libgcc -static-libstdc++)
  target_compile_options(PileRoomSim PUBLIC -DPORTABLE)
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build PileRoomSim. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(PileRoomSim_SCRIPTS
    in.mac vis.mac one.mac 43-Geant4-logoV5.png
  )
  
foreach(_script ${PileRoomSim_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS PileRoomSim DESTINATION bin)

