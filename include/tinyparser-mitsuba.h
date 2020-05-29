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

namespace TPM_NAMESPACE {
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
	PT_FLOAT,
	PT_BOOL,
	PT_STRING,
	PT_POINT,
	PT_VECTOR,
	PT_RGB,
	PT_SPECTRUM
};

// --------------- Vector/Points
struct TPM_LIB Vector {
	float x, y, z;
	inline Vector(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{
	}
};
using Point = Vector;

// --------------- Transform
class TPM_LIB Transform {
};

// --------------- Spectrum
class TPM_LIB Spectrum {
public:
	Spectrum() = default;
	inline Spectrum(const std::vector<int>& wavelengths, const std::vector<float>& weights)
		: mWavelengths(wavelengths)
		, mWeights(weights)
	{
	}

	inline const std::vector<int>& wavelengths() const { return mWavelengths; }
	inline const std::vector<float>& weights() const { return mWeights; }

private:
	std::vector<int> mWavelengths;
	std::vector<float> mWeights;
};

class TPM_LIB Property {
public:
};
} // namespace TPM_NAMESPACE