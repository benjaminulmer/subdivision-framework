#include "EqualAreaDodecahedron.h"

EqualAreaDodecahedron::EqualAreaDodecahedron(Renderable & r, cgiDouble128NS::double128 numFaces) : numFaces(numFaces) {
	numIterations = 200;

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
	
	//Create pentakis pentagon
	d128Vec3 interpZeroDeg = findSlerpVector(cgiDouble128NS::double128(0.0));
	d128Vec3 interp36Deg = findSlerpVector(rad36);
	d128Vec3 interp72Deg = findSlerpVector(2.0 * rad36);

	cgiDouble128NS::double128 t = slerpBinarySearch(northPole, interpZeroDeg, interp72Deg, cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / 60.0);

	d128Vec3 p1 = northPole.slerp(findSlerpVector(0.0), t);
	d128Vec3 p2 = northPole.slerp(findSlerpVector(2.0 * rad36), t);
	d128Vec3 p3 = northPole.slerp(findSlerpVector(4.0 * rad36), t);
	d128Vec3 p4 = northPole.slerp(findSlerpVector(6.0 * rad36), t);
	d128Vec3 p5 = northPole.slerp(findSlerpVector(8.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Trisect previously created triangles
	t = slerpBinarySearch(northPole, interp36Deg, northPole, p1, 0.0, 1.0, cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / 180.0);

	p1 = northPole.slerp(findSlerpVector(rad36), t);
	p2 = northPole.slerp(findSlerpVector(3.0 * rad36), t);
	p3 = northPole.slerp(findSlerpVector(5.0 * rad36), t);
	p4 = northPole.slerp(findSlerpVector(7.0 * rad36), t);
	p5 = northPole.slerp(findSlerpVector(9.0 * rad36), t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	//Bisect previously created triangles

	t = slerpBinarySearch(northPole, verts[1], northPole, p1, 0.0, 1.0, cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / 360.0);

	p1 = northPole.slerp(verts[1], t);
	p2 = northPole.slerp(verts[2], t);
	p3 = northPole.slerp(verts[3], t);
	p4 = northPole.slerp(verts[4], t);
	p5 = northPole.slerp(verts[5], t);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	p1 = verts[1].slerp(verts[2], 0.5);
	p2 = verts[2].slerp(verts[3], 0.5);
	p3 = verts[3].slerp(verts[4], 0.5);
	p4 = verts[4].slerp(verts[5], 0.5);
	p5 = verts[5].slerp(verts[1], 0.5);

	verts.push_back(p1);
	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);

	fillRenderable(r);

	outputCSV();

}

EqualAreaDodecahedron::~EqualAreaDodecahedron() {
}

void EqualAreaDodecahedron::print(cgiDouble128NS::double128 p) {
	char s[128];
	p.str(s);
	std::cout << "p: " << std::endl;
	std::cout << s << std::endl;
}

void EqualAreaDodecahedron::outputCSV() {
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

void EqualAreaDodecahedron::outputOBJ() {
	std::ofstream myfile;
	myfile.open("sphere.obj");
	myfile << "o Sphere\n";
	for (int i = 0; i < verts.size(); i++) {
		char x[128];
		verts[i].x.str(x);

		char y[128];
		verts[i].y.str(y);

		char z[128];
		verts[i].z.str(z);

		myfile << "v " << x << " " << y << " " << z << "\n";
	}
	myfile << "s off\n";
	for (int i = 0; i < faces.size(); i += 3) {
		myfile << "f " << faces[i] << " " << faces[i + 1] << " " << faces[i + 2] << "\n";
	}
}

void EqualAreaDodecahedron::fillRenderable(Renderable & r) {
	for (unsigned int i = 0; i < verts.size(); i++) {
		r.verts.push_back(glm::vec3(double(verts[i].x), double(verts[i].y), double(verts[i].z)));
		if (i != 31) {
			r.normals.push_back(glm::vec3(0.f, 0.f, 0.f));
		} else {
			r.normals.push_back(glm::vec3(1.f, 0.f, 0.f));
		}
	}
}

cgiDouble128NS::double128 EqualAreaDodecahedron::sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3) {
	cgiDouble128NS::double128 a = cgiDouble128NS::acos(p1.dot(p2));
	cgiDouble128NS::double128 b = cgiDouble128NS::acos(p1.dot(p3));
	cgiDouble128NS::double128 c = cgiDouble128NS::acos(p2.dot(p3));
	cgiDouble128NS::double128 s = (a + b + c) / 2.0;
	return 4.0 * cgiDouble128NS::atan(cgiDouble128NS::sqrt(cgiDouble128NS::tan(s / 2.0) * cgiDouble128NS::tan((s - a) / 2.0) * cgiDouble128NS::tan((s - b) / 2.0) * cgiDouble128NS::tan((s - c) / 2.0)));
}

d128Vec3 EqualAreaDodecahedron::findSlerpVector(cgiDouble128NS::double128 longitude) {
	return d128Vec3(cgiDouble128NS::sin(longitude) * sinLatitude, cosLatitude, cgiDouble128NS::cos(longitude) * sinLatitude);
}

cgiDouble128NS::double128 EqualAreaDodecahedron::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper) {
	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	cgiDouble128NS::double128 area;

	for (unsigned int i = 0; i < numIterations; i++) {
		p1 = source.slerp(dest, (upper + lower) / cgiDouble128NS::double128(2.0));

		area = sphericalTriangleArea(known, known2, p1);

		if (cgiDouble128NS::fabs(area - targetArea) < cgiDouble128NS::quadEpsilon()) {
			break;
		} else if (area > targetArea) {
			lower = (upper + lower) / cgiDouble128NS::double128(2.0);
		} else if (area < targetArea) {
			upper = (upper + lower) / cgiDouble128NS::double128(2.0);
		} else {
			break;
		}
	}

	print(area);

	return (upper + lower) / cgiDouble128NS::double128(2.0);
}

cgiDouble128NS::double128 EqualAreaDodecahedron::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper, cgiDouble128NS::double128 targetArea) {
	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0));

	cgiDouble128NS::double128 area;

	for (unsigned int i = 0; i < numIterations; i++) {
		p1 = source.slerp(dest, (upper + lower) / cgiDouble128NS::double128(2.0));

		area = sphericalTriangleArea(known, known2, p1);

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

	print(area);

	return (upper + lower) / cgiDouble128NS::double128(2.0);
}

cgiDouble128NS::double128 EqualAreaDodecahedron::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 dest2, cgiDouble128NS::double128 targetArea) {
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

	return (upper + lower) / cgiDouble128NS::double128(2.0);
}

void EqualAreaDodecahedron::createSouth() {
	//cgiDouble128NS::double128 rad45 = pi / cgiDouble128NS::double128(8.0);
	//cgiDouble128NS::double128 sin36 = cgiDouble128NS::sin(rad45);
	//cgiDouble128NS::double128 cos36 = cgiDouble128NS::cos(rad45);

	unsigned int j = verts.size();
	for (unsigned int i = 0; i < j; i++) {
		verts.push_back(d128Vec3(verts[i].x, -verts[i].y, verts[i].z));
	}
}
