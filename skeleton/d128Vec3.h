#pragma once

#include "double128/double128.h"

class d128Vec3 {
public:
	d128Vec3();
	d128Vec3(cgiDouble128NS::double128 x, cgiDouble128NS::double128 y, cgiDouble128NS::double128 z);
	~d128Vec3();
	
	d128Vec3 operator*(cgiDouble128NS::double128 s);
	d128Vec3 operator+(d128Vec3 s);
	d128Vec3 operator-(d128Vec3 s);

	d128Vec3 cross(d128Vec3 other);
	cgiDouble128NS::double128 dot(d128Vec3 other);
	d128Vec3 slerp(d128Vec3 dest, cgiDouble128NS::double128 t);

	void normalize();

	cgiDouble128NS::double128 length();

	cgiDouble128NS::double128 x;
	cgiDouble128NS::double128 y;
	cgiDouble128NS::double128 z;
};

