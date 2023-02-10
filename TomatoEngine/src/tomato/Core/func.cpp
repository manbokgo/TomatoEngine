#include "pch.h"

namespace tomato
{
    wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath)
    {
        wstring strRelativePath;
        if (-1 == _strPath.find(_strBase))
        {
            return strRelativePath;
        }

        strRelativePath = _strPath.substr(_strBase.length(), _strPath.length());
        return strRelativePath;
    }




    int GetSizeofFormat(DXGI_FORMAT _eFormat)
    {
        int iRetByte = 0;
        switch (_eFormat)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            iRetByte = 128;
            break;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            iRetByte = 96;
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            iRetByte = 64;
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            iRetByte = 32;
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            iRetByte = 16;
            break;
        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
            iRetByte = 8;
            break;
        // Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
        case DXGI_FORMAT_BC2_TYPELESS:
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
        case DXGI_FORMAT_BC3_TYPELESS:
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
        case DXGI_FORMAT_BC5_TYPELESS:
        case DXGI_FORMAT_BC5_UNORM:
        case DXGI_FORMAT_BC5_SNORM:
            iRetByte = 128;
            break;
        // Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
        case DXGI_FORMAT_R1_UNORM:
        case DXGI_FORMAT_BC1_TYPELESS:
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
        case DXGI_FORMAT_BC4_TYPELESS:
        case DXGI_FORMAT_BC4_UNORM:
        case DXGI_FORMAT_BC4_SNORM:
            iRetByte = 64;
            break;
        // Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            iRetByte = 32;
            break;
        // These are compressed, but bit-size information is unclear.
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            iRetByte = 32;
            break;
        case DXGI_FORMAT_UNKNOWN:
        default:
            return -1;
        }

        return iRetByte / 8;
    }

    /*
bool IsValid(Entity* _pObj)
{
	if (nullptr == _pObj)
		return false;

	if (_pObj->IsDead())
	{
		_pObj = nullptr;
		return false;
	}

	return true;
}*/

    void SaveStringToFile(const string& _str, FILE* _pFile)
    {
        size_t iLen = _str.length();
        fwrite(&iLen, sizeof(size_t), 1, _pFile);
        fwrite(_str.data(), sizeof(char), iLen, _pFile);
    }

    void LoadStringFromFile(string& _str, FILE* _pFile)
    {
        size_t iLen = 0;
        fread(&iLen, sizeof(size_t), 1, _pFile);

        char szBuffer[256] = {};
        fread(szBuffer, sizeof(char), iLen, _pFile);

        _str = szBuffer;
    }

    void SaveWStringToFile(const wstring& _str, FILE* _pFile)
    {
        size_t iLen = _str.length();
        fwrite(&iLen, sizeof(size_t), 1, _pFile);
        fwrite(_str.data(), sizeof(wchar_t), iLen, _pFile);
    }

    void LoadWStringFromFile(wstring& _str, FILE* _pFile)
    {
        size_t iLen = 0;
        fread(&iLen, sizeof(size_t), 1, _pFile);

        wchar_t szBuffer[256] = {};
        fread(szBuffer, sizeof(wchar_t), iLen, _pFile);

        _str = szBuffer;
    }

    /*
void Instantiate(Entity* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx)
{
	_pNewObj->Transform()->SetRelativePos(_vWorldPos);

	tEvent evn = {};
	evn.eType = eEventType::CreateObject;
	evn.wParam = (DWORD_PTR)_pNewObj;
	evn.lParam = (DWORD_PTR)_iLayerIdx;

	CEventMgr::GetInst()->AddEvent(evn);
}
*/

    /*
void DebugDrawRect(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuratiion)
{
	CRenderMgr::GetInst()->DebugDraw(eDebugShape::Rect, _vColor, _vPosition, _vScale, _vRotation, 0.f, _fDuratiion);
}

void DebugDrawCircle(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(eDebugShape::Circle, _vColor, _vPosition, Vec3::Zero, Vec3::Zero, _fRadius, _fDuration);
}
*/

    const char* ToString(eResType _type)
    {
        return RES_TYPE_CHAR[(UINT)_type];
    }

    const wchar_t* ToWString(eResType _type)
    {
        return RES_TYPE_WCHAR[(UINT)_type];
    }
    
}
