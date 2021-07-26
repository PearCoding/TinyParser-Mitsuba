#ifndef TINYPARSER_MITSUBA_C_H_
#define TINYPARSER_MITSUBA_C_H_

#include <stdint.h>

#ifdef _WIN32
#ifdef TPM_C_EXPORT
#define TPM_C_LIB __declspec(dllexport)
#elif defined(TPM_C_IMPORT)
#define TPM_C_LIB __declspec(dllimport)
#else
#define TPM_C_LIB
#endif
#elif __GNUC__ >= 4
#define TPM_C_LIB __attribute__((visibility("default")))
#else
#define TPM_C_LIB
#endif

typedef char tpm_bool;
#define TPM_TRUE (1)
#define TPM_FALSE (0)

typedef long long tpm_int64;
typedef intptr_t tpm_ref_t;

enum tpm_object_type {
	TPM_OT_ERROR = -1,
	TPM_OT_SCENE = 0,
	TPM_OT_BSDF,
	TPM_OT_EMITTER,
	TPM_OT_FILM,
	TPM_OT_INTEGRATOR,
	TPM_OT_MEDIUM,
	TPM_OT_PHASE,
	TPM_OT_RFILTER,
	TPM_OT_SAMPLER,
	TPM_OT_SENSOR,
	TPM_OT_SHAPE,
	TPM_OT_SUBSURFACE,
	TPM_OT_TEXTURE,
	TPM_OT_VOLUME
};

enum tpm_property_type {
	TPM_PT_ERROR = -1,
	TPM_PT_NONE	 = 0,
	TPM_PT_ANIMATION,
	TPM_PT_BLACKBODY,
	TPM_PT_BOOL,
	TPM_PT_INTEGER,
	TPM_PT_NUMBER,
	TPM_PT_COLOR,
	TPM_PT_SPECTRUM,
	TPM_PT_STRING,
	TPM_PT_TRANSFORM,
	TPM_PT_VECTOR,
};

/* Core stuff */
typedef struct {
	char type;
	char flags;
	short ref;
	void* ptr;
} _tpm_handle;
typedef _tpm_handle* tpm_object_handle;
typedef _tpm_handle* tpm_property_handle;
typedef _tpm_handle* tpm_spectrum_handle;

TPM_C_LIB void tpm_free_object(tpm_object_handle handle);
TPM_C_LIB void tpm_free_property(tpm_property_handle handle);
TPM_C_LIB void tpm_free_spectrum(tpm_spectrum_handle handle);
TPM_C_LIB tpm_object_type tpm_get_object_type(tpm_object_handle handle);
TPM_C_LIB tpm_property_type tpm_get_property_type(tpm_property_handle handle);

/* Loading stuff */
typedef struct {
	const char** lookup_dirs;
	size_t lookup_dirs_count;

	const char** argument_keys;
	const char** argument_values;
	size_t argument_counts;

	tpm_bool disable_lowercase_conversion;
} tpm_load_options;

TPM_C_LIB tpm_object_handle tpm_load_file(const char* path);
TPM_C_LIB tpm_object_handle tpm_load_file2(const char* path, const tpm_load_options* options);

TPM_C_LIB tpm_object_handle tpm_load_string(const char* str);
TPM_C_LIB tpm_object_handle tpm_load_string2(const char* str, const tpm_load_options* options);

TPM_C_LIB tpm_object_handle tpm_load_memory(const char* data, size_t size);
TPM_C_LIB tpm_object_handle tpm_load_memory2(const char* data, size_t size, const tpm_load_options* options);

/* Version stuff */
typedef struct {
	int major;
	int minor;
	int patch;
} tpm_version;
TPM_C_LIB tpm_version tpm_get_version();
TPM_C_LIB tpm_version tpm_get_scene_version(tpm_object_handle handle);

/* Object interface */
TPM_C_LIB const char* tpm_get_plugin_type(tpm_object_handle handle);
TPM_C_LIB tpm_property_handle tpm_get_property(tpm_object_handle handle, const char* key);
TPM_C_LIB size_t tpm_get_property_count(tpm_object_handle handle);
TPM_C_LIB const char* tpm_get_property_key(tpm_object_handle handle, size_t id);
TPM_C_LIB tpm_object_handle tpm_get_named_child(tpm_object_handle handle, const char* key);
TPM_C_LIB size_t tpm_get_named_child_count(tpm_object_handle handle);
TPM_C_LIB const char* tpm_get_named_child_key(tpm_object_handle handle, size_t id);
TPM_C_LIB tpm_object_handle tpm_get_anonymous_child(tpm_object_handle handle, size_t id);
TPM_C_LIB size_t tpm_get_anonymous_child_count(tpm_object_handle handle);

/* Property interface */
TPM_C_LIB tpm_bool tpm_property_get_bool(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_bool tpm_property_get_bool2(tpm_property_handle handle, tpm_bool def, tpm_bool* ok);
TPM_C_LIB tpm_int64 tpm_property_get_int(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_int64 tpm_property_get_int2(tpm_property_handle handle, tpm_int64 def, tpm_bool* ok);
TPM_C_LIB double tpm_property_get_number(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB double tpm_property_get_number2(tpm_property_handle handle, double def, tpm_bool* ok);

typedef struct {
	double e1;
	double e2;
	double e3;
} tpm_triplet;
TPM_C_LIB tpm_triplet tpm_property_get_vector(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_triplet tpm_property_get_vector2(tpm_property_handle handle, const tpm_triplet* def, tpm_bool* ok);
TPM_C_LIB tpm_triplet tpm_property_get_color(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_triplet tpm_property_get_color2(tpm_property_handle handle, const tpm_triplet* def, tpm_bool* ok);

typedef struct {
	double elements[4 * 4];
} tpm_transform;
TPM_C_LIB tpm_transform tpm_property_get_transform(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_transform tpm_property_get_transform2(tpm_property_handle handle, const tpm_transform* def, tpm_bool* ok);

TPM_C_LIB const char* tpm_property_get_string(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB const char* tpm_property_get_string2(tpm_property_handle handle, const char* def, tpm_bool* ok);

typedef struct {
	double temperature;
	double scale;
} tpm_blackbody;
TPM_C_LIB tpm_blackbody tpm_property_get_blackbody(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_blackbody tpm_property_get_blackbody2(tpm_property_handle handle, const tpm_blackbody* def, tpm_bool* ok);

TPM_C_LIB tpm_spectrum_handle tpm_property_get_spectrum(tpm_property_handle handle, tpm_bool* ok);
TPM_C_LIB tpm_bool tpm_spectrum_is_uniform(tpm_spectrum_handle handle);
TPM_C_LIB size_t tpm_spectrum_get_length(tpm_spectrum_handle handle);
TPM_C_LIB double tpm_spectrum_get_wavelength(tpm_spectrum_handle handle, size_t index);
TPM_C_LIB double tpm_spectrum_get_weight(tpm_spectrum_handle handle, size_t index);

/* TODO: Animation */
#endif /*TINYPARSER_MITSUBA_C_H_*/