#pragma once

namespace tomato
{
    class CPathMgr : public Singleton<CPathMgr>
    {
    public:
        CPathMgr();
        ~CPathMgr();
        void Init();

        [[nodiscard]] const fs::path& GetContentPath() { return m_szContentPath; }

    private:
        fs::path m_szContentPath;
    };
}
