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
#include "setups/SingleWaveCanonicalIsland.h"
#include "io/Csv.h"
#include "io/Parser.h"
#include "io/Stations.h"
#include "io/NetCdf.h"
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

namespace {
  std::string setupArgTypeToString( tsunami_lab::io::ArgType i_type ) {
    switch( i_type ) {
      case tsunami_lab::io::ArgType::Bool:   return "bool";
      case tsunami_lab::io::ArgType::Int:    return "int";
      case tsunami_lab::io::ArgType::Real:   return "real";
      case tsunami_lab::io::ArgType::String: return "string";
    }

    return "unknown";
  }

  std::string setupArgValueToString( tsunami_lab::io::ArgValue const &i_value ) {
    std::ostringstream l_stream;

    std::visit( [&l_stream]( auto const &i_current ) {
      l_stream << i_current;
    }, i_value );

    return l_stream.str();
  }

  std::vector<std::string> getSortedSetupNames() {
    std::vector<std::string> l_setupNames;
    l_setupNames.reserve( tsunami_lab::io::Parser::SETUP_DEFS.size() );

    for( auto const &l_setupDef: tsunami_lab::io::Parser::SETUP_DEFS ) {
      l_setupNames.push_back( l_setupDef.first );
    }

    std::sort( l_setupNames.begin(), l_setupNames.end() );
    return l_setupNames;
  }

  void printHelp( char const *i_programName ) {
    std::cout << "\n"
              << "Usage:\n"
              << "  " << i_programName << " [command]\n"
              << "  " << i_programName << " [key=value ...]\n\n"
              << "Simulate one- and two-dimensional tsunami scenarios.\n\n"
              << "Commands:\n"
              << "  <no arguments>               Print this help and exit.\n"
              << "  help, -h, --help             Print this help and exit.\n"
              << "  printSetups                  List all available setups.\n"
              << "  printSetup=<name>            Print setup-specific YAML arguments.\n"
              << "  printSetupInfo=<name>        Same as printSetup=<name>.\n"
              << "  printSolvers                 Print all available solvers.\n"
              << "  printFormats                 Print all available output formats.\n\n"
              << "Configuration:\n"
              << "  args=<file>                  Read simulation arguments from a YAML file.\n\n"
              << "Common key=value arguments:\n"
              << "  solver=<name>                Solver: roe, fwave, hlle, hybrid,\n"
              << "                               fwave_hydrostatic_reconstruction. Default: roe.\n"
              << "  setup=<name>                 Setup name. Default: damBreak.\n"
              << "  format=<name>                Output format: csv, nc, NONE. Default: csv.\n"
              << "  cellx=<n>                    Number of cells in x-direction. Default: 1.\n"
              << "  celly=<n>                    Number of cells in y-direction. Default: 1.\n"
              << "  endtime=<t>                  End time of the simulation. Default: 3.0.\n"
              << "  dxy=<dx>                     Cell size. Default: 1.\n"
              << "  left=<x>                     X-coordinate of the upper-left cell. Default: 0.\n"
              << "  upper=<y>                    Y-coordinate of the upper-left cell. Default: 0.\n"
              << "  stations=<file>              YAML file with station definitions.\n"
              << "  res=<n>                      Output resolution. Default: 1.\n\n"
              << "Examples:\n"
              << "  " << i_programName << " help\n"
              << "  " << i_programName << " printSetups\n"
              << "  " << i_programName << " printSetup=damBreak\n"
              << "  " << i_programName << " args=utilities/args/argsDambreak1d.yml\n"
              << "  " << i_programName << " setup=damBreak solver=fwave cellx=100 endtime=3\n";
  }

  void printSetups() {
    std::cout << "\nAvailable setups:\n";

    for( std::string const &l_setupName: getSortedSetupNames() ) {
      std::cout << "  " << l_setupName << "\n";
    }
  }

  void printFormats() {
    std::cout << "\nAvailable output formats:\n";

    std::cout << "  " << "nc:        for large two dimensional problems with compression\n"
              << "  " << "csv:       simple ascii encoding\n"
              << "  " << "NONE:      no output, good for benchmarking\n";
  }

  void printSetupInfo( std::string const &i_setupName ) {
    auto l_setupDef = tsunami_lab::io::Parser::SETUP_DEFS.find( i_setupName );

    if( l_setupDef == tsunami_lab::io::Parser::SETUP_DEFS.end() ) {
      std::cerr << "Unknown setup: " << i_setupName << "\n";
      printSetups();
      return;
    }

    std::cout << "\nSetup: " << l_setupDef->second.name << "\n";

    if( l_setupDef->second.args.empty() ) {
      std::cout << "  No setup-specific arguments.\n";
      return;
    }

    std::cout << "YAML setup arguments:\n";
    for( tsunami_lab::io::SetupArgDef const &l_arg: l_setupDef->second.args ) {
      std::cout << "  " << l_arg.name
                << " (" << setupArgTypeToString( l_arg.type ) << ")";

      if( l_arg.required ) {
        std::cout << ", required";
      }
      else {
        std::cout << ", optional";
      }

      if( l_arg.fallback.has_value() ) {
        std::cout << ", default: " << setupArgValueToString( l_arg.fallback.value() );
      }

      std::cout << "\n";
    }
  }

  void printSolvers() {
    std::cout << "\nAvailable solvers:\n";

    std::cout << "  " << "fwave:       for one and two dimensional setups, dry cells reflect water.\n"
              << "  " << "hlle:        for one and two dimensional setups, especially good for supercritical flows.\n"
              << "  " << "roe:         for one dimensional setups.\n"
              << "  " << "hybrid:      composed of fwave and hlle and other tweaks, capable of solving anything, but with increased compute time.\n";
  }
}

int main( int   i_argc,
          char *i_argv[] ) {
  // number of cells in x- and y-direction
  tsunami_lab::t_idx l_nx = 0;
  tsunami_lab::t_idx l_ny = 1;
  
  // id of solver
  tsunami_lab::solvers::Ids l_solverId = tsunami_lab::solvers::FWAVE;

  // id of setup
  tsunami_lab::setups::Ids l_setupId = tsunami_lab::setups::TSUNAMI_EVENT;

  // id of output format
  tsunami_lab::io::Ids l_formatId = tsunami_lab::io::CSV;
  
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

  bool l_printHelp = i_argc == 1 || l_parser.get("help", "null").empty() || l_parser.get("h", "null").empty();
  if (l_printHelp) {
    printHelp( i_argv[0] );

    return 0;
  }

  bool l_printSetups = l_parser.get("printSetups","null").empty();
  if (l_printSetups) {
    printSetups();

    return 0;
  }

  bool l_printSolvers = l_parser.get("printSolvers", "null").empty();
  if (l_printSolvers) {
    printSolvers();

    return 0;
  }

  bool l_printFormats = l_parser.get("printFormats", "null").empty();
  if (l_printFormats) {
    printFormats();

    return 0;
  }



  std::string l_setupInfo = l_parser.get("printSetup", "");

  if (!l_setupInfo.empty()) {
    printSetupInfo( l_setupInfo );

    return 0;
  }

  

  std::string l_setupFile = l_parser.get("args", "");

  std::string l_solverName = "fwave";
  std::string l_setupName = "damBreak";
  std::string l_formatName = "NONE";
  tsunami_lab::io::SetupArgs l_setupArgs;
  bool l_useEntropyFix;
  tsunami_lab::t_real l_manningFactor;
  tsunami_lab::t_idx l_maxTimeStep;
  tsunami_lab::t_idx l_outputInterval = 10;
  tsunami_lab::t_idx l_compressionLevel = 0;
  tsunami_lab::t_idx l_checkpointInterval = 100;

  if (l_setupFile.compare("") != 0){
    l_parser.parseFile( l_setupFile,
                        l_solverName,
                        l_setupName,
                        l_formatName,
                        l_dxy,
                        // l_bathymetryNCFilePath,
                        // l_displacementNCFilePath,
                        l_nx,
                        l_ny,
                        l_endTime,
                        l_stationsFilePath,
                        l_checkPointFilePath,
                        l_appendFile,
                        l_outputResolution,
                        l_manningFactor,
                        l_useEntropyFix,
                        l_maxTimeStep,
                        l_outputInterval,
                        l_compressionLevel,
                        l_checkpointInterval,
                        l_setupArgs
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
  else if (l_solverName.compare("hlle") == 0) l_solverId = tsunami_lab::solvers::HLLE;
  else if (l_solverName.compare("hybrid") == 0) l_solverId = tsunami_lab::solvers::HYBRID;
  else {
    l_solverName = "fwave_hydrostatic_reconstruction";
    l_solverId = tsunami_lab::solvers::FWAVE_HYDROSTATIC_RECONSTRUCTION;
  }
  
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
  else if (l_setupName.compare("singleWaveCanonicalIsland") == 0) l_setupId = tsunami_lab::setups::SINGLE_WAVE_CANONICAL_ISLAND;
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
    l_setup = new tsunami_lab::setups::RareRare1d(l_setupArgs.get<tsunami_lab::t_real>("waterHeight"),
                                                  l_setupArgs.get<tsunami_lab::t_real>("waterMomentum"),
                                                  l_setupArgs.get<tsunami_lab::t_real>("locationDiscontinuity"));
  }
  else if (l_setupId == tsunami_lab::setups::SHOCK_SHOCK){
    l_setup = new tsunami_lab::setups::ShockShock1d(l_setupArgs.get<tsunami_lab::t_real>("waterHeight"),
                                                    l_setupArgs.get<tsunami_lab::t_real>("waterMomentum"),
                                                    l_setupArgs.get<tsunami_lab::t_real>("locationDiscontinuity"));
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
    bool l_bathymetryCup = false;
    bool l_bathymetryCap = false;
    if (l_setupArgs.get<bool>("bathymetryCup")){
      l_bathymetryCup = true;
    } else if (l_setupArgs.get<bool>("bathymetryCap")){
      l_bathymetryCap = true;
    }
    
    tsunami_lab::t_real l_bathymetry[l_cellsX*l_cellsY];
    for (int i = 0; i < l_cellsY; i++){
      for (int j = 0; j < l_cellsX; j++){
        if (l_bathymetryCap) {
          l_bathymetry[j + l_cellsX*i] =  50 -( (i-l_cellsY/2)*(i-l_cellsY/2) + (j-l_cellsX/2)*(j-l_cellsX/2) ) * 0.01;
        } else if (l_bathymetryCup) {
          l_bathymetry[j + l_cellsX*i] =  ( (i-l_cellsY/2)*(i-l_cellsY/2) + (j-l_cellsX/2)*(j-l_cellsX/2) ) * 0.01;
        } else {
          l_bathymetry[j + l_cellsX * i] = -50;
        }
      }
    }
    l_setup = new tsunami_lab::setups::CircularDamBreak2d(l_setupArgs.get<tsunami_lab::t_real>("innerWaterHeight"),
                                                          l_bathymetry,
                                                          l_setupArgs.get<tsunami_lab::t_real>("innerWateRadius"),
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

    int l_batRes = tsunami_lab::io::NetCdf::read(l_setupArgs.get<std::string>("bathymetry"), l_bX, l_bY, l_dxyBat, l_leftBat, l_upperBat, &l_bathymetry);
    int l_disRes = tsunami_lab::io::NetCdf::read(l_setupArgs.get<std::string>("displacement"), l_dX, l_dY, l_dxyDis, l_leftDis, l_upperDis, &l_displacement);

    l_left = l_setupArgs.get<tsunami_lab::t_real>("startCoordX");
    l_upper = l_setupArgs.get<tsunami_lab::t_real>("startCoordY");


    if (l_batRes || l_disRes){
      std::cout << "error reading bathymetry or displacement" << std::endl;
    }

    l_setup = new tsunami_lab::setups::TsunamiEvent2d(l_nx, l_ny, l_dxy, l_left, l_upper, l_bX, l_bY, l_dxyBat, l_leftBat, l_upperBat, 
                                                      l_dX, l_dY, l_dxyDis, l_leftDis, l_upperDis, l_bathymetry, l_displacement);
  
    delete[] l_bathymetry;
    delete[] l_displacement;
  }
  else if (l_setupId == tsunami_lab::setups::CHECK_POINT){
    l_checkPointFilePath = l_setupArgs.get<tsunami_lab::t_real>("inputFile");
    //l_solverId = tsunami_lab::solvers::FWAVE;
    //l_formatId = tsunami_lab::io::NC;
    //l_endTime = 20;
    l_appendFile = l_setupArgs.get<tsunami_lab::t_real>("appendFile");
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
  else if( l_setupId == tsunami_lab::setups::SINGLE_WAVE_CANONICAL_ISLAND){
    l_dxy = 0.1;
    l_nx = tsunami_lab::t_idx(25 / l_dxy);
    l_ny = tsunami_lab::t_idx(30 / l_dxy);
    std::cout << "nx: " << l_nx << ", ny: " << l_ny << ", dxy: " << l_dxy << std::endl;
    l_setup = new tsunami_lab::setups::SingleWaveCanonicalIsland();
  }
  else{
    l_setup = new tsunami_lab::setups::DamBreak1d( l_setupArgs.get<tsunami_lab::t_real>("heightLeft"),
                                                   l_setupArgs.get<tsunami_lab::t_real>("heightRight"),
                                                   l_setupArgs.get<tsunami_lab::t_real>("locationDam"));
    printf("heightL: %f\n", l_setupArgs.get<tsunami_lab::t_real>("heightLeft"));
  }
  
  // construct solver
  tsunami_lab::patches::WavePropagation *l_waveProp;
  if (l_setupId == tsunami_lab::setups::DAM_BREAK_2D ||
      l_setupId == tsunami_lab::setups::ARTIFICIAL_TSUNAMI_2D ||
      l_setupId == tsunami_lab::setups::TSUNAMI_EVENT_2D ||
      l_setupId == tsunami_lab::setups::CHECK_POINT ||
      l_setupId == tsunami_lab::setups::SINGLE_WAVE_CANONICAL_ISLAND){
    l_waveProp = new tsunami_lab::patches::WavePropagation2d( l_nx,
                                                              l_ny,
                                                              l_solverId,
                                                              l_useEntropyFix,
                                                              l_manningFactor);
  } 
  else {
    l_waveProp = new tsunami_lab::patches::WavePropagation1d( l_nx, l_solverId, l_outflowTypeL, l_outflowTypeR , l_useEntropyFix);
  }

  // maximum observed height in the setup
  tsunami_lab::t_real l_hMax = std::numeric_limits< tsunami_lab::t_real >::lowest();
  tsunami_lab::t_real l_uMaxAbs = 0;
  tsunami_lab::t_real l_vMaxAbs = 0;

  // set up solver
  #pragma omp parallel for reduction(max:l_hMax,l_uMaxAbs,l_vMaxAbs)
  for( tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++ ) {
    tsunami_lab::t_real l_y = l_cy * l_dxy; 

    for( tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++ ) {
      tsunami_lab::t_real l_x = l_cx * l_dxy; 

      // get initial values of the setup
      tsunami_lab::t_real l_h = l_setup->getHeight( l_x,
                                                    l_y );
      

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

      if (l_bathymetry > 10){
        std::cout << "Bath error: " << l_bathymetry << std::endl; 
      }

      if (l_solverId == tsunami_lab::solvers::FWAVE_HYDROSTATIC_RECONSTRUCTION){
        tsunami_lab::t_real l_xNext = (l_cx + 1) * l_dxy; 
        tsunami_lab::t_real l_yNext = (l_cy + 1) * l_dxy;

        // get in x direction 
        tsunami_lab::t_real l_bNext = l_setup->getBathymetry( l_xNext, l_y );
        
        tsunami_lab::t_real l_bHalf = std::max(l_bathymetry, l_bNext);

        tsunami_lab::t_real l_hComp = std::max( 
          tsunami_lab::t_real(0),
          l_h + l_bathymetry - l_bHalf
        );

        tsunami_lab::t_real l_uComp = (l_h > 0) ? l_hu / l_h : tsunami_lab::t_real(0);
        tsunami_lab::t_real l_vComp = (l_h > 0) ? l_hv / l_h : tsunami_lab::t_real(0);

        l_hMax = std::max( l_hComp, l_hMax );
        l_uMaxAbs = std::max(std::abs(l_uComp), l_uMaxAbs);
        l_vMaxAbs = std::max(std::abs(l_vComp), l_vMaxAbs);

        // get in y direction
        l_bNext = l_setup->getBathymetry( l_x, l_yNext );
        
        l_bHalf = std::max(l_bathymetry, l_bNext);

        l_hComp = std::max( 
          tsunami_lab::t_real(0),
          l_h + l_bathymetry - l_bHalf
        );

        l_hMax = std::max( l_hComp, l_hMax );
      }
      else{
        l_hMax = std::max( l_h, l_hMax );
        tsunami_lab::t_real l_u = l_hu * l_h;
        tsunami_lab::t_real l_v = l_hv * l_h;
        l_uMaxAbs = std::max(std::abs(l_u), l_uMaxAbs);
        l_vMaxAbs = std::max(std::abs(l_v), l_vMaxAbs);
      }

    }
  }

  // setup stations for measurement
  
  tsunami_lab::io::Stations l_stations(l_nx, l_ny, l_dxy, l_left, l_upper);
  if (!l_stationsFilePath.empty() && l_stationsFilePath != "null"){
    std::cout << "stations file path: " << l_stationsFilePath << "\n";
    l_stations.readFile(l_stationsFilePath);
  } 

  // derive maximum wave speed in setup;   
  std::cout << "h max: " << l_hMax << std::endl;
  tsunami_lab::t_real l_xSpeedMax = std::sqrt( 9.81f * l_hMax ) + l_uMaxAbs;
  tsunami_lab::t_real l_ySpeedMax = std::sqrt( 9.81f * l_hMax ) + l_vMaxAbs;

  // derive constant time step; changes at simulation time are ignored
  tsunami_lab::t_real l_dt = 0.5 * l_dxy / (l_xSpeedMax + l_ySpeedMax);
  std::cout << "delta time: " << l_dt << std::endl; 

  l_waveProp->setDt(l_dt);

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
                                              l_compressionLevel,
                                              "solution.nc");
    } else {
      l_netCdf = new tsunami_lab::io::NetCdf( l_nx,
                                              l_ny,
                                              l_dxy,
                                              l_dt,
                                              l_left,
                                              l_upper,
                                              l_outputResolution,
                                              l_compressionLevel,
                                              l_checkPointFilePath,
                                              true);
    }
  }

  // iterate over time

  double l_timeMeasure = 0;
  while((l_endTime == 0 || l_simTime < l_endTime) && (l_maxTimeStep == 0 || l_maxTimeStep > l_timeStep)){
    if( l_timeStep % l_outputInterval == 0 ) {
      std::cout << "  simulation time / #time steps: "
                << l_simTime << " / " << l_timeStep << std::endl;
      // float maxHu = 0;
      // for (tsunami_lab::t_idx i=1; i<=l_nx * l_ny; ++i){
      //   maxHu = std::max(maxHu,std::abs(l_waveProp->getMomentumX()[i]));
      // }
      // printf("maxHu = %f\n", maxHu);
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
        bool l_writeCheckpoint = l_checkpointInterval != 0 && l_timeStep % l_checkpointInterval == 0;
        l_netCdf->write( l_nx,
                        l_ny,
                        l_nOut,
                        l_simTime,
                        l_waveProp->getStride(),
                        l_waveProp->getHeight(),
                        l_waveProp->getMomentumX(),
                        l_waveProp->getMomentumY(),
                        l_waveProp->getBathymetry(),
                        l_writeCheckpoint);
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

    auto l_timeStart = std::chrono::steady_clock::now();

    l_waveProp->setGhostOutflow();
    l_waveProp->timeStep( l_scaling );

    auto l_timeEnd = std::chrono::steady_clock::now();
    l_timeMeasure += std::chrono::duration_cast<std::chrono::duration<double>>(l_timeEnd - l_timeStart).count();

    l_timeStep++;
    l_simTime += l_dt;
  }

  std::cout << "finished time loop" << std::endl;
  std::cout << "total time measured: " << l_timeMeasure << std::endl;
  std::cout << "normalized time measured: " << l_timeMeasure / (l_nx * l_ny * l_timeStep) << std::endl;

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
