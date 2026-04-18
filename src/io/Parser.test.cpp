#include <catch2/catch.hpp>
#define private public
#include "Parser.h"
#undef public

TEST_CASE( "Test argument parsing.", "[Parser]" ){
    /*
    * Input: argc = 5, argv = {"programName", "solver=roe", "aaa", "cellx=0", "bbb"}
    * get("solver", "bbbb") -> roe
    * get("cellx", 1) -> 0
    * get("ble", 1) ->
    * get("bla", "ble") -> "ble"
    *   
    */

  char name[] = "programName";
  char arg1[] = "solver=roe";
  char arg2[] = "aaa";
  char arg3[] = "cellx=0";
  char arg4[] = "bbb";

  char *argv1[] = {name, arg1, arg2, arg3, arg4};
  auto l_parser = tsunami_lab::io::Parser(
    5,
    argv1
  );

  auto l_solver = l_parser.get("solver", "bbbb");
  tsunami_lab::t_idx l_cellx = l_parser.get("cellx", 1);
  tsunami_lab::t_idx l_bli = l_parser.get("bli", 1);
  auto l_bla = l_parser.get("bla", "ble");

  REQUIRE_THAT(l_solver, Catch::Matchers::Equals("roe", Catch::CaseSensitive::Yes));
  REQUIRE( l_cellx == Approx( 0 ) );
  REQUIRE( l_bli == Approx( 1 ) );
  REQUIRE_THAT(l_bla, Catch::Matchers::Equals("ble", Catch::CaseSensitive::Yes));

}