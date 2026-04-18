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

  char name[] = "programName";
  char arg1_1[] = "solver=10";
  char arg2_1[] = "aaa";
  char arg3_1[] = "cellx=0";
  char arg4_1[] = "bbb";

  char *argv1[] = {name, arg1_1, arg2_1, arg3_1, arg4_1};
  tsunami_lab::t_idx l_solverId1, l_cellx1; 
  tsunami_lab::io::Parser::parse(
    5,
    argv1,
    l_cellx1,
    l_solverId1
  );
  REQUIRE( l_solverId1 == Approx( 0 ) );
  REQUIRE( l_cellx1 == Approx( 1 ) );



  char arg1_2[] = "solver=1";
  char arg2_2[] = "aaa";
  char arg3_2[] = "cellx=10";
  char arg4_2[] = "bbb";

  char *argv2[] = {name, arg1_2, arg2_2, arg3_2, arg4_2};
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