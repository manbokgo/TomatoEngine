#pragma once

namespace tomato
{
    class Res
    {
        friend class CResMgr;

    public:
        Res(eResType resType);
        Res(const Res& other);
        virtual ~Res() = default;
        
        [[nodiscard]] fs::path GetPath() const { return m_Path; }
        [[nodiscard]] eResType GetResType() const { return m_eResType; }

        virtual void Save(const fs::path& path) = 0;
        virtual int  Load(const fs::path& path) = 0;

    private:
        fs::path       m_Path;
        const eResType m_eResType;
    };
}
