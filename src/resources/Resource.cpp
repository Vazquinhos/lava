#include "resources/Resource.h"

namespace lava
{
    CResource::CResource() 
        : mResourceType(Type::eUnknown)
        , mResourceBytesSize(0u)
        , mFilename("Invalid Resource Filename")
    {
    }

    CResource::~CResource()
    {
        mResourceType = Type::eUnknown;
        mResourceBytesSize = 0u;
        mFilename = "Invalid Resource Filename";
    }
}