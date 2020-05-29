#include "tinyparser-mitsuba.h"

#include <tinyxml2.h>

namespace TPM_NAMESPACE {
class SceneLoader {
public:
	static Scene loadFromXML(const tinyxml2::XMLDocument& /*xml*/)
	{
		return Scene();
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

Scene Scene::loadFromStream(std::istream& /*stream*/)
{
	// TODO
	return Scene();
}

Scene Scene::loadFromMemory(const uint8_t* data, size_t size)
{
	tinyxml2::XMLDocument xml;
	xml.Parse(reinterpret_cast<const char*>(data), size);
	return SceneLoader::loadFromXML(xml);
}
} // namespace TPM_NAMESPACE