#pragma once

#include "lava.h"

namespace lava
{
    class ResourceManager;
    class CResource
    {
    public:
      enum class Type
      {
        eUnknown = 0,
        eTexture,
        eMesh,
        eMaterial,
        eShader,
        eSound,

        MAX,
      };
    public:
        Type               GetType() const { return mResourceType; }
        const std::string& GetFilename() const { return mFilename; }
        const uint32_t     GetBytesSize() const { return mResourceBytesSize; }

    protected:
        friend class CResourceManager;

        CResource();
        virtual ~CResource();

        virtual bool Load(const std::string& aFilename) = 0;
        virtual void Unload() = 0;

        void SetFilename(const std::string& aFilename) { mFilename = aFilename; }

        Type mResourceType;
        uint32_t mResourceBytesSize;
        std::string mFilename;
    };
}