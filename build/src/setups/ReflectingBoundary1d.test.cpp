/**
 * @section DESCRIPTION
 * Tests the shock-shock setup.
 */
#include <catch2/catch.hpp>
#include "ReflectingBoundary1d.h"

TEST_CASE( "Test the one-dimensional reflecting boundary setup.", "[ReflectingBoundary1d]" ){
    tsunami_lab::setups::ReflectingBoundary1d l_reflect( 20,
                                                         14,
                                                         7 );

    //left side
    REQUIRE( l_reflect.getHeight(2, 0) == 20);
    
    REQUIRE( l_reflect.getHeight(2, 10) == 20);
    
    REQUIRE( l_reflect.getMomentumX(2, 0) == 14);
    
    REQUIRE( l_reflect.getMomentumX(2, 10) == 14);
    
    REQUIRE( l_reflect.getMomentumY(2, 0) == 0);
    
    REQUIRE( l_reflect.getMomentumY(2, 10) == 0);

    //right side
    REQUIRE( l_reflect.getHeight(11, 0) == 0);
    
    REQUIRE( l_reflect.getHeight(11, 10) == 0);
    
    REQUIRE( l_reflect.getMomentumX(11, 0) == 0);
    
    REQUIRE( l_reflect.getMomentumX(11, 10) == 0);
    
    REQUIRE( l_reflect.getMomentumY(11, 0) == 0);
    
    REQUIRE( l_reflect.getMomentumY(11, 10) == 0);
    
}