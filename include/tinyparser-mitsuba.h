#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
#ifdef TPM_EXPORT
#define TPM_LIB __declspec(dllexport)
#elif defined(TPM_IMPORT)
#define TPM_LIB __declspec(dllimport)
#else
#define TPM_LIB
#endif
#elif __GNUC__ >= 4
#define TPM_LIB __attribute__((visibility("default")))
#else
#define TPM_LIB
#endif

#define TPM_MAJOR_VERSION 0
#define TPM_MINOR_VERSION 1
#define TPM_PATCH_VERSION 0

#define TPM_NAMESPACE tinyparser_mitsuba

// Check for C++ features
/*#ifdef __has_cpp_attribute
#if __has_cpp_attribute(nodiscard)
#define TPM_NODISCARD [[nodiscard]]
#endif
#endif*/

#ifndef TPM_NODISCARD
#if __cpp_constexpr >= 201603L
#define TPM_NODISCARD [[nodiscard]]
#else
#define TPM_NODISCARD
#endif
#endif

namespace TPM_NAMESPACE {
#ifdef TPM_NUMBER_AS_DOUBLE
using Number = double;
#else
using Number = float;
#endif

using Integer = int64_t;

// --------------- Enums
enum EntityType {
	ET_BSDF = 0,
	ET_SHAPE,
	ET_INTEGRATOR,
	ET_SENSOR,
	ET_EMITTER,
	ET_FILM,
	ET_TEXTURE,
	ET_SAMPLER,
	ET_RFILTER
};

enum PropertyType {
	PT_INTEGER = 0,
	PT_NUMBER,
	PT_BOOL,
	PT_STRING,
	PT_POINT,
	PT_VECTOR,
	PT_TRANSFORM,
	PT_RGB,
	PT_SPECTRUM
};

// --------------- Vector/Points
struct TPM_LIB Vector {
	Number x, y, z;
	inline Vector(Number x, Number y, Number z)
		: x(x)
		, y(y)
		, z(z)
	{
	}
};
inline TPM_NODISCARD bool operator==(const Vector& a, const Vector& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
inline TPM_NODISCARD bool operator!=(const Vector& a, const Vector& b)
{
	return !(a == b);
}
using Point = Vector;

// --------------- Transform
class TPM_LIB Transform {
};

// --------------- RGB
struct TPM_LIB RGB {
	Number r, g, b;
	inline RGB(Number r, Number g, Number b)
		: r(r)
		, g(g)
		, b(b)
	{
	}
};
inline TPM_NODISCARD bool operator==(const RGB& a, const RGB& b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b;
}
inline TPM_NODISCARD bool operator!=(const RGB& a, const RGB& b)
{
	return !(a == b);
}

// --------------- Spectrum
class TPM_LIB Spectrum {
public:
	Spectrum() = default;
	inline Spectrum(const std::vector<int>& wavelengths, const std::vector<Number>& weights)
		: mWavelengths(wavelengths)
		, mWeights(weights)
	{
	}

	inline TPM_NODISCARD const std::vector<int>& wavelengths() const { return mWavelengths; }
	inline TPM_NODISCARD const std::vector<Number>& weights() const { return mWeights; }

private:
	std::vector<int> mWavelengths;
	std::vector<Number> mWeights;
};

// --------------- Property
class TPM_LIB Property {
public:
	Property(const Property& other) = default;
	Property(Property&& other)		= default;

	Property& operator=(const Property& other) = default;
	Property& operator=(Property&& other) = default;

	inline TPM_NODISCARD PropertyType type() const { return mType; }

	inline Number getNumber(Number def = Number(0), bool* ok = nullptr) const
	{
		if (mType == PT_NUMBER) {
			if (ok)
				*ok = true;
			return mNumber;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromNumber(Number v)
	{
		Property p(PT_NUMBER);
		p.mNumber = v;
		return p;
	}

	inline Integer getInteger(Integer def = Integer(0), bool* ok = nullptr) const
	{
		if (mType == PT_INTEGER) {
			if (ok)
				*ok = true;
			return mInteger;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromInteger(Integer v)
	{
		Property p(PT_INTEGER);
		p.mInteger = v;
		return p;
	}

	inline bool getBool(bool def = false, bool* ok = nullptr) const
	{
		if (mType == PT_BOOL) {
			if (ok)
				*ok = true;
			return mBool;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromBool(bool b)
	{
		Property p(PT_BOOL);
		p.mBool = b;
		return p;
	}

	inline const Point& getPoint(const Point& def = Point(0, 0, 0), bool* ok = nullptr) const
	{
		if (mType == PT_POINT) {
			if (ok)
				*ok = true;
			return mPoint;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromPoint(const Point& v)
	{
		Property p(PT_POINT);
		p.mPoint = v;
		return p;
	}

	inline const Vector& getVector(const Vector& def = Vector(0, 0, 0), bool* ok = nullptr) const
	{
		if (mType == PT_VECTOR) {
			if (ok)
				*ok = true;
			return mVector;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromVector(const Vector& v)
	{
		Property p(PT_VECTOR);
		p.mVector = v;
		return p;
	}

	inline const Transform& getTransform(const Transform& def = Transform(), bool* ok = nullptr) const
	{
		if (mType == PT_TRANSFORM) {
			if (ok)
				*ok = true;
			return mTransform;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromTransform(const Transform& v)
	{
		Property p(PT_TRANSFORM);
		p.mTransform = v;
		return p;
	}

	inline const RGB& getRGB(const RGB& def = RGB(0, 0, 0), bool* ok = nullptr) const
	{
		if (mType == PT_RGB) {
			if (ok)
				*ok = true;
			return mRGB;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromRGB(const RGB& rgb)
	{
		Property p(PT_RGB);
		p.mRGB = rgb;
		return p;
	}

	inline const std::string& getString(const std::string& def = "", bool* ok = nullptr) const
	{
		if (mType == PT_STRING) {
			if (ok)
				*ok = true;
			return mString;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromString(const std::string& v)
	{
		Property p(PT_STRING);
		p.mString = v;
		return p;
	}

	inline const Spectrum& getSpectrum(const Spectrum& def = Spectrum(), bool* ok = nullptr) const
	{
		if (mType == PT_SPECTRUM) {
			if (ok)
				*ok = true;
			return mSpectrum;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromSpectrum(const Spectrum& spec)
	{
		Property p(PT_SPECTRUM);
		p.mSpectrum = spec;
		return p;
	}

private:
	inline Property(PropertyType type)
		: mType(type)
	{
	}

	PropertyType mType;

	// Data Types
	union {
		Number mNumber;
		Integer mInteger;
		bool mBool;

		Point mPoint;
		Vector mVector;
		Transform mTransform;
		RGB mRGB;
	};
	std::string mString;
	Spectrum mSpectrum;
};
} // namespace TPM_NAMESPACE