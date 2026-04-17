#include <catch2/catch.hpp>
#define private public
#include "Parser.h"
#undef public

TEST_CASE( "Test argument parsing.", "[Parser]" ){
    /*
    * Input: argc = 5, argv = {"programName", "solver=10", "aaa", "cellx=0", "bbb"}
    * Output: o_solverId = 0, o_cellx = 1
    * 
    * Input argc = 5, argv = {"programName", "solver=1", "aaa", "cellx=10", "bbb"}
    * Output: o_solverId = 1, o_cellx = 10
    */

  const char *argv1[] = {"programName", "solver=10", "aaa", "cellx=0", "bbb"};
  tsunami_lab::t_idx l_solverId1, l_cellx1; 
  tsunami_lab::io::Parser::parse(
    5,
    argv1,
    l_cellx1,
    l_solverId1
  );
  REQUIRE( l_solverId1 == Approx( 0 ) );
  REQUIRE( l_cellx1 == Approx( 1 ) );


  const char *argv2[] = {"programName", "solver=1", "aaa", "cellx=10", "bbb"};
  tsunami_lab::t_idx l_solverId2, l_cellx2; 
  tsunami_lab::io::Parser::parse(
    5,
    argv2,
    l_cellx2,
    l_solverId2
  );
  REQUIRE( l_solverId2 == Approx( 1 ) );
  REQUIRE( l_cellx2 == Approx( 10 ) );
}