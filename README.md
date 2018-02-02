# Pile Room Simulation

## Description

This is a GEANT4 implementation of an AmBe source as well as the response generated in one (or several) helium-3 tubes. 

## Installation

To build the software, run 'cmake ..' then 'make' from the build/ directory

## Usage

Before running, source this script:

       <geant_install_dir>/bin/geant4.sh

Run the program with

    PileRoomSim  -m macro.mac -o miscObjects.xml -d he3tubeDescription.xml -g graphiteDescription.xml -all -n nevents -r outfile -s seed1 seed2 -verbose -noGeo

Command line parameters are optional. If none specified, PileRoomSim opens in visualization mode.

	macro.mac:
		A GEANT4 macro. in.mac is an example which sets the energy spectrum of the AmBe and the output root file. 
		It runs for 500 events.	If this is specified, the arguments of -n, -r, and -s are ignored

	miscObjects.xml:
		Any additional objects to implement. Currently only Box and Tube are implemented. These can be located 
		anywhere in the simulated world, or can have their position tied to the location of one of the helium-3 
		tubes. If no file is specified, no additional objects are implemented.

	he3tubeDescription.xml:
		The description of the helium-3 tubes. An example is shown in xml/HE3TUBE.xml.
		If no description is specified, a default is used. (located at $XMLLOCATION/HE3TUBE-default.xml).

	graphiteDescription
		The description of the graphite cube. An example is shown in xml/Graphite.xml.
		If no description is specified, a default is used. (located at $XMLLOCATION/Graphite.xml).

	nevents
		Number if events to run for. An output file must be specified with -r.
		
	outfile
		output filename (without .root extension). Number of events must be specified with -n.

	seed1 seed2
		Seeds for random number generator.

	-all
		If this argument is used, all events are saved to the output ntuple. If not, only events where a neutron 
		is captured are saved. If you want to calculate the diffusion length of neutrons in the cube, this parameter 
		must be used.

	-verbose
		Prints all the GEANT4 debugging information.

	-noGeo
		If this argument is used, geometry information is not saved to the output file

Note: The environment variable XMLLOCATION should be set to the location of Graphite.xml and Room.xml. If this variable is not set, the program looks for these files in the xml/ directory

In xml/, there are python scripts called TubeMaker.py and GraphiteMaker.py. 

TubeMaker.py can be used to create an xml file containing a tube description, or append a new description to an existing xml file. Default values (read from HE3TUBE-default.xml) are used for any parameter not specified.

GraphiteMaker.py can be used to create an xml file containing a description of the graphite cube.

To see how to use these, run

     python <scriptname>.py -h


   
