#include "EqualAreaOctahedron.h"



EqualAreaOctahedron::EqualAreaOctahedron(Renderable & r, cgiDouble128NS::double128 numFaces) : numFaces(numFaces) {
	numIterations = 200;

	d128Vec3 northPole = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), cgiDouble128NS::double128(0.0));

	verts.push_back(northPole);

	pi = cgiDouble128NS::double128("3.1415926535897932384626433832795028841971693993751058");
	targetArea = cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / numFaces;
	//Should be 1
	sinLatitude = cgiDouble128NS::double128(1.0);
	//Should be 0
	cosLatitude = cgiDouble128NS::cos(pi / cgiDouble128NS::double128(2.0));

	//90 degrees converted to radians (splits a circle into 4 parts)
	cgiDouble128NS::double128 rad90 = pi / cgiDouble128NS::double128(2.0);
	cgiDouble128NS::double128 rad45 = pi / cgiDouble128NS::double128(4.0);

	d128Vec3 p1 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0));
	d128Vec3 p2 = d128Vec3(cgiDouble128NS::sin(rad90), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(rad90));

	verts.push_back(p1);
	verts.push_back(p2);

	p1 = findSlerpVector(cgiDouble128NS::double128(0.0));
	p2 = findSlerpVector(rad90);
	d128Vec3 p3 = findSlerpVector(rad45);

	//Trisect each face of the octahedron
	cgiDouble128NS::double128 t = slerpBinarySearch(northPole, p3, northPole, p1, 0.0, 1.0, cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / 24.0);

	d128Vec3 p4 = northPole.slerp(p3, t);
	d128Vec3 pa = p4;
	d128Vec3 pc = p4;
	d128Vec3 p5 = d128Vec3(p4.x * cgiDouble128NS::cos(2.0 * rad45) + p4.z * cgiDouble128NS::sin(2.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(2.0 * rad45)) + p4.z * cgiDouble128NS::cos(2.0 * rad45));
	d128Vec3 p6 = d128Vec3(p4.x * cgiDouble128NS::cos(4.0 * rad45) + p4.z * cgiDouble128NS::sin(4.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(4.0 * rad45)) + p4.z * cgiDouble128NS::cos(4.0 * rad45));
	d128Vec3 p7 = d128Vec3(p4.x * cgiDouble128NS::cos(6.0 * rad45) + p4.z * cgiDouble128NS::sin(6.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(6.0 * rad45)) + p4.z * cgiDouble128NS::cos(6.0 * rad45));

	verts.push_back(p4);
	verts.push_back(p5);
	verts.push_back(p6);
	verts.push_back(p7);

	//Bisect each created face
	t = slerpBinarySearch(northPole, p1, northPole, p4, 0.0, 1.0, cgiDouble128NS::double128(cgiDouble128NS::double128(4.0) * pi) / 48.0);

	print(t);

	p4 = northPole.slerp(p1, t);
	p5 = d128Vec3(p4.x * cgiDouble128NS::cos(2.0 * rad45) + p4.z * cgiDouble128NS::sin(2.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(2.0 * rad45)) + p4.z * cgiDouble128NS::cos(2.0 * rad45));
	p6 = d128Vec3(p4.x * cgiDouble128NS::cos(4.0 * rad45) + p4.z * cgiDouble128NS::sin(4.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(4.0 * rad45)) + p4.z * cgiDouble128NS::cos(4.0 * rad45));
	p7 = d128Vec3(p4.x * cgiDouble128NS::cos(6.0 * rad45) + p4.z * cgiDouble128NS::sin(6.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(6.0 * rad45)) + p4.z * cgiDouble128NS::cos(6.0 * rad45));
	
	verts.push_back(p4);
	verts.push_back(p5);
	verts.push_back(p6);
	verts.push_back(p7);

	//Divide each created face into 5
	d128Vec3 pb = p4;
	d128Vec3 pd = p4;

	for (unsigned int i = 0; i < 5; i++) {
		t = slerpBinarySearch(northPole, pa, pa, pb, 0.0, 1.0);

		print(t);

		p4 = northPole.slerp(pa, t);
		p5 = d128Vec3(p4.x * cgiDouble128NS::cos(2.0 * rad45) + p4.z * cgiDouble128NS::sin(2.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(2.0 * rad45)) + p4.z * cgiDouble128NS::cos(2.0 * rad45));
		p6 = d128Vec3(p4.x * cgiDouble128NS::cos(4.0 * rad45) + p4.z * cgiDouble128NS::sin(4.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(4.0 * rad45)) + p4.z * cgiDouble128NS::cos(4.0 * rad45));
		p7 = d128Vec3(p4.x * cgiDouble128NS::cos(6.0 * rad45) + p4.z * cgiDouble128NS::sin(6.0 * rad45), p4.y, (p4.x * -cgiDouble128NS::sin(6.0 * rad45)) + p4.z * cgiDouble128NS::cos(6.0 * rad45));

		d128Vec3 p8 = d128Vec3(cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0)).slerp(pc, t);
		d128Vec3 p9 = d128Vec3(p8.x * cgiDouble128NS::cos(2.0 * rad45) + p8.z * cgiDouble128NS::sin(2.0 * rad45), p8.y, (p8.x * -cgiDouble128NS::sin(2.0 * rad45)) + p8.z * cgiDouble128NS::cos(2.0 * rad45));
		d128Vec3 p10 = d128Vec3(p8.x * cgiDouble128NS::cos(4.0 * rad45) + p8.z * cgiDouble128NS::sin(4.0 * rad45), p8.y, (p8.x * -cgiDouble128NS::sin(4.0 * rad45)) + p8.z * cgiDouble128NS::cos(4.0 * rad45));
		d128Vec3 p11 = d128Vec3(p8.x * cgiDouble128NS::cos(6.0 * rad45) + p8.z * cgiDouble128NS::sin(6.0 * rad45), p8.y, (p8.x * -cgiDouble128NS::sin(6.0 * rad45)) + p8.z * cgiDouble128NS::cos(6.0 * rad45));

		verts.push_back(p4);
		verts.push_back(p5);
		verts.push_back(p6);
		verts.push_back(p7);

		verts.push_back(p8);
		verts.push_back(p9);
		verts.push_back(p10);
		verts.push_back(p11);
		
		pa = pb;
		pb = p4;

		pc = pd;
		pd = p8;
	}

	createSouth();

	d128Vec3 p8 = d128Vec3(cgiDouble128NS::sin(2.0 * rad90), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(2.0 * rad90));
	d128Vec3 p9 = d128Vec3(cgiDouble128NS::sin(3.0 * rad90), cgiDouble128NS::double128(0.0), cgiDouble128NS::cos(3.0 * rad90));

	verts.push_back(p8);
	verts.push_back(p9);

	fillRenderable(r);

	outputCSV();

}


EqualAreaOctahedron::~EqualAreaOctahedron() {
}

void EqualAreaOctahedron::print(cgiDouble128NS::double128 p) {
	char s[128];
	p.str(s);
	std::cout << "p: " << std::endl;
	std::cout << s << std::endl;
}

void EqualAreaOctahedron::outputCSV() {
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

void EqualAreaOctahedron::outputOBJ() {
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

void EqualAreaOctahedron::fillRenderable(Renderable & r) {
	for (unsigned int i = 0; i < verts.size(); i++) {
		r.verts.push_back(glm::vec3(double(verts[i].x), double(verts[i].y), double(verts[i].z)));
		if (i != 31) {
			r.normals.push_back(glm::vec3(0.f, 0.f, 0.f));
		} else {
			r.normals.push_back(glm::vec3(1.f, 0.f, 0.f));
		}
	}
}

cgiDouble128NS::double128 EqualAreaOctahedron::sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3) {
	cgiDouble128NS::double128 a = cgiDouble128NS::acos(p1.dot(p2));
	cgiDouble128NS::double128 b = cgiDouble128NS::acos(p1.dot(p3));
	cgiDouble128NS::double128 c = cgiDouble128NS::acos(p2.dot(p3));
	cgiDouble128NS::double128 s = (a + b + c) / 2.0;
	return 4.0 * cgiDouble128NS::atan(cgiDouble128NS::sqrt(cgiDouble128NS::tan(s / 2.0) * cgiDouble128NS::tan((s - a) / 2.0) * cgiDouble128NS::tan((s - b) / 2.0) * cgiDouble128NS::tan((s - c) / 2.0)));
}

d128Vec3 EqualAreaOctahedron::findSlerpVector(cgiDouble128NS::double128 longitude) {
	return d128Vec3(cgiDouble128NS::sin(longitude) * sinLatitude, cosLatitude, cgiDouble128NS::cos(longitude) * sinLatitude);
}

cgiDouble128NS::double128 EqualAreaOctahedron::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper) {
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

cgiDouble128NS::double128 EqualAreaOctahedron::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper, cgiDouble128NS::double128 targetArea) {
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

void EqualAreaOctahedron::createSouth() {
	//cgiDouble128NS::double128 rad45 = pi / cgiDouble128NS::double128(8.0);
	//cgiDouble128NS::double128 sin36 = cgiDouble128NS::sin(rad45);
	//cgiDouble128NS::double128 cos36 = cgiDouble128NS::cos(rad45);

	unsigned int j = verts.size();
	for (unsigned int i = 0; i < j; i++) {
		verts.push_back(d128Vec3(verts[i].x, -verts[i].y, verts[i].z));
	}
}
