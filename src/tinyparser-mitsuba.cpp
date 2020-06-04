#include "tinyparser-mitsuba.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

#include <tinyxml2.h>

namespace TPM_NAMESPACE {

static inline Vector normalize(const Vector& v)
{
	const Number n = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return Vector(v.x / n, v.y / n, v.z / n);
}

static inline Vector cross(const Vector& a, const Vector& b)
{
	return Vector(a.y * b.z - a.z * b.y,
				  a.z * b.x - a.x * b.z,
				  a.x * b.y - a.y * b.x);
}

// ------------ Transform
Transform Transform::fromIdentity()
{
	return Transform({ Number(1), Number(0), Number(0), Number(0),
					   Number(0), Number(1), Number(0), Number(0),
					   Number(0), Number(0), Number(1), Number(0),
					   Number(0), Number(0), Number(0), Number(1) });
}

Transform Transform::fromTranslation(const Vector& delta)
{
	return Transform({ Number(1), Number(0), Number(0), delta.x,
					   Number(0), Number(1), Number(0), delta.y,
					   Number(0), Number(0), Number(1), delta.z,
					   Number(0), Number(0), Number(0), Number(1) });
}
Transform Transform::fromScale(const Vector& scale)
{
	return Transform({ scale.x, Number(0), Number(0), Number(0),
					   Number(0), scale.y, Number(0), Number(0),
					   Number(0), Number(0), scale.z, Number(0),
					   Number(0), Number(0), Number(0), Number(1) });
}

Transform Transform::fromRotation(const Vector& axis, Number angle)
{
	const Vector aa = normalize(axis);
	const auto sa	= std::sin(angle);
	const auto ca	= std::cos(angle);
	const auto nca	= Number(1) - ca;

	return Transform({ ca + aa.x * aa.x * nca, aa.x * aa.y * nca - aa.z * sa, aa.x * aa.z * nca + aa.y * sa, Number(0),
					   aa.y * aa.x * nca + aa.z * sa, ca + aa.y * aa.y * nca, aa.y * aa.z * nca - aa.x * sa, Number(0),
					   aa.z * aa.x * nca - aa.y * sa, aa.z * aa.y * nca + aa.x * sa, ca + aa.z * aa.z * nca, Number(0),
					   Number(0), Number(0), Number(0), Number(1) });
}

Transform Transform::fromLookAt(const Vector& origin, const Vector& target, const Vector& up)
{
	const Vector fwd	= normalize(Vector(target.x - origin.x, target.y - origin.y, target.z - origin.z));
	const Vector left	= normalize(cross(up, fwd));
	const Vector alt_up = normalize(cross(fwd, left));

	return Transform({ left.x, alt_up.x, fwd.x, origin.x,
					   left.y, alt_up.y, fwd.y, origin.y,
					   left.z, alt_up.z, fwd.z, origin.z,
					   Number(0), Number(0), Number(0), Number(1) });
}

Transform Transform::multiplyFromRight(const Transform& other) const
{
	Transform result;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			Number sum = 0;
			for (int k = 0; k < 4; ++k)
				sum += (*this)(i, k) * other(k, j);
			result(i, j) = sum;
		}
	}

	return result;
}

// ------------- Parser
template <typename T, typename Func>
inline static int _parseScalars(const std::string& str, T* numbers, int amount, Func func)
{
	size_t offset = 0;
	int counter	  = 0;
	for (counter = 0; counter < amount && offset < str.size(); ++counter) {
		try {
			size_t off;
			numbers[counter] = func(str.substr(offset), &off);
			offset += off;
			if (offset >= str.size())
				return counter + 1;
			if (std::ispunct(str[offset]))
				offset += 1;
		} catch (...) {
			return counter;
		}
	}

	return counter;
}

inline static Integer _parseInteger(const std::string& str, Integer* numbers, int amount)
{
	return _parseScalars(str, numbers, amount,
						 [](const std::string& s, size_t* idx) { return (Integer)std::stoll(s, idx); });
}

inline static int _parseNumber(const std::string& str, Number* numbers, int amount)
{
	return _parseScalars(str, numbers, amount,
						 [](const std::string& s, size_t* idx) { return std::stof(s, idx); });
}

static void _parseVersion(const char* v, int& major, int& minor, int& patch)
{
	major = 0;
	minor = 6;
	patch = 0;

	if (!v)
		return;

	std::string version = v;
	const auto s1		= version.find_first_of('.');

	major = std::stoi(version.substr(0, s1));

	if (s1 == std::string::npos) {
		if (major > 0) {
			minor = 0;
			patch = 0;
		}
		return;
	}

	const auto s2 = version.find_first_of('.', s1 + 1);
	minor		  = std::stoi(version.substr(s1 + 1, s2));

	if (s2 == std::string::npos)
		return;

	patch = std::stoi(version.substr(s2 + 1));
}

static std::string _unpackValues(const char* str, const ArgumentContainer& cnt)
{
	std::string unpackedStr;
	for (int i = 0; str[i];) {
		if (str[i] == '$') {
			std::string variable;
			++i;
			for (; str[i] && std::isalnum(str[i]); ++i) {
				variable += str[i];
			}
			if (!variable.empty()) {
				if (!cnt.count(variable))
					throw std::runtime_error("Unknown variable " + variable);

				unpackedStr += cnt.at(variable);
			}
		} else {
			unpackedStr += str[i];
			++i;
		}
	}
	return unpackedStr;
}

class IDContainer {
public:
	inline void registerID(const std::string& id, const std::shared_ptr<Object>& entity)
	{
		mMap[id] = entity;
	}

	inline bool hasID(const std::string& id) const { return mMap.count(id) > 0; }

	inline std::shared_ptr<Object> getByType(const std::string& id, ObjectType type) const
	{
		if (!mMap.count(id))
			return nullptr;

		if (mMap.at(id)->type() != type)
			return nullptr;

		return mMap.at(id);
	}

	inline void makeAlias(const std::string& id, const std::string& as)
	{
		if (!mMap.count(id))
			return;

		mMap[as] = mMap.at(id);
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Object>> mMap;
};

// Object type to parser flag
#define OT_PF(x) (1 << (x))

enum ParseFlags {
	PF_BSDF		  = OT_PF(OT_BSDF),
	PF_EMITTER	  = OT_PF(OT_EMITTER),
	PF_FILM		  = OT_PF(OT_FILM),
	PF_INTEGRATOR = OT_PF(OT_INTEGRATOR),
	PF_MEDIUM	  = OT_PF(OT_MEDIUM),
	PF_PHASE	  = OT_PF(OT_PHASE),
	PF_RFILTER	  = OT_PF(OT_RFILTER),
	PF_SAMPLER	  = OT_PF(OT_SAMPLER),
	PF_SENSOR	  = OT_PF(OT_SENSOR),
	PF_SHAPE	  = OT_PF(OT_SHAPE),
	PF_SUBSURFACE = OT_PF(OT_SUBSURFACE),
	PF_TEXTURE	  = OT_PF(OT_TEXTURE),
	PF_VOLUME	  = OT_PF(OT_VOLUME),

	PF_REFERENCE = OT_PF(_OT_COUNT + 0),
	PF_DEFAULT	 = OT_PF(_OT_COUNT + 1),
	PF_ALIAS	 = OT_PF(_OT_COUNT + 2),
	PF_PARAMETER = OT_PF(_OT_COUNT + 3),
	PF_INCLUDE	 = OT_PF(_OT_COUNT + 4),
	PF_NULL		 = OT_PF(_OT_COUNT + 5),

	// Compositions extracted from the official schema
	PF_C_OBJECTGROUP = PF_PARAMETER | PF_DEFAULT,
	PF_C_BSDF		 = PF_C_OBJECTGROUP | PF_PHASE | PF_TEXTURE | PF_BSDF | PF_REFERENCE,
	PF_C_EMITTER	 = PF_C_OBJECTGROUP | PF_TEXTURE | PF_EMITTER | PF_MEDIUM | PF_REFERENCE,
	PF_C_FILM		 = PF_C_OBJECTGROUP | PF_RFILTER,
	PF_C_INTEGRATOR	 = PF_C_OBJECTGROUP | PF_INTEGRATOR | PF_SAMPLER,
	PF_C_MEDIUM		 = PF_C_OBJECTGROUP | PF_SHAPE | PF_VOLUME | PF_PHASE,
	PF_C_PHASE		 = PF_C_OBJECTGROUP | PF_PHASE,
	PF_C_SCENE		 = PF_C_OBJECTGROUP | PF_ALIAS | PF_SENSOR | PF_TEXTURE | PF_BSDF | PF_SUBSURFACE | PF_INTEGRATOR | PF_EMITTER | PF_SHAPE | PF_MEDIUM | PF_PHASE | PF_INCLUDE | PF_NULL,
	PF_C_SENSOR		 = PF_C_OBJECTGROUP | PF_SENSOR | PF_FILM | PF_MEDIUM | PF_REFERENCE,
	PF_C_SHAPE		 = PF_C_OBJECTGROUP | PF_BSDF | PF_SUBSURFACE | PF_SENSOR | PF_EMITTER | PF_SHAPE | PF_MEDIUM | PF_TEXTURE | PF_RFILTER | PF_REFERENCE,
	PF_C_SUBSURFACE	 = PF_C_OBJECTGROUP | PF_PHASE | PF_BSDF,
	PF_C_TEXTURE	 = PF_C_OBJECTGROUP | PF_TEXTURE | PF_RFILTER | PF_REFERENCE,
	PF_C_VOLUME		 = PF_C_OBJECTGROUP | PF_VOLUME,
};

static Property parseInteger(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto attrib = element->FindAttribute("value");
	if (!attrib)
		return Property();

	std::string valueStr = _unpackValues(attrib->Value(), cnt);

	Integer value;
	if (_parseInteger(valueStr, &value, 1) != 1)
		return Property();

	return Property::fromInteger(value);
}

static Property parseFloat(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto attrib = element->FindAttribute("value");
	if (!attrib)
		return Property();

	std::string valueStr = _unpackValues(attrib->Value(), cnt);

	Number value;
	if (_parseNumber(valueStr, &value, 1) != 1)
		return Property();

	return Property::fromNumber(value);
}

static Property parseVector(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto attrib = element->FindAttribute("value");
	if (!attrib) { // Try legacy way
		auto xA = element->FindAttribute("x");
		auto yA = element->FindAttribute("y");
		auto zA = element->FindAttribute("z");

		if (!xA || !yA || !zA)
			return Property();

		std::string xStr = _unpackValues(xA->Value(), cnt);
		std::string yStr = _unpackValues(yA->Value(), cnt);
		std::string zStr = _unpackValues(zA->Value(), cnt);

		float x, y, z;
		if (_parseNumber(xStr, &x, 1) != 1)
			return Property();
		if (_parseNumber(yStr, &y, 1) != 1)
			return Property();
		if (_parseNumber(zStr, &z, 1) != 1)
			return Property();

		return Property::fromVector(Vector(x, y, z));
	} else {
		std::string valueStr = _unpackValues(attrib->Value(), cnt);

		Number value[3];

		int i = _parseNumber(valueStr, &value[0], 3);
		if (i <= 0)
			return Property();

		for (int j = i; j < 3; ++j)
			value[j] = 0.0f;

		return Property::fromVector(Vector(value[0], value[1], value[2]));
	}
}

static Property parseBool(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto attrib = element->FindAttribute("value");
	if (!attrib)
		return Property();

	const auto valueStr = _unpackValues(attrib->Value(), cnt);
	if (valueStr == "true")
		return Property::fromBool(true);
	else if (valueStr == "false")
		return Property::fromBool(false);
	else
		return Property();
}

static Property parseRGB(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto attrib = element->FindAttribute("value");
	if (!attrib)
		return Property();

	auto intent = element->Attribute("intent");

	// TODO
	(void)intent;
	(void)cnt;

	return Property();
}

static Property parseSpectrum(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto intent = element->Attribute("intent");
	(void)intent;

	auto filename = element->Attribute("filename");
	if (filename) { // Load from .spd files!
		// TODO
		return Property();
	} else {
		auto value = element->Attribute("value");
		if (!value)
			return Property();

		const auto valueStr = _unpackValues(value, cnt);

		// TODO Parse string
		return Property();
	}
}

static Property parseBlackbody(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto tempA = element->Attribute("temperature");
	if (!tempA)
		return Property();

	std::string tempStr = _unpackValues(tempA, cnt);

	Number temp;
	if (_parseNumber(tempStr, &temp, 1) != 1)
		return Property();

	Number scale = 1.0f;
	auto scaleA	 = element->Attribute("scale");
	if (scaleA) {
		std::string scaleStr = _unpackValues(scaleA, cnt);

		if (_parseNumber(scaleStr, &scale, 1) != 1)
			return Property();
	}

	return Property::fromBlackbody(Blackbody(temp, scale));
}

static Property parseString(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto attrib = element->FindAttribute("value");
	if (!attrib)
		return Property();
	return Property::fromString(_unpackValues(attrib->Value(), cnt));
}

Transform parseInnerMatrix(const ArgumentContainer&, const tinyxml2::XMLElement*);

Transform parseTransformTranslate(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	// TODO
	Transform M = Transform::fromIdentity();
	return M * parseInnerMatrix(cnt, element);
}

Transform parseTransformScale(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto uniformScaleA = element->Attribute("value");
	if (uniformScaleA) {
	}
	// TODO
	Transform M = Transform::fromIdentity();
	return M * parseInnerMatrix(cnt, element);
}

Transform parseTransformRotate(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	// TODO
	Transform M = Transform::fromIdentity();
	return M * parseInnerMatrix(cnt, element);
}

Transform parseTransformLookAt(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	// TODO
	Transform M = Transform::fromIdentity();
	return M * parseInnerMatrix(cnt, element);
}

using TransformParseCallback = Transform (*)(const ArgumentContainer&, const tinyxml2::XMLElement*);
struct {
	const char* Name;
	TransformParseCallback Callback;
} _transformParseElements[] = {
	{ "translate", parseTransformTranslate },
	{ "scale", parseTransformScale },
	{ "rotate", parseTransformRotate },
	{ "lookAt", parseTransformLookAt },
	{ nullptr, nullptr }
};

Transform parseInnerMatrix(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	for (auto childElement = element->FirstChildElement();
		 childElement;
		 childElement = childElement->NextSiblingElement()) {

		for (int i = 0; _transformParseElements[i].Name; ++i) {
			if (strcmp(childElement->Name(), _transformParseElements[i].Name) == 0) {
				return _transformParseElements[i].Callback(cnt, childElement); // Only handle first entry
			}
		}
	}

	return Transform::fromIdentity();
}

static Property parseTransform(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	// TODO
	(void)cnt;
	(void)element;
	return Property();
}

static Property parseAnimation(const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	// TODO
	(void)cnt;
	(void)element;
	return Property();
}

using PropertyParseCallback = Property (*)(const ArgumentContainer&, const tinyxml2::XMLElement*);

static const struct {
	const char* Name;
	PropertyParseCallback Callback;
} _propertyParseElement[] = {
	{ "integer", parseInteger },
	{ "float", parseFloat },
	{ "vector", parseVector },
	{ "point", parseVector },
	{ "bool", parseBool },
	{ "string", parseString },
	{ "rgb", parseRGB },
	{ "spectrum", parseSpectrum },
	{ "blackbody", parseBlackbody },
	{ "transform", parseTransform },
	{ "animation", parseAnimation },
	{ nullptr, nullptr }
};

bool parseParameter(Object* obj, const ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto name = element->Attribute("name");
	if (!name)
		return false;

	for (int i = 0; _propertyParseElement[i].Name; ++i) {
		if (strcmp(element->Name(), _propertyParseElement[i].Name) == 0) {
			auto prop = _propertyParseElement[i].Callback(cnt, element);
			if (prop.isValid())
				obj->setProperty(name, prop);
			return true;
		}
	}

	return false;
}

static void handleAlias(IDContainer& idcontainer, const tinyxml2::XMLElement* element)
{
	auto id = element->Attribute("id");
	auto as = element->Attribute("as");

	if (!id || !as)
		throw std::runtime_error("Invalid alias element");

	if (!idcontainer.hasID(id))
		throw std::runtime_error("Unknown id " + std::string(id));

	if (idcontainer.hasID(as))
		throw std::runtime_error("Id " + std::string(as) + " already existent");

	idcontainer.makeAlias(id, as);
}

static void handleDefault(ArgumentContainer& cnt, const tinyxml2::XMLElement* element)
{
	auto name  = element->Attribute("name");
	auto value = element->Attribute("value");

	if (!name || !value)
		throw std::runtime_error("Invalid default element");

	if (!cnt.count(name))
		cnt[name] = value;
}

static const struct {
	const char* Name;
	ObjectType Type;
	int Flags;
} _parseElements[] = {
	{ "bsdf", OT_BSDF, PF_C_BSDF },
	{ "emitter", OT_EMITTER, PF_C_EMITTER },
	{ "film", OT_FILM, PF_C_FILM },
	{ "integrator", OT_INTEGRATOR, PF_C_INTEGRATOR },
	{ "medium", OT_MEDIUM, PF_C_MEDIUM },
	{ "phase", OT_PHASE, PF_C_PHASE },
	{ "sensor", OT_SENSOR, PF_C_SENSOR },
	{ "shape", OT_SHAPE, PF_C_SHAPE },
	{ "subsurface", OT_SUBSURFACE, PF_C_SUBSURFACE },
	{ "texture", OT_TEXTURE, PF_C_TEXTURE },
	{ "volume", OT_VOLUME, PF_C_VOLUME },
	{ nullptr, ObjectType(0), 0 }
};

static void parseObject(Object* obj, const ArgumentContainer& prev_cnt, IDContainer& ids, const tinyxml2::XMLElement* element, int flags)
{
	// Copy container to make sure recursive elements do not overwrite it
	ArgumentContainer cnt = prev_cnt;

	for (auto childElement = element->FirstChildElement();
		 childElement;
		 childElement = childElement->NextSiblingElement()) {

		if ((flags & PF_PARAMETER) && parseParameter(obj, cnt, childElement))
			continue;

		if ((flags & PF_REFERENCE) && strcmp(childElement->Name(), "ref") == 0) {
			// Handle reference
		} else if ((flags & PF_DEFAULT) && strcmp(childElement->Name(), "default") == 0) {
			handleDefault(cnt, childElement);
		} else if ((flags & PF_INCLUDE) && strcmp(childElement->Name(), "include") == 0) {
			// Handle include
		} else if ((flags & PF_ALIAS) && strcmp(childElement->Name(), "alias") == 0) {
			handleAlias(ids, childElement);
		} else if ((flags & PF_NULL) && strcmp(childElement->Name(), "null") == 0) {
			// Handle null
		} else {
			std::shared_ptr<Object> child;

			for (int i = 0; _parseElements[i].Name; ++i) {
				if ((OT_PF(_parseElements[i].Type) & flags)
					&& strcmp(childElement->Name(), _parseElements[i].Name) == 0) {
					child = std::make_shared<Object>(_parseElements[i].Type);
					parseObject(child.get(), cnt, ids, childElement, _parseElements[i].Flags);
					break;
				}
			}

			if (child) {
				const char* id = childElement->Attribute("id");
				if (id) {
					if (!ids.hasID(id)) {
						ids.registerID(id, child);
					} else {
						// TODO: Warning
					}
				}

				obj->addObject(child);
			} else {
				std::stringstream stream;
				stream << "Found invalid tag '" << childElement->Name() << "'";
				throw std::runtime_error(stream.str());
			}
		}
	}
}

class SceneLoader {
public:
	static Scene loadFromXML(const ArgumentContainer& cnt, const tinyxml2::XMLDocument& xml)
	{
		const auto rootScene = xml.RootElement();
		if (strcmp(rootScene->Name(), "scene") != 0)
			throw std::runtime_error("Expected root element to be 'scene'");

		Scene scene;
		IDContainer idcontainer;

		try {
			_parseVersion(rootScene->Attribute("version"), scene.mVersionMajor, scene.mVersionMinor, scene.mVersionPatch);
		} catch (...) {
			throw std::runtime_error("Invalid version element");
		}

		parseObject(&scene, cnt, idcontainer, rootScene, PF_C_SCENE);

		return scene;
	}
};

Scene Scene::loadFromFile(const char* path, const ArgumentContainer& cnt)
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(path);
	return SceneLoader::loadFromXML(cnt, xml);
}

Scene Scene::loadFromString(const char* str, const ArgumentContainer& cnt)
{
	tinyxml2::XMLDocument xml;
	xml.Parse(str);
	return SceneLoader::loadFromXML(cnt, xml);
}

/*Scene Scene::loadFromStream(std::istream& stream, const ArgumentContainer& cnt)
{
	// TODO
	return Scene();
}*/

Scene Scene::loadFromMemory(const uint8_t* data, size_t size, const ArgumentContainer& cnt)
{
	tinyxml2::XMLDocument xml;
	xml.Parse(reinterpret_cast<const char*>(data), size);
	return SceneLoader::loadFromXML(cnt, xml);
}
} // namespace TPM_NAMESPACE