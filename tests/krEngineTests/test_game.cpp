#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/game.h>

TEST_CASE("Global Game Loop Registry", "[game]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  int check1 = 0;
  int check2 = 0;

  SECTION("Basics")
  {
    GlobalGameLoop::set("foo", [&]() { ++check1; });
    GlobalGameLoop::set("bar", [&]() { ++check2; });

    GlobalGameLoop::tick();
    REQUIRE(check1 == 1);
    REQUIRE(check2 == 1);

    REQUIRE(GlobalGameLoop::remove("foo").Succeeded());
    REQUIRE(GlobalGameLoop::remove("bar").Succeeded());
    REQUIRE(GlobalGameLoop::remove("bar").Failed());
  }

  SECTION("Priority")
  {
    GlobalGameLoop::set("bar", [&]() { REQUIRE(check1 == 1); ++check1; });
    GlobalGameLoop::set("foo", [&]() { REQUIRE(check1 == 0); ++check1; });

    GlobalGameLoop::setPriority("foo", 10);
    GlobalGameLoop::setPriority("bar", -10);

    GlobalGameLoop::tick();
    REQUIRE(check1 == 2);
    REQUIRE(check2 == 0);
  }
}
