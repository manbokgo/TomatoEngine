#pragma once

namespace tomato
{
    inline constexpr Vector3 ToRadian(Vector3 vDegrees) noexcept
    {
        return {
            XMConvertToRadians(vDegrees.x),
            XMConvertToRadians(vDegrees.y),
            XMConvertToRadians(vDegrees.z),
        };
    }

    inline constexpr Vector3 ToDegree(Vector3 vRadians) noexcept
    {
        return {
            XMConvertToDegrees(vRadians.x),
            XMConvertToDegrees(vRadians.y),
            XMConvertToDegrees(vRadians.z),
        };
    }

    inline string WstringToString(const wstring& input)
    {
        char      multiByteStr[MAX_PATH];
        const int requiredLength = WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, nullptr, 0, nullptr, nullptr);
        WideCharToMultiByte(CP_ACP, 0, input.c_str(), -1, multiByteStr, requiredLength, nullptr, nullptr);

        return multiByteStr;
    }

    inline wstring StringToWstring(const string& input)
    {
        wchar_t   wideStr[MAX_PATH];
        const int requiredLength = MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, nullptr, 0);
        MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, wideStr, requiredLength);

        return wideStr;
    }

    inline void WcharToChar(const wchar_t* input, char* output, size_t length)
    {
        const int requiredLength = WideCharToMultiByte(CP_ACP, 0, input, -1, nullptr, 0, nullptr, nullptr);
        if (length < requiredLength)
        {
            assert(false);
        }
        WideCharToMultiByte(CP_ACP, 0, input, -1, output, requiredLength, nullptr, nullptr);
    }

    inline void CharToWchar(const char* input, wchar_t* output, size_t length)
    {
        const int requiredLength = MultiByteToWideChar(CP_ACP, 0, input, -1, nullptr, 0);
        if (length < requiredLength)
        {
            assert(false);
        }
        MultiByteToWideChar(CP_ACP, 0, input, -1, output, requiredLength);
    }


    template <typename T>
    void Safe_Del_Vec(vector<T*>& _vec)
    {
        typename vector<T*>::iterator iter = _vec.begin();
        for (; iter != _vec.end(); ++iter)
        {
            if (nullptr != (*iter))
                delete (*iter);
        }
    }

    template <typename T1, typename T2>
    void Safe_Del_Map(map<T1, T2>& _map)
    {
        typename map<T1, T2>::iterator iter = _map.begin();

        for (; iter != _map.end(); ++iter)
        {
            if (nullptr != iter->second)
                delete iter->second;
        }
        _map.clear();
    }

    template <typename T, int SIZE>
    void Safe_Del_Array(T* (&_arr)[SIZE])
    {
        for (int i = 0; i < SIZE; ++i)
        {
            if (nullptr != _arr[i])
                delete _arr[i];
        }
    }

    // Relative Path 가져오기
    wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath);

    int GetSizeofFormat(DXGI_FORMAT _eFormat);

    // bool IsValid(Entity*& _pObj);

    // ================
    // File Save / Load
    // ================
    void SaveStringToFile(const string& _str, FILE* _pFile);
    void LoadStringFromFile(string& _str, FILE* _pFile);

    void SaveWStringToFile(const wstring& _str, FILE* _pFile);
    void LoadWStringFromFile(wstring& _str, FILE* _pFile);

    template <typename T>
    void SaveResourceRef(Ref<T> _res, FILE* _pFile)
    {
        int bExist = !!_res;
        fwrite(&bExist, sizeof(int), 1, _pFile);

        if (bExist)
        {
            SaveWStringToFile(_res->GetPath(), _pFile);
        }
    }

    template <typename T>
    void LoadResourceRef(Ref<T>& _Res, FILE* _pFile)
    {
        int bExist = 0;
        fread(&bExist, sizeof(int), 1, _pFile);

        if (bExist)
        {
            wstring strKey;
            LoadWStringFromFile(strKey, _pFile);

            // TODO 시발
            // _Res = CResMgr::GetInst()->Load<T>(strKey);
        }
    }

    const char*    ToString(eResType);
    const wchar_t* ToWString(eResType);


    // ===============
    // Event 관련 함수
    // ===============
    // void Instantiate(Entity* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx = 0);
}
