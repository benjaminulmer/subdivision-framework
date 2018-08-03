#include "DGGS.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <deque>

DGGS::DGGS() {
}

DGGS::DGGS(std::vector<d128Vec3> vertsRhombicTriacontahedron, std::vector<d128Vec3> vertsSubdivided, std::vector<glm::u32vec3> facesSubdivided) : vertsRhombicTriacontahedron(vertsRhombicTriacontahedron), vertsSubdivided(vertsSubdivided), facesSubdivided(facesSubdivided) {
	//Load database
	//Construct ACM
	baseNeighbors = std::vector<std::array<unsigned int, 4>>(30);

	baseNeighbors[0] = { 5, 6, 4, 1 };
	baseNeighbors[1] = { 6, 7, 0, 2 };
	baseNeighbors[2] = { 7, 8, 1, 3 };
	baseNeighbors[3] = { 8, 9, 2, 4 };
	baseNeighbors[4] = { 9, 5, 3, 0 };
	baseNeighbors[5] = { 10, 0, 19, 4 };
	baseNeighbors[6] = { 12, 1, 11, 0 };
	baseNeighbors[7] = { 14, 2, 13, 1 };
	baseNeighbors[8] = { 16, 3, 15, 2 };
	baseNeighbors[9] = { 18, 4, 17, 3 };
	baseNeighbors[10] = { 19, 20, 5, 11 };
	baseNeighbors[11] = { 20, 12, 10, 6 };
	baseNeighbors[12] = { 11, 21, 6, 13 };
	baseNeighbors[13] = { 21, 14, 12, 7 };
	baseNeighbors[14] = { 13, 22, 7, 15 };
	baseNeighbors[15] = { 22, 16, 14, 8 };
	baseNeighbors[16] = { 15, 23, 8, 17 };
	baseNeighbors[17] = { 23, 18, 16, 9 };
	baseNeighbors[18] = { 17, 24, 9, 19 };
	baseNeighbors[19] = { 24, 10, 18, 5 };
	baseNeighbors[20] = { 10, 25, 11, 26 };
	baseNeighbors[21] = { 12, 26, 13, 27 };
	baseNeighbors[22] = { 14, 27, 15, 28 };
	baseNeighbors[23] = { 16, 28, 17, 29 };
	baseNeighbors[24] = { 18, 29, 19, 25 };
	baseNeighbors[25] = { 20, 24, 26, 29 };
	baseNeighbors[26] = { 21, 20, 27, 25 };
	baseNeighbors[27] = { 22, 21, 28, 26 };
	baseNeighbors[28] = { 23, 22, 29, 27 };
	baseNeighbors[29] = { 24, 23, 25, 28 };

	transforms = std::vector<std::array<unsigned int, 4>>(30);

	transforms[0] = { 0, 0, 0, 0 };
	transforms[1] = { 0, 0, 0, 0 };
	transforms[2] = { 0, 0, 0, 0 };
	transforms[3] = { 0, 0, 0, 0 };
	transforms[4] = { 0, 0, 0, 0 };
	transforms[5] = { 1, 1, 0, 1 };
	transforms[6] = { 1, 1, 0, 1 };
	transforms[7] = { 1, 1, 0, 1 };
	transforms[8] = { 1, 1, 0, 1 };
	transforms[9] = { 1, 1, 0, 1 };
	transforms[10] = { 0, 1, 1, 0 };
	transforms[11] = { 1, 1, 0, 0 };
	transforms[12] = { 0, 1, 1, 0 };
	transforms[13] = { 1, 1, 0, 0 };
	transforms[14] = { 0, 1, 1, 0 };
	transforms[15] = { 1, 1, 0, 0 };
	transforms[16] = { 0, 1, 1, 0 };
	transforms[17] = { 1, 1, 0, 0 };
	transforms[18] = { 0, 1, 1, 0 };
	transforms[19] = { 1, 1, 0, 0 };
	transforms[20] = { 0, 1, 1, 1 };
	transforms[21] = { 0, 1, 1, 1 };
	transforms[22] = { 0, 1, 1, 1 };
	transforms[23] = { 0, 1, 1, 1 };
	transforms[24] = { 0, 1, 1, 1 };
	transforms[25] = { 0, 0, 0, 0 };
	transforms[26] = { 0, 0, 0, 0 };
	transforms[27] = { 0, 0, 0, 0 };
	transforms[28] = { 0, 0, 0, 0 };
	transforms[29] = { 0, 0, 0, 0 };

	selectedPopulationDensity = cgiDouble128NS::double128(0.0);
	resolution = 7;
	switchResolution = 4;
}

DGGS::~DGGS() {

}

void DGGS::print(cgiDouble128NS::double128 p) {
	char s[128];
	p.str(s);
	std::cout << s << std::endl;
}

bool DGGS::cellToVerts(unsigned int cell, unsigned int resolution, d128Vec3* v1, d128Vec3* v2, d128Vec3* v3) {
	if (resolution < switchResolution) {
		unsigned int c1;
		unsigned int c2;
		unsigned int c3;
		unsigned int c4;
		children(cell, resolution, &c1, &c2, &c3, &c4);
		unsigned int current0 = c1;
		unsigned int current1 = c2;
		unsigned int current2 = c4;
		for (unsigned int i = 0; i < switchResolution - resolution - 1; i++) {
			children(current0, resolution + i + 1, &c1, &c2, &c3, &c4);
			current0 = c1;
			children(current1, resolution + i + 1, &c1, &c2, &c3, &c4);
			current1 = c1;
			children(current2, resolution + i + 1, &c1, &c2, &c3, &c4);
			current2 = c4;
		}
		*v1 = vertsSubdivided[facesSubdivided[current0].y];
		*v2 = vertsSubdivided[facesSubdivided[current1].y];
		*v3 = vertsSubdivided[facesSubdivided[current2].x];
	} else if (resolution > switchResolution) {
		unsigned int bigCell = cell;
		std::vector<unsigned int> childNums;
		for (unsigned int i = 0; i < resolution - switchResolution; i++) {
			childNums.push_back(parent(bigCell, resolution - i, &bigCell));
		}
		
		d128Vec3 bigv1 = vertsSubdivided[facesSubdivided[bigCell].x];
		d128Vec3 bigv2 = vertsSubdivided[facesSubdivided[bigCell].y];
		d128Vec3 bigv3 = vertsSubdivided[facesSubdivided[bigCell].z];

		for (int i = childNums.size() - 1; i >= 0; i--) {
			if (childNums[i] == 0) {
				*v1 = bigv1 * 0.5 + bigv2 * 0.5;
				*v2 = bigv2;
				*v3 = bigv2 * 0.5 + bigv3 * 0.5;
			} else if (childNums[i] == 1) {
				*v1 = bigv1 * 0.5 + bigv2 * 0.5;
				*v2 = bigv3;
				*v3 = bigv2 * 0.5 + bigv3 * 0.5;
			} else if (childNums[i] == 2) {
				*v1 = bigv3;
				*v2 = bigv1 * 0.5 + bigv2 * 0.5;
				*v3 = bigv1 * 0.5 + bigv3 * 0.5;
			} else { //childNums[i] == 3
				*v1 = bigv1;
				*v2 = bigv1 * 0.5 + bigv2 * 0.5;
				*v3 = bigv1 * 0.5 + bigv3 * 0.5;
			}
			bigv1 = *v1;
			bigv2 = *v2;
			bigv3 = *v3;
		}

	} else { //resolution == switchResolution
		*v1 = vertsSubdivided[facesSubdivided[cell].x];
		*v2 = vertsSubdivided[facesSubdivided[cell].y];
		*v3 = vertsSubdivided[facesSubdivided[cell].z];
	}
	return false;
}

unsigned int DGGS::pointToCell(d128Vec3 point) {
	return 0;
}

unsigned int DGGS::pointToCell(d128Vec3 point, unsigned int resolution) {
	return 0;
}

unsigned int DGGS::pointToCell(d128Vec2 latlon) {
	//Convert lat lon to theta, phi
	cgiDouble128NS::double128 phi = (M_PI / 2.0) - latlon.x;
	cgiDouble128NS::double128 theta = latlon.y;

	d128Vec3 xyz = d128Vec3(cgiDouble128NS::cos(theta) * cgiDouble128NS::sin(phi), cgiDouble128NS::sin(theta) * cgiDouble128NS::sin(phi), cgiDouble128NS::cos(phi));

	unsigned int baseCell;

	//Divide longitude into 10 regions, binary searching which region we are in
	if (theta <= M_PI) {
		if (theta <= (2.0 * M_PI) / 5.0) {
			if (theta <= M_PI / 5.0) {
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
			if (theta <= (3.0 * M_PI) / 5.0) {
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
			} else if (theta <= (4.0 * M_PI) / 5.0) {
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
		if (theta <= (7.0* M_PI) / 5.0) {
			if (theta <= (6.0 * M_PI) / 5.0) {
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
			if (theta <= (8.0* M_PI) / 5.0) {
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
			} else if (theta <= (9.0* M_PI) / 5.0) {
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
	int numRowCol = std::pow(2, switchResolution - 1);
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

	unsigned int guess = std::pow(4, switchResolution) * baseCell + std::pow(2, switchResolution + 1) * row + (4 * col) + offset;

	bool found = false;
	std::deque<unsigned int> facesToCheck;
	std::vector<unsigned int> checked;
	facesToCheck.push_back(guess);

	unsigned int result;
	d128Vec3 p0;
	d128Vec3 p1;
	d128Vec3 p2;
	d128Vec3 p;
	do {
		result = facesToCheck.front();
		facesToCheck.pop_front();
		checked.push_back(result);

		p0 = vertsSubdivided[facesSubdivided[result].x];
		p1 = vertsSubdivided[facesSubdivided[result].y];
		p2 = vertsSubdivided[facesSubdivided[result].z];

		d128Vec3 normal = (p0 - p1).cross(p0 - p2);

		p = xyz * ((p0.dot(normal)) / (xyz.dot(normal)));

		d128Vec3 v0 = p0 - p1;
		d128Vec3 v1 = p2 - p1;
		d128Vec3 v2 = p - p1;

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
			unsigned int n1 = -1;
			unsigned int n2 = -1;
			unsigned int n3 = -1;
			neighbors(result, &n1, &n2, &n3, switchResolution);

			std::vector<std::pair<cgiDouble128NS::double128, int>> directions;

			d128Vec3 p0n1 = vertsSubdivided[facesSubdivided[n1].x];
			d128Vec3 p1n1 = vertsSubdivided[facesSubdivided[n1].y];
			d128Vec3 p2n1 = vertsSubdivided[facesSubdivided[n1].z];

			d128Vec3 center = p0n1 * (1.0 / 3.0) + p1n1 * (1.0 / 3.0) + p2n1 * (1.0 / 3.0);

			cgiDouble128NS::double128 direction = center.dot(p);

			directions.push_back(std::pair<cgiDouble128NS::double128, int>(direction, n1));

			d128Vec3 p0n2 = vertsSubdivided[facesSubdivided[n2].x];
			d128Vec3 p1n2 = vertsSubdivided[facesSubdivided[n2].y];
			d128Vec3 p2n2 = vertsSubdivided[facesSubdivided[n2].z];

			center = p0n2 * (1.0 / 3.0) + p1n2 * (1.0 / 3.0) + p2n2 * (1.0 / 3.0);

			direction = center.dot(p);

			directions.push_back(std::pair<cgiDouble128NS::double128, int>(direction, n2));

			d128Vec3 p0n3 = vertsSubdivided[facesSubdivided[n3].x];
			d128Vec3 p1n3 = vertsSubdivided[facesSubdivided[n3].y];
			d128Vec3 p2n3 = vertsSubdivided[facesSubdivided[n3].z];

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
	} while (!found);

	if (resolution < switchResolution) {
		for (unsigned int i = 0; i < switchResolution - resolution; i++) {
			unsigned int temp;
			parent(result, switchResolution - i, &temp);
			result = temp;
		}
	} else if (resolution > switchResolution) {
		//Use barycentric coordinates u and v
		int numRowCol = std::pow(2, resolution - switchResolution);

		d128Vec3 r0;
		d128Vec3 r1;
		d128Vec3 r2;
		if (result % 4 == 0) {
			r0 = p1;
			r1 = p0;
			r2 = (p0 + ((p2 - p0) * 2.0));
		} else if (result % 4 == 1) {
			r0 = (p1 + ((p2 - p1) * 2.0));
			r1 = p0;
			r2 = (p0 + ((p2 - p0) * 2.0));
		} else if (result % 4 == 2) {
			r0 = p1;
			r1 = (p0 + ((p2 - p0) * 2.0));
			r2 = p0;
		} else { // result % 4 == 3
			r0 = (p1 + ((p2 - p1) * 2.0));
			r1 = (p0 + ((p2 - p0) * 2.0));
			r2 = p0;
		}
		
		d128Vec3 v0 = (r1 - r0);
		d128Vec3 v1 = (r2 - r0);
		d128Vec3 v2 = (p - r0);

		cgiDouble128NS::double128 dot00 = v0.dot(v0);
		cgiDouble128NS::double128 dot01 = v0.dot(v1);
		cgiDouble128NS::double128 dot02 = v0.dot(v2);
		cgiDouble128NS::double128 dot11 = v1.dot(v1);
		cgiDouble128NS::double128 dot12 = v1.dot(v2);

		// Compute barycentric coordinates
		cgiDouble128NS::double128 invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		cgiDouble128NS::double128 u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		cgiDouble128NS::double128 v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		int row = cgiDouble128NS::floor(v * numRowCol);
		int col = cgiDouble128NS::floor(u * numRowCol);

		cgiDouble128NS::double128 rowOffset = (v * numRowCol) - row;
		cgiDouble128NS::double128 colOffset = (u * numRowCol) - col;

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

		unsigned int baseCell = result / std::pow(4, switchResolution);
		unsigned int cellRelativeToBaseMesh = result % (int)std::pow(4, switchResolution);
		unsigned int rowNum = cellRelativeToBaseMesh / std::pow(2, switchResolution + 1);
		unsigned int colNum = (cellRelativeToBaseMesh % (int)std::pow(2, switchResolution + 1)) / 4;

		unsigned int littleRow = numRowCol * rowNum + row;
		unsigned int littleCol = numRowCol * colNum + col;

		unsigned int littleNumRowCol = std::pow(2, resolution - 1);
		result = std::pow(4, resolution) * baseCell + littleNumRowCol * 4 * littleRow + 4 * littleCol + offset;
	}

	return result;
}

unsigned int DGGS::pointToCell(d128Vec2 latlon, unsigned int resolution) {
	return 0;
}

bool DGGS::neighbors(unsigned int cell, unsigned int * n1, unsigned int * n2, unsigned int * n3) {
	unsigned int baseCell = cell / (int)std::pow(4, resolution);
	unsigned int cellRelativeToBaseMesh = cell % (int)std::pow(4, resolution);
	
	//Find the first neighbor
	*n1 = cell ^ 2;
	//Find the second neighbor
	*n2 = cell ^ 1;

	//Find the third neighbor
	if ((cellRelativeToBaseMesh % 4 == 0) && (cellRelativeToBaseMesh < std::pow(2, resolution + 1))) {
		//Top Edge case
		unsigned int u = cellRelativeToBaseMesh / 4;
		//0 adjcent to 1
		if (transforms[baseCell][0] == 0) {
			unsigned int v = std::pow(2, resolution - 1) - u - 1;
			*n3 = ((v + 1) * std::pow(2, resolution + 1) - 3) + (baseNeighbors[baseCell][0] * std::pow(4, resolution));
		//0 adjcent to 2
		} else if (transforms[baseCell][0] == 1) {
			unsigned int v = u;
			*n3 = (2 + v * std::pow(2, resolution + 1)) + (baseNeighbors[baseCell][0] * std::pow(4, resolution));
		}
		return true;
	} else if ((cellRelativeToBaseMesh % 4 == 3) && (cellRelativeToBaseMesh >= (std::pow(4, resolution) - std::pow(2, resolution + 1)))) {
		//Bottom Edge Case
		unsigned int u = (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1)) / 4;
		//3 adjcent to 2
		if (transforms[baseCell][3] == 0) {
			unsigned int v = std::pow(2, resolution - 1) - u - 1;
			*n3 = (2 + v * std::pow(2, resolution + 1)) + (baseNeighbors[baseCell][3] * std::pow(4, resolution));
		//3 adjcent to 1
		} else if (transforms[baseCell][3] == 1) {
			unsigned int v = u;
			*n3 = ((v + 1) * std::pow(2, resolution + 1) - 3) + (baseNeighbors[baseCell][3] * std::pow(4, resolution));
		}
		return true;
	} else if (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1) == 2) {
		//Left Edge Case
		unsigned int v = cellRelativeToBaseMesh / std::pow(2, resolution + 1);
		//2 adjcent to 3
		if (transforms[baseCell][2] == 0) {
			*n3 = std::pow(4, resolution) - 1 - (4 * v) + (baseNeighbors[baseCell][2] * std::pow(4, resolution));
		//2 adjcent to 0
		} else if (transforms[baseCell][2] == 1) {
			unsigned int u = v;
			*n3 = (4 * u) + (baseNeighbors[baseCell][2] * std::pow(4, resolution));
		}
		return true;
	} else if (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1) == std::pow(2, resolution + 1) - 3) {
		//Right Edge Case
		unsigned int v = cellRelativeToBaseMesh / std::pow(2, resolution + 1);
		//1 adjcent to 3
		if (transforms[baseCell][1] == 0) {
			unsigned int uRev = std::pow(2, resolution - 1) - v - 1;
			*n3 = std::pow(4, resolution) - 1 - (4 * uRev) + (baseNeighbors[baseCell][1] * std::pow(4, resolution));
		//1 adjcent to 0
		} else if (transforms[baseCell][1] == 1) {
			unsigned int u = std::pow(2, resolution - 1) - v - 1;
			*n3 = (4 * u) + (baseNeighbors[baseCell][1] * std::pow(4, resolution));
		}
		return true;
	} else if (cellRelativeToBaseMesh % 4 == 0) {
		*n3 = cell - (std::pow(2, resolution + 1)) + 3;
		return true;
	} else if (cellRelativeToBaseMesh % 4 == 1) {
		*n3 = cell + 5;
		return true;
	} else if (cellRelativeToBaseMesh % 4 == 2) {
		*n3 = cell - 5;
		return true;
	} else { // cellRelativeToBaseMesh % 4 == 3
		*n3 = cell + (std::pow(2, resolution + 1)) - 3;
		return true;
	}
}

bool DGGS::neighbors(unsigned int cell, unsigned int * n1, unsigned int * n2, unsigned int * n3, unsigned int resolution) {
	unsigned int baseCell = cell / (int)std::pow(4, resolution);
	unsigned int cellRelativeToBaseMesh = cell % (int)std::pow(4, resolution);

	//Find the first neighbor
	*n1 = cell ^ 2;
	//Find the second neighbor
	*n2 = cell ^ 1;

	//Find the third neighbor
	if ((cellRelativeToBaseMesh % 4 == 0) && (cellRelativeToBaseMesh < std::pow(2, resolution + 1))) {
		//Top Edge case
		unsigned int u = cellRelativeToBaseMesh / 4;
		//0 adjcent to 1
		if (transforms[baseCell][0] == 0) {
			unsigned int v = std::pow(2, resolution - 1) - u - 1;
			*n3 = ((v + 1) * std::pow(2, resolution + 1) - 3) + (baseNeighbors[baseCell][0] * std::pow(4, resolution));
			//0 adjcent to 2
		} else if (transforms[baseCell][0] == 1) {
			unsigned int v = u;
			*n3 = (2 + v * std::pow(2, resolution + 1)) + (baseNeighbors[baseCell][0] * std::pow(4, resolution));
		}
		return true;
	} else if ((cellRelativeToBaseMesh % 4 == 3) && (cellRelativeToBaseMesh >= (std::pow(4, resolution) - std::pow(2, resolution + 1)))) {
		//Bottom Edge Case
		unsigned int u = (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1)) / 4;
		//3 adjcent to 2
		if (transforms[baseCell][3] == 0) {
			unsigned int v = std::pow(2, resolution - 1) - u - 1;
			*n3 = (2 + v * std::pow(2, resolution + 1)) + (baseNeighbors[baseCell][3] * std::pow(4, resolution));
			//3 adjcent to 1
		} else if (transforms[baseCell][3] == 1) {
			unsigned int v = u;
			*n3 = ((v + 1) * std::pow(2, resolution + 1) - 3) + (baseNeighbors[baseCell][3] * std::pow(4, resolution));
		}
		return true;
	} else if (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1) == 2) {
		//Left Edge Case
		unsigned int v = cellRelativeToBaseMesh / std::pow(2, resolution + 1);
		//2 adjcent to 3
		if (transforms[baseCell][2] == 0) {
			*n3 = std::pow(4, resolution) - 1 - (4 * v) + (baseNeighbors[baseCell][2] * std::pow(4, resolution));
			//2 adjcent to 0
		} else if (transforms[baseCell][2] == 1) {
			unsigned int u = v;
			*n3 = (4 * u) + (baseNeighbors[baseCell][2] * std::pow(4, resolution));
		}
		return true;
	} else if (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1) == std::pow(2, resolution + 1) - 3) {
		//Right Edge Case
		unsigned int v = cellRelativeToBaseMesh / std::pow(2, resolution + 1);
		//1 adjcent to 3
		if (transforms[baseCell][1] == 0) {
			unsigned int uRev = std::pow(2, resolution - 1) - v - 1;
			*n3 = std::pow(4, resolution) - 1 - (4 * uRev) + (baseNeighbors[baseCell][1] * std::pow(4, resolution));
			//1 adjcent to 0
		} else if (transforms[baseCell][1] == 1) {
			unsigned int u = std::pow(2, resolution - 1) - v - 1;
			*n3 = (4 * u) + (baseNeighbors[baseCell][1] * std::pow(4, resolution));
		}
		return true;
	} else if (cellRelativeToBaseMesh % 4 == 0) {
		*n3 = cell - (std::pow(2, resolution + 1)) + 3;
		return true;
	} else if (cellRelativeToBaseMesh % 4 == 1) {
		*n3 = cell + 5;
		return true;
	} else if (cellRelativeToBaseMesh % 4 == 2) {
		*n3 = cell - 5;
		return true;
	} else { // cellRelativeToBaseMesh % 4 == 3
		*n3 = cell + (std::pow(2, resolution + 1)) - 3;
		return true;
	}
}

unsigned int DGGS::parent(unsigned int cell, unsigned int resolution, unsigned int * parent) {

	if (resolution == 1) {
		*parent = cell;
		//No parent
		return 4;
	}

	unsigned int cellNumber = cell / (unsigned int)std::pow(4, resolution);
	unsigned int cellRelativeToBaseMesh = cell % (int)std::pow(4, resolution);
	unsigned int rowNum = cellRelativeToBaseMesh / std::pow(2, resolution + 2);
	unsigned int colNum = (cellRelativeToBaseMesh % (int)std::pow(2, resolution + 1)) / 8;

	unsigned int topStart = rowNum * std::pow(2, resolution + 1) * 2 + (8 * colNum);
	unsigned int bottomStart = rowNum * std::pow(2, resolution + 1) * 2 + std::pow(2, resolution + 1) + (8 * colNum);

	unsigned int cellStart = rowNum * std::pow(2, resolution) + (4 * colNum);

	if (cellRelativeToBaseMesh == topStart) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart;
		return 0;
	} else if (cellRelativeToBaseMesh == topStart + 1) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart;
		return 1;
	} else if (cellRelativeToBaseMesh == topStart + 2) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 2;
		return 0;
	} else if (cellRelativeToBaseMesh == topStart + 3) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 2;
		return 1;
	} else if (cellRelativeToBaseMesh == topStart + 4) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart;
		return 3;
	} else if (cellRelativeToBaseMesh == topStart + 5) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 1;
		return 3;
	} else if (cellRelativeToBaseMesh == topStart + 6) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart;
		return 2;
	} else if (cellRelativeToBaseMesh == topStart + 7) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 1;
		return 2;
	} else if (cellRelativeToBaseMesh == bottomStart) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 2;
		return 2;
	} else if (cellRelativeToBaseMesh == bottomStart + 1) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 3;
		return 2;
	} else if (cellRelativeToBaseMesh == bottomStart + 2) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 2;
		return 3;
	} else if (cellRelativeToBaseMesh == bottomStart + 3) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 3;
		return 3;
	} else if (cellRelativeToBaseMesh == bottomStart + 4) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 1;
		return 1;
	} else if (cellRelativeToBaseMesh == bottomStart + 5) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 1;
		return 0;
	} else if (cellRelativeToBaseMesh == bottomStart + 6) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 3;
		return 1;
	} else if (cellRelativeToBaseMesh == bottomStart + 7) {
		*parent = cellNumber * (unsigned int)std::pow(4, resolution - 1) + cellStart + 3;
		return 0;
	}

}

bool DGGS::children(unsigned int cell, unsigned int resolution, unsigned int * c1, unsigned int * c2, unsigned int * c3, unsigned int * c4) {

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
		*c1 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 2;
		*c2 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart;
		*c3 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 3;
		*c4 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + topStart + 2;
	} else if (cellRelativeToBaseMesh % 4 == 3) {
		*c1 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 7;
		*c2 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 6;
		*c3 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 3;
		*c4 = cellNumber * (unsigned int)std::pow(4, resolution + 1) + bottomStart + 1;
	}
	return true;
}

bool DGGS::select(d128Vec3 point) {
	//Call pointToCell and add to selected
	return false;
}

bool DGGS::select(d128Vec3 point, unsigned int resolution) {
	return false;
}

bool DGGS::select(d128Vec2 point) {
	unsigned int result = pointToCell(point);
	//TODO Grab cell from database
	//selected.insert();
	//TODO Change selectedPopulationDensity
	return false;
}

bool DGGS::select(d128Vec2 point, unsigned int resolution) {
	return false;
}

bool DGGS::selectArea(std::vector<d128Vec3> points, d128Vec3 inside) {
	//Use region growing algorithm from pointToCell(inside)
	return false;
}

bool DGGS::selectArea(std::vector<d128Vec3> points, d128Vec3 inside, unsigned int resolution) {
	return false;
}

bool DGGS::selectArea(std::vector<d128Vec2> area, d128Vec2 point) {
	return false;
}

bool DGGS::selectArea(std::vector<d128Vec2> area, d128Vec2 point, unsigned int resolution) {
	return false;
}

bool DGGS::selectRegion(std::string regionName) {
	//Retrieve cells from the database
	return false;
}

bool DGGS::deSelect(d128Vec3 point) {
	//Use pointToCell(point) then remove from selected if it exists
	return false;
}

bool DGGS::deSelect(d128Vec3 point, unsigned int resolution) {
	return false;
}

bool DGGS::deSelect(d128Vec2 point) {
	unsigned int result = pointToCell(point);
	selected.erase(result);
	return false;
}

bool DGGS::deSelect(d128Vec2 point, unsigned int resolution) {
	return false;
}

bool DGGS::deSelectAll() {
	selected.clear();
	selectedBoundary.clear();
	return true;
}

void DGGS::viewUpdate(glm::vec3 camZ, double zoom) {
	//First check how far the view has moved restart region grow if it has moved too far
	//Update visible and visibleBoundry for the new view
}

cgiDouble128NS::double128 DGGS::populationDensityQuery() {
	//Return the summary statistic
	return selectedPopulationDensity;
}