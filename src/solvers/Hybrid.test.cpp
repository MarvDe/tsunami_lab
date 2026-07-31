#include <catch2/catch.hpp>
#include <cmath>

#include "Hybrid.h"
#include "F_wave.h"
#include "hlle.h"

namespace {
  using tsunami_lab::t_real;
  using tsunami_lab::solvers::Fwave;
  using tsunami_lab::solvers::Hlle;
  using tsunami_lab::solvers::Hybrid;

  void requireFinite2( t_real const i_values[2] ) {
    REQUIRE( std::isfinite( i_values[0] ) );
    REQUIRE( std::isfinite( i_values[1] ) );
  }

  void requireFinite3( t_real const i_values[3] ) {
    REQUIRE( std::isfinite( i_values[0] ) );
    REQUIRE( std::isfinite( i_values[1] ) );
    REQUIRE( std::isfinite( i_values[2] ) );
  }
}

TEST_CASE( "Hybrid hydrostatic reconstruction preserves flat bathymetry", "[Hybrid]" ) {
  t_real l_hL = 0;
  t_real l_hR = 0;
  t_real l_huL = 0;
  t_real l_huR = 0;
  t_real l_sourceL = 1;
  t_real l_sourceR = 1;

  Hybrid::hydrostatic_reconstruction( 0,
                                      0,
                                      4,
                                      4,
                                      8,
                                      -12,
                                      l_hL,
                                      l_hR,
                                      l_huL,
                                      l_huR,
                                      l_sourceL,
                                      l_sourceR );

  REQUIRE( l_hL == Approx( 4 ) );
  REQUIRE( l_hR == Approx( 4 ) );
  REQUIRE( l_huL == Approx( 8 ) );
  REQUIRE( l_huR == Approx( -12 ) );
  REQUIRE( l_sourceL == Approx( 0 ) );
  REQUIRE( l_sourceR == Approx( 0 ) );
}

TEST_CASE( "Hybrid hydrostatic reconstruction clips at bathymetry steps", "[Hybrid]" ) {
  t_real l_hL = 0;
  t_real l_hR = 0;
  t_real l_huL = 0;
  t_real l_huR = 0;
  t_real l_sourceL = 0;
  t_real l_sourceR = 0;

  Hybrid::hydrostatic_reconstruction( 2,
                                      5,
                                      10,
                                      8,
                                      20,
                                      -16,
                                      l_hL,
                                      l_hR,
                                      l_huL,
                                      l_huR,
                                      l_sourceL,
                                      l_sourceR );

  REQUIRE( l_hL == Approx( 7 ) );
  REQUIRE( l_hR == Approx( 8 ) );
  REQUIRE( l_huL == Approx( 14 ) );
  REQUIRE( l_huR == Approx( -16 ) );
  REQUIRE( l_sourceL == Approx( -250.155 ) );
  REQUIRE( l_sourceR == Approx( 0 ) );

  Hybrid::hydrostatic_reconstruction( 0,
                                      3,
                                      1,
                                      4,
                                      2,
                                      8,
                                      l_hL,
                                      l_hR,
                                      l_huL,
                                      l_huR,
                                      l_sourceL,
                                      l_sourceR );

  REQUIRE( l_hL == Approx( 0 ) );
  REQUIRE( l_hR == Approx( 4 ) );
  REQUIRE( l_huL == Approx( 0 ) );
  REQUIRE( l_huR == Approx( 8 ) );
  REQUIRE( l_sourceL == Approx( -4.905 ) );
  REQUIRE( l_sourceR == Approx( 0 ) );
}

TEST_CASE( "Hybrid keeps a lake at rest well-balanced over bathymetry", "[Hybrid]" ) {
  t_real l_netUpdateL[3] = { 0, 0, 0 };
  t_real l_netUpdateR[3] = { 0, 0, 0 };

  Hybrid::netUpdates( 10,
                      8,
                      0,
                      0,
                      0,
                      2,
                      0,
                      0,
                      false,
                      l_netUpdateL,
                      l_netUpdateR );

  requireFinite3( l_netUpdateL );
  requireFinite3( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( 0 ).margin( 1e-5 ) );
  REQUIRE( l_netUpdateL[1] == Approx( 0 ).margin( 1e-5 ) );
  REQUIRE( l_netUpdateL[2] == Approx( 0 ).margin( 1e-5 ) );
  REQUIRE( l_netUpdateR[0] == Approx( 0 ).margin( 1e-5 ) );
  REQUIRE( l_netUpdateR[1] == Approx( 0 ).margin( 1e-5 ) );
  REQUIRE( l_netUpdateR[2] == Approx( 0 ).margin( 1e-5 ) );
}

TEST_CASE( "Hybrid delegates wet subcritical interfaces to F-wave", "[Hybrid]" ) {
  t_real l_hybridL[3] = { 0, 0, 0 };
  t_real l_hybridR[3] = { 0, 0, 0 };
  t_real l_fwaveL[2] = { 0, 0 };
  t_real l_fwaveR[2] = { 0, 0 };

  Hybrid::netUpdates( 10,
                      9,
                      -30,
                      27,
                      0,
                      0,
                      5,
                      -7,
                      false,
                      l_hybridL,
                      l_hybridR );

  Fwave::netUpdates( 10,
                     9,
                     -30,
                     27,
                     0,
                     0,
                     0,
                     0,
                     false,
                     l_fwaveL,
                     l_fwaveR );

  requireFinite3( l_hybridL );
  requireFinite3( l_hybridR );
  requireFinite2( l_fwaveL );
  requireFinite2( l_fwaveR );
  REQUIRE( l_hybridL[0] == Approx( l_fwaveL[0] ) );
  REQUIRE( l_hybridL[1] == Approx( l_fwaveL[1] ) );
  REQUIRE( l_hybridR[0] == Approx( l_fwaveR[0] ) );
  REQUIRE( l_hybridR[1] == Approx( l_fwaveR[1] ) );
  REQUIRE( l_hybridL[2] == Approx( 0 ) );
  REQUIRE( l_hybridR[2] == Approx( 0 ) );
}

TEST_CASE( "Hybrid delegates supercritical flat interfaces to HLLE diagonal updates", "[Hybrid]" ) {
  t_real l_hybridL[3] = { 0, 0, 0 };
  t_real l_hybridR[3] = { 0, 0, 0 };
  t_real l_hlleL[3] = { 0, 0, 0 };
  t_real l_hlleR[3] = { 0, 0, 0 };

  Hybrid::netUpdates( 1,
                      3,
                      10,
                      10,
                      0,
                      0,
                      1,
                      6,
                      false,
                      l_hybridL,
                      l_hybridR );

  Hlle::netUpdatesDiag( 1,
                        3,
                        10,
                        10,
                        0,
                        0,
                        1,
                        6,
                        false,
                        l_hlleL,
                        l_hlleR );

  requireFinite3( l_hybridL );
  requireFinite3( l_hybridR );
  REQUIRE( l_hybridL[0] == Approx( l_hlleL[0] ) );
  REQUIRE( l_hybridL[1] == Approx( l_hlleL[1] ) );
  REQUIRE( l_hybridL[2] == Approx( l_hlleL[2] ) );
  REQUIRE( l_hybridR[0] == Approx( l_hlleR[0] ) );
  REQUIRE( l_hybridR[1] == Approx( l_hlleR[1] ) );
  REQUIRE( l_hybridR[2] == Approx( l_hlleR[2] ) );
}

TEST_CASE( "Hybrid dry interfaces stay finite", "[Hybrid]" ) {
  t_real l_netUpdateL[3] = { 0, 0, 0 };
  t_real l_netUpdateR[3] = { 0, 0, 0 };

  Hybrid::netUpdates( 0,
                      0,
                      0,
                      0,
                      0,
                      3,
                      0,
                      0,
                      false,
                      l_netUpdateL,
                      l_netUpdateR );

  requireFinite3( l_netUpdateL );
  requireFinite3( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[1] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[2] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[1] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[2] == Approx( 0 ) );

  Hybrid::netUpdates( 1,
                      4,
                      2,
                      8,
                      0,
                      3,
                      0,
                      0,
                      false,
                      l_netUpdateL,
                      l_netUpdateR );

  requireFinite3( l_netUpdateL );
  requireFinite3( l_netUpdateR );
}
