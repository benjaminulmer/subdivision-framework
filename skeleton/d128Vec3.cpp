#include "d128Vec3.h"

d128Vec3::d128Vec3() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

d128Vec3::d128Vec3(cgiDouble128NS::double128 x, cgiDouble128NS::double128 y, cgiDouble128NS::double128 z) : x(x), y(y), z(z) {

}

d128Vec3::~d128Vec3() {

}

d128Vec3 d128Vec3::operator*(cgiDouble128NS::double128 s) {
	return d128Vec3(x*s, y*s, z*s);
}

d128Vec3 d128Vec3::operator+(d128Vec3 s) {
	return d128Vec3(x + s.x, y + s.y, z + s.z);
}

d128Vec3 d128Vec3::operator-(d128Vec3 s) {
	return d128Vec3(x - s.x, y - s.y, z - s.z);
}

d128Vec3 d128Vec3::cross(d128Vec3 other) {
	return d128Vec3((y * other.z) - (z * other.y), (z * other.x) - (x * other.z), (x * other.y) - (y * other.x));
}

cgiDouble128NS::double128 d128Vec3::dot(d128Vec3 other) {
	return ( (x * other.x) + (y * other.y) + (z * other.z) );
}

d128Vec3 d128Vec3::slerp(d128Vec3 dest, cgiDouble128NS::double128 t) {
	cgiDouble128NS::double128 dot = this->dot(dest);
	cgiDouble128NS::double128 theta = cgiDouble128NS::acos(dot);
	d128Vec3 term1 = (*this) * (cgiDouble128NS::sin((1.0 - t) * theta) / cgiDouble128NS::sin(theta));
	d128Vec3 term2 = dest * (cgiDouble128NS::sin(t * theta) / cgiDouble128NS::sin(theta));
	return d128Vec3(term1.x + term2.x, term1.y + term2.y, term1.z + term2.z);
}

void d128Vec3::normalize() {
	cgiDouble128NS::double128 length = cgiDouble128NS::sqrt((x * x) + (y * y) + (z * z));
	x = x / length;
	y = y / length;
	z = z / length;
}

cgiDouble128NS::double128 d128Vec3::length() {
	return cgiDouble128NS::sqrt((x * x) + (y * y) + (z * z));
}
