#include "pch.h"
#include "AnimClip.h"

#include "tomato/Resources/Texture.h"
#include "tomato/Core/CPathMgr.h"
#include "tomato/Core/CResMgr.h"

namespace tomato
{
    void AnimClip::CreateFromAtlasFile(const fs::path& atlasPath)
    {
        fs::path atlasFullPath = CPathMgr::GetInst()->GetContentPath();
        atlasFullPath += atlasPath;

        std::ifstream atlas(atlasFullPath, std::ios::binary);
        assert(atlas);

        char signature[5] = {};
        atlas.read(signature, 4);
        assert(strcmp(signature, "BATL") == 0);

        while (true)
        {
            char fileNameLen = 0;
            atlas.read(&fileNameLen, sizeof(char));
            if (fileNameLen == 0)
            {
                // End of Atlas
                break;
            }

            char fileName[256] = {};
            atlas.read(fileName, fileNameLen);

            fs::path texturePath = atlasPath / ".." / fileName;
            texturePath = texturePath.lexically_normal();
            Ref<Texture> texture = CResMgr::GetInst()->LoadRelative<Texture>(texturePath);

            while (true)
            {
                char nameLen = 0;
                atlas.read(&nameLen, sizeof(char));
                if (nameLen == 0)
                {
                    // End of file
                    break;
                }

                string name;
                name.resize(nameLen);
                atlas.read(name.data(), nameLen);

                char index = 0;
                atlas.read(&index, sizeof(char));

                /*
            // 이름 속에 인덱스가 있는지 검사
            if (index == 0)
            {
                static const string DELIMITER = "_";

                const size_t delimiterPos = name.find(DELIMITER);
                if (delimiterPos != string::npos)
                {
                    string indexInName = name.substr(delimiterPos + DELIMITER.length(), name.length());
                    index              = std::stoi(indexInName);

                    name.erase(delimiterPos, name.length());
                }
            }*/

                bool rotate = false;
                atlas.read((char*)&rotate, sizeof(bool));

                struct RectShort
                {
                    short x;
                    short y;
                };

                RectShort leftTop;
                atlas.read((char*)&leftTop, sizeof(RectShort));

                RectShort size;
                atlas.read((char*)&size, sizeof(RectShort));

                RectShort offset;
                atlas.read((char*)&offset, sizeof(RectShort));

                RectShort origSize;
                atlas.read((char*)&origSize, sizeof(RectShort));

                SpriteFrm frm;
                frm.LeftTop = Vec2{(float)leftTop.x, (float)leftTop.y};
                frm.Size = Vec2{(float)size.x, (float)size.y};
                frm.Offset = Vec2{(float)offset.x, (float)offset.y};
                frm.OrigSize = Vec2{(float)origSize.x, (float)origSize.y};

                texture->AddFrame(frm, name);
            }
        }
    }

    void AnimClip::AddFrame(AnimationFrm frame)
    {
        m_Frames.push_back(frame);
    }

    void AnimClip::SortFrameByIndex()
    {
        std::sort(m_Frames.begin(), m_Frames.end(), [](AnimationFrm a, AnimationFrm b) { return a.Index < b.Index; });
    }

    const AnimationFrm& AnimClip::GetFrame(int idx) const
    {
        assert(idx < m_Frames.size());
        return m_Frames[idx];
    }

    void AnimClip::AddBonePosition(const string& boneName, Vec3 bonePos)
    {
        m_BonePositions[boneName].push_back(bonePos);
    }

    void AnimClip::AddEventToFrame(int idx, AnimEvent event)
    {
        m_Frames[idx].Events.push_back(event);
    }

    Vec3 AnimClip::GetBonePosition(const string& boneName, int idx) const
    {
        if (!m_BonePositions.count(boneName))
        {
            // TODO 예외 처리
            return {};
        }

        return m_BonePositions.at(boneName)[idx];
    }

    AnimClip::AnimClip()
        : Res(eResType::AnimClip) { }

    AnimClip::~AnimClip() {}
}
