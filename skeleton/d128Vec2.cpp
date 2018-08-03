#include "d128Vec2.h"

d128Vec2::d128Vec2() {
	x = 0.0;
	y = 0.0;
}

d128Vec2::d128Vec2(cgiDouble128NS::double128 x, cgiDouble128NS::double128 y) : x(x), y(y) {

}

d128Vec2::~d128Vec2() {
}

d128Vec2 d128Vec2::operator*(cgiDouble128NS::double128 s) {
	return d128Vec2(x*s, y*s);
}

d128Vec2 d128Vec2::operator+(d128Vec2 s) {
	return d128Vec2(x + s.x, y + s.y);
}

d128Vec2 d128Vec2::operator-(d128Vec2 s) {
	return d128Vec2(x - s.x, y - s.y);
}