#include <catch2/catch.hpp>
#include "SmallCity.h"

TEST_CASE( "Test small city setup.", "[SmallCity]" ) {
  tsunami_lab::setups::SmallCity l_setup = {};

  SECTION("Water") {
    // in water
    REQUIRE( l_setup.getBathymetry( 0.0f , 0 ) == Approx(-0.32f) );
    REQUIRE( l_setup.getHeight( 0.0f, 0 ) == Approx(0.32000000000004836f) );
    REQUIRE( l_setup.getMomentumX( 0.0f, 0 ) == Approx(2.75816614547046e-13f) );
    // peak of wave
    REQUIRE( l_setup.getBathymetry( 2.0f , 0 ) == Approx(-0.32f) );
    REQUIRE( l_setup.getHeight( 2.0f, 0 ) == Approx(3.32f) );
    REQUIRE( l_setup.getMomentumX( 2.0f, 0 ) == Approx(17.120829419160742f) );
  }

  SECTION("Beach") {
    // middle of beach
    REQUIRE( l_setup.getBathymetry( 4.0f, 0 ) == Approx(-0.10999999999999988f) );
    REQUIRE( l_setup.getHeight( 4.0f, 0 ) == Approx(0.11000000000004821f) );
    REQUIRE( l_setup.getMomentumX( 4.0f, 0 ) == Approx(9.481196125057428e-14f) );
  }

  SECTION("Buildings") {
    // ontop of beach 
    REQUIRE( l_setup.getBathymetry( 5.0f, 0 ) == Approx(0.1f) );
    REQUIRE( l_setup.getHeight( 5.0f, 0 ) == Approx(0.0f) );
    REQUIRE( l_setup.getMomentumX( 5.0f, 0 ) == Approx(0.0f) );
  }
}