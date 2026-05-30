/**
 * @section DESCRIPTION
 * Tests the 2d tsunami event setup.
 */
#include <catch2/catch.hpp>
#include "TsunamiEvent2d.h"
#include <iostream>

using namespace tsunami_lab;

TEST_CASE( "Test the two-dimensional tsunami event setup.", "[TsunamiEvent2d]" ){
    
    constexpr t_idx l_nx = 5;
    constexpr t_idx l_ny = 5;

    t_real l_dxy = 1;
    t_real l_left = 0;
    t_real l_upper = 0;

    t_real l_bathymetry[l_nx * l_ny];
    t_real l_displacement[l_nx * l_ny];

    for (t_idx l_iy = 0; l_iy < 5; l_iy++){
        for (t_idx l_ix = 0; l_ix < l_nx; l_ix++){
            t_real l_x = l_ix;
            t_real l_y = l_iy;
            l_bathymetry[l_iy * l_nx + l_ix] = -l_x * 10;
            l_displacement[l_iy * l_nx + l_ix] = l_y;
        }
    }

    // b[4,.] = -40
    // d[.,4] = 4
    // h[4,4] = 40

    // b[3,.] = -30
    // d[.,2] = 2
    // h[3,2] = 30

    setups::TsunamiEvent2d l_tsunamiEvent(l_nx, l_ny, l_dxy, l_left, l_upper, l_nx, l_ny, l_dxy, l_left, l_upper, 
                                          l_nx, l_ny, l_dxy, l_left, l_upper, l_bathymetry, l_displacement);

    REQUIRE( l_tsunamiEvent.getBathymetry(4, 0) == Approx(-40) );
    REQUIRE( l_tsunamiEvent.getBathymetry(4, 4) == Approx(-36) );
    REQUIRE( l_tsunamiEvent.getBathymetry(3, 0) == Approx(-30) );

    REQUIRE( l_tsunamiEvent.getHeight(4, 4) == Approx(40) );
    REQUIRE( l_tsunamiEvent.getHeight(3, 2) == Approx(30) );

    REQUIRE( l_tsunamiEvent.getMomentumX(0, 0) == Approx(0) );
    REQUIRE( l_tsunamiEvent.getMomentumY(0, 0) == Approx(0) );

}