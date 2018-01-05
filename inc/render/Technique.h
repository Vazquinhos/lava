/* Copyright (C) 2017 Alejandro Vazquez vazquinhos@gmail.com */
#pragma once

#include "lava.h"

namespace lava
{
  class CTechnique
  {
    SERIALIZABLE(CTechnique)

  public:
    enum class Stage
    {
      eVertexShader = 0,
      eFragmentShader,

      MAX
    };

  public:
    CTechnique()
    {
      mShaders.resize(static_cast<size_t>(Stage::MAX));
    }

    virtual ~CTechnique() = default;

    void SetShader(Stage aStage, CShaderPtr aShader)
    {
      mShaders[static_cast<size_t>(aStage)] = aShader;
    }

    CShaderPtr GetShader(Stage aStage)
    {
      return mShaders[static_cast<size_t>(aStage)];
    }

  private:
    std::vector<CShaderPtr> mShaders;
  };

  Begin_Enum_String(CTechnique::Stage)
  {
    Register_Enum_String(CTechnique::Stage::eVertexShader, "VS");
    Register_Enum_String(CTechnique::Stage::eFragmentShader, "FS");
  }
  End_Enum_String;
}