#pragma once

#include "lava.h"

#pragma warning( push )
#pragma warning( disable :4996)

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>

#include <cereal/types/base_class.hpp>

#include <cereal/archives/json.hpp>

#pragma warning( pop ) 

#define SERIALIZABLE(ClassName) \
template<class Archive > friend void load(Archive &, ClassName &);\
template<class Archive > friend void save(Archive &, const ClassName &);