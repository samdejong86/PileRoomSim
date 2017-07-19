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
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

option(STATIC_LIBS "Make a static build")
if(STATIC_LIBS)
  find_package(Geant4 REQUIRED static)
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
if(STATIC_LIBS)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  SET(BUILD_SHARED_LIBRARIES OFF)
  SET(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

add_executable(PileRoomSim PileRoomSim.cc ${sources} ${headers} ${TOOLS_FORTRAN_OBJECTS})
target_link_libraries(PileRoomSim ${Geant4_LIBRARIES} ${HBOOK_LIBRARIES})

if(STATIC_LIBS)
  #set(CMAKE_EVE_LINKER_FLAGS " -static")
  target_link_libraries(PileRoomSim -static-libgcc -static-libstdc++)
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build PileRoomSim. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(PileRoomSim_SCRIPTS
    rndmSeed.mac PileRoomSim.in PileRoomSim.out vis.mac
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

