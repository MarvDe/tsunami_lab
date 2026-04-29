#include <catch2/catch.hpp>
#define private public
#include "SupercriticalFlow1d.h"
#undef public

TEST_CASE( "Test the getter functions of the supercritical flow setup.", "[SupercriticalFlow]" ){
    

    tsunami_lab::setups::Setup* obj = new tsunami_lab::setups::SupercriticalFlow1d();

    tsunami_lab::t_real number;

    //bathymetry
    number = obj->getBathymetry(10,0);
    REQUIRE( number == -0.13f );

    number = obj->getBathymetry(8,0);
    REQUIRE( number == -0.33f );

    number = obj->getBathymetry(12,0);
    REQUIRE( number == -0.33f );

    number = obj->getBathymetry(200,0);
    REQUIRE( number == -0.33f );

    number = obj->getBathymetry(0,0);
    REQUIRE( number == -0.33f );

    number = obj->getBathymetry(9,0);
    REQUIRE( number == -0.18f );

    number = obj->getBathymetry(10,0);
    REQUIRE( number == -0.13f );

    number = obj->getBathymetry(11,0);
    REQUIRE( number == -0.18f );

    number = obj->getBathymetry(9.5,0);
    REQUIRE( number == -0.1425f );


    //height = -bathymetry
    number = obj->getHeight(10,0);
    REQUIRE( number == 0.13f );

    number = obj->getHeight(8,0);
    REQUIRE( number == 0.33f );

    number = obj->getHeight(12,0);
    REQUIRE( number == 0.33f );

    number = obj->getHeight(200,0);
    REQUIRE( number == 0.33f );

    number = obj->getHeight(0,0);
    REQUIRE( number == 0.33f );

    number = obj->getHeight(9,0);
    REQUIRE( number == 0.18f );

    number = obj->getHeight(10,0);
    REQUIRE( number == 0.13f );

    number = obj->getHeight(11,0);
    REQUIRE( number == 0.18f );

    number = obj->getHeight(9.5,0);
    REQUIRE( number == 0.1425f );

    //X momentum
    number = obj->getMomentumX(11,0);
    REQUIRE( number == 0.18f );

    number = obj->getMomentumX(0,0);
    REQUIRE( number == 0.18f );
    
    number = obj->getMomentumX(11,10);
    REQUIRE( number == 0.18f );
    
    number = obj->getMomentumX(0,10);
    REQUIRE( number == 0.18f );
    
    //Y momentum
    number = obj->getMomentumY(11,0);
    REQUIRE( number == 0.f );

    number = obj->getMomentumY(0,0);
    REQUIRE( number == 0.f );
    
    number = obj->getMomentumY(11,10);
    REQUIRE( number == 0.f );
    
    number = obj->getMomentumY(0,10);
    REQUIRE( number == 0.f );

    delete obj;
}