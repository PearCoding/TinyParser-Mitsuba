#pragma once

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
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

#if __cplusplus >= 201703L // 2017
#define TPM_NODISCARD [[nodiscard]]
#endif

#ifndef TPM_NODISCARD
#define TPM_NODISCARD
#endif

#ifndef TPM_HAS_STRING_VIEW
#if __cpp_lib_string_view >= 201606L
#define TPM_HAS_STRING_VIEW
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
enum ObjectType {
	OT_SCENE = 0,
	OT_BSDF,
	OT_EMITTER,
	OT_FILM,
	OT_INTEGRATOR,
	OT_MEDIUM,
	OT_PHASE,
	OT_RFILTER,
	OT_SAMPLER,
	OT_SENSOR,
	OT_SHAPE,
	OT_SUBSURFACE,
	OT_TEXTURE,
	OT_VOLUME,
	_OT_COUNT
};

enum PropertyType {
	PT_NONE = 0,
	PT_BLACKBODY, // Uses Number
	PT_BOOL,
	PT_INTEGER,
	PT_NUMBER,
	PT_RGB,
	PT_SPECTRUM,
	PT_STRING,
	PT_TRANSFORM,
	PT_VECTOR,
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
struct TPM_LIB Transform {
	std::array<float, 4 * 4> matrix; // Row major

	inline float& operator()(int i, int j) { return matrix[i * 4 + j]; }
	inline float operator()(int i, int j) const { return matrix[i * 4 + j]; }
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

// --------------- Blackbody
struct TPM_LIB Blackbody {
	Number temperature, scale;
	inline Blackbody(Number temperature, Number scale)
		: temperature(temperature)
		, scale(scale)
	{
	}
};
inline TPM_NODISCARD bool operator==(const Blackbody& a, const Blackbody& b)
{
	return a.temperature == b.temperature && a.scale == b.scale;
}
inline TPM_NODISCARD bool operator!=(const Blackbody& a, const Blackbody& b)
{
	return !(a == b);
}

// --------------- Property
class TPM_LIB Property {
public:
	inline Property()
		: mType(PT_NONE)
	{
	}

	Property(const Property& other) = default;
	Property(Property&& other)		= default;

	Property& operator=(const Property& other) = default;
	Property& operator=(Property&& other) = default;

	inline TPM_NODISCARD PropertyType type() const { return mType; }
	inline TPM_NODISCARD bool isValid() const { return mType != PT_NONE; }

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

	inline Blackbody getBlackbody(const Blackbody& def = Blackbody(6504, 1), bool* ok = nullptr) const
	{
		if (mType == PT_BLACKBODY) {
			if (ok)
				*ok = true;
			return mBlackbody;
		} else {
			if (ok)
				*ok = false;
			return def;
		}
	}
	static TPM_NODISCARD inline Property fromBlackbody(const Blackbody& v)
	{
		Property p(PT_BLACKBODY);
		p.mBlackbody = v;
		return p;
	}

private:
	inline explicit Property(PropertyType type)
		: mType(type)
	{
	}

	PropertyType mType;

	// Data Types
	union {
		Number mNumber;
		Integer mInteger;
		bool mBool;

		Vector mVector;
		Transform mTransform;
		RGB mRGB;
		Blackbody mBlackbody;
	};
	std::string mString;
	Spectrum mSpectrum;
};

// --------------- Object
class TPM_LIB Object {
public:
	inline explicit Object(ObjectType type)
		: mType(type)
	{
	}

	inline TPM_NODISCARD ObjectType type() const { return mType; }

	inline TPM_NODISCARD Property property(const std::string& key) const
	{
		return mProperties.count(key) ? mProperties.at(key) : Property();
	}

	inline void setProperty(const std::string& key, const Property& prop)
	{
		mProperties[key] = prop;
	}

	inline TPM_NODISCARD Property& operator[](const std::string& key) { return mProperties[key]; }

	inline TPM_NODISCARD Property operator[](const std::string& key) const { return property(key); }

	inline void addObject(const std::shared_ptr<Object>& obj)
	{
		mChildren.push_back(obj);
	}

	inline TPM_NODISCARD const std::vector<std::shared_ptr<Object>>& objects() const { return mChildren; }

private:
	ObjectType mType;
	std::unordered_map<std::string, Property> mProperties;
	std::vector<std::shared_ptr<Object>> mChildren;
};

// --------------- ArgumentContainer
using ArgumentContainer = std::unordered_map<std::string, std::string>;

// --------------- Scene
class TPM_LIB Scene : public Object {
	friend class SceneLoader;

public:
	static inline TPM_NODISCARD Scene loadFromFile(const std::string& path, const ArgumentContainer& cnt = ArgumentContainer())
	{
		return loadFromFile(path.c_str(), cnt);
	}
	static inline TPM_NODISCARD Scene loadFromString(const std::string& str, const ArgumentContainer& cnt = ArgumentContainer());

#ifdef TPM_HAS_STRING_VIEW
	static inline TPM_NODISCARD Scene loadFromString(const std::string_view& str, const ArgumentContainer& cnt = ArgumentContainer())
	{
		return loadFromString(str.data(), str.size(), cnt);
	}
#endif

	// static TPM_NODISCARD Scene loadFromStream(std::istream& stream);

	static TPM_NODISCARD Scene loadFromFile(const char* path, const ArgumentContainer& cnt = ArgumentContainer());
	static TPM_NODISCARD Scene loadFromString(const char* str, const ArgumentContainer& cnt = ArgumentContainer());
	static TPM_NODISCARD Scene loadFromMemory(const uint8_t* data, size_t size, const ArgumentContainer& cnt = ArgumentContainer());

	Scene(const Scene& other) = default;
	Scene(Scene&& other)	  = default;

	Scene& operator=(const Scene& other) = default;
	Scene& operator=(Scene&& other) = default;

	inline TPM_NODISCARD int versionMajor() const { return mVersionMajor; }
	inline TPM_NODISCARD int versionMinor() const { return mVersionMinor; }
	inline TPM_NODISCARD int versionPatch() const { return mVersionPatch; }

private:
	inline Scene()
		: Object(OT_SCENE)
	{
	}

	int mVersionMajor;
	int mVersionMinor;
	int mVersionPatch;
};
} // namespace TPM_NAMESPACE