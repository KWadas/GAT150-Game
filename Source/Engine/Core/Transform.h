#pragma once
#include "Vector2.h"

namespace kiko
{
	//structs difference is that it defaults to public while class defaults to private

	class Transform
	{
	public:
		vec2 position;
		float rotation = 0;
		float scale = 1;
	public:
		Transform() = default;
		Transform(const vec2& position, float rotation, float scale = 1) :
			position{position},
			rotation{rotation},
			scale{scale}
		{}
	};
}