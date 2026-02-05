#pragma once
#include"includes.h"
#define ADD_INTERFACE( type, var_name, address ) type* var_name = address.cast< type* >();
#define ADD_INTERFACE_CUSTOM( type, var_name, function ) type* var_name = function();
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define AutoPadding(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
#define AutoPadding_0(type, name, offset) type name

inline int aim_target = 0;

namespace ctx
{
	
	namespace data
	{
		inline uintptr_t base;
	}


	struct bocw
	{
		//maybe move refdef to here. makes more sense

	};

	extern bocw mw2;
}
