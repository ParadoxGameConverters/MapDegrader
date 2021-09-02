#include "ColorMapper.h"
#include "../DefinitionsScraper/Definitions.h"
#include "../LandedTitlesScraper/LandedTitles.h"
#include "../LocalizationScraper/LocalizationScraper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>

void ColorMapper::craftReplacementPixelList(const LandedTitles& landedTitles, const Definitions& definitions)
{
	LOG(LogLevel::Info) << "Building Color Replacement Map";
	auto counter = 0;

	// Create a county/barony map: map<c_county, map<<1, chroma>, <2, chroma> ...>>
	std::map<std::string, std::map<int, unsigned int>> countyBaronies; // we'll ping off the FIRST barony in map (smallest key).

	for (const auto& title: landedTitles.getTitles())
	{
		if (title.first.find("b_") == 0)
		{
			const auto& chroma = definitions.getChromaForProvinceID(title.second.getProvinceID());
			if (chroma)
				countyBaronies[title.second.getDJLiege()].insert(std::pair(title.second.getProvinceID(), *chroma));
			else
				throw std::runtime_error("Barony " + title.first + " ID " + std::to_string(title.second.getProvinceID()) + " has no definition!");
			++counter;
		}
	}
	Log(LogLevel::Info) << counter << " provinces linked into " << countyBaronies.size() << " counties";

	// And fill out the matrix.
	for (const auto& [countyName, baronies]: countyBaronies)
	{
		// Which has the smallest barony ID?
		auto smallestProvinceID = baronies.begin()->first; // first one. They are ints and map is ordered.
		auto smallestProvinceChroma = baronies.begin()->second;
		for (const auto& [baronyID, baronyChroma]: baronies)
			if (baronyID != smallestProvinceID)
			{
				// add all the pixels into this so we can efficiently update them later
				const auto& pixels = definitions.getPixelsForProvinceID(baronyID);
				if (const auto& replacementItr = replacementPixels.find(smallestProvinceChroma); replacementItr != replacementPixels.end())
				{
					replacementItr->second.insert(replacementItr->second.end(), pixels.begin(), pixels.end());
				}
				else
				{
					replacementPixels[smallestProvinceChroma] = pixels;
				}
			}
		countyColors.insert(std::pair(smallestProvinceID, std::pair(countyName, smallestProvinceChroma)));
	}
	Log(LogLevel::Info) << "Recognized chromas: " << replacementPixels.size();
}

void ColorMapper::exportDefinitions(const LocalizationScraper& localization) const
{
	std::ofstream defFile("export/definition.csv");
	defFile << "ProvID;r;g;b;title;x;\n";
	for (const auto& county: countyColors)
	{
		auto name = county.second.first;
		const auto& loc = localization.getLocForKey(name);
		if (loc)
		{
			name = commonItems::convertUTF8ToWin1252(*loc);
		}
		const auto [r, g, b] = pixelUnpack(county.second.second);
		defFile << county.first << ";" << std::to_string(r) << ";" << std::to_string(g) << ";" << std::to_string(b) << ";" << name << ";x;\n";
	}
	defFile.close();
}

std::string ColorMapper::exportDefinitionsToString(const LocalizationScraper& localization) const
{
	std::stringstream defStream;
	defStream << "ProvID;r;g;b;title;x;\n";
	for (const auto& county: countyColors)
	{
		auto name = county.second.first;
		const auto& loc = localization.getLocForKey(name);
		if (loc)
		{
			name = commonItems::convertUTF8ToWin1252(*loc);
		}
		const auto [r, g, b] = pixelUnpack(county.second.second);
		defStream << county.first << ";" << std::to_string(r) << ";" << std::to_string(g) << ";" << std::to_string(b) << ";" << name << ";x;\n";
	}
	return defStream.str();
}
