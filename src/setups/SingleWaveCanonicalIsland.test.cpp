#include <catch2/catch.hpp>
#include "SingleWaveCanonicalIsland.h"

TEST_CASE( "Test single wave canonical island setup.", "[SingleWaveCanonicalIsland]" ) {


  tsunami_lab::setups::SingleWaveCanonicalIsland l_setup = {};

  SECTION("Water") {
    // in water
    REQUIRE( l_setup.getBathymetry( 0.0f , 0 ) == Approx(-0.32f) );
    REQUIRE( l_setup.getHeight( 0.0f, 0 ) == Approx(0.32f) );
    REQUIRE( l_setup.getMomentumX( 0.0f, 0 ) == Approx(1.9235e-26f) );
    // peak of wave
    REQUIRE( l_setup.getBathymetry( 3.0f , 0 ) == Approx(-0.32f) );
    REQUIRE( l_setup.getHeight( 3.0f, 0 ) == Approx(5.32f) );
    REQUIRE( l_setup.getMomentumX( 3.0f, 0 ) == Approx(36.12104649646796f) );
  }

  SECTION("Island") {
    // middle of island
    REQUIRE( l_setup.getBathymetry( 12.96f, 13.80f ) == Approx(0.305f) );
    REQUIRE( l_setup.getHeight( 12.96f, 13.80f ) == Approx(0.0f) );
    REQUIRE( l_setup.getMomentumX( 12.96f, 13.80f ) == Approx(0.0f) );
    // on slope of island 
    REQUIRE( l_setup.getBathymetry( 10.61f, 11.45f ) == Approx(-0.25085046789419324f) );
    REQUIRE( l_setup.getHeight( 10.61f, 11.45f ) == Approx(0.25085046789419324f) );
    REQUIRE( l_setup.getMomentumX( 10.61f, 11.45f ) == Approx(0.0f) );
  }


}