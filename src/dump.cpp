#include <iostream>
#include <sstream>

#include "tinyparser-mitsuba.h"

using namespace TPM_NAMESPACE;
static const char* objectTypeStr(ObjectType type)
{
	switch (type) {
	case OT_SCENE:
		return "scene";
	case OT_BSDF:
		return "bsdf";
	case OT_EMITTER:
		return "emitter";
	case OT_FILM:
		return "film";
	case OT_INTEGRATOR:
		return "integrator";
	case OT_MEDIUM:
		return "medium";
	case OT_PHASE:
		return "phase";
	case OT_RFILTER:
		return "rfilter";
	case OT_SAMPLER:
		return "sampler";
	case OT_SENSOR:
		return "sensor";
	case OT_SHAPE:
		return "shape";
	case OT_SUBSURFACE:
		return "subsurface";
	case OT_TEXTURE:
		return "texture";
	case OT_VOLUME:
		return "volume";
	default:
		return "unknown";
	}
}

static const char* propertyTypeStr(PropertyType type)
{
	switch (type) {
	case PT_ANIMATION:
		return "animation";
	case PT_BLACKBODY:
		return "blackbody";
	case PT_BOOL:
		return "boolean";
	case PT_INTEGER:
		return "integer";
	case PT_NUMBER:
		return "number";
	case PT_COLOR:
		return "rgb";
	case PT_SPECTRUM:
		return "spectrum";
	case PT_STRING:
		return "string";
	case PT_TRANSFORM:
		return "transform";
	case PT_VECTOR:
		return "vector";
	default:
		return "unknown";
	}
}

static std::string propertyValueStr(const Property& prop)
{
	switch (prop.type()) {
	case PT_ANIMATION:
		return "TODO";
	case PT_BLACKBODY: {
		std::stringstream stream;
		stream << "T: " << prop.getBlackbody().temperature << " S: " << prop.getBlackbody().scale;
		return stream.str();
	}
	case PT_BOOL:
		return prop.getBool() ? "true" : "false";
	case PT_INTEGER: {
		std::stringstream stream;
		stream << prop.getInteger();
		return stream.str();
	}
	case PT_NUMBER: {
		std::stringstream stream;
		stream << prop.getNumber();
		return stream.str();
	}
	case PT_COLOR: {
		std::stringstream stream;
		stream << "R: " << prop.getColor().r << " G: " << prop.getColor().g << " B: " << prop.getColor().b;
		return stream.str();
	}
		return "rgb";
	case PT_SPECTRUM: {
		std::stringstream stream;
		const auto& spec = prop.getSpectrum();
		if (spec.isUniform())
			stream << spec.uniformValue();
		else
			stream << "L: " << spec.wavelengths().size();
		return stream.str();
	}
	case PT_STRING:
		return prop.getString();
	case PT_TRANSFORM: {
		std::stringstream stream;
		const auto& t = prop.getTransform();
		for (size_t i = 0; i < t.matrix.size(); ++i)
			stream << t.matrix[i] << " ";
		return stream.str();
	}
	case PT_VECTOR: {
		std::stringstream stream;
		stream << "X: " << prop.getVector().x << " Y: " << prop.getVector().y << " Z: " << prop.getVector().z;
		return stream.str();
	}
	default:
		return "INVALID";
	}
}

static void appendWS(int depth)
{
	for (int i = 0; i < depth; ++i)
		std::cout << "  ";
}

static void dumpProperty(const Property& property, const std::string& name, int depth)
{
	appendWS(depth);
	std::cout << name << "[" << propertyTypeStr(property.type()) << "]=" << propertyValueStr(property) << std::endl;
}

static void dumpObject(Object* obj, const std::string& name, int depth)
{
	appendWS(depth);
	std::cout << name << "[" << objectTypeStr(obj->type()) << "]";
	if (obj->hasPluginType())
		std::cout << "(" << obj->pluginType() << ")";
	if (obj->hasID())
		std::cout << "(#" << obj->id() << ")";
	std::cout << std::endl;

	// Properties
	for (const auto& prop : obj->properties()) {
		dumpProperty(prop.second, prop.first, depth + 1);
	}

	// Unnamed children
	for (const auto& child : obj->anonymousChildren()) {
		dumpObject(child.get(), "", depth + 1);
	}

	// Named children
	for (const auto& child : obj->namedChildren()) {
		dumpObject(child.second.get(), child.first, depth + 1);
	}
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cout << "Arguments missing." << std::endl;
		std::cout << "Call with " << (argc >= 1 ? argv[0] : "tpm_dump") << " FILENAME" << std::endl;
		return EXIT_FAILURE;
	}

	const std::string filename = argv[1];

	try {
		SceneLoader loader;
		auto scene = loader.loadFromFile(argv[1]);
		dumpObject(&scene, "", 0);
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
