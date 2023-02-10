#pragma once

namespace tomato
{
    struct Collision2DData;
    enum class FieldType
    {
        Unknown = 0,
        Float,
        Double,
        Bool,
        Char,
        Byte,
        Short,
        Int,
        Long,
        UByte,
        UShort,
        UInt,
        ULong,
        String,

        Vector2,
        Vector3,
        Vector4,
        Color
    };

    struct ScriptField
    {
        std::string Name;
        FieldType   Type = FieldType::Unknown;
        void*       Field = nullptr;

        // For attributes
        std::string Header;
        std::string Tooltip;
        float       Min = 0.0f;
        float       Max = 0.0f;
    };


    class Script
    {
        friend class Entity;

    public:
        Script() = default;
        virtual ~Script() = default;

        virtual const char* GetName() const = 0;

        virtual void OnStart() {};
        virtual void OnUpdate() {};
        virtual void OnLateUpdate() {};

        virtual void OnSensorExit2D(const Collision2DData& collisionData) {}
        virtual void OnSensorEnter2D(const Collision2DData& collisionData) {}
        virtual void OnCollisionExit2D(const Collision2DData& collisionData) {}
        virtual void OnCollisionEnter2D(const Collision2DData& collisionData) {}

        void AddScriptField(string_view name, FieldType     type, void*      field, string_view header = {},
                            string_view tooltip = {}, float min = 0.f, float max = 0.f);

        [[nodiscard]] const vector<ScriptField>& GetScriptFields() const { return m_ScriptFields; }
        [[nodiscard]] Entity* GetGameObject() const { return m_GameObject; }

    private:
        Entity* m_GameObject;
        vector<ScriptField> m_ScriptFields;
    };
}
