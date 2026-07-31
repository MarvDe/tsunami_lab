#include <catch2/catch.hpp>
#include <cstdio>
#include <string>

#include "CheckPoint.h"
#include "../io/NetCdf.h"

namespace {
  using tsunami_lab::t_idx;
  using tsunami_lab::t_real;
  using tsunami_lab::io::ArgType;
  using tsunami_lab::io::NetCdf;
  using tsunami_lab::setups::CheckPoint;
}

TEST_CASE( "CheckPoint reads metadata and last time slice from NetCDF", "[CheckPoint]" ) {
  /* Test was started with:
   * args:
   *  solverName: hybrid
   *  setupName: artificialTsunami
   *  cellSize: 1
   *  cellx: 1000
   *  celly: 1000
   *  endTime: 0
   *  timeSteps: 50
   * 
   * setup:
   *
   * tweaks:
   *  useEntropyFix: true
   *  manningFactor: 0.02
   *
   * output:
   *  formatName: nc
   *  outputInterval: 5
   *  outputResolution: 1
   *  compressionLevel: 0
   *  checkpointInterval: 10
   *  stations: ""
   *
   * omp:
   *  omp_num_threads: 8
   *  omp_schedule: static,8
   * 
   * run:
   * simulation time / #time steps: 0 / 0
   * simulation time / #time steps: 0.00798189 / 5
   * simulation time / #time steps: 0.0159638 / 10
   * simulation time / #time steps: 0.0239457 / 15
   * simulation time / #time steps: 0.0319275 / 20
   * simulation time / #time steps: 0.0399094 / 25
   * ^C⏎ 
   * 
   * => last saved timestep = 20 -> 5 written timesteps, index 4
   */
  std::string l_fileName = "utilities/checkpoint_test.nc";
  // std::string l_fileName = "solution.nc";


  t_real l_lastSimTime = 0;
  t_idx l_lastTimeStep = 0;
  t_idx l_nx = 0;
  t_idx l_ny = 0;
  t_real l_dxy = 0;
  t_real l_globalX = 0;
  t_real l_globalY = 0;

  {
    CheckPoint l_checkPoint( l_fileName,
                             l_lastSimTime,
                             l_lastTimeStep,
                             l_nx,
                             l_ny,
                             l_dxy,
                             l_globalX,
                             l_globalY );

    REQUIRE( l_lastSimTime == Approx( 0.0319275 ) );
    REQUIRE( l_lastTimeStep == 4 );
    REQUIRE( l_nx == 1000 );
    REQUIRE( l_ny == 1000 );
    REQUIRE( l_dxy == Approx( 1 ) );
    REQUIRE( l_globalX == Approx( 0 ) );
    REQUIRE( l_globalY == Approx( 0 ) );
  }
}

TEST_CASE( "CheckPoint reports NetCDF open errors", "[CheckPoint]" ) {
  t_real l_lastSimTime = 0;
  t_idx l_lastTimeStep = 0;
  t_idx l_nx = 0;
  t_idx l_ny = 0;
  t_real l_dxy = 0;
  t_real l_globalX = 0;
  t_real l_globalY = 0;

  REQUIRE_THROWS_AS( CheckPoint( "missing_checkpoint_setup_test.nc",
                                 l_lastSimTime,
                                 l_lastTimeStep,
                                 l_nx,
                                 l_ny,
                                 l_dxy,
                                 l_globalX,
                                 l_globalY ),
                     std::runtime_error );
}
