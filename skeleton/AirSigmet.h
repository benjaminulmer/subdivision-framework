#pragma once

#include <rapidjson/document.h>

#include <string>
#include <vector>

#include "SphCoord.h"

enum class AirSigmetType {
	OUTLOOK,
	AIRMET,
	SIGMET
};

enum class HazardType {
	MTN,
	OBSCN,
	IFR,
	TURB,
	ICE,
	CONVECTIVE,
	ASH
};

enum class Severity {
	NONE,
	LT_MOD,
	MOD,
	MOD_SEV,
	SEV
};

struct AirSigmet {

	double minAltKM;
	double maxAltKM;
	std::vector<SphCoord> polygon;

	std::string validFrom;
	std::string validUntil;
	int dirDeg;
	int speedKT;
	int hazard;
	int severity;
	int type;

	void gridInsertion(double gridRadius, int maxDepth, std::vector<std::string>& interior, std::vector<std::string>& boundary) const;
	static void readFromJson(const rapidjson::Document& d, std::vector<AirSigmet>& out);
};

