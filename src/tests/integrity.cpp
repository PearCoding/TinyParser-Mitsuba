#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "tinyparser-mitsuba.h"

using namespace TPM_NAMESPACE;

TEST_CASE("Version Detection", "[integrity]")
{
	SceneLoader loader;
	auto scene = loader.loadFromString("<scene version='1.2.3'></scene>");
	REQUIRE(scene.versionMajor() == 1);
	REQUIRE(scene.versionMinor() == 2);
	REQUIRE(scene.versionPatch() == 3);
}

TEST_CASE("Version Detection FAILURE", "[integrity]")
{
	SceneLoader loader;
	auto build = [&]() { auto scene = loader.loadFromString("<scene version='12.3.'></scene>"); (void)scene; };
	CHECK_THROWS(build());
}

TEST_CASE("Arguments", "[integrity]")
{
	SceneLoader loader1;
	loader1.addArgument("test", "42");

	// Without default + argument
	auto scene = loader1.loadFromString("<scene version='0.6'><integer name='test' value='$test' /></scene>");
	auto prop  = scene["test"];
	REQUIRE(prop.getInteger() == 42);

	// Wit default + argument
	scene = loader1.loadFromString("<scene version='0.6'><default name='test' value='56'/><integer name='test' value='$test' /></scene>");
	prop  = scene["test"];
	REQUIRE(prop.getInteger() == 42);

	SceneLoader loader2;

	// With default + without argument
	scene = loader2.loadFromString("<scene version='0.6'><default name='test' value='56'/><integer name='test' value='$test' /></scene>");
	prop  = scene["test"];
	REQUIRE(prop.getInteger() == 56);

	// Without default + without argument
	auto build = [&]() { auto scene = loader2.loadFromString("<scene version='0.6'><integer name='test' value='$test' /></scene>"); (void)scene; };
	CHECK_THROWS(build());
}

TEST_CASE("Vector", "[integrity]")
{
	SceneLoader loader;
	const Vector v(56, 42, 7);

	auto scene = loader.loadFromString("<scene version='0.6'><point name='test' value='5.6e1, 42.0; 7' /></scene>");
	auto prop  = scene["test"];
	REQUIRE(prop.getVector() == v);

	scene = loader.loadFromString("<scene version='0.6'><vector name='test' x='5.6e1' y='42.0' z='7' /></scene>");
	prop  = scene["test"];
	REQUIRE(prop.getVector() == v);
}

TEST_CASE("Spectrum", "[integrity]")
{
	SceneLoader loader;
	auto scene = loader.loadFromString("<scene version='0.6'><spectrum name='test' value='42' /></scene>");
	auto prop  = scene["test"];
	REQUIRE(prop.getSpectrum().isUniform());
	REQUIRE(prop.getSpectrum().uniformValue() == 42);

	scene = loader.loadFromString("<scene version='0.6'><spectrum name='test' value='560:0.5, 630:1 720:0.5' /></scene>");
	prop  = scene["test"];
	REQUIRE(!prop.getSpectrum().isUniform());

	auto wvls	 = prop.getSpectrum().wavelengths();
	auto weights = prop.getSpectrum().weights();
	REQUIRE(wvls.size() == 3);
	REQUIRE(wvls[0] == 560);
	REQUIRE(wvls[1] == 630);
	REQUIRE(wvls[2] == 720);
	REQUIRE(weights.size() == 3);
	REQUIRE(weights[0] == 0.5);
	REQUIRE(weights[1] == 1);
	REQUIRE(weights[2] == 0.5);
}