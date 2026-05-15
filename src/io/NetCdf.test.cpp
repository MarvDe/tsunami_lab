#include <catch2/catch.hpp>
#include "NetCdf.h"

using namespace tsunami_lab;

TEST_CASE( "test the NetCdf displacement reader.", "[NetCdf]"){
    t_real l_dis[100*100];
    for (int l_y = 0; l_y < 100; l_y++){
        for (int l_x = 0; l_x < 100; l_x++){
            l_dis[l_x + l_y * 100] = 0;
        }
    }

    io::NetCdf::readDisplacement(0,0,"utilities/artificialtsunami_displ_1000.nc", l_dis);
    for (int l_y = 20; l_y < 100; l_y+=50){
        for (int l_x = 0; l_x < 100; l_x+=50){
            REQUIRE( l_dis[l_y * 100 + l_x] != 0);
        }
    }
}