#include "EqualAreaTetrahedron.h"

#include <algorithm>
#include <deque>

EqualAreaTetrahedron::EqualAreaTetrahedron(Renderable & r, unsigned int subdivisionLevel) {
	numTries = 0;
	maxNumTries = 0;
	this->subdivisionLevel = subdivisionLevel;
	numIterations = 200;

	pi = cgiDouble128NS::double128("3.1415926535897932384626433832795028841971693993751058");
	targetArea = (cgiDouble128NS::double128(4.0) * pi) / cgiDouble128NS::double128(120.0);

	std::vector<glm::u32vec3> newFaces;

	//Icosahedron

	//North Pole (z axis is up)
	//Index 0
	verts.push_back(d128Vec3(cgiDouble128NS::double128(0.0), 
							 cgiDouble128NS::double128(0.0), 
							 cgiDouble128NS::double128(1.0)));

	//Northern Hemisphere verts
	//0 = 0 degrees longitude. atan(1/2) ~ 26.57 degrees latitude. phi = pi/2 - atan(1/2).
	//Index 1
	verts.push_back(d128Vec3(cgiDouble128NS::double128(2.0) / cgiDouble128NS::sqrt(5.0), 
							 cgiDouble128NS::double128(0.0), 
							 cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//2pi/5 = 72 degrees longitude. atan(1/2) ~ 26.57 degrees latitude. phi = pi/2 - atan(1/2).
	//Index 2
	verts.push_back(d128Vec3((cgiDouble128NS::double128(5.0) - cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::sqrt( (cgiDouble128NS::double128(5.0) + cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//4pi/5 = 144 degrees longitude. atan(1/2) ~ 26.57 degrees latitude. phi = pi/2 - atan(1/2).
	//Index 3
	verts.push_back(d128Vec3((cgiDouble128NS::double128(-5.0) - cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) - cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//6pi/5 = 216 degrees longitude. atan(1/2) ~ 26.57 degrees latitude. phi = pi/2 - atan(1/2).
	//Index 4
	verts.push_back(d128Vec3((cgiDouble128NS::double128(-5.0) - cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) - cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//8pi/5 = 288 degrees longitude. atan(1/2) ~ 26.57 degrees latitude. phi = pi/2 - atan(1/2).
	//Index 5
	verts.push_back(d128Vec3((cgiDouble128NS::double128(5.0) - cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) + cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//Southern Hemisphere verts
	//pi/5 = 36 degrees longitude. -atan(1/2) ~ -26.57 degrees latitude. phi = pi/2 + atan(1/2).
	//Index 6
	verts.push_back(d128Vec3((cgiDouble128NS::double128(5.0) + cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) - cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//3pi/5 = 108 degrees longitude. atan(1/2) ~ -26.57 degrees latitude. phi = pi/2 + atan(1/2).
	//Index 7
	verts.push_back(d128Vec3((cgiDouble128NS::sqrt(5.0) - cgiDouble128NS::double128(5.0)) / (cgiDouble128NS::double128(10.0)),
							  cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) + cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//pi = 180 degrees longitude. atan(1/2) ~ -26.57 degrees latitude. phi = pi/2 + atan(1/2).
	//Index 8
	verts.push_back(d128Vec3(-(cgiDouble128NS::double128(2.0) * cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(5.0)),
							  cgiDouble128NS::double128(0.0),
							  -cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//7pi/5 = 252 degrees longitude. atan(1/2) ~ -26.57 degrees latitude. phi = pi/2 + atan(1/2).
	//Index 9
	verts.push_back(d128Vec3((cgiDouble128NS::sqrt(5.0) - cgiDouble128NS::double128(5.0)) / (cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) + cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//9pi/5 = 324 degrees longitude. atan(1/2) ~ -26.57 degrees latitude. phi = pi/2 + atan(1/2).
	//Index 10
	verts.push_back(d128Vec3((cgiDouble128NS::double128(5.0) + cgiDouble128NS::sqrt(5.0)) / (cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::sqrt((cgiDouble128NS::double128(5.0) - cgiDouble128NS::sqrt(5.0)) / cgiDouble128NS::double128(10.0)),
							  -cgiDouble128NS::double128(1.0) / cgiDouble128NS::sqrt(5.0)));

	//South Pole (z axis is up)
	//Index 11
	verts.push_back(d128Vec3(cgiDouble128NS::double128(0.0),
							 cgiDouble128NS::double128(0.0),
							 cgiDouble128NS::double128(-1.0)));

	//North Hemisphere 5 faces
	faces.push_back(glm::u32vec3(0, 1, 2));
	faces.push_back(glm::u32vec3(0, 2, 3));
	faces.push_back(glm::u32vec3(0, 3, 4));
	faces.push_back(glm::u32vec3(0, 4, 5));
	faces.push_back(glm::u32vec3(0, 5, 1));

	//Equator 10 faces
	faces.push_back(glm::u32vec3(1, 6, 2));
	faces.push_back(glm::u32vec3(2, 7, 3));
	faces.push_back(glm::u32vec3(3, 8, 4));
	faces.push_back(glm::u32vec3(4, 9, 5));
	faces.push_back(glm::u32vec3(5, 10, 1));
	faces.push_back(glm::u32vec3(1, 10, 6));
	faces.push_back(glm::u32vec3(2, 6, 7));
	faces.push_back(glm::u32vec3(3, 7, 8));
	faces.push_back(glm::u32vec3(4, 8, 9));
	faces.push_back(glm::u32vec3(5, 9, 10));

	//South Hemisphere 5 faces
	faces.push_back(glm::u32vec3(11, 6, 10));
	faces.push_back(glm::u32vec3(11, 7, 6));
	faces.push_back(glm::u32vec3(11, 8, 7));
	faces.push_back(glm::u32vec3(11, 9, 8));
	faces.push_back(glm::u32vec3(11, 10, 9));
	
	cgiDouble128NS::double128 t = slerpBinarySearch(verts[1].slerp(verts[2], cgiDouble128NS::double128(0.5)), verts[0], verts[1].slerp(verts[2], cgiDouble128NS::double128(0.5)), verts[1], cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), targetArea);

	for (unsigned int i = 0; i < faces.size(); i++) {
		//p1 is in the center of the face
		d128Vec3 p1 = verts[faces[i].x].slerp(verts[faces[i].y], cgiDouble128NS::double128(0.5)).slerp(verts[faces[i].z], t);
		unsigned int p1Index = verts.size();
		verts.push_back(p1);
		//p2 is between x and y
		d128Vec3 p2 = verts[faces[i].x].slerp(verts[faces[i].y], cgiDouble128NS::double128(0.5));
		unsigned int p2Index = verts.size();
		for (unsigned int j = 0; j < verts.size(); j++) {
			if (cgiDouble128NS::fabs(verts[j].x - p2.x) <= cgiDouble128NS::quadEpsilon() && cgiDouble128NS::fabs(verts[j].y - p2.y) <= cgiDouble128NS::quadEpsilon() && cgiDouble128NS::fabs(verts[j].z - p2.z) <= cgiDouble128NS::quadEpsilon()) {
				p2Index = j;
				break;
			}
		}
		if (p2Index == verts.size()) {
			verts.push_back(p2);
		}
		//p3 is between x and z
		d128Vec3 p3 = verts[faces[i].x].slerp(verts[faces[i].z], cgiDouble128NS::double128(0.5));
		unsigned int p3Index = verts.size();
		for (unsigned int j = 0; j < verts.size(); j++) {
			if (cgiDouble128NS::fabs(verts[j].x - p3.x) <= cgiDouble128NS::quadEpsilon() && cgiDouble128NS::fabs(verts[j].y - p3.y) <= cgiDouble128NS::quadEpsilon() && cgiDouble128NS::fabs(verts[j].z - p3.z) <= cgiDouble128NS::quadEpsilon()) {
				p3Index = j;
				break;
			}
		}
		if (p3Index == verts.size()) {
			verts.push_back(p3);
		}
		//p4 is between y and z
		d128Vec3 p4 = verts[faces[i].y].slerp(verts[faces[i].z], cgiDouble128NS::double128(0.5));
		unsigned int p4Index = verts.size();
		for (unsigned int j = 0; j < verts.size(); j++) {
			if (cgiDouble128NS::fabs(verts[j].x - p4.x) <= cgiDouble128NS::quadEpsilon() && cgiDouble128NS::fabs(verts[j].y - p4.y) <= cgiDouble128NS::quadEpsilon() && cgiDouble128NS::fabs(verts[j].z - p4.z) <= cgiDouble128NS::quadEpsilon()) {
				p4Index = j;
				break;
			}
		}
		if (p4Index == verts.size()) {
			verts.push_back(p4);
		}

		newFaces.push_back(glm::u32vec3(faces[i].x, p1Index, p2Index));
		newFaces.push_back(glm::u32vec3(faces[i].y, p1Index, p2Index));
		newFaces.push_back(glm::u32vec3(faces[i].y, p1Index, p4Index));
		newFaces.push_back(glm::u32vec3(faces[i].z, p1Index, p4Index));
		newFaces.push_back(glm::u32vec3(faces[i].z, p1Index, p3Index));
		newFaces.push_back(glm::u32vec3(faces[i].x, p1Index, p3Index));
	}

	faces.clear();
	for (unsigned int i = 0; i < newFaces.size(); i++) {
		faces.push_back(newFaces[i]);
	}

	newFaces.clear();

	//Reindex the faces
	newFaces.push_back(faces[1]);
	newFaces.push_back(faces[28]);
	newFaces.push_back(faces[0]);
	newFaces.push_back(faces[29]);
	newFaces.push_back(faces[25]);
	newFaces.push_back(faces[22]);
	newFaces.push_back(faces[24]);
	newFaces.push_back(faces[23]);
	newFaces.push_back(faces[19]);
	newFaces.push_back(faces[16]);

	newFaces.push_back(faces[18]);
	newFaces.push_back(faces[17]);
	newFaces.push_back(faces[13]);
	newFaces.push_back(faces[10]);
	newFaces.push_back(faces[12]);
	newFaces.push_back(faces[11]);
	newFaces.push_back(faces[7]);
	newFaces.push_back(faces[4]);
	newFaces.push_back(faces[6]);
	newFaces.push_back(faces[5]);

	newFaces.push_back(faces[35]);
	newFaces.push_back(faces[2]);
	newFaces.push_back(faces[34]);
	newFaces.push_back(faces[3]);
	newFaces.push_back(faces[59]);
	newFaces.push_back(faces[26]);
	newFaces.push_back(faces[58]);
	newFaces.push_back(faces[27]);
	newFaces.push_back(faces[53]);
	newFaces.push_back(faces[20]);

	newFaces.push_back(faces[52]);
	newFaces.push_back(faces[21]);
	newFaces.push_back(faces[47]);
	newFaces.push_back(faces[14]);
	newFaces.push_back(faces[46]);
	newFaces.push_back(faces[15]);
	newFaces.push_back(faces[41]);
	newFaces.push_back(faces[8]);
	newFaces.push_back(faces[40]);
	newFaces.push_back(faces[9]);

	newFaces.push_back(faces[31]);
	newFaces.push_back(faces[64]);
	newFaces.push_back(faces[30]);
	newFaces.push_back(faces[65]);
	newFaces.push_back(faces[61]);
	newFaces.push_back(faces[56]);
	newFaces.push_back(faces[60]);
	newFaces.push_back(faces[57]);
	newFaces.push_back(faces[55]);
	newFaces.push_back(faces[88]);

	newFaces.push_back(faces[54]);
	newFaces.push_back(faces[89]);
	newFaces.push_back(faces[85]);
	newFaces.push_back(faces[50]);
	newFaces.push_back(faces[84]);
	newFaces.push_back(faces[51]);
	newFaces.push_back(faces[49]);
	newFaces.push_back(faces[82]);
	newFaces.push_back(faces[48]);
	newFaces.push_back(faces[83]);

	newFaces.push_back(faces[79]);
	newFaces.push_back(faces[44]);
	newFaces.push_back(faces[78]);
	newFaces.push_back(faces[45]);
	newFaces.push_back(faces[43]);
	newFaces.push_back(faces[76]);
	newFaces.push_back(faces[42]);
	newFaces.push_back(faces[77]);
	newFaces.push_back(faces[73]);
	newFaces.push_back(faces[38]);

	newFaces.push_back(faces[72]);
	newFaces.push_back(faces[39]);
	newFaces.push_back(faces[37]);
	newFaces.push_back(faces[70]);
	newFaces.push_back(faces[36]);
	newFaces.push_back(faces[71]);
	newFaces.push_back(faces[67]);
	newFaces.push_back(faces[32]);
	newFaces.push_back(faces[66]);
	newFaces.push_back(faces[33]);

	newFaces.push_back(faces[63]);
	newFaces.push_back(faces[92]);
	newFaces.push_back(faces[62]);
	newFaces.push_back(faces[93]);
	newFaces.push_back(faces[87]);
	newFaces.push_back(faces[116]);
	newFaces.push_back(faces[86]);
	newFaces.push_back(faces[117]);
	newFaces.push_back(faces[81]);
	newFaces.push_back(faces[110]);

	newFaces.push_back(faces[80]);
	newFaces.push_back(faces[111]);
	newFaces.push_back(faces[75]);
	newFaces.push_back(faces[104]);
	newFaces.push_back(faces[74]);
	newFaces.push_back(faces[105]);
	newFaces.push_back(faces[69]);
	newFaces.push_back(faces[98]);
	newFaces.push_back(faces[68]);
	newFaces.push_back(faces[99]);

	newFaces.push_back(faces[91]);
	newFaces.push_back(faces[100]);
	newFaces.push_back(faces[90]);
	newFaces.push_back(faces[101]);
	newFaces.push_back(faces[115]);
	newFaces.push_back(faces[94]);
	newFaces.push_back(faces[114]);
	newFaces.push_back(faces[95]);
	newFaces.push_back(faces[109]);
	newFaces.push_back(faces[118]);

	newFaces.push_back(faces[108]);
	newFaces.push_back(faces[119]);
	newFaces.push_back(faces[103]);
	newFaces.push_back(faces[112]);
	newFaces.push_back(faces[102]);
	newFaces.push_back(faces[113]);
	newFaces.push_back(faces[97]);
	newFaces.push_back(faces[106]);
	newFaces.push_back(faces[96]);
	newFaces.push_back(faces[107]);

	faces.clear();
	for (unsigned int i = 0; i < newFaces.size(); i++) {
		faces.push_back(newFaces[i]);
	}

	for (unsigned int i = 0; i < faces.size(); i += 4) {
		vertsRhombicTriacontahedron.push_back(verts[faces[i].y]);
		vertsRhombicTriacontahedron.push_back(verts[faces[i].x]);
		vertsRhombicTriacontahedron.push_back(verts[faces[i + 3].x]);
		vertsRhombicTriacontahedron.push_back(verts[faces[i + 3].y]);
	}

	//Longest Edge Splitting
	for (unsigned int i = 0; i < subdivisionLevel; i++) {
		newFaces.clear();
		newFaces.resize(30 * (int)std::pow(4, i + 2));
		targetArea /= cgiDouble128NS::double128(2.0);

		for (unsigned int j = 0; j < faces.size(); j++) {
			//p0, p1 make up the longest edge
			d128Vec3 p0 = verts[faces[j].x];
			unsigned int p0Index = faces[j].x;

			d128Vec3 p1 = verts[faces[j].y];
			unsigned int p1Index = faces[j].y;

			//p2 is the remaining vertex in the triangle
			d128Vec3 p2 = verts[faces[j].z];
			unsigned int p2Index = faces[j].z;

			cgiDouble128NS::double128 t = slerpBinarySearch(p0, p1, p0, p2, cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), targetArea);

			d128Vec3 p3 = p0.slerp(p1, t);
			unsigned int p3Index = verts.size();
			for (unsigned int k = 0; k < verts.size(); k++) {
				if ((verts[k] - p3).length() <= 1024.0 * cgiDouble128NS::quadEpsilon()) {
					p3Index = k;
					break;
				}
			}
			if (p3Index == verts.size()) {
				verts.push_back(p3);
			}

			t = slerpBinarySearch(p1, p2, p1, p3, cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), targetArea / 2.0);

			d128Vec3 p4 = p1.slerp(p2, t);
			unsigned int p4Index = verts.size();
			for (unsigned int k = 0; k < verts.size(); k++) {
				if ((verts[k] - p4).length() <= 1024.0 * cgiDouble128NS::quadEpsilon()) {
					p4Index = k;
					break;
				}
			}
			if (p4Index == verts.size()) {
				verts.push_back(p4);
			}

			t = slerpBinarySearch(p0, p2, p0, p3, cgiDouble128NS::double128(0.0), cgiDouble128NS::double128(1.0), targetArea / 2.0);

			d128Vec3 p5 = p0.slerp(p2, t);
			unsigned int p5Index = verts.size();
			for (unsigned int k = 0; k < verts.size(); k++) {
				if ((verts[k] - p5).length() <= 1024.0 * cgiDouble128NS::quadEpsilon()) {
					p5Index = k;
					break;
				}
			}
			if (p5Index == verts.size()) {
				verts.push_back(p5);
			}

			unsigned int c1;
			unsigned int c2;
			unsigned int c3;
			unsigned int c4;
			children(j, &c1, &c2, &c3, &c4, i + 1);

			newFaces[c1] = glm::u32vec3(p3Index, p1Index, p4Index);
			newFaces[c2] = glm::u32vec3(p3Index, p2Index, p4Index);
			newFaces[c3] = glm::u32vec3(p2Index, p3Index, p5Index);
			newFaces[c4] = glm::u32vec3(p0Index, p3Index, p5Index);
		}

		targetArea /= cgiDouble128NS::double128(2.0);

		faces.clear();
		for (unsigned int i = 0; i < newFaces.size(); i++) {
			faces.push_back(newFaces[i]);
		}

	}

	//Vertex Merge
	for (unsigned int i = 0; i < verts.size(); i++) {
		for (unsigned int j = i + 1; j < verts.size(); j++) {
			if ((verts[j] - verts[i]).length() < cgiDouble128NS::double128(0.002)) {
				d128Vec3 temp = verts[i].slerp(verts[j], 0.5);
				verts[i].x = temp.x;
				verts[i].y = temp.y;
				verts[i].z = temp.z;
				for (unsigned int k = 0; k < faces.size(); k++) {
					if (faces[k].x == j) {
						faces[k].x = i;
					} else if (faces[k].x > j) {
						faces[k].x--;
					}

					if (faces[k].y == j) {
						faces[k].y = i;
					} else if (faces[k].y > j) {
						faces[k].y--;
					}

					if (faces[k].z == j) {
						faces[k].z = i;
					} else if (faces[k].z > j) {
						faces[k].z--;
					}
				}
				verts.erase(verts.begin() + j);
			}
		}
	}

	for (unsigned int i = 0; i < verts.size(); i++) {
		flatVerts.push_back(verts[i]);
	}

	for (unsigned int i = 0; i < faces.size(); i++) {
		flatFaces.push_back(faces[i]);
	}


	//Perform one level of 4:1 flat subdivision
	planarSubdivisionLevel = 3;

	for (unsigned int i = 0; i < planarSubdivisionLevel; i++) {
		targetArea /= cgiDouble128NS::double128(4.0);
		newFaces.clear();
		newFaces.resize(30 * (int)std::pow(4, subdivisionLevel + i + 2));
		std::vector<unsigned int> check;
		for (unsigned int j = 0; j < flatFaces.size(); j++) {
			//p0, p1 make up the longest edge
			d128Vec3 p0 = flatVerts[flatFaces[j].x];
			unsigned int p0Index = flatFaces[j].x;

			d128Vec3 p1 = flatVerts[flatFaces[j].y];
			unsigned int p1Index = flatFaces[j].y;

			//p2 is the remaining vertex in the triangle
			d128Vec3 p2 = flatVerts[flatFaces[j].z];
			unsigned int p2Index = flatFaces[j].z;

			d128Vec3 p3 = (p0 * 0.5) + (p1 * 0.5);
			unsigned int p3Index = flatVerts.size();
			for (unsigned int k = 0; k < check.size(); k++) {
				if ((flatVerts[check[k]] - p3).length() <= 64.0 * cgiDouble128NS::quadEpsilon()) {
					p3Index = check[k];
					check.erase(check.begin() + k);
					break;
				}
			}
			if (p3Index == flatVerts.size()) {
				check.push_back(p3Index);
				flatVerts.push_back(p3);
			}

			d128Vec3 p4 = (p1 * 0.5) + (p2 * 0.5);
			unsigned int p4Index = flatVerts.size();
			for (unsigned int k = 0; k < check.size(); k++) {
				if ((flatVerts[check[k]] - p4).length() <= 64.0 * cgiDouble128NS::quadEpsilon()) {
					p4Index = check[k];
					check.erase(check.begin() + k);
					break;
				}
			}
			if (p4Index == flatVerts.size()) {
				check.push_back(p4Index);
				flatVerts.push_back(p4);
			}

			d128Vec3 p5 = (p0 * 0.5) + (p2 * 0.5);
			unsigned int p5Index = flatVerts.size();
			for (unsigned int k = 0; k < check.size(); k++) {
				if ((flatVerts[check[k]] - p5).length() <= 64.0 * cgiDouble128NS::quadEpsilon()) {
					p5Index = check[k];
					check.erase(check.begin() + k);
					break;
				}
			}
			if (p5Index == flatVerts.size()) {
				check.push_back(p5Index);
				flatVerts.push_back(p5);
			}

			unsigned int c1;
			unsigned int c2;
			unsigned int c3;
			unsigned int c4;
			children(j, &c1, &c2, &c3, &c4, subdivisionLevel + i + 1);

			newFaces[c1] = glm::u32vec3(p3Index, p1Index, p4Index);
			newFaces[c2] = glm::u32vec3(p3Index, p2Index, p4Index);
			newFaces[c3] = glm::u32vec3(p2Index, p3Index, p5Index);
			newFaces[c4] = glm::u32vec3(p0Index, p3Index, p5Index);

		}

		flatFaces.clear();
		for (unsigned int i = 0; i < newFaces.size(); i++) {
			flatFaces.push_back(newFaces[i]);
		}
	}

	//outputArea();

	//outputAreaDifference();

	//outputCompactness();

	fillRenderable(r, true);

	outputOBJ(true);
}

EqualAreaTetrahedron::~EqualAreaTetrahedron() {
}

void EqualAreaTetrahedron::print(cgiDouble128NS::double128 p) {
	char s[128];
	p.str(s);
	std::cout << "p: " << std::endl;
	std::cout << s << std::endl;
}

void EqualAreaTetrahedron::outputArea() {
	for (unsigned int i = 0; i < faces.size(); i++) {
		std::cout << i << std::endl;
		print(sphericalTriangleArea(verts[faces[i].x], verts[faces[i].y], verts[faces[i].z]));
	}
}

void EqualAreaTetrahedron::outputAreaDifference() {
	cgiDouble128NS::double128 expectedArea = targetArea;
	std::ofstream myfile;
	myfile.open("areaDifference.csv");
	for (int i = 0; i < faces.size(); i++) {
		
		cgiDouble128NS::double128 areaDiff = cgiDouble128NS::fabs(planarTriangleArea(verts[faces[i].x], verts[faces[i].y], verts[faces[i].z]) - expectedArea);
		cgiDouble128NS::double128 areaOnGlobe = (areaDiff / (4.0 * pi)) * cgiDouble128NS::double128(510000000000000.0);

		char a[128];
		areaOnGlobe.str(a);

		myfile << a << "\n";
	}
	myfile.close();
}

void EqualAreaTetrahedron::outputCompactness() {
	std::ofstream myfile;
	myfile.open("compactness.csv");
	for (int i = 0; i < faces.size(); i++) {

		cgiDouble128NS::double128 area = (planarTriangleArea(verts[faces[i].x], verts[faces[i].y], verts[faces[i].z]) / (4.0 * pi)) * cgiDouble128NS::double128(510000000000000.0);
		cgiDouble128NS::double128 radius = cgiDouble128NS::double128(6371000.0);

		cgiDouble128NS::double128 perimeter = (verts[faces[i].x] * radius - verts[faces[i].y] * radius).length() + (verts[faces[i].x] * radius - verts[faces[i].z] * radius).length() + (verts[faces[i].y] * radius - verts[faces[i].z] * radius).length();
		
		cgiDouble128NS::double128 compactness = cgiDouble128NS::sqrt(cgiDouble128NS::double128(4.0) * pi * area - ((area * area) / (radius * radius))) / perimeter;

		char a[128];
		compactness.str(a);

		myfile << a << "\n";
	}
	myfile.close();
}

void EqualAreaTetrahedron::outputCSV() {
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

void EqualAreaTetrahedron::outputOBJ(bool mode) {
	std::ofstream myfile;
	myfile.open("sphere.obj");
	if (mode) {
		for (int i = 0; i < flatVerts.size(); i++) {
			char x[128];
			flatVerts[i].x.str(x);

			char y[128];
			flatVerts[i].y.str(y);

			char z[128];
			flatVerts[i].z.str(z);

			myfile << "v " << x << " " << y << " " << z << "\n";
		}
		myfile << "s off" << "\n";
		for (int i = 0; i < flatFaces.size(); i++) {
			myfile << "f " << flatFaces[i].x + 1 << " " << flatFaces[i].y + 1 << " " << flatFaces[i].z + 1 << "\n";
		}
	} else {
		
		for (int i = 0; i < verts.size(); i++) {
			char x[128];
			verts[i].x.str(x);

			char y[128];
			verts[i].y.str(y);

			char z[128];
			verts[i].z.str(z);

			myfile << "v " << x << " " << y << " " << z << "\n";
		}
		myfile << "s off" << "\n";
		for (int i = 0; i < faces.size(); i++) {
			myfile << "f " << faces[i].x + 1 << " " << faces[i].y + 1 << " " << faces[i].z + 1 << "\n";
		}

	}
	myfile.close();
}

cgiDouble128NS::double128 EqualAreaTetrahedron::planarTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3) {
	d128Vec3 cp = (p1 - p2).cross(p1 - p3);
	return cgiDouble128NS::sqrt(cp.x * cp.x + cp.y * cp.y + cp.z * cp.z) / cgiDouble128NS::double128(2.0);
}

cgiDouble128NS::double128 EqualAreaTetrahedron::sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3) {
	cgiDouble128NS::double128 a = cgiDouble128NS::acos(p1.dot(p2));
	cgiDouble128NS::double128 b = cgiDouble128NS::acos(p1.dot(p3));
	cgiDouble128NS::double128 c = cgiDouble128NS::acos(p2.dot(p3));
	cgiDouble128NS::double128 s = (a + b + c) / cgiDouble128NS::double128(2.0);
	return cgiDouble128NS::double128(4.0) * cgiDouble128NS::atan(cgiDouble128NS::sqrt(cgiDouble128NS::tan(s / cgiDouble128NS::double128(2.0)) * cgiDouble128NS::tan((s - a) / cgiDouble128NS::double128(2.0)) * cgiDouble128NS::tan((s - b) / cgiDouble128NS::double128(2.0)) * cgiDouble128NS::tan((s - c) / cgiDouble128NS::double128(2.0))));
}

cgiDouble128NS::double128 EqualAreaTetrahedron::slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper, cgiDouble128NS::double128 targetArea) {
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

void EqualAreaTetrahedron::fillRenderable(Renderable & r, bool mode) {

	if (mode) {
		for (unsigned int i = 0; i < flatFaces.size(); i++) {

			r.verts.push_back(glm::vec3(double(flatVerts[flatFaces[i].x].x), double(flatVerts[flatFaces[i].x].y), double(flatVerts[flatFaces[i].x].z)));
			r.verts.push_back(glm::vec3(double(flatVerts[flatFaces[i].y].x), double(flatVerts[flatFaces[i].y].y), double(flatVerts[flatFaces[i].y].z)));
			r.verts.push_back(glm::vec3(double(flatVerts[flatFaces[i].z].x), double(flatVerts[flatFaces[i].z].y), double(flatVerts[flatFaces[i].z].z)));

			cgiDouble128NS::double128 theta1 = cgiDouble128NS::atan2(flatVerts[flatFaces[i].x].y, flatVerts[flatFaces[i].x].x);
			cgiDouble128NS::double128 u1 = (theta1 + M_PI) / (2.0 * M_PI);
			cgiDouble128NS::double128 phi1 = cgiDouble128NS::acos(flatVerts[flatFaces[i].x].z);
			cgiDouble128NS::double128 v1 = phi1 / M_PI;

			cgiDouble128NS::double128 theta2 = cgiDouble128NS::atan2(flatVerts[flatFaces[i].y].y, flatVerts[flatFaces[i].y].x);
			cgiDouble128NS::double128 u2 = (theta2 + M_PI) / (2.0 * M_PI);
			cgiDouble128NS::double128 phi2 = cgiDouble128NS::acos(flatVerts[flatFaces[i].y].z);
			cgiDouble128NS::double128 v2 = phi2 / M_PI;

			cgiDouble128NS::double128 theta3 = cgiDouble128NS::atan2(flatVerts[flatFaces[i].z].y, flatVerts[flatFaces[i].z].x);
			cgiDouble128NS::double128 u3 = (theta3 + M_PI) / (2.0 * M_PI);
			cgiDouble128NS::double128 phi3 = cgiDouble128NS::acos(flatVerts[flatFaces[i].z].z);
			cgiDouble128NS::double128 v3 = phi3 / M_PI;

			if (u1 > 0.9 && (u2 < 0.1 || u3 < 0.1)) {
				u1 -= 1.0;
			}
			if (u2 > 0.9 && (u1 < 0.1 || u3 < 0.1)) {
				u2 -= 1.0;
			}
			if (u3 > 0.9 && (u1 < 0.1 || u2 < 0.1)) {
				u3 -= 1.0;
			}

			r.uvs.push_back(glm::vec2((double)u1, 1.0 - (double)v1));
			r.uvs.push_back(glm::vec2((double)u2, 1.0 - (double)v2));
			r.uvs.push_back(glm::vec2((double)u3, 1.0 - (double)v3));

			cgiDouble128NS::double128 diff = planarTriangleArea(flatVerts[flatFaces[i].x], flatVerts[flatFaces[i].y], flatVerts[flatFaces[i].z]) - targetArea;
			cgiDouble128NS::double128 factor = cgiDouble128NS::double128(100.0) * cgiDouble128NS::fabs(diff) / targetArea;

		}
	} else {
		for (unsigned int i = 0; i < faces.size(); i++) {

			r.verts.push_back(glm::vec3(double(verts[faces[i].x].x), double(verts[faces[i].x].y), double(verts[faces[i].x].z)));
			r.verts.push_back(glm::vec3(double(verts[faces[i].y].x), double(verts[faces[i].y].y), double(verts[faces[i].y].z)));
			r.verts.push_back(glm::vec3(double(verts[faces[i].z].x), double(verts[faces[i].z].y), double(verts[faces[i].z].z)));

			cgiDouble128NS::double128 diff = planarTriangleArea(verts[faces[i].x], verts[faces[i].y], verts[faces[i].z]) - targetArea;
			cgiDouble128NS::double128 factor = cgiDouble128NS::double128(100.0) * cgiDouble128NS::fabs(diff) / targetArea;

			if (diff > 0) {
				r.normals.push_back(glm::vec3(double(factor), 0.0, 0.0));
				r.normals.push_back(glm::vec3(double(factor), 0.0, 0.0));
				r.normals.push_back(glm::vec3(double(factor), 0.0, 0.0));
			} else {
				if (i == 912) {
					r.normals.push_back(glm::vec3(0.0, 0.0, 1.0));
					r.normals.push_back(glm::vec3(0.0, 0.0, 1.0));
					r.normals.push_back(glm::vec3(0.0, 0.0, 1.0));
				} else {
					r.normals.push_back(glm::vec3(0.0, double(factor), 0.0));
					r.normals.push_back(glm::vec3(0.0, double(factor), 0.0));
					r.normals.push_back(glm::vec3(0.0, double(factor), 0.0));
				}
			}

		}
	}
}

bool EqualAreaTetrahedron::children(unsigned int cell, unsigned int * c1, unsigned int * c2, unsigned int * c3, unsigned int * c4, unsigned int resolution) {
	unsigned int cellNumber = cell / (unsigned int)std::pow(4, resolution);
	unsigned int cellRelativeToBaseMesh = cell % (int)std::pow(4, resolution);
	unsigned int rowNum = cellRelativeToBaseMesh / std::pow(2, resolution + 1);
	unsigned int colNum = (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1)) / 4;

	unsigned int topStart = rowNum * std::pow(2, resolution + 2) * 2 + (8 * colNum);
	unsigned int bottomStart = rowNum * std::pow(2, resolution + 2) * 2 + std::pow(2, resolution + 2) + (8 * colNum);

	if (cellRelativeToBaseMesh % 4 == 0) {
		*c1 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart;
		*c2 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 1;
		*c3 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 6;
		*c4 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 4;
	} else if (cellRelativeToBaseMesh % 4 == 1) {
		*c1 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 5;
		*c2 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 4;
		*c3 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 7;
		*c4 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 5;
	} else if (cellRelativeToBaseMesh % 4 == 2) {
		*c1 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 2;
		*c2 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 3;
		*c3 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart;
		*c4 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 2;
	} else if (cellRelativeToBaseMesh % 4 == 3) {
		*c1 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 7;
		*c2 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 6;
		*c3 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 1;
		*c4 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 3;
	}
	return true;
}

void EqualAreaTetrahedron::findNeighbor(unsigned int cell, int * n1, int * n2, int * n3) {
	for (int i = 0; i < faces.size(); i++) {
		bool shareVert1 = false;
		bool shareVert2 = false;
		bool shareVert3 = false;
		if (faces[cell].x == faces[i].x || faces[cell].x == faces[i].y || faces[cell].x == faces[i].z) {
			shareVert1 = true;
		}
		if (faces[cell].y == faces[i].x || faces[cell].y == faces[i].y || faces[cell].y == faces[i].z) {
			shareVert2 = true;
		}
		if (faces[cell].z == faces[i].x || faces[cell].z == faces[i].y || faces[cell].z == faces[i].z) {
			shareVert3 = true;
		}
		if (((shareVert1 && shareVert2) || (shareVert1 && shareVert3) || (shareVert2 && shareVert3)) && !(shareVert1 && shareVert2 && shareVert3)) {
			if (*n1 == -1) {
				*n1 = i;
			} else if (*n2 == -1) {
				*n2 = i;
			} else if (*n3 == -1) {
				*n3 = i;
			}
		}
		
	}
}

unsigned int EqualAreaTetrahedron::pointToCell(d128Vec2 latlon) {
	//Convert lat lon to theta, phi
	cgiDouble128NS::double128 phi = (pi / 2.0) - latlon.x;
	cgiDouble128NS::double128 theta = latlon.y;

	d128Vec3 xyz = d128Vec3(cgiDouble128NS::cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));

	unsigned int baseCell;

	//Divide longitude into 10 regions, binary searching which region we are in
	if (theta <= pi) {
		if (theta <= (2.0* pi) / 5.0) {
			if (theta <= pi / 5.0) {
				//theta between 0 and pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 5 + 1].cross(vertsRhombicTriacontahedron[4 * 5]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 0 or cell 5
					crossp1p2 = vertsRhombicTriacontahedron[4 * 5 + 1].cross(vertsRhombicTriacontahedron[4 * 5 + 3]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 0
						baseCell = 0;
					} else {
						//In cell 5
						baseCell = 5;
					}
				} else {
					//Point is in cells 10, 20 or 25
					crossp1p2 = vertsRhombicTriacontahedron[4 * 10 + 3].cross(vertsRhombicTriacontahedron[4 * 10 + 1]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 10
						baseCell = 10;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 20 + 3].cross(vertsRhombicTriacontahedron[4 * 20 + 1]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 20
							baseCell = 20;
						} else {
							//In cell 25
							baseCell = 25;
						}
					}
				}
			} else { 
				//theta between pi/5 and 2pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 5].cross(vertsRhombicTriacontahedron[4 * 5 + 2]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 4 or cell 5
					crossp1p2 = vertsRhombicTriacontahedron[4 * 5 + 3].cross(vertsRhombicTriacontahedron[4 * 5 + 2]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 4
						baseCell = 4;
					} else {
						//In cell 5
						baseCell = 5;
					}
				} else {
					//Point is in cells 19, 24 or 25
					crossp1p2 = vertsRhombicTriacontahedron[4 * 19 + 1].cross(vertsRhombicTriacontahedron[4 * 19]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 19
						baseCell = 19;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 24 + 2].cross(vertsRhombicTriacontahedron[4 * 24 + 3]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 24
							baseCell = 24;
						} else {
							//In cell 25
							baseCell = 25;
						}
					}
				}
			}
		} else {
			if (theta <= (3.0 * pi) / 5.0) {
				//theta between 2pi/5 and 3pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 9 + 1].cross(vertsRhombicTriacontahedron[4 * 9]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 4 or cell 9
					crossp1p2 = vertsRhombicTriacontahedron[4 * 9 + 1].cross(vertsRhombicTriacontahedron[4 * 9 + 3]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 4
						baseCell = 4;
					} else {
						//In cell 9
						baseCell = 9;
					}
				} else {
					//Point is in cells 18, 24 or 29
					crossp1p2 = vertsRhombicTriacontahedron[4 * 18 + 3].cross(vertsRhombicTriacontahedron[4 * 18 + 1]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 18
						baseCell = 18;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 24 + 3].cross(vertsRhombicTriacontahedron[4 * 24 + 1]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 24
							baseCell = 24;
						} else {
							//In cell 29
							baseCell = 29;
						}
					}
				}
			} else if (theta <= (4.0 * pi) / 5.0) {
				//theta between 3pi/5 and 4pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 9].cross(vertsRhombicTriacontahedron[4 * 9 + 2]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 3 or cell 9
					crossp1p2 = vertsRhombicTriacontahedron[4 * 9 + 3].cross(vertsRhombicTriacontahedron[4 * 9 + 2]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 3
						baseCell = 3;
					} else {
						//In cell 9
						baseCell = 9;
					}
				} else {
					//Point is in cells 17, 23 or 29
					crossp1p2 = vertsRhombicTriacontahedron[4 * 17 + 1].cross(vertsRhombicTriacontahedron[4 * 17]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 17
						baseCell = 17;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 23 + 2].cross(vertsRhombicTriacontahedron[4 * 23 + 3]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 23
							baseCell = 23;
						} else {
							//In cell 29
							baseCell = 29;
						}
					}
				}
			} else {
				//theta between 4pi/5 and pi
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 8 + 1].cross(vertsRhombicTriacontahedron[4 * 8]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 3 or cell 8
					crossp1p2 = vertsRhombicTriacontahedron[4 * 8 + 1].cross(vertsRhombicTriacontahedron[4 * 8 + 3]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 3
						baseCell = 3;
					} else {
						//In cell 8
						baseCell = 8;
					}
				} else {
					//Point is in cells 16, 23 or 28
					crossp1p2 = vertsRhombicTriacontahedron[4 * 16 + 3].cross(vertsRhombicTriacontahedron[4 * 16 + 1]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 16
						baseCell = 16;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 23 + 3].cross(vertsRhombicTriacontahedron[4 * 23 + 1]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 23
							baseCell = 23;
						} else {
							//In cell 28
							baseCell = 28;
						}
					}
				}
			}
		}
	} else { // theta greater than pi
		if (theta <= (7.0* pi) / 5.0) {
			if (theta <= (6.0* pi) / 5.0) {
				//theta between pi and 6pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 8].cross(vertsRhombicTriacontahedron[4 * 8 + 2]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 2 or cell 8
					crossp1p2 = vertsRhombicTriacontahedron[4 * 8 + 3].cross(vertsRhombicTriacontahedron[4 * 8 + 2]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 2
						baseCell = 2;
					} else {
						//In cell 8
						baseCell = 8;
					}
				} else {
					//Point is in cells 15, 22 or 28
					crossp1p2 = vertsRhombicTriacontahedron[4 * 15 + 1].cross(vertsRhombicTriacontahedron[4 * 15]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 15
						baseCell = 15;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 22 + 2].cross(vertsRhombicTriacontahedron[4 * 22 + 3]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 22
							baseCell = 22;
						} else {
							//In cell 28
							baseCell = 28;
						}
					}
				}
			} else {
				//theta between 6pi/5 and 7pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 7 + 1].cross(vertsRhombicTriacontahedron[4 * 7]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 2 or cell 7
					crossp1p2 = vertsRhombicTriacontahedron[4 * 7 + 1].cross(vertsRhombicTriacontahedron[4 * 7 + 3]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 2
						baseCell = 2;
					} else {
						//In cell 7
						baseCell = 7;
					}
				} else {
					//Point is in cells 14, 23 or 27
					crossp1p2 = vertsRhombicTriacontahedron[4 * 14 + 3].cross(vertsRhombicTriacontahedron[4 * 14 + 1]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 14
						baseCell = 14;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 22 + 3].cross(vertsRhombicTriacontahedron[4 * 22 + 1]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 22
							baseCell = 22;
						} else {
							//In cell 27
							baseCell = 27;
						}
					}
				}
			}
		} else {
			if (theta <= (8.0* pi) / 5.0) {
				//theta between 7pi/5 and 8pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 7].cross(vertsRhombicTriacontahedron[4 * 7 + 2]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 1 or cell 7
					crossp1p2 = vertsRhombicTriacontahedron[4 * 7 + 3].cross(vertsRhombicTriacontahedron[4 * 7 + 2]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 1
						baseCell = 1;
					} else {
						//In cell 7
						baseCell = 7;
					}
				} else {
					//Point is in cells 13, 21 or 27
					crossp1p2 = vertsRhombicTriacontahedron[4 * 13 + 1].cross(vertsRhombicTriacontahedron[4 * 13]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 13
						baseCell = 13;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 21 + 2].cross(vertsRhombicTriacontahedron[4 * 21 + 3]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 21
							baseCell = 21;
						} else {
							//In cell 27
							baseCell = 27;
						}
					}
				}
			} else if (theta <= (9.0* pi) / 5.0) {
				//theta between 8pi/5 and 9pi/5
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 6 + 1].cross(vertsRhombicTriacontahedron[4 * 6]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 1 or cell 6
					crossp1p2 = vertsRhombicTriacontahedron[4 * 6 + 1].cross(vertsRhombicTriacontahedron[4 * 6 + 3]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 1
						baseCell = 1;
					} else {
						//In cell 6
						baseCell = 6;
					}
				} else {
					//Point is in cells 12, 21 or 26
					crossp1p2 = vertsRhombicTriacontahedron[4 * 12 + 3].cross(vertsRhombicTriacontahedron[4 * 12 + 1]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 12
						baseCell = 12;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 21 + 3].cross(vertsRhombicTriacontahedron[4 * 21 + 1]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 21
							baseCell = 21;
						} else {
							//In cell 26
							baseCell = 26;
						}
					}
				}
			} else {
				//theta between 9pi/5 and 2pi
				d128Vec3 crossp1p2 = vertsRhombicTriacontahedron[4 * 6].cross(vertsRhombicTriacontahedron[4 * 6 + 2]);
				if (xyz.dot(crossp1p2) > 0.0) {
					//Point is in cell 0 or cell 6
					crossp1p2 = vertsRhombicTriacontahedron[4 * 6 + 3].cross(vertsRhombicTriacontahedron[4 * 6 + 2]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 0
						baseCell = 0;
					} else {
						//In cell 6
						baseCell = 6;
					}
				} else {
					//Point is in cells 11, 20 or 26
					crossp1p2 = vertsRhombicTriacontahedron[4 * 11 + 1].cross(vertsRhombicTriacontahedron[4 * 11]);
					if (xyz.dot(crossp1p2) > 0.0) {
						//In cell 11
						baseCell = 11;
					} else {
						crossp1p2 = vertsRhombicTriacontahedron[4 * 20 + 2].cross(vertsRhombicTriacontahedron[4 * 20 + 3]);
						if (xyz.dot(crossp1p2) > 0.0) {
							//In cell 20
							baseCell = 20;
						} else {
							//In cell 26
							baseCell = 26;
						}
					}
				}
			}
		}
	}

	//basep1 is between face 0 and 2
	d128Vec3 basep1 = vertsRhombicTriacontahedron[4 * baseCell];
	//basep2 is between face 0 and 1
	d128Vec3 basep2 = vertsRhombicTriacontahedron[4 * baseCell + 1];
	//basep3 is between face 2 and 3
	d128Vec3 basep3 = vertsRhombicTriacontahedron[4 * baseCell + 2];
	//basep4 is between face 1 and 3
	d128Vec3 basep4 = vertsRhombicTriacontahedron[4 * baseCell + 3];

	d128Vec3 plane1Normalv = basep2.cross(basep1);
	plane1Normalv.normalize();
	d128Vec3 plane2Normalv = basep4.cross(basep3);
	plane2Normalv.normalize();

	d128Vec3 intersectionPoint = (plane1Normalv.cross(plane2Normalv));
	intersectionPoint.normalize();

	d128Vec3 plane3Normalv = intersectionPoint.cross(xyz);
	plane3Normalv.normalize();

	cgiDouble128NS::double128 angleTotalv = cgiDouble128NS::acos(plane1Normalv.dot(plane2Normalv));
	cgiDouble128NS::double128 anglev = cgiDouble128NS::acos(plane1Normalv.dot(plane3Normalv));

	cgiDouble128NS::double128 anglevRatio = anglev / angleTotalv;

	d128Vec3 plane1Normalu = basep1.cross(basep3);
	plane1Normalu.normalize();
	d128Vec3 plane2Normalu = basep2.cross(basep4);
	plane2Normalu.normalize();

	d128Vec3 intersectionPoint2 = (plane1Normalu.cross(plane2Normalu));
	intersectionPoint2.normalize();

	d128Vec3 plane3Normalu = intersectionPoint2.cross(xyz);
	plane3Normalu.normalize();

	cgiDouble128NS::double128 angleTotalu = cgiDouble128NS::acos(plane1Normalu.dot(plane2Normalu));
	cgiDouble128NS::double128 angleu = cgiDouble128NS::acos(plane1Normalu.dot(plane3Normalu));

	cgiDouble128NS::double128 angleuRatio = angleu / angleTotalu;

	//subdivisionLevel = resolution - 1
	int numRowCol = std::pow(2, subdivisionLevel);
	int row = cgiDouble128NS::floor(anglevRatio * (double)numRowCol);
	int col = cgiDouble128NS::floor(angleuRatio * (double)numRowCol);

	cgiDouble128NS::double128 rowOffset = (anglevRatio * (double)numRowCol) - row;
	cgiDouble128NS::double128 colOffset = (angleuRatio * (double)numRowCol) - col;

	unsigned int offset;
	if (colOffset > rowOffset) {
		//Offset is either 0 or 1
		if (colOffset < (1.0 - rowOffset)) {
			offset = 0;
		} else {
			offset = 1;
		}
	} else {
		//Offset is either 2 or 3
		if (colOffset < (1.0 - rowOffset)) {
			offset = 2;
		} else {
			offset = 3;
		}
	}

	unsigned int guess = std::pow(4, subdivisionLevel + 1) * baseCell + std::pow(2, subdivisionLevel + 2) * row + (4 * col) + offset;

	bool found = false;
	std::deque<unsigned int> facesToCheck;
	std::vector<unsigned int> checked;
	facesToCheck.push_back(guess);
	
	unsigned int result;
	unsigned int thisIter = 0;
	do {
		result = facesToCheck.front();
		facesToCheck.pop_front();
		checked.push_back(result);

		d128Vec3 p0 = verts[faces[result].x];
		d128Vec3 p1 = verts[faces[result].y];
		d128Vec3 p2 = verts[faces[result].z];

		d128Vec3 normal = (p0 - p1).cross(p0 - p2);

		d128Vec3 p = xyz * ((p0.dot(normal)) / (xyz.dot(normal)));

		d128Vec3 v0 = p2 - p0;
		d128Vec3 v1 = p1 - p0;
		d128Vec3 v2 = p - p0;

		cgiDouble128NS::double128 dot00 = v0.dot(v0);
		cgiDouble128NS::double128 dot01 = v0.dot(v1);
		cgiDouble128NS::double128 dot02 = v0.dot(v2);
		cgiDouble128NS::double128 dot11 = v1.dot(v1);
		cgiDouble128NS::double128 dot12 = v1.dot(v2);

		// Compute barycentric coordinates
		cgiDouble128NS::double128 invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		cgiDouble128NS::double128 u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		cgiDouble128NS::double128 v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		found = (u >= 0) && (v >= 0) && (u + v < 1);

		if (!found) {
			int n1 = -1;
			int n2 = -1;
			int n3 = -1;
			findNeighbor(result, &n1, &n2, &n3);

			std::vector<std::pair<cgiDouble128NS::double128, int>> directions;

			d128Vec3 p0n1 = verts[faces[n1].x];
			d128Vec3 p1n1 = verts[faces[n1].y];
			d128Vec3 p2n1 = verts[faces[n1].z];

			d128Vec3 center = p0n1 * (1.0 / 3.0) + p1n1 * (1.0 / 3.0) + p2n1 * (1.0 / 3.0);

			cgiDouble128NS::double128 direction = center.dot(p);

			directions.push_back(std::pair<cgiDouble128NS::double128, int>(direction, n1));

			d128Vec3 p0n2 = verts[faces[n2].x];
			d128Vec3 p1n2 = verts[faces[n2].y];
			d128Vec3 p2n2 = verts[faces[n2].z];

			center = p0n2 * (1.0 / 3.0) + p1n2 * (1.0 / 3.0) + p2n2 * (1.0 / 3.0);

			direction = center.dot(p);

			directions.push_back(std::pair<cgiDouble128NS::double128, int>(direction, n2));

			d128Vec3 p0n3 = verts[faces[n3].x];
			d128Vec3 p1n3 = verts[faces[n3].y];
			d128Vec3 p2n3 = verts[faces[n3].z];

			center = p0n3 * (1.0 / 3.0) + p1n3 * (1.0 / 3.0) + p2n3 * (1.0 / 3.0);

			direction = center.dot(p);

			directions.push_back(std::pair<cgiDouble128NS::double128, int>(direction, n3));
			
			//Note the use of reverse iterators to sort in descending order
			std::sort(directions.rbegin(), directions.rend());

			bool checkedNearest = false;
			bool checkedSecond = false;
			bool checkedThird = false;
			for (unsigned int i = 0; i < checked.size(); i++) {
				if (directions[0].second == checked[i]) {
					checkedNearest = true;
				}
				if (directions[1].second == checked[i]) {
					checkedSecond = true;
				}
				if (directions[2].second == checked[i]) {
					checkedThird = true;
				}
			}
			for (unsigned int i = 0; i < facesToCheck.size(); i++) {
				if (directions[0].second == facesToCheck[i]) {
					checkedNearest = true;
				}
				if (directions[1].second == facesToCheck[i]) {
					checkedSecond = true;
				}
				if (directions[2].second == facesToCheck[i]) {
					checkedThird = true;
				}
			}

			if (!checkedNearest) {
				facesToCheck.push_back(directions[0].second);
			} else if (!checkedSecond) {
				facesToCheck.push_back(directions[1].second);
			} else {
				facesToCheck.push_back(directions[2].second);
			}
		}
		
		numTries++;
		thisIter++;
	} while (!found);

	if (thisIter > maxNumTries) {
		maxNumTries = thisIter;
	}

	//Need to find face at higher and lower subdivision levels when we switch to planar subdivision

	return result;
}
