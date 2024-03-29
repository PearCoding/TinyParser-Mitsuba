#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "tinyparser-mitsuba.h"

using namespace TPM_NAMESPACE;

TEST_CASE("Property Construction", "[property]")
{
	REQUIRE(Property::fromNumber(Number(0)).type() == PT_NUMBER);
	REQUIRE(Property::fromInteger(Integer(0)).type() == PT_INTEGER);
	REQUIRE(Property::fromBool(false).type() == PT_BOOL);
	REQUIRE(Property::fromVector(Vector(0, 0, 0)).type() == PT_VECTOR);
	REQUIRE(Property::fromColor(Color(0, 0, 0)).type() == PT_COLOR);
	REQUIRE(Property::fromTransform(Transform::fromIdentity()).type() == PT_TRANSFORM);
	REQUIRE(Property::fromString("").type() == PT_STRING);
	REQUIRE(Property::fromSpectrum(Spectrum()).type() == PT_SPECTRUM);
}

TEST_CASE("Property Access", "[property]")
{
	REQUIRE(Property::fromNumber(Number(1)).getNumber() == Number(1));
	REQUIRE(Property::fromInteger(Integer(1)).getInteger() == Integer(1));
	REQUIRE(Property::fromBool(true).getBool() == true);
	REQUIRE(Property::fromVector(Vector(1, 0, 0)).getVector() == Vector(1, 0, 0));
	REQUIRE(Property::fromColor(Color(1, 0, 0)).getColor() == Color(1, 0, 0));
	//REQUIRE(Property::fromTransform(Transform()).getTransform() == Transform());
	REQUIRE(Property::fromString("TEST").getString() == "TEST");
	//REQUIRE(Property::fromSpectrum(Spectrum()).getSpectrum() == Spectrum());
}