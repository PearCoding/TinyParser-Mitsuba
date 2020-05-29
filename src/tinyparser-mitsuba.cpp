#include "tinyparser-mitsuba.h"

#include <sstream>
#include <stdexcept>

#include <tinyxml2.h>

namespace TPM_NAMESPACE {

static void parseVersion(const char* v, int& major, int& minor, int& patch)
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

class IDContainer {
public:
	inline void registerID(const std::string& id, const std::shared_ptr<Entity>& entity)
	{
		mMap[id] = entity;
	}

	inline bool hasID(const std::string& id) const { return mMap.count(id) > 0; }

	inline std::shared_ptr<BSDF> getBSDF(const std::string& id) const { return std::static_pointer_cast<BSDF>(getByType(id, ET_BSDF)); }
	inline std::shared_ptr<Emitter> getEmitter(const std::string& id) const { return std::static_pointer_cast<Emitter>(getByType(id, ET_EMITTER)); }
	inline std::shared_ptr<Film> getFilm(const std::string& id) const { return std::static_pointer_cast<Film>(getByType(id, ET_FILM)); }
	inline std::shared_ptr<Integrator> getIntegrator(const std::string& id) const { return std::static_pointer_cast<Integrator>(getByType(id, ET_INTEGRATOR)); }
	inline std::shared_ptr<RFilter> getRFilter(const std::string& id) const { return std::static_pointer_cast<RFilter>(getByType(id, ET_RFILTER)); }
	inline std::shared_ptr<Sampler> getSampler(const std::string& id) const { return std::static_pointer_cast<Sampler>(getByType(id, ET_SAMPLER)); }
	inline std::shared_ptr<Sensor> getSensor(const std::string& id) const { return std::static_pointer_cast<Sensor>(getByType(id, ET_SENSOR)); }
	inline std::shared_ptr<Shape> getShape(const std::string& id) const { return std::static_pointer_cast<Shape>(getByType(id, ET_SHAPE)); }
	inline std::shared_ptr<Texture> getTexture(const std::string& id) const { return std::static_pointer_cast<Texture>(getByType(id, ET_TEXTURE)); }

private:
	inline std::shared_ptr<Entity> getByType(const std::string& id, EntityType type) const
	{
		if (!mMap.count(id))
			return nullptr;

		if (mMap.at(id)->type() != type)
			return nullptr;

		return mMap.at(id);
	}

	std::unordered_map<std::string, std::shared_ptr<Entity>> mMap;
};

class SceneLoader {
public:
	static Scene loadFromXML(const tinyxml2::XMLDocument& xml)
	{
		const auto rootScene = xml.RootElement();
		if (strcmp(rootScene->Name(), "scene") != 0)
			throw std::runtime_error("Expected root element to be 'scene'");

		Scene scene;
		IDContainer idcontainer;

		try {
			parseVersion(rootScene->Attribute("version"), scene.mVersionMajor, scene.mVersionMinor, scene.mVersionPatch);
		} catch (...) {
			throw std::runtime_error("Invalid version element");
		}

		for (auto child = rootScene->FirstChildElement();
			 child;
			 child = child->NextSiblingElement()) {
			if (strcmp(child->Name(), "shape") == 0) {
				auto shape = parseShape(idcontainer, child);
				if (shape)
					scene.mEntities.push_back(std::move(shape));
			} else if (strcmp(child->Name(), "bsdf") == 0) {
				parseBsdf(idcontainer, child);
			} else if (strcmp(child->Name(), "integrator") == 0) {
				auto integrator = parseIntegrator(idcontainer, child);
				if (integrator)
					scene.mEntities.push_back(std::move(integrator));
			} else if (strcmp(child->Name(), "sensor") == 0) {
				auto sensor = parseSensor(idcontainer, child);
				if (sensor)
					scene.mEntities.push_back(std::move(sensor));
			} else if (strcmp(child->Name(), "texture") == 0) {
				parseTexture(idcontainer, child);
			} else if (strcmp(child->Name(), "emitter") == 0) {
				auto emitter = parseEmitter(idcontainer, child);
				if (emitter)
					scene.mEntities.push_back(std::move(emitter));
			} else {
				std::stringstream stream;
				stream << "Found invalid tag '" << child->Name() << "'";
				throw std::runtime_error(stream.str());
			}
		}

		return scene;
	}

	static void handleID(IDContainer& idcontainer, const std::shared_ptr<Entity>& entity, const tinyxml2::XMLElement* element)
	{
		const char* id = element->Attribute("id");
		if (!id)
			return;

		// TODO: Warning if already in use?

		idcontainer.registerID(id, entity);
	}

	static std::shared_ptr<Shape> parseShape(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<BSDF> parseBsdf(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<Integrator> parseIntegrator(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<Sensor> parseSensor(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<Texture> parseTexture(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<Film> parseFilm(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<RFilter> parseRFilter(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}

	static std::shared_ptr<Emitter> parseEmitter(IDContainer& /*idcontainer*/, const tinyxml2::XMLElement* /*element*/)
	{
		// TODO
		return nullptr;
	}
};

Scene Scene::loadFromFile(const char* path)
{
	tinyxml2::XMLDocument xml;
	xml.LoadFile(path);
	return SceneLoader::loadFromXML(xml);
}

Scene Scene::loadFromString(const char* str)
{
	tinyxml2::XMLDocument xml;
	xml.Parse(str);
	return SceneLoader::loadFromXML(xml);
}

/*Scene Scene::loadFromStream(std::istream& stream)
{
	// TODO
	return Scene();
}*/

Scene Scene::loadFromMemory(const uint8_t* data, size_t size)
{
	tinyxml2::XMLDocument xml;
	xml.Parse(reinterpret_cast<const char*>(data), size);
	return SceneLoader::loadFromXML(xml);
}
} // namespace TPM_NAMESPACE