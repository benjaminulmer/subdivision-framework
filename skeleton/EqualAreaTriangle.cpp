#include "EqualAreaTriangle.h"


EqualAreaTriangle::EqualAreaTriangle(Renderable & r, cgiDouble128NS::double128 numFaces) : numFaces(numFaces) {

	d128Vec3 northPole = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), cgiDouble128NS::double128(0.0));

	verts.push_back(northPole);

	pi = cgiDouble128NS::double128("3.1415926535897932384626433832795028841971693993751058");
	targetArea = cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / numFaces;
	//Should be 1
	sinLatitude = cgiDouble128NS::double128(1.0);
	//Should be 0
	cosLatitude = cgiDouble128NS::cos(pi / cgiDouble128NS::double128(2.0));

	//36 degrees converted to radians (splits a circle into 10 parts)
	cgiDouble128NS::double128 rad36 = pi / cgiDouble128NS::double128(5.0);

	numIterations = 200;

	//Create initial pentagonal pyramids
	d128Vec3 interpZeroDeg = findSlerpVector(cgiDouble128NS::double128(0.0));
	d128Vec3 interp72Deg = findSlerpVector(2.0 * rad36);

	cgiDouble128NS::double128 t = slerpBinarySearch(northPole, interpZeroDeg, interp72Deg);

	d128Vec3 p3 = northPole.slerp(findSlerpVector(4.0 * rad36), t);
	d128Vec3 p4 = northPole.slerp(findSlerpVector(6.0 * rad36), t);
	d128Vec3 p5 = northPole.slerp(findSlerpVector(8.0 * rad36), t);

	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Create star around pentagonal pyramid
	d128Vec3 interp36Deg = findSlerpVector(rad36);

	t = slerpBinarySearch(northPole, interp36Deg, verts[1], verts[2], 0.0, 1.0);

	d128Vec3 p1 = northPole.slerp(interp36Deg, t);
	d128Vec3 p2 = northPole.slerp(findSlerpVector(3.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(5.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(7.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(9.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Finish the second ring
	t = slerpBinarySearch(northPole, interpZeroDeg, verts[1], verts[6], 0.0, 1.0);

	p1 = northPole.slerp(interpZeroDeg, t);
	p2 = northPole.slerp(findSlerpVector(2.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(4.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(6.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(8.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Create two hexagon pts on third ring
	//requiredLongitude is an output parameter
	cgiDouble128NS::double128 requiredLongitude = cgiDouble128NS::double128(0.0);
	t = longitudeBinarySearch(cgiDouble128NS::double128(0.0), rad36, northPole, verts[11], verts[6], &requiredLongitude, 0.0, 1.0);

	cgiDouble128NS::double128 requiredLongitude2 = (2.0 * rad36) - requiredLongitude;

	p1 = northPole.slerp(findSlerpVector((2.0 * rad36) + requiredLongitude), t);
	p2 = northPole.slerp(findSlerpVector((2.0 * rad36) + requiredLongitude2), t);

	p3 = northPole.slerp(findSlerpVector((4.0 * rad36) + requiredLongitude), t);
	p4 = northPole.slerp(findSlerpVector((4.0 * rad36) + requiredLongitude2), t);

	p5 = northPole.slerp(findSlerpVector((6.0 * rad36) + requiredLongitude), t);
	d128Vec3 p6 = northPole.slerp(findSlerpVector((6.0 * rad36) + requiredLongitude2), t);

	d128Vec3 p7 = northPole.slerp(findSlerpVector((8.0 * rad36) + requiredLongitude), t);
	d128Vec3 p8 = northPole.slerp(findSlerpVector((8.0 * rad36) + requiredLongitude2), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);
	verts.push_back(p6);
	verts.push_back(p7);
	verts.push_back(p8);

	//Finish the third ring
	t = slerpBinarySearch(northPole, interpZeroDeg, verts[11], verts[16], 0.0, 1.0);

	p1 = northPole.slerp(interpZeroDeg, t);
	p2 = northPole.slerp(findSlerpVector(2.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(4.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(6.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(8.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Start fourth ring with 36deg slerp from 2pt hexagon optimization
	t = slerpBinarySearch(northPole, interp36Deg, verts[16], verts[17], 0.0, 1.0);

	p1 = northPole.slerp(interp36Deg, t);
	p2 = northPole.slerp(findSlerpVector(3.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(5.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(7.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(9.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//longitude slerp for pts between 0 and 22ish deg in fourth ring
	t = longitudeBinarySearch(cgiDouble128NS::double128(0.0), requiredLongitude, northPole, verts[26], verts[16], verts[31], &requiredLongitude);

	requiredLongitude2 = (2.0 * rad36) - requiredLongitude;

	p1 = northPole.slerp(findSlerpVector((2.0 * rad36) + requiredLongitude), t);

	p2 = northPole.slerp(findSlerpVector((2.0 * rad36) + requiredLongitude2), t);
	p3 = northPole.slerp(findSlerpVector((4.0 * rad36) + requiredLongitude), t);
	
	p4 = northPole.slerp(findSlerpVector((4.0 * rad36) + requiredLongitude2), t);
	p5 = northPole.slerp(findSlerpVector((6.0 * rad36) + requiredLongitude), t);
	
	p6 = northPole.slerp(findSlerpVector((6.0 * rad36) + requiredLongitude2), t);
	p7 = northPole.slerp(findSlerpVector((8.0 * rad36) + requiredLongitude), t);
	
	p8 = northPole.slerp(findSlerpVector((8.0 * rad36) + requiredLongitude2), t);
	d128Vec3 p9 = northPole.slerp(findSlerpVector(requiredLongitude2), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);
	verts.push_back(p6);
	verts.push_back(p7);
	verts.push_back(p8);
	verts.push_back(p9);

	//Finish fourth ring
	t = slerpBinarySearch(northPole, interpZeroDeg, verts[26], verts[36], 0.0, 1.0);

	p1 = northPole.slerp(interpZeroDeg, t);
	p2 = northPole.slerp(findSlerpVector(2.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(4.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(6.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(8.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Create pentagons at the equator
	/*t = slerpBinarySearch(northPole.slerp(interp36Deg, 0.5), interp36Deg, verts[36], verts[31]);

	p1 = northPole.slerp(interp36Deg, 0.5).slerp(interp36Deg, t);
	p2 = northPole.slerp(findSlerpVector(3.0 * rad36), 0.5).slerp(findSlerpVector(3.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(5.0 * rad36), 0.5).slerp(findSlerpVector(5.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(7.0 * rad36), 0.5).slerp(findSlerpVector(7.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(9.0 * rad36), 0.5).slerp(findSlerpVector(9.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);*/

	//Hexagon points near equator
	t = longitudeBinarySearch(cgiDouble128NS::double128(0.0), rad36, northPole, verts[36], verts[31], &requiredLongitude, 0.8, 1.2);

	requiredLongitude2 = (2.0 * rad36) - requiredLongitude;

	p1 = northPole.slerp(findSlerpVector((2.0 * rad36) + requiredLongitude), t);
	p2 = northPole.slerp(findSlerpVector((2.0 * rad36) + requiredLongitude2), t);

	p3 = northPole.slerp(findSlerpVector((4.0 * rad36) + requiredLongitude), t);
	p4 = northPole.slerp(findSlerpVector((4.0 * rad36) + requiredLongitude2), t);

	p5 = northPole.slerp(findSlerpVector((6.0 * rad36) + requiredLongitude), t);
	p6 = northPole.slerp(findSlerpVector((6.0 * rad36) + requiredLongitude2), t);

	p7 = northPole.slerp(findSlerpVector((8.0 * rad36) + requiredLongitude), t);
	p8 = northPole.slerp(findSlerpVector((8.0 * rad36) + requiredLongitude2), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);
	verts.push_back(p6);
	verts.push_back(p7);
	verts.push_back(p8);

	//Duplicate vertices into the south pole (must be rotated by 36 degrees)
	createSouth();

	//t = angleExhaustiveSearch(-rad36 - rad36/2.0, -rad36 + rad36/2.0, verts[51], verts[45], verts[47]);
	//print(t);

	//Final hexagon pts
	//t = longitudeBinarySearch2(cgiDouble128NS::double128(0.0), rad36, northPole, verts[46], verts[36], verts[51], &requiredLongitude);

	//print(t);

	fillRenderable(r);

	outputCSV();
}


EqualAreaTriangle::~EqualAreaTriangle() {

}

void EqualAreaTriangle::print(cgiDouble128NS::double128 p) {
	char s[128];
	p.str(s);
	std::cout << "p: " << std::endl;
	std::cout << s << std::endl;
}

void EqualAreaTriangle::outputCSV() {
	std::ofstream myfile;
	myfile.open("vertices.csv");
	for (int i = 0; i < verts.size(); i++) {
		char x[128];
		verts[i].x.str(x);

		char y[128];
		verts[i].y.str(y);

		char z[128];
		verts[i].z.str(z);

		myfile << x << "," << y << "," << z << "\n";
	}
	myfile.close();
}

cgiDouble128NS::double128 EqualAreaTriangle::sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3) {
	cgiDouble128NS::double128 a = cgiDouble128NS::acos(p1.dot(p2));
	cgiDouble128NS::double128 b = cgiDouble128NS::acos(p1.dot(p3));
	cgiDouble128NS::double128 c = cgiDouble128NS::acos(p2.dot(p3));
	cgiDouble128NS::double128 s = (a + b + c) / 2.0;
	return 4.0 * cgiDouble128NS::atan(cgiDouble128NS::sqrt(cgiDouble128NS::tan(s / 2.0) * cgiDouble128NS::tan((s - a) / 2.0) * cgiDouble128NS::tan((s - b) / 2.0) * cgiDouble128NS::tan((s - c) / 2.0)));
}

d128Vec3 EqualAreaTriangle::findSlerpVector(cgiDouble128NS::double128 longitude) {
	return d128Vec3(cgiDouble128NS::sin(longitude) * sinLatitude, cosLatitude, cgiDouble128NS::cos(longitude) * sinLatitude);
}

cgiDouble128NS::double128 EqualAreaTriangle::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 dest2) {
	cgiDouble128NS::double128 lower = cgiDouble128NS::double128(0.0);
	cgiDouble128NS::double128 upper = cgiDouble128NS::double128(1.0);

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));
	d128Vec3 p2 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	for (unsigned int i = 0; i < numIterations; i++) {
		p1 = source.slerp(dest, (upper + lower) / cgiDouble128NS::double128(2.0));
		p2 = source.slerp(dest2, (upper + lower) / cgiDouble128NS::double128(2.0));

		cgiDouble128NS::double128 area = sphericalTriangleArea(p1, p2, source);

		if (cgiDouble128NS::fabs(area - targetArea) < cgiDouble128NS::quadEpsilon()) {
			break;
		} else if (area < targetArea) {
			lower = (upper + lower) / cgiDouble128NS::double128(2.0);
		} else if (area > targetArea) {
			upper = (upper + lower) / cgiDouble128NS::double128(2.0);
		} else {
			break;
		}
	}

	verts.push_back(p1);
	verts.push_back(p2);

	return (upper + lower) / cgiDouble128NS::double128(2.0);

}


cgiDouble128NS::double128 EqualAreaTriangle::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper) {

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	for (unsigned int i = 0; i < numIterations; i++) {
		p1 = source.slerp(dest, (upper + lower) / cgiDouble128NS::double128(2.0));

		cgiDouble128NS::double128 area = sphericalTriangleArea(known, known2, p1);

		if (cgiDouble128NS::fabs(area - targetArea) < cgiDouble128NS::quadEpsilon()) {
			break;
		} else if (area < targetArea) {
			lower = (upper + lower) / cgiDouble128NS::double128(2.0);
		} else if (area > targetArea) {
			upper = (upper + lower) / cgiDouble128NS::double128(2.0);
		} else {
			break;
		}
	}

	return (upper + lower) / cgiDouble128NS::double128(2.0);
}

cgiDouble128NS::double128 EqualAreaTriangle::longitudeBinarySearch(cgiDouble128NS::double128 minLon, cgiDouble128NS::double128 maxLon, d128Vec3 source, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128* outLon, cgiDouble128NS::double128 l, cgiDouble128NS::double128 u) {
	cgiDouble128NS::double128 lower = minLon;
	cgiDouble128NS::double128 upper = maxLon;

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));
	d128Vec3 p2 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	cgiDouble128NS::double128 t = cgiDouble128NS::double128(0.0);

	for (unsigned int i = 0; i < numIterations; i++) {
		cgiDouble128NS::double128 m = (lower + upper) / cgiDouble128NS::double128(2.0);
		d128Vec3 v1 = findSlerpVector(m);
		d128Vec3 v2 = findSlerpVector((2.0 * maxLon) - m);

		t = slerpBinarySearch(source, v1, known, known2, l, u);

		p1 = source.slerp(v1, t);
		p2 = source.slerp(v2, t);

		cgiDouble128NS::double128 area = sphericalTriangleArea(p1, p2, known2);

		if (cgiDouble128NS::fabs(area - targetArea) < cgiDouble128NS::quadEpsilon()) {
			break;
		} else if (area < targetArea) {
			upper = m;
		} else if (area > targetArea) {
			lower = m;
		} else {
			break;
		}

	}

	verts.push_back(p1);
	verts.push_back(p2);

	*outLon = (lower + upper) / cgiDouble128NS::double128(2.0);

	return t;
}

cgiDouble128NS::double128 EqualAreaTriangle::longitudeBinarySearch(cgiDouble128NS::double128 minLon, cgiDouble128NS::double128 maxLon, d128Vec3 source, d128Vec3 known, d128Vec3 known2, d128Vec3 known3, cgiDouble128NS::double128 * outLon) {
	cgiDouble128NS::double128 lower = minLon;
	cgiDouble128NS::double128 upper = maxLon;

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	cgiDouble128NS::double128 t = cgiDouble128NS::double128(0.0);

	for (unsigned int i = 0; i < numIterations; i++) {
		cgiDouble128NS::double128 m = (lower + upper) / cgiDouble128NS::double128(2.0);
		d128Vec3 v1 = findSlerpVector(m);

		t = slerpBinarySearch(source, v1, known2, known3, 0.0, 1.0);

		p1 = source.slerp(v1, t);

		cgiDouble128NS::double128 area = sphericalTriangleArea(p1, known, known2);

		if (cgiDouble128NS::fabs(area - targetArea) < cgiDouble128NS::quadEpsilon()) {
			break;
		} else if (area < targetArea) {
			lower = m;
		} else if (area > targetArea) {
			upper = m;
		} else {
			break;
		}

	}

	verts.push_back(p1);

	*outLon = (lower + upper) / cgiDouble128NS::double128(2.0);

	return t;
}

cgiDouble128NS::double128 EqualAreaTriangle::longitudeBinarySearch2(cgiDouble128NS::double128 minLon, cgiDouble128NS::double128 maxLon, d128Vec3 source, d128Vec3 known, d128Vec3 known2, d128Vec3 known3, cgiDouble128NS::double128 * outLon) {
	cgiDouble128NS::double128 lower = minLon;
	cgiDouble128NS::double128 upper = maxLon;

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	cgiDouble128NS::double128 t = cgiDouble128NS::double128(0.0);

	for (unsigned int i = 0; i < numIterations; i++) {
		cgiDouble128NS::double128 m = (lower + upper) / cgiDouble128NS::double128(2.0);
		d128Vec3 v1 = findSlerpVector(m);

		t = slerpBinarySearch(source.slerp(v1, 0.9), v1, known2, known3, 0.0, 1.0);

		p1 = source.slerp(v1, 0.9).slerp(v1, t);

		cgiDouble128NS::double128 area = sphericalTriangleArea(p1, known, known2);

		if (cgiDouble128NS::fabs(area - targetArea) < cgiDouble128NS::quadEpsilon()) {
			break;
		} else if (area < targetArea) {
			lower = m;
		} else if (area > targetArea) {
			upper = m;
		} else {
			break;
		}

	}

	verts.push_back(p1);

	*outLon = (lower + upper) / cgiDouble128NS::double128(2.0);

	return t;
}

cgiDouble128NS::double128 EqualAreaTriangle::angleExhaustiveSearch(cgiDouble128NS::double128 minAngle, cgiDouble128NS::double128 maxAngle, d128Vec3 known, d128Vec3 known2, d128Vec3 known3) {
	
	for (cgiDouble128NS::double128 angle = minAngle; angle < maxAngle; angle += cgiDouble128NS::double128(0.0000001)) {
		cgiDouble128NS::double128 sinAngle = cgiDouble128NS::sin(angle);
		cgiDouble128NS::double128 cosAngle = cgiDouble128NS::cos(angle);

		d128Vec3 p1 = d128Vec3(known.x * cosAngle + known.z * sinAngle, -known.y, (known.x * -sinAngle) + known.z * cosAngle);
		d128Vec3 p2 = d128Vec3(known2.x * cosAngle + known2.z * sinAngle, -known2.y, (known2.x * -sinAngle) + known2.z * cosAngle);
		d128Vec3 p3 = d128Vec3(known3.x * cosAngle + known3.z * sinAngle, -known3.y, (known3.x * -sinAngle) + known3.z * cosAngle);

		d128Vec3 centroid = verts[46].slerp(p3, 0.5);

		cgiDouble128NS::double128 area = sphericalTriangleArea(centroid, verts[46], p1);
		cgiDouble128NS::double128 area2 = sphericalTriangleArea(centroid, verts[46], verts[36]);
		cgiDouble128NS::double128 area3 = sphericalTriangleArea(centroid, verts[51], verts[36]);

		/*std::cout << "area: " << std::endl;
		print(area);
		std::cout << "area2: " << std::endl;
		print(area2);
		std::cout << "area3: " << std::endl;
		print(area3);*/

		if (cgiDouble128NS::fabs(area3 - targetArea) < cgiDouble128NS::double128(0.00000001)) {

			std::cout << "area: " << std::endl;
			print(area);
			std::cout << "area2: " << std::endl;
			print(area2);
			std::cout << "area3: " << std::endl;
			print(area3);

			verts.push_back(p1);
			verts.push_back(p2);
			verts.push_back(p3);
			verts.push_back(centroid);
			return angle;
			break;
		}

	}

	return cgiDouble128NS::double128(0.0);
}

void EqualAreaTriangle::createSouth() {
	cgiDouble128NS::double128 rad36 = pi / cgiDouble128NS::double128(5.0);
	cgiDouble128NS::double128 sin36 = cgiDouble128NS::sin(rad36);
	cgiDouble128NS::double128 cos36 = cgiDouble128NS::cos(rad36);

	unsigned int j = verts.size();
	for (unsigned int i = 0; i < j; i++) {
		verts.push_back(d128Vec3(verts[i].x * cos36 + verts[i].z * sin36, -verts[i].y, (verts[i].x * -sin36) + verts[i].z * cos36));
	}
}

void EqualAreaTriangle::fillRenderable(Renderable & r) {
	for (unsigned int i = 0; i < verts.size(); i++) {
		r.verts.push_back(glm::vec3(double(verts[i].x), double(verts[i].y), double(verts[i].z)));
		if (i != 31) {
			r.normals.push_back(glm::vec3(0.f, 0.f, 0.f));
		} else {
			r.normals.push_back(glm::vec3(1.f, 0.f, 0.f));
		}
	}
}
