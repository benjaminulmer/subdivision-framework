#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <cstring>

#include <vector>
#include <string>

#include "Renderable.h"


class ContentReadWrite {

public:
	static bool loadOBJ(const char* path, Renderable& r);
};

