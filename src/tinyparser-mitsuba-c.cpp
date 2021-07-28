#include "tinyparser-mitsuba-c.h"

// Actual implementation
#include "tinyparser-mitsuba.h"
#include <cassert>
#include <cstring>
#include <exception>

// Utility
enum tpm_handle_type {
	TPM_H_OBJECT = 0,
	TPM_H_PROPERTY,
	TPM_H_SPECTRUM
};

constexpr short TPM_H_F_DELETE = 0x1;

static bool _handle_valid(_tpm_handle* handle)
{
	return handle && handle->ref > 0 && handle->ptr;
}

static TPM_NAMESPACE::Object* _extract_object(tpm_object_handle handle)
{
	if (_handle_valid(handle) && handle->type == TPM_H_OBJECT) {
		return reinterpret_cast<TPM_NAMESPACE::Object*>(handle->ptr);
	} else {
		return nullptr;
	}
}

static tpm_object_handle _pack_object(TPM_NAMESPACE::Object* obj, bool isScene)
{
	tpm_object_handle handle = new _tpm_handle;
	handle->ptr				 = obj;
	handle->ref				 = 1;
	handle->type			 = TPM_H_OBJECT;
	handle->flags			 = isScene ? TPM_H_F_DELETE : 0;
	return handle;
}

static TPM_NAMESPACE::Property* _extract_property(tpm_property_handle handle)
{
	if (_handle_valid(handle) && handle->type == TPM_H_PROPERTY) {
		return reinterpret_cast<TPM_NAMESPACE::Property*>(handle->ptr);
	} else {
		return nullptr;
	}
}

static tpm_property_handle _pack_property(const TPM_NAMESPACE::Property& prop)
{
	tpm_property_handle handle = new _tpm_handle;
	handle->ptr				   = new TPM_NAMESPACE::Property(prop); // TODO: Better solution?
	handle->ref				   = 1;
	handle->type			   = TPM_H_PROPERTY;
	handle->flags			   = TPM_H_F_DELETE;
	return handle;
}

static TPM_NAMESPACE::Spectrum* _extract_spectrum(tpm_spectrum_handle handle)
{
	if (_handle_valid(handle) && handle->type == TPM_H_SPECTRUM) {
		return reinterpret_cast<TPM_NAMESPACE::Spectrum*>(handle->ptr);
	} else {
		return nullptr;
	}
}

static tpm_spectrum_handle _pack_spectrum(const TPM_NAMESPACE::Spectrum& spectrum)
{
	tpm_property_handle handle = new _tpm_handle;
	handle->ptr				   = new TPM_NAMESPACE::Spectrum(spectrum); // TODO: Better solution?
	handle->ref				   = 1;
	handle->type			   = TPM_H_SPECTRUM;
	handle->flags			   = TPM_H_F_DELETE;
	return handle;
}

static void _tpm_free(_tpm_handle* handle)
{
	if (!handle)
		return;

	assert(handle->ref > 0);

	handle->ref -= 1;
	if (handle->ref == 0) {
		if (handle->flags & TPM_H_F_DELETE) {
			if (handle->type == TPM_H_OBJECT) {
				auto ptr = _extract_object(handle);
				if (ptr->type() == TPM_NAMESPACE::OT_SCENE) {
					delete reinterpret_cast<TPM_NAMESPACE::Scene*>(ptr);
				} else {
					delete ptr;
				}
			} else if (handle->type == TPM_H_PROPERTY) {
				auto ptr = _extract_property(handle);
				delete ptr;
			} else if (handle->type == TPM_H_SPECTRUM) {
				auto ptr = _extract_spectrum(handle);
				delete ptr;
			}

			handle->ptr = nullptr; // For debug purposes
		}

		delete handle;
	}
}

static TPM_NAMESPACE::SceneLoader _construct_loader(const tpm_load_options* options)
{
	if (!options)
		return TPM_NAMESPACE::SceneLoader();

	TPM_NAMESPACE::SceneLoader loader;
	if (options->lookup_dirs) {
		for (size_t i = 0; i < options->lookup_dirs_count; ++i)
			loader.addLookupDir(options->lookup_dirs[i]);
	}
	if (options->argument_keys && options->argument_values) {
		for (size_t i = 0; i < options->argument_counts; ++i)
			loader.addArgument(options->argument_keys[i], options->argument_values[i]);
	}

	loader.disableLowerCaseConversion(options->disable_lowercase_conversion == TPM_TRUE);
	return loader;
}

// Implementation

void tpm_free_object(tpm_object_handle handle)
{
	_tpm_free(handle);
}

void tpm_free_property(tpm_property_handle handle)
{
	_tpm_free(handle);
}

void tpm_free_spectrum(tpm_spectrum_handle handle)
{
	_tpm_free(handle);
}

tpm_object_type tpm_get_object_type(tpm_object_handle handle)
{
	auto ptr = _extract_object(handle);
	if (ptr)
		return static_cast<tpm_object_type>(ptr->type());
	else
		return TPM_OT_ERROR;
}

tpm_property_type tpm_get_property_type(tpm_property_handle handle)
{
	auto ptr = _extract_property(handle);
	if (ptr)
		return static_cast<tpm_property_type>(ptr->type());
	else
		return TPM_PT_ERROR;
}

/* Loading stuff */
tpm_object_handle tpm_load_file(const char* path)
{
	return tpm_load_file2(path, nullptr);
}

tpm_object_handle tpm_load_file2(const char* path, const tpm_load_options* options)
{
	TPM_NAMESPACE::SceneLoader loader = _construct_loader(options);
	try {
		auto scene = loader.loadFromFile(path);
		return _pack_object(new TPM_NAMESPACE::Scene(scene), true);
	} catch (const std::exception& e) {
		if (options->error_callback)
			options->error_callback(e.what());
		return nullptr;
	}
}

tpm_object_handle tpm_load_string(const char* str)
{
	return tpm_load_string2(str, nullptr);
}
tpm_object_handle tpm_load_string2(const char* str, const tpm_load_options* options)
{
	TPM_NAMESPACE::SceneLoader loader = _construct_loader(options);
	try {
		auto scene = loader.loadFromString(str);
		return _pack_object(new TPM_NAMESPACE::Scene(scene), true);
	} catch (const std::exception& e) {
		if (options->error_callback)
			options->error_callback(e.what());
		return nullptr;
	}
}

tpm_object_handle tpm_load_memory(const char* data, size_t size)
{
	return tpm_load_memory2(data, size, nullptr);
}
tpm_object_handle tpm_load_memory2(const char* data, size_t size, const tpm_load_options* options)
{
	TPM_NAMESPACE::SceneLoader loader = _construct_loader(options);
	try {
		auto scene = loader.loadFromMemory(reinterpret_cast<const uint8_t*>(data), size);
		return _pack_object(new TPM_NAMESPACE::Scene(scene), true);
	} catch (const std::exception& e) {
		if (options->error_callback)
			options->error_callback(e.what());
		return nullptr;
	}
}

/* Version stuff */
tpm_version tpm_get_version()
{
	return tpm_version{ TPM_MAJOR_VERSION, TPM_MINOR_VERSION, TPM_PATCH_VERSION };
}

tpm_version tpm_get_scene_version(tpm_object_handle handle)
{
	auto ptr = _extract_object(handle);
	if (ptr && ptr->type() == TPM_NAMESPACE::OT_SCENE) {
		auto ptr2 = reinterpret_cast<const TPM_NAMESPACE::Scene*>(ptr);
		return tpm_version{ ptr2->versionMajor(), ptr2->versionMinor(), ptr2->versionPatch() };
	} else {
		return tpm_version{ -1, -1, -1 };
	}
}

/* Object interface */
size_t tpm_get_plugin_type(tpm_object_handle handle, char* str)
{
	auto ptr = _extract_object(handle);
	if (ptr) {
		size_t size = ptr->pluginType().size() + 1;
		if (str)
			std::memcpy(str, ptr->pluginType().c_str(), size);
		return size;
	} else {
		return 0;
	}
}

tpm_property_handle tpm_get_property(tpm_object_handle handle, const char* key)
{
	auto ptr = _extract_object(handle);
	if (ptr) {
		const auto prop = ptr->property(key);
		if (prop.isValid())
			return _pack_property(prop);
		else
			return nullptr;
	} else {
		return nullptr;
	}
}
size_t tpm_get_property_count(tpm_object_handle handle)
{
	auto ptr = _extract_object(handle);
	if (ptr)
		return ptr->properties().size();
	else
		return 0;
}

size_t tpm_get_property_key(tpm_object_handle handle, size_t id, char* str)
{
	auto ptr = _extract_object(handle);
	if (ptr) {
		auto it = ptr->properties().begin();
		std::advance(it, id);
		if (it != ptr->properties().end()) {
			size_t size = it->first.size() + 1;
			if (str)
				std::memcpy(str, it->first.c_str(), size + 1);
			return size;
		}
	}
	return 0;
}

tpm_object_handle tpm_get_named_child(tpm_object_handle handle, const char* key)
{
	auto ptr = _extract_object(handle);
	if (ptr) {
		const auto child = ptr->namedChild(key);
		if (child)
			return _pack_object(child.get(), false);
		else
			return nullptr;
	} else {
		return nullptr;
	}
}

size_t tpm_get_named_child_count(tpm_object_handle handle)
{
	auto ptr = _extract_object(handle);
	if (ptr)
		return ptr->namedChildren().size();
	else
		return 0;
}

size_t tpm_get_named_child_key(tpm_object_handle handle, size_t id, char* str)
{
	auto ptr = _extract_object(handle);
	if (ptr) {
		auto it = ptr->namedChildren().begin();
		std::advance(it, id);
		if (it != ptr->namedChildren().end()) {
			size_t size = it->first.size() + 1;
			if (str)
				std::memcpy(str, it->first.c_str(), size + 1);
			return size;
		}
	}
	return 0;
}

tpm_object_handle tpm_get_anonymous_child(tpm_object_handle handle, size_t id)
{
	auto ptr = _extract_object(handle);
	if (ptr) {
		if (id < ptr->anonymousChildren().size()) {
			const auto child = ptr->anonymousChildren()[id];
			if (child)
				return _pack_object(child.get(), false);
		}
	}

	return nullptr;
}
size_t tpm_get_anonymous_child_count(tpm_object_handle handle)
{
	auto ptr = _extract_object(handle);
	if (ptr)
		return ptr->anonymousChildren().size();
	else
		return 0;
}

/* Property interface */
tpm_bool tpm_property_get_bool(tpm_property_handle handle, tpm_bool* ok)
{
	return tpm_property_get_bool2(handle, TPM_FALSE, ok);
}
tpm_bool tpm_property_get_bool2(tpm_property_handle handle, tpm_bool def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2 = false;
		bool res = ptr->getBool(def == TPM_TRUE, &ok2);
		if (ok)
			*ok = ok2 ? TPM_TRUE : TPM_FALSE;
		return res ? TPM_TRUE : TPM_FALSE;
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return TPM_FALSE;
	}
}

tpm_int64 tpm_property_get_int(tpm_property_handle handle, tpm_bool* ok)
{
	return tpm_property_get_int2(handle, 0, ok);
}
tpm_int64 tpm_property_get_int2(tpm_property_handle handle, tpm_int64 def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2	   = false;
		const auto var = ptr->getInteger(def, &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			return var;
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return def;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return def;
	}
}

double tpm_property_get_number(tpm_property_handle handle, tpm_bool* ok)
{
	return tpm_property_get_number2(handle, 0.0, ok);
}
double tpm_property_get_number2(tpm_property_handle handle, double def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2	   = false;
		const auto var = ptr->getNumber(def, &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			return var;
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return def;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return def;
	}
}

tpm_triplet tpm_property_get_vector(tpm_property_handle handle, tpm_bool* ok)
{
	tpm_triplet def = tpm_triplet{ 0, 0, 0 };
	return tpm_property_get_vector2(handle, &def, ok);
}
tpm_triplet tpm_property_get_vector2(tpm_property_handle handle, const tpm_triplet* def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2	   = false;
		const auto var = ptr->getVector(TPM_NAMESPACE::Vector(def->e1, def->e2, def->e3), &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			return tpm_triplet{ var.x, var.y, var.z };
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return *def;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return *def;
	}
}

tpm_triplet tpm_property_get_color(tpm_property_handle handle, tpm_bool* ok)
{
	tpm_triplet def = tpm_triplet{ 0, 0, 0 };
	return tpm_property_get_color2(handle, &def, ok);
}
tpm_triplet tpm_property_get_color2(tpm_property_handle handle, const tpm_triplet* def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2	   = false;
		const auto var = ptr->getColor(TPM_NAMESPACE::Color(def->e1, def->e2, def->e3), &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			return tpm_triplet{ var.r, var.g, var.b };
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return *def;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return *def;
	}
}

tpm_transform tpm_property_get_transform(tpm_property_handle handle, tpm_bool* ok)
{
	tpm_transform transform;
	std::memset(&transform, 0, sizeof(tpm_transform));
	transform.elements[0 * 4 + 0] = 1;
	transform.elements[1 * 4 + 1] = 1;
	transform.elements[2 * 4 + 2] = 1;
	transform.elements[3 * 4 + 3] = 1;
	return tpm_property_get_transform2(handle, &transform, ok);
}
tpm_transform tpm_property_get_transform2(tpm_property_handle handle, const tpm_transform* def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2 = false;
		TPM_NAMESPACE::Transform transform;
		for (size_t i = 0; i < transform.matrix.size(); ++i)
			transform.matrix[i] = def->elements[i];
		const auto& var = ptr->getTransform(transform, &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			tpm_transform res;
			for (size_t i = 0; i < 4 * 4; ++i)
				res.elements[i] = var.matrix[i];
			return res;
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return *def;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return *def;
	}
}

size_t tpm_property_get_string(tpm_property_handle handle, tpm_bool* ok, char* str)
{
	const char* def = "";
	return tpm_property_get_string2(handle, def, ok, str);
}

size_t tpm_property_get_string2(tpm_property_handle handle, const char* def, tpm_bool* ok, char* str)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2		= false;
		const auto& var = ptr->getString(def, &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;

			size_t size = var.size() + 1;
			if (str)
				std::memcpy(str, var.c_str(), size);
			return size;
		}
	}

	if (ok)
		*ok = TPM_FALSE;
	size_t size = std::strlen(def) + 1;
	if (str)
		std::memcpy(str, def, size);
	return size;
}

tpm_blackbody tpm_property_get_blackbody(tpm_property_handle handle, tpm_bool* ok)
{
	tpm_blackbody def;
	def.temperature = 6404;
	def.scale		= 1;
	return tpm_property_get_blackbody2(handle, &def, ok);
}
tpm_blackbody tpm_property_get_blackbody2(tpm_property_handle handle, const tpm_blackbody* def, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2	   = false;
		const auto var = ptr->getBlackbody(TPM_NAMESPACE::Blackbody(def->temperature, def->scale), &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			return tpm_blackbody{ var.temperature, var.scale };
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return *def;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return *def;
	}
}

tpm_spectrum_handle tpm_property_get_spectrum(tpm_property_handle handle, tpm_bool* ok)
{
	auto ptr = _extract_property(handle);
	if (ptr) {
		bool ok2		 = false;
		const auto& spec = ptr->getSpectrum({}, &ok2);

		if (ok2) {
			if (ok)
				*ok = TPM_TRUE;
			return _pack_spectrum(spec);
		} else {
			if (ok)
				*ok = TPM_FALSE;
			return nullptr;
		}
	} else {
		if (ok)
			*ok = TPM_FALSE;
		return nullptr;
	}
}
tpm_bool tpm_spectrum_is_uniform(tpm_spectrum_handle handle)
{
	auto ptr = _extract_spectrum(handle);
	if (ptr)
		return ptr->isUniform();
	else
		return TPM_FALSE;
}
size_t tpm_spectrum_get_length(tpm_spectrum_handle handle)
{
	auto ptr = _extract_spectrum(handle);
	if (ptr)
		return ptr->isUniform() ? 1 : ptr->wavelengths().size();
	else
		return 0;
}
double tpm_spectrum_get_wavelength(tpm_spectrum_handle handle, size_t index)
{
	auto ptr = _extract_spectrum(handle);
	if (ptr)
		return ptr->isUniform() ? 0 : (index < ptr->wavelengths().size() ? ptr->wavelengths()[index] : 0);
	else
		return 0;
}
double tpm_spectrum_get_weight(tpm_spectrum_handle handle, size_t index)
{
	auto ptr = _extract_spectrum(handle);
	if (ptr)
		return ptr->isUniform() ? ptr->uniformValue() : (index < ptr->weights().size() ? ptr->weights()[index] : 0);
	else
		return 0;
}