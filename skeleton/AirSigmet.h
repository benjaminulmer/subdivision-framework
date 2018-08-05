#pragma once

#include "SphCoord.h"

#include <rapidjson/document.h>

#include <string>
#include <vector>

enum class AirSigmetType {
	NONE,
	OUTLOOK,
	AIRMET,
	SIGMET
};

enum class HazardType {
	NONE,
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

// Class for storing information about an AirSigmet
struct AirSigmet {

	double minAltKM;
	double maxAltKM;
	std::vector<SphCoord> polygon;

	std::string validFrom;
	std::string validUntil;
	int dirDeg;
	int speedKT;
	HazardType hazard;
	Severity severity;
	AirSigmetType type;

	void gridInsertion(double gridRadius, int maxDepth, std::vector<std::string>& interior, std::vector<std::string>& boundary) const;

	static void readFromJson(const rapidjson::Document& d, std::vector<AirSigmet>& out);
	static AirSigmetType stringToAirSigmetType(const std::string& s);
	static HazardType stringToHazardType(const std::string& s);
	static Severity stringToSeverity(const std::string& s);
};

// Struct for storing an AirSigmet and the cells that are boundary and interior to it
struct AirSigmetCells {

	AirSigmet airSigmet;
	std::vector<std::string> interior;
	std::vector<std::string> boundary;
};