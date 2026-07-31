#include <catch2/catch.hpp>
#include "SolitaryWaveBeach1d.h"

TEST_CASE( "Test solitary wave beach setup.", "[SolitaryWaveBeach1d]" ) {
  tsunami_lab::setups::SolitaryWaveBeach1d l_setup( 1.0f,
                                                    0.019f,
                                                    19.85f,
                                                    50.0f,
                                                    95.0f,
                                                   -1.0f );

  SECTION("Plane beach bathymetry uses land-positive convention") {
    REQUIRE( l_setup.getBathymetry( 30.15f, 0 ) == Approx(1.0f) );
    REQUIRE( l_setup.getBathymetry( 50.0f, 0 ) == Approx(0.0f) );
    REQUIRE( l_setup.getBathymetry( 69.85f, 0 ) == Approx(-1.0f) );
    REQUIRE( l_setup.getBathymetry( 95.0f, 0 ) == Approx(-1.0f) );
  }

  SECTION("Initial water column is dry on high land and positive offshore") {
    REQUIRE( l_setup.getHeight( 0.0f, 0 ) == Approx(0.0f) );
    REQUIRE( l_setup.getHeight( 95.0f, 0 ) == Approx(1.019f) );
  }

  SECTION("Wave initially travels toward the beach") {
    REQUIRE( l_setup.getMomentumX( 95.0f, 0 ) < 0.0f );
    REQUIRE( l_setup.getMomentumY( 95.0f, 0 ) == Approx(0.0f) );
  }
}
