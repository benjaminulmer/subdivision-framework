#include "EqualAreaTriangleSubdivision.h"



EqualAreaTriangleSubdivision::EqualAreaTriangleSubdivision(Renderable & r, cgiDouble128NS::double128 numFaces) : numFaces(numFaces) {
	numIterations = 200;

	d128Vec3 northPole = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), cgiDouble128NS::double128(0.0));

	verts.push_back(northPole);

	pi = cgiDouble128NS::double128("3.1415926535897932384626433832795028841971693993751058");
	targetArea = cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / numFaces;
	//Should be 1
	sinLatitude = cgiDouble128NS::double128(1.0);
	//Should be 0
	cosLatitude = cgiDouble128NS::cos(pi / cgiDouble128NS::double128(2.0));

	//60 degrees converted to radians (splits a circle into 6 parts)
	cgiDouble128NS::double128 rad45 = pi / cgiDouble128NS::double128(8.0);

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0));
	d128Vec3 p2 = d128Vec3(cgiDouble128NS::sin(rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(rad45));

	verts.push_back(p1);
	verts.push_back(p2);

	p1 = findSlerpVector(cgiDouble128NS::double128(0.0));
	p2 = findSlerpVector(rad45);

	for (unsigned int i = 0; i < 8; i++) {

		cgiDouble128NS::double128 t = slerpBinarySearch(northPole, p1, p1, p2, 0.0, 1.0);

		d128Vec3 p3 = northPole.slerp(p1, t);
		d128Vec3 p4 = d128Vec3(p3.x * cgiDouble128NS::cos(2.0 * rad45) + p3.z * cgiDouble128NS::sin(2.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(2.0 * rad45)) + p3.z * cgiDouble128NS::cos(2.0 * rad45));
		d128Vec3 p5 = d128Vec3(p3.x * cgiDouble128NS::cos(4.0 * rad45) + p3.z * cgiDouble128NS::sin(4.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(4.0 * rad45)) + p3.z * cgiDouble128NS::cos(4.0 * rad45));
		d128Vec3 p6 = d128Vec3(p3.x * cgiDouble128NS::cos(6.0 * rad45) + p3.z * cgiDouble128NS::sin(6.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(6.0 * rad45)) + p3.z * cgiDouble128NS::cos(6.0 * rad45));
		d128Vec3 p7 = d128Vec3(p3.x * cgiDouble128NS::cos(8.0 * rad45) + p3.z * cgiDouble128NS::sin(8.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(8.0 * rad45)) + p3.z * cgiDouble128NS::cos(8.0 * rad45));
		d128Vec3 p8 = d128Vec3(p3.x * cgiDouble128NS::cos(10.0 * rad45) + p3.z * cgiDouble128NS::sin(10.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(10.0 * rad45)) + p3.z * cgiDouble128NS::cos(10.0 * rad45));
		d128Vec3 p9 = d128Vec3(p3.x * cgiDouble128NS::cos(12.0 * rad45) + p3.z * cgiDouble128NS::sin(12.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(12.0 * rad45)) + p3.z * cgiDouble128NS::cos(12.0 * rad45));
		d128Vec3 p10 = d128Vec3(p3.x * cgiDouble128NS::cos(14.0 * rad45) + p3.z * cgiDouble128NS::sin(14.0 * rad45), p3.y, (p3.x * -cgiDouble128NS::sin(14.0 * rad45)) + p3.z * cgiDouble128NS::cos(14.0 * rad45));
		
		verts.push_back(p3);
		verts.push_back(p4);
		verts.push_back(p5);
		verts.push_back(p6);
		verts.push_back(p7);
		verts.push_back(p8);
		verts.push_back(p9);
		verts.push_back(p10);

		p1 = p2;
		p2 = p3;
	}

	createSouth();

	//Create vertices at the equator
	d128Vec3 p3 = d128Vec3(cgiDouble128NS::sin(2.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(2.0 * rad45));
	d128Vec3 p4 = d128Vec3(cgiDouble128NS::sin(3.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(3.0 * rad45));
	d128Vec3 p5 = d128Vec3(cgiDouble128NS::sin(4.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(4.0 * rad45));
	d128Vec3 p6 = d128Vec3(cgiDouble128NS::sin(5.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(5.0 * rad45));
	d128Vec3 p7 = d128Vec3(cgiDouble128NS::sin(6.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(6.0 * rad45));
	d128Vec3 p8 = d128Vec3(cgiDouble128NS::sin(7.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(7.0 * rad45));
	d128Vec3 p9 = d128Vec3(cgiDouble128NS::sin(8.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(8.0 * rad45));
	d128Vec3 p10 = d128Vec3(cgiDouble128NS::sin(9.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(9.0 * rad45));
	d128Vec3 p11 = d128Vec3(cgiDouble128NS::sin(10.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(10.0 * rad45));
	d128Vec3 p12 = d128Vec3(cgiDouble128NS::sin(11.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(11.0 * rad45));
	d128Vec3 p13 = d128Vec3(cgiDouble128NS::sin(12.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(12.0 * rad45));
	d128Vec3 p14 = d128Vec3(cgiDouble128NS::sin(13.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(13.0 * rad45));
	d128Vec3 p15 = d128Vec3(cgiDouble128NS::sin(14.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(14.0 * rad45));
	d128Vec3 p16 = d128Vec3(cgiDouble128NS::sin(15.0 * rad45), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(15.0 * rad45));


	verts.push_back(p3);
	verts.push_back(p4);
	verts.push_back(p5);
	verts.push_back(p6);
	verts.push_back(p7);
	verts.push_back(p8);
	verts.push_back(p9);
	verts.push_back(p10);
	verts.push_back(p11);
	verts.push_back(p12);
	verts.push_back(p13);
	verts.push_back(p14);
	verts.push_back(p15);
	verts.push_back(p16);

	fillRenderable(r);

	outputCSV();
}


EqualAreaTriangleSubdivision::~EqualAreaTriangleSubdivision() {
}

void EqualAreaTriangleSubdivision::print(cgiDouble128NS::double128 p) {
	char s[128];
	p.str(s);
	std::cout << "p: " << std::endl;
	std::cout << s << std::endl;
}

void EqualAreaTriangleSubdivision::outputCSV() {
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

void EqualAreaTriangleSubdivision::outputOBJ() {
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

		myfile << "v "<< x << " " << y << " " << z << "\n";
	}
	myfile << "s off\n";
	for (int i = 0; i < faces.size(); i+=3) {
		myfile << "f " << faces[i] << " " << faces[i + 1] << " " << faces[i + 2] << "\n";
	}
}

void EqualAreaTriangleSubdivision::fillRenderable(Renderable & r) {
	for (unsigned int i = 0; i < verts.size(); i++) {
		r.verts.push_back(glm::vec3(double(verts[i].x), double(verts[i].y), double(verts[i].z)));
		if (i != 31) {
			r.normals.push_back(glm::vec3(0.f, 0.f, 0.f));
		} else {
			r.normals.push_back(glm::vec3(1.f, 0.f, 0.f));
		}
	}
}

cgiDouble128NS::double128 EqualAreaTriangleSubdivision::sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3) {
	cgiDouble128NS::double128 a = cgiDouble128NS::acos(p1.dot(p2));
	cgiDouble128NS::double128 b = cgiDouble128NS::acos(p1.dot(p3));
	cgiDouble128NS::double128 c = cgiDouble128NS::acos(p2.dot(p3));
	cgiDouble128NS::double128 s = (a + b + c) / 2.0;
	return 4.0 * cgiDouble128NS::atan(cgiDouble128NS::sqrt(cgiDouble128NS::tan(s / 2.0) * cgiDouble128NS::tan((s - a) / 2.0) * cgiDouble128NS::tan((s - b) / 2.0) * cgiDouble128NS::tan((s - c) / 2.0)));
}

d128Vec3 EqualAreaTriangleSubdivision::findSlerpVector(cgiDouble128NS::double128 longitude) {
	return d128Vec3(cgiDouble128NS::sin(longitude) * sinLatitude, cosLatitude, cgiDouble128NS::cos(longitude) * sinLatitude);
}

cgiDouble128NS::double128 EqualAreaTriangleSubdivision::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper) {
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

void EqualAreaTriangleSubdivision::createSouth() {
	cgiDouble128NS::double128 rad45 = pi / cgiDouble128NS::double128(8.0);
	cgiDouble128NS::double128 sin36 = cgiDouble128NS::sin(rad45);
	cgiDouble128NS::double128 cos36 = cgiDouble128NS::cos(rad45);

	unsigned int j = verts.size();
	for (unsigned int i = 0; i < j; i++) {
		verts.push_back(d128Vec3(verts[i].x * cos36 + verts[i].z * sin36, -verts[i].y, (verts[i].x * -sin36) + verts[i].z * cos36));
	}
}
