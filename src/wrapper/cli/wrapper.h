#pragma once
// Disable clang-format as it is specified for C++ and not C++/CLI
// clang-format off

#pragma unmanaged
#include "tinyparser-mitsuba.h"

#pragma managed

#include <msclr\marshal_cppstd.h>

namespace TinyParserMitsuba {
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

    property bool Uniform {
        bool get() { return IsUniform(); }
    }
    
    property int Length {
        int get() { return GetLength(); }
    }

    property TPM_NAMESPACE::Number Wavelengths[int] {
        TPM_NAMESPACE::Number get(int i) {
            return mInternal->isUniform() ? 0 : mInternal->wavelengths().at(i);
        }
    }

    property TPM_NAMESPACE::Number Weights[int] {
        TPM_NAMESPACE::Number get(int i) {
            return mInternal->isUniform() ? mInternal->uniformValue() : mInternal->weights().at(i);
        }
    }

    int GetLength() { 
        return mInternal->isUniform() ? 1 : mInternal->wavelengths().size();
    }

    bool IsUniform() {
        return mInternal->isUniform();
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

    property TPM_NAMESPACE::Number Temperature {
        TPM_NAMESPACE::Number get() { return GetTemperature(); }
    }
    
    property TPM_NAMESPACE::Number Scale {
        TPM_NAMESPACE::Number get() { return GetScale(); }
    }

    TPM_NAMESPACE::Number GetTemperature() {
        return mInternal->temperature;
    }

    TPM_NAMESPACE::Number GetScale() {
        return mInternal->scale;
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
        return ok ? gcnew TPM_NAMESPACE::Integer{ v } : nullptr;
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
        return ok ? gcnew TPM_NAMESPACE::Number{ v } : nullptr;
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
        return ok ? gcnew bool{ v } : nullptr;
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
        return ok ? gcnew array<TPM_NAMESPACE::Number>(3){ v.x, v.y, v.z } : nullptr;
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
        return ok ? transform2array(v) : nullptr;
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
        return ok ? gcnew array<TPM_NAMESPACE::Number>(3){ v.r, v.g, v.b } : nullptr;
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
        return ok ? msclr::interop::marshal_as<String^>(v) : nullptr;
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
        return ok ? gcnew Spectrum(new TPM_NAMESPACE::Spectrum(std::move(v))) : nullptr;
    }

    // Blackbody
    Blackbody^ GetBlackbody() {
        const TPM_NAMESPACE::Blackbody v = mProperty->getBlackbody();
        return gcnew Blackbody(new TPM_NAMESPACE::Blackbody(std::move(v)));
    }
    
    Blackbody^ GetBlackbody(Blackbody^ def) {
        TPM_NAMESPACE::Blackbody defV(def->Temperature, def->Scale);
        const TPM_NAMESPACE::Blackbody v = mProperty->getBlackbody(defV);
        return gcnew Blackbody(new TPM_NAMESPACE::Blackbody(std::move(v)));
    }
    
    Blackbody^ GetBlackbodyUnsafe() {
        bool ok = false;
        const TPM_NAMESPACE::Blackbody v = mProperty->getBlackbody(TPM_NAMESPACE::Blackbody(6504, 1), &ok);
        return ok ? gcnew Blackbody(new TPM_NAMESPACE::Blackbody(std::move(v))) : nullptr;
    }

    // Animation
    // TODO

    // Other
    property PropertyType Type {
        PropertyType get() {
            return GetType();
        }
    }

    PropertyType GetType() {
        return PropertyType(mProperty->type());
    }
private:
    const TPM_NAMESPACE::Property* mProperty;
};

ref class PropertyIndexer;
ref class NamedIndexer;
ref class AnonymousIndexer;
public ref class SceneObject {
internal:
    SceneObject(const TPM_NAMESPACE::Object* obj) : mObject(obj) {}
public:
    property ObjectType Type {
        ObjectType get() { return GetType(); }
    }

    property String^ PluginType {
        String^ get() { return GetPluginType(); }
    }
    
    property PropertyIndexer^ Properties {
        PropertyIndexer^ get() { return GetProperties(); }
    }

    property NamedIndexer^ NamedChildren {
        NamedIndexer^ get() { return GetNamedChildren(); }
    }
    
    property AnonymousIndexer^ AnonymousChildren {
        AnonymousIndexer^ get() { return GetAnonymousChildren(); }
    }
    
    // ---
    ObjectType GetType() {
        return ObjectType(mObject->type());
    }

    String^ GetPluginType() {
        return msclr::interop::marshal_as<String^>(mObject->pluginType());
    }

    PropertyIndexer^ GetProperties();
    NamedIndexer^ GetNamedChildren();
    AnonymousIndexer^ GetAnonymousChildren();
private:
    const TPM_NAMESPACE::Object* mObject;
};

public ref class PropertyIndexer : public IEnumerable<KeyValuePair<String^,Property^>>  {
internal:
    PropertyIndexer(const TPM_NAMESPACE::Object* obj) : mObject(obj) {}
public:
    property Property^ default[String^] {
        Property^ get(String^ key) {
            const auto prop = mObject->property(msclr::interop::marshal_as<std::string>(key));
            return prop.isValid() ? gcnew Property(new TPM_NAMESPACE::Property(std::move(prop))) : nullptr;
        }
    }

    property int Length {
        int get() { return GetLength(); }
    }

    int GetLength() { return mObject->properties().size(); }

    Dictionary<String^, Property^>^ ToDictionary() {
        Dictionary<String^, Property^>^ arr = gcnew Dictionary<String^, Property^>();
        for (const auto& pair: mObject->properties()) {
            arr->Add(msclr::interop::marshal_as<String^>(pair.first),
                     gcnew Property(new TPM_NAMESPACE::Property(pair.second)));
        }
        return arr;
    }

    static operator Dictionary<String^, Property^>^ ( PropertyIndexer val ) {
        return val.ToDictionary();
    }

    virtual IEnumerator<KeyValuePair<String^,Property^>>^ GetEnumerator()
    {
        // TODO: Maybe implement own version...
        return ToDictionary()->GetEnumerator();
    }

    virtual System::Collections::IEnumerator^ GetEnumerator2() = System::Collections::IEnumerable::GetEnumerator
    {
        // TODO: Maybe implement own version...
        return ToDictionary()->GetEnumerator();
    }
private:
    const TPM_NAMESPACE::Object* mObject;
};

public ref class NamedIndexer : public IEnumerable<KeyValuePair<String^,SceneObject^>> {
internal:
    NamedIndexer(const TPM_NAMESPACE::Object* obj) : mObject(obj) {}
public:
    property SceneObject^ default[String^] {
        SceneObject^ get(String^ key) {
            const auto prop = mObject->namedChild(msclr::interop::marshal_as<std::string>(key));
            return prop ? gcnew SceneObject(new TPM_NAMESPACE::Object(*prop)) : nullptr;
        }
    }
    
    property int Length {
        int get() { return GetLength(); }
    }

    int GetLength() { return mObject->namedChildren().size(); }

    Dictionary<String^, SceneObject^>^ ToDictionary() {
        Dictionary<String^, SceneObject^>^ arr = gcnew Dictionary<String^, SceneObject^>();
        for (const auto& pair: mObject->namedChildren()) {
            arr->Add(msclr::interop::marshal_as<String^>(pair.first),
                        gcnew SceneObject(new TPM_NAMESPACE::Object(*pair.second)));
        }
        return arr;
    }

    static operator Dictionary<String^, SceneObject^>^ ( NamedIndexer val ) {
        return val.ToDictionary();
    }

    virtual IEnumerator<KeyValuePair<String^,SceneObject^>>^ GetEnumerator()
    {
        // TODO: Maybe implement own version...
        return ToDictionary()->GetEnumerator();
    }

    virtual System::Collections::IEnumerator^ GetEnumerator2() = System::Collections::IEnumerable::GetEnumerator
    {
        // TODO: Maybe implement own version...
        return ToDictionary()->GetEnumerator();
    }
private:
    const TPM_NAMESPACE::Object* mObject;
};

public ref class AnonymousIndexer : public IEnumerable<SceneObject^> {
internal:
    AnonymousIndexer(const TPM_NAMESPACE::Object* obj) : mObject(obj) {}
public:
    property SceneObject^ default[int] {
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

    property int Length {
        int get() { return GetLength(); }
    }

    int GetLength() { return mObject->anonymousChildren().size(); }

    array<SceneObject^>^ ToArray() {
        array<SceneObject^>^ arr = gcnew array<SceneObject^>(mObject->anonymousChildren().size());
        int i = 0;
        for (const auto& obj: mObject->anonymousChildren()) {
            arr[i] = gcnew SceneObject(new TPM_NAMESPACE::Object(*obj));
            ++i;
        }
        return arr;
    }

    static operator array<SceneObject^>^ ( AnonymousIndexer val ) {
        return val.ToArray();
    }

    ref struct Enumerator : IEnumerator<SceneObject^>
    {
        Enumerator( AnonymousIndexer^ indexer ) : indexer(indexer), currentIndex(-1) {}
        ~Enumerator() {}

        virtual bool MoveNext() = IEnumerator<SceneObject^>::MoveNext
        {
            if( currentIndex < indexer->Length - 1 )
            {
                currentIndex++;
                return true;
            }
            return false;
        }

        property SceneObject^ Current
        {
            virtual SceneObject^ get() = IEnumerator<SceneObject^>::Current::get
            {
                return indexer->default[currentIndex];
            }
        };

        property Object^ Current2
        {
            virtual Object^ get() = System::Collections::IEnumerator::Current::get
            {
                return indexer->default[currentIndex];
            }
        };

        virtual void Reset() = IEnumerator<SceneObject^>::Reset {}

    private:
        AnonymousIndexer^ indexer;
        int currentIndex;
    };

    virtual IEnumerator<SceneObject^>^ GetEnumerator()
    {
        return gcnew Enumerator(this);
    }

    virtual System::Collections::IEnumerator^ GetEnumerator2() = System::Collections::IEnumerable::GetEnumerator
    {
        return gcnew Enumerator(this);
    }
private:
    const TPM_NAMESPACE::Object* mObject;
};

PropertyIndexer^ SceneObject::GetProperties() {
    return gcnew PropertyIndexer(mObject);
}

NamedIndexer^ SceneObject::GetNamedChildren() {
    return gcnew NamedIndexer(mObject);
}
    
AnonymousIndexer^ SceneObject::GetAnonymousChildren() {
    return gcnew AnonymousIndexer(mObject);
}

public ref class Scene : public SceneObject {
internal:
    Scene(const TPM_NAMESPACE::Scene* scene) : SceneObject(scene), mScene(scene) {}
public:
    ~Scene() {
        delete mScene;
    }

    property array<int>^ Version {
        array<int>^ get() { return GetVersion(); }
    }

    array<int>^ GetVersion() {
        return gcnew array<int>(3){ mScene->versionMajor(), mScene->versionMinor(), mScene->versionPatch() };
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

    property bool LowerCaseConversion {
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
}// namespace TinyParserMitsuba