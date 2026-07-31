#include <catch2/catch.hpp>
#define private public
#include "Parser.h"
#undef private

TEST_CASE( "Test argument parsing.", "[Parser]" ){
    /*
    * Input: argc = 5, argv = {"programName", "solver=roe", "aaa", "cellx=0", "bbb=1.001"}
    * get("solver", "bbbb") -> roe
    * get("cellx", 1) -> 0
    * get("bli", 1) -> 1
    * get("bbb", 1.0) -> 1.001
    * get("bla", "ble") -> "ble"
    * 
    *   
    */

  char name[] = "programName";
  char arg1[] = "solver=roe";
  char arg2[] = "aaa";
  char arg3[] = "cellx=0";
  char arg4[] = "bbb=1.001";

  char *argv1[] = {name, arg1, arg2, arg3, arg4};

  REQUIRE_THROWS(tsunami_lab::io::Parser(
    5,
    argv1
  ));

  char arg5[] = "args=testfile.yaml";
  char arg6[] = "help";

  char *argv2[] = {name, arg5, arg6};

  tsunami_lab::io::Parser* l_parser = new tsunami_lab::io::Parser(3, argv2);


  std::string l_args;
  tsunami_lab::t_idx l_bli;
  float l_bbb;
  std::string l_bla;

  l_args = l_parser -> get("args", "bbbb");
  l_bli = l_parser -> get("bli", (tsunami_lab::t_real) 1);
  l_bbb = l_parser -> get("bbb", (float) 1.0);
  l_bla = l_parser -> get("bla", "ble");

  REQUIRE_THAT(l_args, Catch::Matchers::Equals("testfile.yaml", Catch::CaseSensitive::Yes));
  REQUIRE( l_bli == 1 );
  REQUIRE( l_bbb == 1.0f );
  REQUIRE_THAT(l_bla, Catch::Matchers::Equals("ble", Catch::CaseSensitive::Yes));
  

  char arg7[] = "solver";

  char* argv3[] = {name, arg7};

  REQUIRE_THROWS( tsunami_lab::io::Parser(2, argv3));

  delete l_parser;

}
