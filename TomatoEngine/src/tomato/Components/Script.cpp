#include "pch.h"
#include "Script.h"

namespace tomato
{
    void Script::AddScriptField(string_view name, FieldType type, void* field, string_view header, string_view tooltip,
                                float       min, float      max)
    {
        ScriptField scriptField;
        scriptField.Name = name;
        scriptField.Type = type;
        scriptField.Field = field;

        scriptField.Header = header;
        scriptField.Tooltip = tooltip;
        scriptField.Min = min;
        scriptField.Max = max;

        m_ScriptFields.push_back(scriptField);
    }
}
