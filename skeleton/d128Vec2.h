#pragma once

#include "double128/double128.h"

class d128Vec2 {
public:
	d128Vec2();
	d128Vec2(cgiDouble128NS::double128 x, cgiDouble128NS::double128 y);
	~d128Vec2();

	d128Vec2 operator*(cgiDouble128NS::double128 s);
	d128Vec2 operator+(d128Vec2 s);
	d128Vec2 operator-(d128Vec2 s);

	cgiDouble128NS::double128 x;
	cgiDouble128NS::double128 y;
};

