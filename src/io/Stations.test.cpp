#include <catch2/catch.hpp>
#include <sstream>
#include <string>
#define private public
#include "Stations.h"
#undef public


TEST_CASE( "Test stations.", "[Stations]" ){
    /*
    * Testing writing of data at stations.
    *   
    */

    const tsunami_lab::t_idx l_cellX = 5;
    const tsunami_lab::t_idx l_cellY = 5;
    tsunami_lab::t_real l_dxy = 1;

    tsunami_lab::t_real l_h[l_cellX * l_cellY];
    tsunami_lab::t_real l_u[l_cellX * l_cellY];
    tsunami_lab::t_real l_v[l_cellX * l_cellY];
    tsunami_lab::t_real l_b[l_cellX * l_cellY];
    
    std::stringstream l_stream1;
    std::stringstream l_stream2;
    std::stringstream l_stream3;
    std::stringstream l_stream4;
    std::stringstream l_stream5;
    tsunami_lab::io::Stations l_stations(l_cellX, l_cellY, l_dxy);

    // station at beginning
    l_stations.addStation("station1", 0, 0, &l_stream1);
    // station at center
    l_stations.addStation("station2", (l_cellX - 1) / 2, (l_cellY - 1) / 2, &l_stream2);
    // station at last index
    l_stations.addStation("station3", l_cellX - 1, l_cellY - 1, &l_stream3);
    // stations out of bounds
    l_stations.addStation("station4", l_cellX, l_cellY - 1, &l_stream4);
    l_stations.addStation("station5", l_cellX - 1, l_cellY, &l_stream5);

    for (tsunami_lab::t_idx l_i = 0; l_i < 3; l_i++){
        
        // generating some data
        for (tsunami_lab::t_idx l_y = 0; l_y < l_cellY; l_y++){
            for (tsunami_lab::t_idx l_x = 0; l_x < l_cellX; l_x++){
                l_h[l_x + l_y * l_cellX] = l_y + l_i;
                l_u[l_x + l_y * l_cellX] = l_x + l_i;
                l_v[l_x + l_y * l_cellX] = l_y * l_x + l_i;
                l_b[l_x + l_y * l_cellX] = l_y * l_cellX + l_x + l_i;
            }
        }
    


        l_stations.write(
            l_i, l_h, l_u, l_v, l_b, l_cellX
        );

    }


    std::string l_ref1 = R"V0G0N(sim_time,name,x,y,height,momentum_x,momentum_y,bathymetry
0,station1,0,0,0,0,0,0
1,station1,0,0,1,1,1,1
2,station1,0,0,2,2,2,2
)V0G0N";

    std::string l_ref2 = R"V0G0N(sim_time,name,x,y,height,momentum_x,momentum_y,bathymetry
0,station2,2,2,2,2,4,12
1,station2,2,2,3,3,5,13
2,station2,2,2,4,4,6,14
)V0G0N";

    std::string l_ref3 = R"V0G0N(sim_time,name,x,y,height,momentum_x,momentum_y,bathymetry
0,station3,4,4,4,4,16,24
1,station3,4,4,5,5,17,25
2,station3,4,4,6,6,18,26
)V0G0N";

    std::string l_ref4 = R"V0G0N(sim_time,name,x,y,height,momentum_x,momentum_y,bathymetry
0,station4,5,4
1,station4,5,4
2,station4,5,4
)V0G0N";

    std::string l_ref5 = R"V0G0N(sim_time,name,x,y,height,momentum_x,momentum_y,bathymetry
0,station5,4,5
1,station5,4,5
2,station5,4,5
)V0G0N";

    REQUIRE( l_stream1.str().size() == l_ref1.size() );
    REQUIRE( l_stream1.str() == l_ref1 );

    REQUIRE( l_stream2.str().size() == l_ref2.size() );
    REQUIRE( l_stream2.str() == l_ref2 );

    REQUIRE( l_stream3.str().size() == l_ref3.size() );
    REQUIRE( l_stream3.str() == l_ref3 );

    REQUIRE( l_stream4.str().size() == l_ref4.size() );
    REQUIRE( l_stream4.str() == l_ref4 );

    REQUIRE( l_stream5.str().size() == l_ref5.size() );
    REQUIRE( l_stream5.str() == l_ref5 );
  
}