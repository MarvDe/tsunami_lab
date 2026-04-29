#include <catch2/catch.hpp>
#define private public
#include "SubcriticalFlow1d.h"
#undef public

TEST_CASE( "Test the getter functions of the subcritical flow setup.", "[SubcriticalFlow]" ){
    

    tsunami_lab::setups::Setup* obj = new tsunami_lab::setups::SubcriticalFlow1d();

    tsunami_lab::t_real number;

    //bathymetry
    number = obj->getBathymetry(10,0);
    REQUIRE( number == -1.8f );

    number = obj->getBathymetry(8,0);
    REQUIRE( number == -2.f );

    number = obj->getBathymetry(12,0);
    REQUIRE( number == -2.f );

    number = obj->getBathymetry(200,0);
    REQUIRE( number == -2.f );

    number = obj->getBathymetry(0,0);
    REQUIRE( number == -2.f );

    number = obj->getBathymetry(9,0);
    REQUIRE( number == -1.85f );

    number = obj->getBathymetry(10,0);
    REQUIRE( number == -1.8f );

    number = obj->getBathymetry(11,0);
    REQUIRE( number == -1.85f );


    //height = -bathymetry
    number = obj->getHeight(10,0);
    REQUIRE( number == 1.8f );

    number = obj->getHeight(8,0);
    REQUIRE( number == 2.f );

    number = obj->getHeight(12,0);
    REQUIRE( number == 2.f );

    number = obj->getHeight(200,0);
    REQUIRE( number == 2.f );

    number = obj->getHeight(0,0);
    REQUIRE( number == 2.f );

    number = obj->getHeight(9,0);
    REQUIRE( number == 1.85f );

    number = obj->getHeight(10,0);
    REQUIRE( number == 1.8f );

    number = obj->getHeight(11,0);
    REQUIRE( number == 1.85f );

    //X momentum
    number = obj->getMomentumX(11,0);
    REQUIRE( number == 4.42f );

    number = obj->getMomentumX(0,0);
    REQUIRE( number == 4.42f );
    
    number = obj->getMomentumX(11,10);
    REQUIRE( number == 4.42f );
    
    number = obj->getMomentumX(0,10);
    REQUIRE( number == 4.42f );
    
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