/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "constants.h"
#include "patches/WavePropagation1d.h"
#include "patches/WavePropagation2d.h"
#include "setups/DamBreak1d.h"
#include "setups/RareRare1d.h"
#include "setups/ShockShock1d.h"
#include "setups/TsunamiEvent1d.h"
#include "setups/SubcriticalFlow1d.h"
#include "setups/SupercriticalFlow1d.h"
#include "setups/CircularDamBreak2d.h"
#include "setups/ArtificialTsunami2d.h"
#include "setups/TsunamiEvent2d.h"
#include "setups/CheckPoint.h"
#include "io/Csv.h"
#include "io/Parser.h"
#include "io/Stations.h"
#include "io/NetCdf.h"
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

  // id of output format
  tsunami_lab::t_idx l_formatId = tsunami_lab::io::CSV;
  
  // set cell size
  tsunami_lab::t_real l_dxy = 1;
  
  // coordinates of upper left cell
  tsunami_lab::t_real l_left = 0;
  tsunami_lab::t_real l_upper = 0;

  // outflow types
  tsunami_lab::t_idx l_outflowTypeL = 0;
  tsunami_lab::t_idx l_outflowTypeR = 0;
  
  // amount of cells which will be merged together in output (= 1: every cell will be written, > 1: cells will be merged)
  tsunami_lab::t_idx l_outputResolution = 1;  

  // bathymetry file path
  std::string l_bathymetryFilePath = "profile_commas.csv";
  
  // bathymetry nc file path
  std::string l_bathymetryNCFilePath = "large_data/tohoku_gebco08_ucsb3_250m_bath.nc";//"utilities/artificialtsunami_bathymetry_1000.nc";

  // displacement nc file path
  std::string l_displacementNCFilePath = "large_data/tohoku_gebco08_ucsb3_250m_displ.nc"; //"utilities/artificialtsunami_displ_1000.nc";
  // max simulation time
  tsunami_lab::t_real l_endTime;

  // path to station file
  std::string l_stationsFilePath;

  // path to check point file
  std::string l_checkPointFilePath = "src/utilities/artificialTsunami_crashed.nc";

  // start timestep of simulation
  tsunami_lab::t_idx  l_timeStep = 0;
  
  // start time of simulation
  tsunami_lab::t_real l_simTime = 0;

  // flag to append data when using checkpoints
  bool l_appendFile = false;
  
  

  std::cout << "####################################" << std::endl;
  std::cout << "### Tsunami Lab                  ###" << std::endl;
  std::cout << "###                              ###" << std::endl;
  std::cout << "### https://scalable.uni-jena.de ###" << std::endl;
  std::cout << "####################################" << std::endl;


  // parse runtime arguments
  auto l_parser = tsunami_lab::io::Parser(i_argc, i_argv);

  std::string l_setupFile = l_parser.get("args", "");

  std::string l_solverName = "fwave";
  std::string l_setupName = "damBreak";
  std::string l_formatName = "NONE";

  if (l_setupFile.compare("") != 0){
    l_parser.parseFile( l_setupFile,
                        l_solverName,
                        l_setupName,
                        l_formatName,
                        l_dxy,
                        l_bathymetryNCFilePath,
                        l_displacementNCFilePath,
                        l_nx,
                        l_ny,
                        l_endTime,
                        l_stationsFilePath,
                        l_left,
                        l_upper,
                        l_checkPointFilePath,
                        l_appendFile,
                        l_outputResolution
                        );
  }
  else {
    // choose solver
    l_solverName = l_parser.get("solver", "roe");
    
  
    // choose setup
    l_setupName = l_parser.get("setup", "damBreak");
    
  
    // choose ouput format
    l_formatName = l_parser.get("format", "csv");
    
  
    // select number of cells in x direction
    l_nx = l_parser.get("cellx", (tsunami_lab::t_idx)1);
    
  
    l_ny = l_parser.get("celly", (tsunami_lab::t_idx)1);
  
    // select number of cells in x direction
    l_endTime = l_parser.get("endtime", (tsunami_lab::t_real)3.0);

  
    // set stations yaml file;
    l_stationsFilePath = l_parser.get("stations", "");

    // select size of cell
    l_dxy = l_parser.get("dxy", (tsunami_lab::t_real)1);

    // select left most coordiante
    l_left = l_parser.get("left", (tsunami_lab::t_real)0);

    // select upper most coordinate
    l_upper = l_parser.get("upper", (tsunami_lab::t_real)0);

    l_outputResolution = l_parser.get("res", (tsunami_lab::t_idx) 1);
  }

  if (l_solverName.compare("roe") == 0) l_solverId = tsunami_lab::solvers::ROE;
  else if (l_solverName.compare("fwave") == 0) l_solverId = tsunami_lab::solvers::FWAVE;
  else l_solverName = "roe";
  
  if (l_setupName.compare("damBreak") == 0) l_setupId = tsunami_lab::setups::DAM_BREAK;
  else if (l_setupName.compare("rareRare") == 0) l_setupId = tsunami_lab::setups::RARE_RARE;
  else if (l_setupName.compare("shockShock") == 0) l_setupId = tsunami_lab::setups::SHOCK_SHOCK;
  else if (l_setupName.compare("tsunamiEvent") == 0) l_setupId = tsunami_lab::setups::TSUNAMI_EVENT;
  else if (l_setupName.compare("supercriticalFlow") == 0) l_setupId = tsunami_lab::setups::SUPERCRITICAL_FLOW;
  else if (l_setupName.compare("subcriticalFlow") == 0) l_setupId = tsunami_lab::setups::SUBCRITICAL_FLOW;
  else if (l_setupName.compare("damBreak2d") == 0) l_setupId = tsunami_lab::setups::DAM_BREAK_2D;
  else if (l_setupName.compare("artificialTsunami") == 0) l_setupId = tsunami_lab::setups::ARTIFICIAL_TSUNAMI_2D;
  else if (l_setupName.compare("tsunamiEvent2d") == 0) l_setupId = tsunami_lab::setups::TSUNAMI_EVENT_2D;
  else if (l_setupName.compare("checkPoint") == 0) l_setupId = tsunami_lab::setups::CHECK_POINT;
  else l_setupName = "damBreak";

  if (l_formatName.compare("nc") == 0) l_formatId = tsunami_lab::io::NC;
  else if (l_formatName.compare("NONE") == 0) l_formatId = tsunami_lab::io::NONE;
  else if (l_formatName.compare("csv")) l_formatId = tsunami_lab::io::CSV;
  else l_formatId = tsunami_lab::io::CSV;

  if (l_nx == 0) l_nx = 1;

  if (l_ny == 0) l_ny = 1;

  if (l_endTime < 0.0) l_endTime = 3.0;

  std::cout << "runtime configuration" << std::endl;
  std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
  std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
  std::cout << "  cell size:                      " << l_dxy << std::endl;
  std::cout << "  solver:                         " << l_solverName << std::endl;
  std::cout << "  setup:                          " << l_setupName << std::endl;
  std::cout << "  format:                         " << l_formatName << std::endl;
  std::cout << "  end time:                       " << l_endTime << std::endl;
  std::cout << "  output resolution:              " << l_outputResolution << std::endl;

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
                                                 50 );
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
  else if(l_setupId == tsunami_lab::setups::DAM_BREAK_2D){
    constexpr int l_cellsX = 100;
    constexpr int l_cellsY = 100;
    l_dxy = 1;
    l_nx = l_cellsX;
    l_ny = l_cellsY;
    tsunami_lab::t_real l_bathymetry[l_cellsX*l_cellsY];
    for (int i = 0; i < l_cellsY; i++){
      for (int j = 0; j < l_cellsX; j++){
        //l_bathymetry[j + l_cellsX*i] = - ( (i-l_cellsY/2)*(i-l_cellsY/2) + (j-l_cellsX/2)*(j-l_cellsX/2) ) * 0.01;
        //l_bathymetry[j+ l_cellsX*i] = std::sin(j*0.1);
        //l_bathymetry[j + l_cellsX*i] -= 30;
        //tsunami_lab::t_real l_cx = l_cellsX / 2 - j + 30;
        //tsunami_lab::t_real l_cy = l_cellsY / 2 - i + 30;
        //tsunami_lab::t_real l_r = std::sqrt(l_cx * l_cx + l_cy * l_cy);
        //l_r <= 20 && l_cx < 10 && l_cy < 10
        if (j > 20 && j < 25 && (i < 40 || i > 60)){
          //l_bathymetry[j + l_cellsX*i] = 10;
        }

        l_bathymetry[j + l_cellsX * i] = -50;
      }
    }
    l_setup = new tsunami_lab::setups::CircularDamBreak2d(50,
                                                          l_bathymetry,
                                                          10,
                                                          l_nx,
                                                          l_ny,
                                                          1);
  }
  else if (l_setupId == tsunami_lab::setups::ARTIFICIAL_TSUNAMI_2D){
    l_setup = new tsunami_lab::setups::ArtificialTsunami2d;
    l_dxy = 1;
    l_nx = 1000;
    l_ny = 1000;
  }
  else if (l_setupId == tsunami_lab::setups::TSUNAMI_EVENT_2D){
    tsunami_lab::t_idx l_bX = 0;
    tsunami_lab::t_idx l_bY = 0;
    tsunami_lab::t_idx l_dX = 0;
    tsunami_lab::t_idx l_dY = 0;
    tsunami_lab::t_real l_dxyBat = 0;
    tsunami_lab::t_real l_dxyDis = 0;
    tsunami_lab::t_real l_leftBat = 0;
    tsunami_lab::t_real l_leftDis = 0;
    tsunami_lab::t_real l_upperBat = 0;
    tsunami_lab::t_real l_upperDis = 0;
  
    tsunami_lab::t_real * l_bathymetry = nullptr;
    tsunami_lab::t_real * l_displacement = nullptr;

    int l_batRes = tsunami_lab::io::NetCdf::read(l_bathymetryNCFilePath, l_bX, l_bY, l_dxyBat, l_leftBat, l_upperBat, &l_bathymetry);
    int l_disRes = tsunami_lab::io::NetCdf::read(l_displacementNCFilePath, l_dX, l_dY, l_dxyDis, l_leftDis, l_upperDis, &l_displacement);

    if (l_batRes || l_disRes){
      std::cout << "error reading bathymetry or displacement" << std::endl;
    }

    l_setup = new tsunami_lab::setups::TsunamiEvent2d(l_nx, l_ny, l_dxy, l_left, l_upper, l_bX, l_bY, l_dxyBat, l_leftBat, l_upperBat, 
                                                      l_dX, l_dY, l_dxyDis, l_leftDis, l_upperDis, l_bathymetry, l_displacement);
  
    delete[] l_bathymetry;
    delete[] l_displacement;
  }
  else if (l_setupId == tsunami_lab::setups::CHECK_POINT){
    //l_checkPointFilePath = "utilities/solution.nc";
    //l_solverId = tsunami_lab::solvers::FWAVE;
    //l_formatId = tsunami_lab::io::NC;
    //l_endTime = 20;
    //l_appendFile = true;
    l_setup = new tsunami_lab::setups::CheckPoint(  l_checkPointFilePath,
                                                    l_simTime,
                                                    l_timeStep,
                                                    l_nx,
                                                    l_ny,
                                                    l_dxy,
                                                    l_left,
                                                    l_upper
                                                  );
    std::cout << "CheckPoint: " << std::endl;
    std::cout << "sim time: " << l_simTime << std::endl;
    std::cout << "time steps: " << l_timeStep << std::endl;
    std::cout << "nx: " << l_nx << std::endl;
    std::cout << "ny: " << l_ny << std::endl;
    std::cout << "dxy: " << l_dxy << std::endl;
  }
  else{
    l_setup = new tsunami_lab::setups::DamBreak1d( 50,
                                                 100,
                                                 60 );
  }
  
  // construct solver
  tsunami_lab::patches::WavePropagation *l_waveProp;
  if (l_setupId == tsunami_lab::setups::DAM_BREAK_2D ||
      l_setupId == tsunami_lab::setups::ARTIFICIAL_TSUNAMI_2D ||
      l_setupId == tsunami_lab::setups::TSUNAMI_EVENT_2D ||
      l_setupId == tsunami_lab::setups::CHECK_POINT ){
    l_waveProp = new tsunami_lab::patches::WavePropagation2d( l_nx,
                                                              l_ny,
                                                              l_solverId);
  } 
  else {
    l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx, l_solverId, l_outflowTypeL, l_outflowTypeR );
  }

  // maximum observed height in the setup
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();
  tsunami_lab::t_real l_uMaxAbs = 0;
  tsunami_lab::t_real l_vMaxAbs = 0;

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

  // setup stations for measurement
  
  tsunami_lab::io::Stations l_stations(l_nx, l_ny, l_dxy);
  if (l_stationsFilePath.compare("") != 0){
    l_stations.readFile(l_stationsFilePath);
  } 

  // derive maximum wave speed in setup;  
  tsunami_lab::t_real l_xSpeedMax = std::sqrt( 9.81f * l_hMax ) + l_uMaxAbs;
  tsunami_lab::t_real l_ySpeedMax = std::sqrt( 9.81f * l_hMax ) + l_vMaxAbs;

  // derive constant time step; changes at simulation time are ignored
  tsunami_lab::t_real l_dt = 0.5 * l_dxy / (l_xSpeedMax + l_ySpeedMax);
  std::cout << "delta time: " << l_dt << std::endl; 

  // derive scaling for a time step
  tsunami_lab::t_real l_scaling = l_dt / l_dxy;

  // set up time and print control
  tsunami_lab::t_idx  l_nOut = 0;


  std::cout << "entering time loop" << std::endl;

  //setup NetCdf
  tsunami_lab::io::NetCdf* l_netCdf = nullptr;
  if (l_formatId == tsunami_lab::io::NC){
    if (!l_appendFile){
      l_netCdf = new tsunami_lab::io::NetCdf( l_nx,
                                              l_ny,
                                              l_dxy,
                                              l_dt,
                                              l_left,
                                              l_upper,
                                              l_outputResolution,
                                              "solution.nc");
    } else {
      l_netCdf = new tsunami_lab::io::NetCdf( l_nx,
                                              l_ny,
                                              l_dxy,
                                              l_dt,
                                              l_left,
                                              l_upper,
                                              l_outputResolution,
                                              l_checkPointFilePath,
                                              true);
    }
  }

  // iterate over time
  while( l_simTime < l_endTime ){
    if( l_timeStep % 25 == 0 ) {
      std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;
      if (l_formatId == tsunami_lab::io::CSV){
        std::string l_path = "solution_" + std::to_string(l_nOut) + ".csv";
        std::cout << "  writing wave field to " << l_path << std::endl;
  
        std::ofstream l_file;
        l_file.open( l_path  );
  
        tsunami_lab::io::Csv::write( l_dxy,
                                      l_nx,
                                      l_ny,
                                      l_waveProp->getStride(),
                                      l_waveProp->getHeight(),
                                      l_waveProp->getMomentumX(),
                                      l_waveProp->getMomentumY(),
                                      l_waveProp->getBathymetry(),
                                      l_file );
      
        l_file.close();
        l_nOut++;
      }
      else if (l_formatId == tsunami_lab::io::NC){
        l_netCdf->write( l_nx,
                        l_ny,
                        l_nOut,
                        l_simTime,
                        l_waveProp->getStride(),
                        l_waveProp->getHeight(),
                        l_waveProp->getMomentumX(),
                        l_waveProp->getMomentumY(),
                        l_waveProp->getBathymetry());
        l_nOut++;
      }
    }
    if (l_stationsFilePath.compare("") != 0){
      l_stations.write(
        l_simTime,
        l_waveProp->getHeight(),
        l_waveProp->getMomentumX(),
        l_waveProp->getMomentumY(),
        l_waveProp->getBathymetry(),
        l_waveProp->getStride()
      );
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
  if (l_formatId == tsunami_lab::io::NC){
    delete l_netCdf;
  }
  
  std::cout << "finished, exiting" << std::endl;
  return EXIT_SUCCESS;
}
