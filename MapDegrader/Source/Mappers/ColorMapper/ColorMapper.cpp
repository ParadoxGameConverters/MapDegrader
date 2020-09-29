#include "ColorMapper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include "../LandedTitlesScraper/LandedTitles.h"
#include "../DefinitionsScraper/Definitions.h"
#include "../LocalizationScraper/LocalizationScraper.h"

void ColorMapper::craftReplacementColorMatrix(const LandedTitles& landedTitles, const Definitions& definitions)
{
	LOG(LogLevel::Info) << "Building Color Replacement Map";
	auto counter = 0;

	// Create a county/barony map: map<c_county, map<<1, {0 4 5}>, <2, {5 6 7}> ...>>
	for (const auto& title : landedTitles.getTitles())
	{
		if (title.first.find("b_") == 0)
		{
			const auto& color = definitions.getColorForProvinceID(title.second.getProvinceID());
			if (color)
				countyBaronies[title.second.getDJLiege()].insert(std::pair(title.second.getProvinceID(), *color));
			else
				throw std::runtime_error("Barony " + title.first + " ID " + std::to_string(title.second.getProvinceID()) + " has no definition!");
			++counter;
		}
	}
	Log(LogLevel::Info) << counter << " provinces linked into " << countyBaronies.size() << " counties";

	// And fill out the matrix.
	for (const auto& county : countyBaronies)
	{
		// Which has the largest barony ID?
		auto largestProvinceID = county.second.rbegin()->first; // last one. They are ints and map is ordered.
		for (const auto& barony : county.second)
			if (barony.first != largestProvinceID)
				replacementMatrix.emplace_back(std::pair(barony.second, county.second.find(largestProvinceID)->second));

		countyColors.insert(std::pair(largestProvinceID, std::pair(county.first, county.second.find(largestProvinceID)->second)));
	}
	Log(LogLevel::Info) << "Matrix size: " << replacementMatrix.size();
}

void ColorMapper::exportDefinitions(const LocalizationScraper& localization) const
{
	std::ofstream defFile("export/definition.csv");
	defFile << "ProvID;r;g;b;title;x;\n";
	for (const auto& county : countyColors)
	{
		auto name = county.second.first;
		const auto& loc = localization.getLocForKey(name);
		if (loc)
		{
			name = commonItems::convertUTF8ToWin1252(*loc);
		}
		defFile << county.first << ";" << county.second.second.r() << ";" << county.second.second.g() << ";" << county.second.second.b() << ";" << name << ";x;\n";
	}
	defFile.close();
}

std::string ColorMapper::exportDefinitionsToString(const LocalizationScraper& localization) const
{
	std::stringstream defStream;
	defStream << "ProvID;r;g;b;title;x;\n";
	for (const auto& county : countyColors)
	{
		auto name = county.second.first;
		const auto& loc = localization.getLocForKey(name);
		if (loc)
		{
			name = commonItems::convertUTF8ToWin1252(*loc);
		}
		defStream << county.first << ";" << county.second.second.r() << ";" << county.second.second.g() << ";" << county.second.second.b() << ";" << name << ";x;\n";
	}
	return defStream.str();
}
