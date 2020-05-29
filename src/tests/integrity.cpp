#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "tinyparser-mitsuba.h"

using namespace TPM_NAMESPACE;

TEST_CASE("Version Detection", "[integrity]")
{
	auto scene = Scene::loadFromString("<scene version='1.2.3'></scene>");
	REQUIRE(scene.versionMajor() == 1);
	REQUIRE(scene.versionMinor() == 2);
	REQUIRE(scene.versionPatch() == 3);
}

TEST_CASE("Version Detection FAILURE", "[integrity]")
{
	auto build = []() { auto scene = Scene::loadFromString("<scene version='12.3.'></scene>"); (void)scene; };
	CHECK_THROWS(build());
}