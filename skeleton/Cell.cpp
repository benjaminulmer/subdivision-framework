#include "Cell.h"

Cell::Cell(unsigned int index, d128Vec3 v1, d128Vec3 v2, d128Vec3 v3, cgiDouble128NS::double128 populationDensity) : index(index), v1(v1), v2(v2), v3(v3), populationDensity(populationDensity) {
	//TODO make sure initialization list works
}

Cell::~Cell() {
}
