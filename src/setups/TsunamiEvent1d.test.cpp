/**
 * @section DESCRIPTION
 * Tests the tsunami event setup.
 */
#include <catch2/catch.hpp>
#include "TsunamiEvent1d.h"

TEST_CASE( "Test the one-dimensional tsunami event setup.", "[TsunamiEvent1d]" ){
    
    tsunami_lab::t_real l_bathymetry[5] = {-10, -20, -30, -40, -50};

    tsunami_lab::setups::TsunamiEvent1d l_tsunamiEvent(5, l_bathymetry, 1);
   
    REQUIRE( l_tsunamiEvent.getBathymetry(0, 0) == Approx(-20) );
    REQUIRE( l_tsunamiEvent.getBathymetry(1, 0) == Approx(-20) );
    REQUIRE( l_tsunamiEvent.getBathymetry(2, 0) == Approx(-30) );
    REQUIRE( l_tsunamiEvent.getBathymetry(3, 0) == Approx(-40) );
    REQUIRE( l_tsunamiEvent.getBathymetry(4, 0) == Approx(-50) );

    REQUIRE( l_tsunamiEvent.getHeight(0, 0) == Approx(20) );
    REQUIRE( l_tsunamiEvent.getHeight(1, 0) == Approx(20) );
    REQUIRE( l_tsunamiEvent.getHeight(2, 0) == Approx(30) );
    REQUIRE( l_tsunamiEvent.getHeight(3, 0) == Approx(40) );
    REQUIRE( l_tsunamiEvent.getHeight(4, 0) == Approx(50) );

    REQUIRE( l_tsunamiEvent.getMomentumX(0, 0) == Approx(0) );

}