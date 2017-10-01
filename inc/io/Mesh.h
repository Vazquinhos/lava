#pragma once

#include "lava.h"

namespace lava
{
	namespace io
	{
		class Mesh
		{
		public:
			Mesh() = default;
			virtual ~Mesh() = default;
			bool Load( const std::string& _filePath );
		};
	}
}
