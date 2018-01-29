#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <fstream>
#include <cstring>

#include <vector>
#include <string>

#include "Renderable.h"


class ContentReadWrite {

public:
	static bool loadOBJ(const char* path, Renderable& r);
	static rapidjson::Document readJSON(std::string path);
};

