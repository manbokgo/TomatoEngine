#pragma once
#include <string>

namespace tomato
{
    class IName
    {
        friend class SceneHierarchyPanel;

    public:
        IName() = default;
        virtual ~IName() = default;

        void SetName(const std::string& name) { m_Name = name; }

        [[nodiscard]] const std::string& GetName() { return m_Name; }

    private:
        std::string m_Name;
    };
}
