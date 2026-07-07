#include <catch2/catch.hpp>
#include <cmath>

#define private public
#include "hlle.h"
#undef private

namespace {
  using tsunami_lab::t_real;
  using tsunami_lab::solvers::Hlle;

  void requireFinite2( t_real const i_values[2] ) {
    REQUIRE( std::isfinite( i_values[0] ) );
    REQUIRE( std::isfinite( i_values[1] ) );
  }

  void requireFinite3( t_real const i_values[3] ) {
    REQUIRE( std::isfinite( i_values[0] ) );
    REQUIRE( std::isfinite( i_values[1] ) );
    REQUIRE( std::isfinite( i_values[2] ) );
  }

  t_real momentumFlux( t_real i_h, t_real i_hu ) {
    t_real l_h = i_h > 0 ? i_h : t_real(0);
    t_real l_u = l_h > 1e-12 ? i_hu / l_h : t_real(0);
    return l_h * l_u * l_u + t_real(0.5) * t_real(9.80665) * l_h * l_h;
  }
}

TEST_CASE( "HLLE wave speeds use Einfeldt bounds", "[Hlle]" ) {
  t_real l_waveSpeedL = 0;
  t_real l_waveSpeedR = 0;

  Hlle::waveSpeeds( 10,
                    9,
                    -3,
                    3,
                    l_waveSpeedL,
                    l_waveSpeedR );

  REQUIRE( l_waveSpeedL == Approx( -12.9028531253 ) );
  REQUIRE( l_waveSpeedR == Approx(  12.3946713630 ) );
}

TEST_CASE( "HLLE returns zero updates for steady and fully dry interfaces", "[Hlle]" ) {
  t_real l_netUpdateL[2] = { -5, 3 };
  t_real l_netUpdateR[2] = {  4, 7 };

  Hlle::netUpdates( 7,
                    7,
                    14,
                    14,
                    0,
                    0,
                    0,
                    0,
                    false,
                    l_netUpdateL,
                    l_netUpdateR );

  requireFinite2( l_netUpdateL );
  requireFinite2( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[1] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[1] == Approx( 0 ) );

  l_netUpdateL[0] = -5;
  l_netUpdateL[1] =  3;
  l_netUpdateR[0] =  4;
  l_netUpdateR[1] =  7;

  Hlle::netUpdates( 0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    false,
                    l_netUpdateL,
                    l_netUpdateR );

  requireFinite2( l_netUpdateL );
  requireFinite2( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[1] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[1] == Approx( 0 ) );
}

TEST_CASE( "HLLE computes transonic dam-break style updates", "[Hlle]" ) {
  t_real l_netUpdateL[2] = { 0 };
  t_real l_netUpdateR[2] = { 0 };

  Hlle::netUpdates( 10,
                    8,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    false,
                    l_netUpdateL,
                    l_netUpdateR );

  requireFinite2( l_netUpdateL );
  requireFinite2( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx(  9.6420710050 ) );
  REQUIRE( l_netUpdateL[1] == Approx( -90.5840883311 ) );
  REQUIRE( l_netUpdateR[0] == Approx( -9.6420710050 ) );
  REQUIRE( l_netUpdateR[1] == Approx( -85.9356116689 ) );

  REQUIRE( l_netUpdateL[0] + l_netUpdateR[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[1] + l_netUpdateR[1] == Approx( momentumFlux(8, 0) - momentumFlux(10, 0) ) );
}

TEST_CASE( "HLLE upwinds purely right- and left-going flow", "[Hlle]" ) {
  t_real l_netUpdateL[2] = { 0 };
  t_real l_netUpdateR[2] = { 0 };

  Hlle::netUpdates( 1,
                    3,
                    10,
                    10,
                    0,
                    0,
                    0,
                    0,
                    false,
                    l_netUpdateL,
                    l_netUpdateR );

  requireFinite2( l_netUpdateL );
  requireFinite2( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[1] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[1] == Approx( -27.4400666667 ) );

  Hlle::netUpdates( 1,
                    3,
                    -30,
                    -30,
                    0,
                    0,
                    0,
                    0,
                    false,
                    l_netUpdateL,
                    l_netUpdateR );

  requireFinite2( l_netUpdateL );
  requireFinite2( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateL[1] == Approx( -560.7734 ) );
  REQUIRE( l_netUpdateR[0] == Approx( 0 ) );
  REQUIRE( l_netUpdateR[1] == Approx( 0 ) );
}

TEST_CASE( "HLLE handles wet-dry interfaces without NaNs and conserves flux jumps", "[Hlle]" ) {
  t_real l_netUpdateL[2] = { 0 };
  t_real l_netUpdateR[2] = { 0 };

  Hlle::netUpdates( 0,
                    3,
                    0,
                    6,
                    0,
                    0,
                    0,
                    0,
                    false,
                    l_netUpdateL,
                    l_netUpdateR );

  requireFinite2( l_netUpdateL );
  requireFinite2( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx( -3.2253843936 ) );
  REQUIRE( l_netUpdateL[1] == Approx(  2.2964995546 ) );
  REQUIRE( l_netUpdateR[0] == Approx(  9.2253843936 ) );
  REQUIRE( l_netUpdateR[1] == Approx( 53.8334254454 ) );

  REQUIRE( l_netUpdateL[0] + l_netUpdateR[0] == Approx( 6 ) );
  REQUIRE( l_netUpdateL[1] + l_netUpdateR[1] == Approx( momentumFlux(3, 6) ) );
}

TEST_CASE( "HLLE diagonal updates include tangential momentum", "[Hlle]" ) {
  t_real l_netUpdateL[3] = { -1, -1, -1 };
  t_real l_netUpdateR[3] = { -1, -1, -1 };

  Hlle::netUpdatesDiag( 0,
                        0,
                        0,
                        0,
                        0,
                        0,
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

  Hlle::netUpdatesDiag( 10,
                        9,
                        -30,
                        27,
                        5,
                        -7,
                        l_netUpdateL,
                        l_netUpdateR );

  requireFinite3( l_netUpdateL );
  requireFinite3( l_netUpdateR );
  REQUIRE( l_netUpdateL[0] == Approx(  35.3943427425 ) );
  REQUIRE( l_netUpdateL[1] == Approx( -412.4519781470 ) );
  REQUIRE( l_netUpdateL[2] == Approx(   72.8016834887 ) );
  REQUIRE( l_netUpdateR[0] == Approx(  21.6056572575 ) );
  REQUIRE( l_netUpdateR[1] == Approx(  310.2888031470 ) );
  REQUIRE( l_netUpdateR[2] == Approx(  -78.8016834887 ) );

  REQUIRE( l_netUpdateL[2] + l_netUpdateR[2] == Approx( -6 ) );
}
