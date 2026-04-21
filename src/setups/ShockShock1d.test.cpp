/**
 * @section DESCRIPTION
 * Tests the shock-shock setup.
 */
#include <catch2/catch.hpp>
#include "ShockShock1d.h"

TEST_CASE( "Test the one-dimensional shock-shock setup.", "[ShockShock1d]" ){
    tsunami_lab::setups::ShockShock1d l_shockShock( 20,
                                                    14,
                                                    7 );

    //left side
    REQUIRE( l_shockShock.getHeight(2, 0) == 20);
    
    REQUIRE( l_shockShock.getHeight(2, 10) == 20);
    
    REQUIRE( l_shockShock.getMomentumX(2, 0) == 14);
    
    REQUIRE( l_shockShock.getMomentumX(2, 10) == 14);
    
    REQUIRE( l_shockShock.getMomentumY(2, 0) == 0);
    
    REQUIRE( l_shockShock.getMomentumY(2, 10) == 0);

    //right side
    REQUIRE( l_shockShock.getHeight(11, 0) == 20);
    
    REQUIRE( l_shockShock.getHeight(11, 10) == 20);
    
    REQUIRE( l_shockShock.getMomentumX(11, 0) == -14);
    
    REQUIRE( l_shockShock.getMomentumX(11, 10) == -14);
    
    REQUIRE( l_shockShock.getMomentumY(11, 0) == 0);
    
    REQUIRE( l_shockShock.getMomentumY(11, 10) == 0);
    
}