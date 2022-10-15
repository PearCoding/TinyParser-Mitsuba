#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "tinyparser-mitsuba.h"

using namespace TPM_NAMESPACE;

TEST_CASE("Identity", "[transform]")
{
	Transform M = Transform::fromIdentity();
	REQUIRE(M(0, 0) == 1);
	REQUIRE(M(0, 1) == 0);
	REQUIRE(M(0, 2) == 0);
	REQUIRE(M(0, 3) == 0);
	REQUIRE(M(1, 0) == 0);
	REQUIRE(M(1, 1) == 1);
	REQUIRE(M(1, 2) == 0);
	REQUIRE(M(1, 3) == 0);
	REQUIRE(M(2, 0) == 0);
	REQUIRE(M(2, 1) == 0);
	REQUIRE(M(2, 2) == 1);
	REQUIRE(M(2, 3) == 0);
	REQUIRE(M(3, 0) == 0);
	REQUIRE(M(3, 1) == 0);
	REQUIRE(M(3, 2) == 0);
	REQUIRE(M(3, 3) == 1);
}

TEST_CASE("Translation", "[transform]")
{
	Transform M = Transform::fromTranslation(Vector(1, 2, 3));
	REQUIRE(M(0, 0) == 1);
	REQUIRE(M(0, 1) == 0);
	REQUIRE(M(0, 2) == 0);
	REQUIRE(M(0, 3) == 1);
	REQUIRE(M(1, 0) == 0);
	REQUIRE(M(1, 1) == 1);
	REQUIRE(M(1, 2) == 0);
	REQUIRE(M(1, 3) == 2);
	REQUIRE(M(2, 0) == 0);
	REQUIRE(M(2, 1) == 0);
	REQUIRE(M(2, 2) == 1);
	REQUIRE(M(2, 3) == 3);
	REQUIRE(M(3, 0) == 0);
	REQUIRE(M(3, 1) == 0);
	REQUIRE(M(3, 2) == 0);
	REQUIRE(M(3, 3) == 1);
}

TEST_CASE("Scale", "[transform]")
{
	Transform M = Transform::fromScale(Vector(1, 2, 3));
	REQUIRE(M(0, 0) == 1);
	REQUIRE(M(0, 1) == 0);
	REQUIRE(M(0, 2) == 0);
	REQUIRE(M(0, 3) == 0);
	REQUIRE(M(1, 0) == 0);
	REQUIRE(M(1, 1) == 2);
	REQUIRE(M(1, 2) == 0);
	REQUIRE(M(1, 3) == 0);
	REQUIRE(M(2, 0) == 0);
	REQUIRE(M(2, 1) == 0);
	REQUIRE(M(2, 2) == 3);
	REQUIRE(M(2, 3) == 0);
	REQUIRE(M(3, 0) == 0);
	REQUIRE(M(3, 1) == 0);
	REQUIRE(M(3, 2) == 0);
	REQUIRE(M(3, 3) == 1);
}

TEST_CASE("Rotation", "[transform]")
{
	constexpr auto angle = Number(180);
	const auto ca		 = std::cos(degToRad(angle));
	const auto sa		 = std::sin(degToRad(angle));

	Transform M = Transform::fromRotation(Vector(1, 0, 0), angle); // Clockwise order
	REQUIRE(M(0, 0) == 1);
	REQUIRE(M(0, 1) == 0);
	REQUIRE(M(0, 2) == 0);
	REQUIRE(M(0, 3) == 0);
	REQUIRE(M(1, 0) == 0);
	REQUIRE(M(1, 1) == Catch::Approx(ca));
	REQUIRE(M(1, 2) == Catch::Approx(-sa));
	REQUIRE(M(1, 3) == 0);
	REQUIRE(M(2, 0) == 0);
	REQUIRE(M(2, 1) == Catch::Approx(sa));
	REQUIRE(M(2, 2) == Catch::Approx(ca));
	REQUIRE(M(2, 3) == 0);
	REQUIRE(M(3, 0) == 0);
	REQUIRE(M(3, 1) == 0);
	REQUIRE(M(3, 2) == 0);
	REQUIRE(M(3, 3) == 1);
}

TEST_CASE("LookAt", "[transform]")
{
	Transform M = Transform::fromLookAt(Vector(1, 0, 0), Vector(0, 0, 0), Vector(0, 0, 1));
	REQUIRE(M(0, 0) == 0);
	REQUIRE(M(0, 1) == 0);
	REQUIRE(M(0, 2) == -1);
	REQUIRE(M(0, 3) == 1);

	REQUIRE(M(1, 0) == -1);
	REQUIRE(M(1, 1) == 0);
	REQUIRE(M(1, 2) == 0);
	REQUIRE(M(1, 3) == 0);

	REQUIRE(M(2, 0) == 0);
	REQUIRE(M(2, 1) == 1);
	REQUIRE(M(2, 2) == 0);
	REQUIRE(M(2, 3) == 0);

	REQUIRE(M(3, 0) == 0);
	REQUIRE(M(3, 1) == 0);
	REQUIRE(M(3, 2) == 0);
	REQUIRE(M(3, 3) == 1);
}

TEST_CASE("Simple Transform", "[transform]")
{
	Transform M = Transform::fromTranslation(Vector(1, 0, 0)) * Transform::fromScale(Vector(2,2,2));
	REQUIRE(M(0, 0) == 2);
	REQUIRE(M(0, 1) == 0);
	REQUIRE(M(0, 2) == 0);
	REQUIRE(M(0, 3) == 1);
	REQUIRE(M(1, 0) == 0);
	REQUIRE(M(1, 1) == 2);
	REQUIRE(M(1, 2) == 0);
	REQUIRE(M(1, 3) == 0);
	REQUIRE(M(2, 0) == 0);
	REQUIRE(M(2, 1) == 0);
	REQUIRE(M(2, 2) == 2);
	REQUIRE(M(2, 3) == 0);
	REQUIRE(M(3, 0) == 0);
	REQUIRE(M(3, 1) == 0);
	REQUIRE(M(3, 2) == 0);
	REQUIRE(M(3, 3) == 1);
}