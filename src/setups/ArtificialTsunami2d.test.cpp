#include <catch2/catch.hpp>
#include <cmath>

#include "ArtificialTsunami2d.h"

using namespace tsunami_lab;

TEST_CASE("getDisplacement produces bounded sinusoidal values", "[ArtificialTsunami2d]") {

    setups::ArtificialTsunami2d tsuami;

    REQUIRE(tsuami.getDisplacement(500, 500) == Approx(0.0).margin(1e-6));

    REQUIRE(tsuami.getDisplacement(0, 500) == Approx(0.0).margin(1e-6));

    t_real d = tsuami.getDisplacement(250, 0);
    REQUIRE(std::isfinite(d));
}

TEST_CASE("getBathymetry adds displacement and small offset", "[ArtificialTsunami2d]") {

    setups::ArtificialTsunami2d tsuami;

    t_real disp = tsuami.getDisplacement(100, 200);
    t_real expected = 1e-6 + disp;

    REQUIRE(tsuami.getBathymetry(100, 200) == Approx(expected).margin(1e-6));
}

TEST_CASE("getHeight follows threshold logic", "[ArtificialTsunami2d]") {

    setups::ArtificialTsunami2d tsuami;

    for (int l_x = 0; l_x < 1000; l_x += 100){
        t_real h = tsuami.getHeight(l_x, 500);
        t_real b = tsuami.getBathymetry(l_x, 500);
        
        if (-b < 0) {
            REQUIRE(h == 0);
        }
        else if (-b < 1e-6) {
            REQUIRE(h == Approx(1e-6).margin(1e-6));
        }
        else {
            REQUIRE(h > 1e-6);
        }
    }
}