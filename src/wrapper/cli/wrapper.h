#pragma once
// Disable clang-format as it is specified for C++ and not C++/CLI
// clang-format off

#pragma unmanaged
#include "tinyparser-mitsuba.h"

#pragma managed

#include <msclr\marshal_cppstd.h>
using namespace System;
using namespace System::Collections::Generic;

public enum class ObjectType {
    Scene,
    Bsdf,
    Emitter,
    Film,
    Integrator,
    Medium,
    Phase,
    RFilter,
    Sampler,
    Sensor,
    Shape,
    Subsurface,
    Texture,
    Volume
};

public enum class PropertyType {
    None,
    Animation,
    Blackbody,
    Bool,
    Integer,
    Number,
    Color,
    Spectrum,
    String,
    Transform,
    Vector
};

public ref class Animation {
    // TODO
};

public ref class Spectrum {
internal:
    Spectrum(const TPM_NAMESPACE::Spectrum* o) : mInternal(o) {}
public:
    ~Spectrum() {
        delete mInternal;
    }

    property bool uniform {
        bool get() {
            return mInternal->isUniform();
        }
    }
    
    property int length {
        int get() {
            return mInternal->isUniform() ? 1 : mInternal->wavelengths().size();
        }
    }

    property TPM_NAMESPACE::Number wavelenghts[int] {
        TPM_NAMESPACE::Number get(int i) {
            return mInternal->isUniform() ? 0 : mInternal->wavelengths().at(i);
        }
    }

    property TPM_NAMESPACE::Number weights[int] {
        TPM_NAMESPACE::Number get(int i) {
            return mInternal->isUniform() ? mInternal->uniformValue() : mInternal->weights().at(i);
        }
    }

private:
    const TPM_NAMESPACE::Spectrum* mInternal;
};

public ref class Blackbody {
internal:
    Blackbody(const TPM_NAMESPACE::Blackbody* o) : mInternal(o) {}
public:
    ~Blackbody() {
        delete mInternal;
    }

    property TPM_NAMESPACE::Number temperature {
        TPM_NAMESPACE::Number get() {
            return mInternal->temperature;
        }
    }
    property TPM_NAMESPACE::Number scale {
        TPM_NAMESPACE::Number get() {
            return mInternal->scale;
        }
    }

private:
    const TPM_NAMESPACE::Blackbody* mInternal;
};

inline array<TPM_NAMESPACE::Number, 2>^ transform2array(const TPM_NAMESPACE::Transform& t) {
    array<TPM_NAMESPACE::Number, 2>^ arr = gcnew array<TPM_NAMESPACE::Number, 2>(4,4);

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            arr[i,j] = t.matrix[i * 4 + j];

    return arr;
}

inline TPM_NAMESPACE::Transform array2transform(array<TPM_NAMESPACE::Number, 2>^ arr) {
    TPM_NAMESPACE::Transform t = TPM_NAMESPACE::Transform::fromIdentity();

    for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            t.matrix[i * 4 + j] = arr[i,j];

    return t;
}

public ref class Property {
internal:
    Property(const TPM_NAMESPACE::Property* prop) : mProperty(prop) {}
public:
    ~Property() {
        delete mProperty;
    }
        
    // Integer
    TPM_NAMESPACE::Integer GetInteger() {
        return mProperty->getInteger();
    }

    TPM_NAMESPACE::Integer GetInteger(TPM_NAMESPACE::Integer def) {
        return mProperty->getInteger(def);
    }
    
    TPM_NAMESPACE::Integer^ GetIntegerUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Integer v = mProperty->getInteger(0, &ok);
        if(ok)
            return gcnew TPM_NAMESPACE::Integer{ v };
        else
            return nullptr;
    }
    
    // Number
    TPM_NAMESPACE::Number GetNumber() {
        return mProperty->getNumber();
    }

    TPM_NAMESPACE::Number GetNumber(TPM_NAMESPACE::Number def) {
        return mProperty->getNumber(def);
    }
    
    TPM_NAMESPACE::Number^ GetNumberUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Number v = mProperty->getNumber(0, &ok);
        if(ok)
            return gcnew TPM_NAMESPACE::Number{ v };
        else
            return nullptr;
    }
    
    // Bool
    bool GetBool() {
        return mProperty->getBool();
    }

    bool GetBool(bool def) {
        return mProperty->getBool(def);
    }
    
    bool^ GetBoolUnsafe() {
        bool ok = false;
        const bool v = mProperty->getBool(false, &ok);
        if(ok)
            return gcnew bool{ v };
        else
            return nullptr;
    }

    // Vector
    array<TPM_NAMESPACE::Number>^ GetVector() {
        const TPM_NAMESPACE::Vector v = mProperty->getVector();
        return gcnew array<TPM_NAMESPACE::Number>(3){ v.x, v.y, v.z };
    }

    array<TPM_NAMESPACE::Number>^ GetVector(array<TPM_NAMESPACE::Number>^ def) {
        TPM_NAMESPACE::Vector defV(def[0], def[1], def[2]);
        const TPM_NAMESPACE::Vector v = mProperty->getVector(defV);
        return gcnew array<TPM_NAMESPACE::Number>(3){ v.x, v.y, v.z };
    }
    
    array<TPM_NAMESPACE::Number>^ GetVectorUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Vector v = mProperty->getVector(TPM_NAMESPACE::Vector(), &ok);
        if(ok)
            return gcnew array<TPM_NAMESPACE::Number>(3){ v.x, v.y, v.z };
        else
            return nullptr;
    }

    // Transform
    array<TPM_NAMESPACE::Number,2>^ GetTransform() {
        const TPM_NAMESPACE::Transform v = mProperty->getTransform();
        return transform2array(v);
    }

    array<TPM_NAMESPACE::Number,2>^ GetTransform(array<TPM_NAMESPACE::Number,2>^ def) {
        const TPM_NAMESPACE::Transform v= mProperty->getTransform(array2transform(def));
        return transform2array(v);
    }
    
    array<TPM_NAMESPACE::Number,2>^ GetTransformUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Transform v = mProperty->getTransform(TPM_NAMESPACE::Transform::fromIdentity(), &ok);
        if(ok)
            return transform2array(v);
        else
            return nullptr;
    }

    // Color
    array<TPM_NAMESPACE::Number>^ GetColor() {
        const TPM_NAMESPACE::Color v = mProperty->getColor();
        return gcnew array<TPM_NAMESPACE::Number>(3){ v.r, v.g, v.b };
    }

    array<TPM_NAMESPACE::Number>^ GetColor(array<TPM_NAMESPACE::Number>^ def) {
        TPM_NAMESPACE::Color defV(def[0], def[1], def[2]);
        const TPM_NAMESPACE::Color v = mProperty->getColor(defV);
        return gcnew array<TPM_NAMESPACE::Number>(3){ v.r, v.g, v.b };
    }
    
    array<TPM_NAMESPACE::Number>^ GetColorUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Color v = mProperty->getColor(TPM_NAMESPACE::Color(0,0,0), &ok);
        if(ok)
            return gcnew array<TPM_NAMESPACE::Number>(3){ v.r, v.g, v.b };
        else
            return nullptr;
    }

    // String
    String^ GetString() {
        const std::string v = mProperty->getString();
        return msclr::interop::marshal_as<String^>(v);
    }

    String^ GetString(String^ def) {
        const std::string defV = msclr::interop::marshal_as<std::string>(def);
        const std::string v = mProperty->getString(defV);
        return msclr::interop::marshal_as<String^>(v);
    }
    
    String^ GetStringUnsafe() {
        bool ok = false;
        const std::string v = mProperty->getString({}, &ok);
        if(ok)
            return msclr::interop::marshal_as<String^>(v);
        else
            return nullptr;
    }

    // Spectrum
    Spectrum^ GetSpectrum() {
        const TPM_NAMESPACE::Spectrum v = mProperty->getSpectrum();
        return gcnew Spectrum(new TPM_NAMESPACE::Spectrum(std::move(v)));
    }

    // TODO: Default value
    
    Spectrum^ GetSpectrumUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Spectrum v = mProperty->getSpectrum({}, &ok);
        if(ok)
            return gcnew Spectrum(new TPM_NAMESPACE::Spectrum(std::move(v)));
        else
            return nullptr;
    }

    // Blackbody
    Blackbody^ GetBlackbody() {
        const TPM_NAMESPACE::Blackbody v = mProperty->getBlackbody();
        return gcnew Blackbody(new TPM_NAMESPACE::Blackbody(std::move(v)));
    }
    
    Blackbody^ GetBlackbody(Blackbody^ def) {
        TPM_NAMESPACE::Blackbody defV(def->temperature, def->scale);
        const TPM_NAMESPACE::Blackbody v = mProperty->getBlackbody(defV);
        return gcnew Blackbody(new TPM_NAMESPACE::Blackbody(std::move(v)));
    }
    
    Blackbody^ GetBlackbodyUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Blackbody v = mProperty->getBlackbody(TPM_NAMESPACE::Blackbody(6504, 1), &ok);
        if(ok)
            return gcnew Blackbody(new TPM_NAMESPACE::Blackbody(std::move(v)));
        else
            return nullptr;
    }

    // Animation
    // TODO

    // Other
    property PropertyType type {
        PropertyType get() {
            return PropertyType(mProperty->type());
        }
    }
private:
    const TPM_NAMESPACE::Property* mProperty;
};

public ref class SceneObject {
internal:
    SceneObject(const TPM_NAMESPACE::Object* obj) : mObject(obj) {}
public:
    // Properties
    property Property^ properties[String^] {
        Property^ get(String^ key) {
            const auto prop = mObject->property(msclr::interop::marshal_as<std::string>(key));
            return prop.isValid() ? gcnew Property(new TPM_NAMESPACE::Property(std::move(prop))) : nullptr;
        }
    }

    property Dictionary<String^, Property^>^ allProperties {
        Dictionary<String^, Property^>^ get() {
            Dictionary<String^, Property^>^ arr = gcnew Dictionary<String^, Property^>();
            for (const auto& pair: mObject->properties()) {
                arr->Add(msclr::interop::marshal_as<String^>(pair.first),
                         gcnew Property(new TPM_NAMESPACE::Property(pair.second)));
            }
            return arr;
        }
    }
    
    // Named Children
    property SceneObject^ namedChildren[String^] {
        SceneObject^ get(String^ key) {
            const auto prop = mObject->namedChild(msclr::interop::marshal_as<std::string>(key));
            return prop ? gcnew SceneObject(new TPM_NAMESPACE::Object(*prop)) : nullptr;
        }
    }

    property Dictionary<String^, SceneObject^>^ allNamedChildren {
        Dictionary<String^, SceneObject^>^ get() {
            Dictionary<String^, SceneObject^>^ arr = gcnew Dictionary<String^, SceneObject^>();
            for (const auto& pair: mObject->namedChildren()) {
                arr->Add(msclr::interop::marshal_as<String^>(pair.first),
                         gcnew SceneObject(new TPM_NAMESPACE::Object(*pair.second)));
            }
            return arr;
        }
    }
    
    // Anonymous Children
    property SceneObject^ anonymousChildren[int] {
        SceneObject^ get(int key) {
            if(key < mObject->anonymousChildren().size()) {
                const auto prop = mObject->anonymousChildren().at(key);
                return gcnew SceneObject(new TPM_NAMESPACE::Object(*prop));
            }
            else {
                return nullptr;
            }
        }
    }

    property array<SceneObject^>^ allAnonymousChildren {
        array<SceneObject^>^ get() {
            array<SceneObject^>^ arr = gcnew array<SceneObject^>(mObject->anonymousChildren().size());
            int i = 0;
            for (const auto& obj: mObject->anonymousChildren()) {
                arr[i] = gcnew SceneObject(new TPM_NAMESPACE::Object(*obj));
                ++i;
            }
            return arr;
        }
    }

    // Other
    property ObjectType type {
        ObjectType get() {
            return ObjectType(mObject->type());
        }
    }

    property String^ pluginType {
        String^ get() {
            return msclr::interop::marshal_as<String^>(mObject->pluginType());
        }
    }
private:
    const TPM_NAMESPACE::Object* mObject;
};

public ref class Scene : public SceneObject {
internal:
    Scene(const TPM_NAMESPACE::Scene* scene) : SceneObject(scene), mScene(scene) {}
public:
    ~Scene() {
        delete mScene;
    }

    property array<int>^ version {
        array<int>^ get() {
            return gcnew array<int>(3){ mScene->versionMajor(), mScene->versionMinor(), mScene->versionPatch() };
        }
    }

private:
    const TPM_NAMESPACE::Scene* mScene;
};

public ref class SceneLoader {
public:
    SceneLoader() : mInternal(new TPM_NAMESPACE::SceneLoader()) {}
    ~SceneLoader() { delete mInternal; }

    Scene^ LoadFromFile(String^ path) {
        const TPM_NAMESPACE::Scene scene = mInternal->loadFromFile(msclr::interop::marshal_as<std::string>(path));
        return gcnew Scene(new TPM_NAMESPACE::Scene(std::move(scene)));
    }

    Scene^ LoadFromString(String^ string) {
        const TPM_NAMESPACE::Scene scene = mInternal->loadFromString(msclr::interop::marshal_as<std::string>(string));
        return gcnew Scene(new TPM_NAMESPACE::Scene(std::move(scene)));
    }

    void AddLookupDir(String^ path) {
        mInternal->addLookupDir(msclr::interop::marshal_as<std::string>(path));
    }

    void AddArgument(String^ key, String^ value) {
        mInternal->addArgument(msclr::interop::marshal_as<std::string>(key),
                               msclr::interop::marshal_as<std::string>(value));
    }

    property bool lowerCaseConversion {
        bool get() {
            return mInternal->isLowerCaseConversionDisabled();
        }

        void set(bool b) {
            mInternal->disableLowerCaseConversion(b);
        }
    }

private:
    TPM_NAMESPACE::SceneLoader* mInternal;
};