#pragma once
#include "Res.h"

namespace tomato
{
    class Texture;
    class AnimClip : public Res
    {
        friend class AnimState;
        friend class AnimationEditorUI;


    public:
        AnimClip();
        ~AnimClip();

        void Save(const fs::path& path) override {};
        int  Load(const fs::path& path) override { return S_OK; };


        void CreateFromAtlasFile(const fs::path& atlasPath);
        void SortFrameByIndex();

        void AddFrame(AnimationFrm frame);
        void AddBonePosition(const string& boneName, Vec3 bonePos);
        void AddEventToFrame(int idx, AnimEvent event);
        void SetHasBone(bool bHasBone) { m_bHasBone = bHasBone; }

        [[nodiscard]] const AnimationFrm&         GetFrame(int idx) const;
        [[nodiscard]] const vector<AnimationFrm>& GetFrames() const { return m_Frames; }


        [[nodiscard]] Vec3 GetBonePosition(const string& boneName, int idx) const;
        [[nodiscard]] bool IsHasBone() const { return m_bHasBone; }
        
    private:
        // TODO Transform 등 다른 프로퍼티도 수정할 수 있도록
        // TODO Curve 구현
        vector<AnimationFrm> m_Frames;

        bool                      m_bHasBone;
        map<string, vector<Vec3>> m_BonePositions;
    };
}
