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

TEST_CASE("Arguments", "[integrity]")
{
	ArgumentContainer cnt;
	cnt["test"] = "42";

	// Without default + argument
	auto scene = Scene::loadFromString("<scene version='0.6'><integer name='test' value='$test' /></scene>", cnt);
	auto prop  = scene["test"];
	REQUIRE(prop.getInteger() == 42);

	// Wit default + argument
	scene = Scene::loadFromString("<scene version='0.6'><default name='test' value='56'/><integer name='test' value='$test' /></scene>", cnt);
	prop  = scene["test"];
	REQUIRE(prop.getInteger() == 42);

	// With default + without argument
	scene = Scene::loadFromString("<scene version='0.6'><default name='test' value='56'/><integer name='test' value='$test' /></scene>" /*, cnt*/);
	prop  = scene["test"];
	REQUIRE(prop.getInteger() == 56);

	// Without default + without argument
	auto build = []() { auto scene = Scene::loadFromString("<scene version='0.6'><integer name='test' value='$test' /></scene>"/*, cnt*/); (void)scene; };
	CHECK_THROWS(build());
}

TEST_CASE("Vector", "[integrity]")
{
	const Vector v(56, 42, 7);

	auto scene = Scene::loadFromString("<scene version='0.6'><point name='test' value='5.6e1, 42.0; 7' /></scene>");
	auto prop  = scene["test"];
	REQUIRE(prop.getVector() == v);

	// Wit default + argument
	scene = Scene::loadFromString("<scene version='0.6'><vector name='test' x='5.6e1' y='42.0' z='7' /></scene>");
	prop  = scene["test"];
	REQUIRE(prop.getVector() == v);
}