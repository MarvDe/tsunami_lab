#include <catch2/catch.hpp>
#define private public
#include "F_wave.h"
#undef public

TEST_CASE( "Test the derivation of the F-Wave Roe speeds.", "[FwaveSpeeds]" ){
    /*
    * Test case:
    *  h: 10 | 9
    *  u: -3 | 3
    *
    * roe height: 9.5
    * roe velocity: (sqrt(10) * -3 + 3 * 3) / ( sqrt(10) + sqrt(9) )
    *               = -0.0790021169691720
    * roe speeds: s1 = -0.079002116969172024 - sqrt(9.80665 * 9.5) = -9.7311093998375095
    *             s2 = -0.079002116969172024 + sqrt(9.80665 * 9.5) =  9.5731051658991654
    */
  float l_waveSpeedL = 0;
  float l_waveSpeedR = 0;
  tsunami_lab::solvers::Fwave::waveSpeeds( 10,
                                         9,
                                         -3,
                                         3,
                                         l_waveSpeedL,
                                         l_waveSpeedR );

  REQUIRE( l_waveSpeedL == Approx( -9.7311093998375095 ) );
  REQUIRE( l_waveSpeedR == Approx(  9.5731051658991654 ) );
}


TEST_CASE( "Test the derivation of the F-Wave Roe wave speeds.", "[FwaveStrengths]" ) {
  /*
   * Test case:
   *  h:   10 | 9
   *  u:   -3 | 3
   *  hu: -30 | 27
   *
   * The derivation of the Roe speeds (s1, s2) is given above.
   *
   *  Matrix of right eigenvectors:
   *
   *      | 1   1 |
   *  R = |       |
   *      | s1 s2 |
   *
   * Inversion yields:
   *
   * wolframalpha.com query: invert {{1, 1}, {-9.7311093998375095, 9.5731051658991654}}
   *
   *        | 0.49590751974393229 -0.051802159398648326 |
   * Rinv = |                                           |
   *        | 0.50409248025606771  0.051802159398648326 |
   * 
   *      |          hu          |
   * f =  |                      |
   *      |hu*u + 0.5 * g * h * h|
   * 
   * wolframalpha.com query: {27, 27 * 3 + 0.5 * 9.80665 * 9 * 9} - {-30, -30 * -3 + 0.5 * 9.80665 * 10 * 10} 
   * 
   * df = {57, -98.5182}
   * 
   * wolframalpha.com query: {{0.49590751974393229, -0.051802159398648326}, {0.50409248025606771, 0.051802159398648326}} * (57, -102.163)
   * 
   * (33.559, 23.441)

   */
  float l_strengthL = 0;
  float l_strengthR = 0;

  tsunami_lab::solvers::Fwave::waveStrengths( 10,
                                            9,
                                            -3,
                                            3,
                                            -30,
                                            27,
                                            -9.7311093998375095,
                                            9.5731051658991654,
                                            l_strengthL,
                                            l_strengthR );

  REQUIRE( l_strengthL == Approx(33.559) );
  REQUIRE( l_strengthR == Approx(23.441) );
}


TEST_CASE( "Test the derivation of the F-Wave Roe net-updates.", "[FWaveUpdates]" ) {
  /*
   * Test case:
   *
   *      left | right
   *  h:    10 | 9
   *  u:    -3 | 3
   *  hu:  -30 | 27
   *
   * The derivation of the Roe speeds (s1, s2) and wave strengths (a1, a1) is given above.
   *
   * zL = {33.559, 33.559 * -9.7311093998375095} = {33.559, -326.56630034914696}
   * zR = {23.44, 23.44 * 9.5731051658991654} = {23.44, 224.39358508867642}
   *
   *                      
   * update #1: A-dQ = {0,0} + {33.559, -326.56630034914696}
   *
   *                     
   * update #2: A+dQ = {0,0} + {23.44, 224.39358508867642}
   *
   *   
   */
  float l_netUpdatesL[2] = { -5, 3 };
  float l_netUpdatesR[2] = {  4, 7 };

  tsunami_lab::solvers::Fwave::netUpdates( 10,
                                         9,
                                         -30,
                                         27,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx( 33.5590017014261447899292 ) );
  REQUIRE( l_netUpdatesL[1] == Approx( -326.56631690591093200508 ) );

  REQUIRE( l_netUpdatesR[0] == Approx( 23.4409982985738561366777 ) );
  REQUIRE( l_netUpdatesR[1] == Approx( 224.403141905910928927533 ) );

  /*
   * Test case (dam break):
   *
   *     left | right
   *   h:  10 | 8
   *   hu:   0 | 0
   *
   * Roe speeds are given as:
   *
   *   s1 = -sqrt(9.80665 * 9)
   *   s2 =  sqrt(9.80665 * 9)
   *
   * Inversion of the matrix of right Eigenvectors:
   * 
   *   wolframalpha.com query: invert {{1, 1}, {-sqrt(9.80665 * 9), sqrt(9.80665 * 9)}}
   *
   *          | 0.5 -0.0532217 |
   *   Rinv = |                |
   *          | 0.5 0.0532217 |
   *
   * Multiplicaton with the jump in quantities gives the wave strengths:
   *
   *        | 8 - 10 |   | -1 |   | a1 |
   * Rinv * |        | = |    | = |    |
   *        |  0 - 0 |   | -1 |   | a2 |
   * 
   *      |          hu          |
   * f =  |                      |
   *      |hu*u + 0.5 * g * h * h|
   * 
   * wolframalpha.com query: {0, 0 * 0 + 0.5 * 9.80665 * 8 * 8} - {0, 0 * 0 + 0.5 * 9.80665 * 10 * 10}
   * 
   * df = {0, -176.52}
   * 
   * wolframalpha.com query: {{0.5, -0.0532217}, {0.5, 0.0532217}} * (0, -176.52)
   * 
   *  (9.39469, -9.39469)
   * 
   * zL = {9.39469, 9.39469 * -sqrt(9.80665 * 9)} = {9.39469, -88.26002509787632}
   * zR = {-9.39469, -9.39469 * sqrt(9.80665 * 9)} = {-9.39469, -88.26002509787632}
   *
   *                      
   * update #1: A-dQ = {0,0} + {9.39469, -88.26002509787632}
   *
   *                     
   * update #2: A+dQ = {0,0} + {-9.39469, -88.26002509787632}
   *
   * 
   */
  tsunami_lab::solvers::Fwave::netUpdates( 10,
                                         8,
                                         0,
                                         0,
                                         l_netUpdatesL,
                                         l_netUpdatesR ); 

  REQUIRE( l_netUpdatesL[0] ==  Approx(9.394671362) );
  REQUIRE( l_netUpdatesL[1] == Approx(-88.25984f)    );

  REQUIRE( l_netUpdatesR[0] == Approx(-9.39467f) );
  REQUIRE( l_netUpdatesR[1] == Approx(-88.25985)    );

  /*
   * Test case (supersonic problem):
   *
   *     left | right
   *   h:   1 |  3
   *   hu:   10 |  10
   * 
   * sL = 10 - sqrt(9.80665 * 2) > 0
   * sR = 10 + sqrt(9.80665 * 2) > 0
   * 
   * A-dQ = {0,0}
   * A+dQ = {0,df}
   * 
   * df = {10, 10 * 10/3 + 0.5 * 9.80665 * 3 * 3} - {10, 10 * 10 + 0.5 * 9.80665 * 1 * 1} = {0, -27.4401}
   */
  
  tsunami_lab::solvers::Fwave::netUpdates( 1,
                                         3,
                                         10,
                                         10,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(0) );

  REQUIRE( l_netUpdatesR[0] == Approx(0) );
  REQUIRE( l_netUpdatesR[1] == Approx(-27.44007f) );

  /*
   * Test case (supersonic problem):
   *
   *     left | right
   *   h:   1 |  3
   *   hu:-10 |  -10
   * 
   * sL = -10 - sqrt(9.80665 * 2) < 0
   * sR = -10 + sqrt(9.80665 * 2) < 0
   *
   * A-dQ = {0,df}
   * A+dQ = {0,0}
   *     
   * df = {-10, -10 * -10/3 + 0.5 * 9.80665 * 3 * 3} - {-10, -10 * -10 + 0.5 * 9.80665 * 1 * 1} = {0, -27.4401}
   */

  tsunami_lab::solvers::Fwave::netUpdates( 1,
                                         3,
                                         -10,
                                         -10,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(-27.44007f) );

  REQUIRE( l_netUpdatesR[0] == Approx(0) );
  REQUIRE( l_netUpdatesR[1] == Approx(0) );

  /*
   * Test case (even dam break):
   *
   *     left | right
   *   h:   10 |  10
   *   hu:  0  |  0
   * 
   * A-dQ = {0,0}
   * A+dQ = {0,0}
   */

  tsunami_lab::solvers::Fwave::netUpdates( 10,
                                         10,
                                         0,
                                         0,
                                         l_netUpdatesL,
                                         l_netUpdatesR );

  REQUIRE( l_netUpdatesL[0] == Approx(0) );
  REQUIRE( l_netUpdatesL[1] == Approx(0) );

  REQUIRE( l_netUpdatesR[0] == Approx(0) );
  REQUIRE( l_netUpdatesR[1] == Approx(0) );


}

#include <iostream>

TEST_CASE( "Test sanity check.", "[FwaveUpdate]" ){
    /*
    * Test case:
    *  first 10 values of middle_states.csv
    */

  float l_netUpdatesL[2];
  float l_netUpdatesR[2];
  
  float l_sL;
  float l_sR;
  float l_hm;

  // hLeft, hRight, huLeft, huRight
  float l_inputs[] = {
    8899.326826472694,8899.326826472694,122.0337839252433,-122.0337839252433,
    9894.065328676988,9894.065328676988,763.616897222239,-763.616897222239,
    1387.176994373967,1387.176994373967,-101.9619713277172,101.9619713277172,
    9976.904476606509,9976.904476606509,-906.6229611756387,906.6229611756387,
    6907.360046520149,6907.360046520149,-180.6064611678331,180.6064611678331,
    1065.53384380855,1065.53384380855,64.19327065348772,-64.19327065348772,
    3042.136044684769,3042.136044684769,-27.52440428024561,27.52440428024561,
    5973.686834276039,5973.686834276039,417.2334213893989,-417.2334213893989,
    10458.94716344883,10458.94716344883,-366.1438461063776,366.1438461063776,
    10539.5774699817,10539.5774699817,-988.5578370907829,988.5578370907829
  };
  // hStar
  float l_outputs[] = {
    8899.739847378269f,
    9896.516538751875f,
    1386.303079031417f,
    9974.006714260977f,
    6906.666250464617f,
    1066.161808657808f,
    3041.976718035753f,
    5975.410506492273f,
    10457.80412356421f,
    10536.50332526859f
  };

  for (int i = 0; i < 10; i++){

    tsunami_lab::solvers::Fwave::waveSpeeds(
      l_inputs[i * 4], // hLeft
      l_inputs[i * 4 + 1], // hRight
      l_inputs[i * 4 + 2] / l_inputs[i * 4], // uLeft
      l_inputs[i * 4 + 3] / l_inputs[i * 4 + 1], // uRight
      l_sL,
      l_sR
    );

    tsunami_lab::solvers::Fwave::netUpdates( l_inputs[i * 4], // hLeft
                                         l_inputs[i * 4 + 1], // hRight
                                         l_inputs[i * 4 + 2], // huLeft
                                         l_inputs[i * 4 + 3], // huRight
                                        l_netUpdatesL,
                                        l_netUpdatesR );
    
    l_hm = l_inputs[i * 4] + l_netUpdatesL[0] / l_sL;
    
    REQUIRE( l_hm == Approx(l_outputs[i]) );
  }

}
