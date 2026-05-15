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

    io::NetCdf::readDisplacement(100,100,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
    for (int l_y = 20; l_y < 100; l_y+=50){
        for (int l_x = 0; l_x < 100; l_x+=50){
            REQUIRE( l_dis[l_y * 100 + l_x] != 0);
        }
    }

    SECTION( "Test dimension error." ){
        int ret = io::NetCdf::readDisplacement(100,99,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readDisplacement(99,100,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readDisplacement(101,100,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readDisplacement(100,101,"utilities/artificialtsunami_displ_1000.nc", l_dis, 0);
        REQUIRE(ret == -1);
    }
}

TEST_CASE( "test the NetCdf bathymetry reader.", "[NetCd2f]"){
    t_real l_bathy[1000*1000];
    for (int l_y = 0; l_y < 1000; l_y++){
        for (int l_x = 0; l_x < 1000; l_x++){
            l_bathy[l_x + l_y * 1000] = 0;
        }
    }

    io::NetCdf::readBathymetry(1000,1000,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
    for (int l_y = 20; l_y < 1000; l_y+=500){
        for (int l_x = 0; l_x < 1000; l_x+=500){
            REQUIRE( l_bathy[l_y * 1000 + l_x] != 0);
        }
    }

    SECTION( "Test dimension error." ){
        int ret = io::NetCdf::readBathymetry(1000,999,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readBathymetry(999,1000,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readBathymetry(1001,1000,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);

        ret = io::NetCdf::readBathymetry(1000,1001,"utilities/artificialtsunami_bathymetry_1000.nc", l_bathy, 0);
        REQUIRE(ret == -1);
    }
}