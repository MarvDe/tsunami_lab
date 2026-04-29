/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "patches/WavePropagation1d.h"
#include "setups/DamBreak1d.h"
#include "setups/RareRare1d.h"
#include "setups/ShockShock1d.h"
#include "setups/TsunamiEvent1d.h"
#include "setups/SubcriticalFlow1d.h"
#include "setups/SupercriticalFlow1d.h"
#include "io/Csv.h"
#include "io/Parser.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <string>

int main( int   i_argc,
          char *i_argv[] ) {
  // number of cells in x- and y-direction
  tsunami_lab::t_idx l_nx = 0;
  tsunami_lab::t_idx l_ny = 1;
  
  // id of solver
  tsunami_lab::t_idx l_solverId = tsunami_lab::solvers::FWAVE;

  // id of setup
  tsunami_lab::t_idx l_setupId = tsunami_lab::setups::TSUNAMI_EVENT;

  // set cell size
  tsunami_lab::t_real l_dxy = 1;

  // outflow types
  tsunami_lab::t_idx l_outflowTypeL = 0;
  tsunami_lab::t_idx l_outflowTypeR = 0;

  // bathymetry file path
  std::string l_bathymetryFilePath = "profile_commas.csv";

  std::cout << "####################################" << std::endl;
  std::cout << "### Tsunami Lab                  ###" << std::endl;
  std::cout << "###                              ###" << std::endl;
  std::cout << "### https://scalable.uni-jena.de ###" << std::endl;
  std::cout << "####################################" << std::endl;


  // parse runtime arguments
  auto l_parser = tsunami_lab::io::Parser(i_argc, i_argv);

  // choose solver
  std::string l_solverName = l_parser.get("solver", "roe");
  if (l_solverName.compare("roe") == 0) l_solverId = tsunami_lab::solvers::ROE;
  else if (l_solverName.compare("fwave") == 0) l_solverId = tsunami_lab::solvers::FWAVE;
  else l_solverName = "roe";

  // choose setup
  std::string l_setupName = l_parser.get("setup", "damBreak");
  if (l_setupName.compare("damBreak") == 0) l_setupId = tsunami_lab::setups::DAM_BREAK;
  else if (l_setupName.compare("rareRare") == 0) l_setupId = tsunami_lab::setups::RARE_RARE;
  else if (l_setupName.compare("shockShock") == 0) l_setupId = tsunami_lab::setups::SHOCK_SHOCK;
  else if (l_setupName.compare("tsunamiEvent") == 0) l_setupId = tsunami_lab::setups::TSUNAMI_EVENT;
  else l_setupName = "damBreak";

  // select number of cells in x direction
  l_nx = l_parser.get("cellx", (tsunami_lab::t_idx)1);
  if (l_nx == 0) l_nx = 1;

  // select number of cells in x direction
  tsunami_lab::t_real l_endTime = l_parser.get("endtime", (tsunami_lab::t_real)3.0);
  if (l_endTime < 0.0) l_endTime = 3.0;

  std::cout << "runtime configuration" << std::endl;
  std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
  std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
  std::cout << "  cell size:                      " << l_dxy << std::endl;
  std::cout << "  solver:                         " << l_solverName << std::endl;
  std::cout << "  setup:                          " << l_setupName << std::endl;
  std::cout << "  end time:                       " << l_endTime << std::endl;


  // construct setup
  tsunami_lab::setups::Setup *l_setup;
  if (l_setupId == tsunami_lab::setups::RARE_RARE){
    l_setup = new tsunami_lab::setups::RareRare1d( 10,
                                                 5,
                                                 5 );
  }
  else if (l_setupId == tsunami_lab::setups::SHOCK_SHOCK){
    l_setup = new tsunami_lab::setups::ShockShock1d( 10,
                                                 5,
                                                 100 );
  }
  else if (l_setupId == tsunami_lab::setups::TSUNAMI_EVENT){

    std::cout << "Extracting bathymetry data from '" << l_bathymetryFilePath << "'." << std::endl;
    l_nx = 1762;
    l_dxy = 250;
    std::ifstream l_inFile(l_bathymetryFilePath); 
    if (!l_inFile.is_open()){
      std::cerr << "Could not open bathymetry file " << l_bathymetryFilePath << std::endl;
      return EXIT_FAILURE;
    }

    tsunami_lab::t_real *l_bathymetry = new tsunami_lab::t_real[l_nx]; 
    tsunami_lab::io::Csv::readBathymetry(l_nx, l_inFile, l_bathymetry);

    l_setup = new tsunami_lab::setups::TsunamiEvent1d(l_nx, l_bathymetry, l_dxy);
    // cleaning up 
    delete[] l_bathymetry;
  }
  else if(l_setupId == tsunami_lab::setups::SUBCRITICAL_FLOW){
    l_setup = new tsunami_lab::setups::SubcriticalFlow1d();
    l_dxy = 0.1;
    l_nx /=l_dxy;
  }
  else if(l_setupId == tsunami_lab::setups::SUPERCRITICAL_FLOW){
    l_setup = new tsunami_lab::setups::SupercriticalFlow1d();
    l_dxy = 0.1;
    l_nx /=l_dxy;
  }
  else{
    l_setup = new tsunami_lab::setups::DamBreak1d( 10,
                                                 5,
                                                 20 );
  }
  
  // construct solver
  tsunami_lab::patches::WavePropagation *l_waveProp;
  l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx, l_solverId, l_outflowTypeL, l_outflowTypeR );

  // maximum observed height in the setup
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();

  // set up solver
  for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ) {
    tsunami_lab::t_real l_y = l_cy * l_dxy; 

    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ) {
      tsunami_lab::t_real l_x = l_cx * l_dxy; 

      // get initial values of the setup
      tsunami_lab::t_real l_h = l_setup->getHeight( l_x,
                                                    l_y );
      l_hMax = std::max( l_h, l_hMax );

      tsunami_lab::t_real l_hu = l_setup->getMomentumX( l_x,
                                                        l_y );
      tsunami_lab::t_real l_hv = l_setup->getMomentumY( l_x,
                                                        l_y );
      tsunami_lab::t_real l_bathymetry = l_setup->getBathymetry( l_x, l_y );

      // set initial values in wave propagation solver
      l_waveProp->setHeight( l_cx,
                             l_cy,
                             l_h );

      l_waveProp->setMomentumX( l_cx,
                                l_cy,
                                l_hu );

      l_waveProp->setMomentumY( l_cx,
                                l_cy,
                                l_hv );
      
      l_waveProp->setBathymetry( l_cx, l_cy, l_bathymetry );

    }
  }

  // derive maximum wave speed in setup; the momentum is ignored
  tsunami_lab::t_real l_speedMax = std::sqrt( 9.81 * l_hMax );

  // derive constant time step; changes at simulation time are ignored
  tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

  // derive scaling for a time step
  tsunami_lab::t_real l_scaling = l_dt / l_dxy;

  // set up time and print control
  tsunami_lab::t_idx  l_timeStep = 0;
  tsunami_lab::t_idx  l_nOut = 0;
  tsunami_lab::t_real l_simTime = 0;

  std::cout << "entering time loop" << std::endl;

  // iterate over time
  while( l_simTime < l_endTime ){
    if( l_timeStep % 25 == 0 ) {
      std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;

      std::string l_path = "solution_" + std::to_string(l_nOut) + ".csv";
      std::cout << "  writing wave field to " << l_path << std::endl;

      std::ofstream l_file;
      l_file.open( l_path  );

      tsunami_lab::io::Csv::write( l_dxy,
                                   l_nx,
                                   1,
                                   1,
                                   l_waveProp->getHeight(),
                                   l_waveProp->getMomentumX(),
                                   nullptr,
                                   l_waveProp->getBathymetry(),
                                   l_file );
      l_file.close();
      l_nOut++;
    }

    l_waveProp->setGhostOutflow();
    l_waveProp->timeStep( l_scaling );

    l_timeStep++;
    l_simTime += l_dt;
  }

  std::cout << "finished time loop" << std::endl;

  // free memory
  std::cout << "freeing memory" << std::endl;
  delete l_setup;
  delete l_waveProp;

  std::cout << "finished, exiting" << std::endl;
  return EXIT_SUCCESS;
}
