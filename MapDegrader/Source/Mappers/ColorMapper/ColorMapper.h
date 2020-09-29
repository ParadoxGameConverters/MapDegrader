#ifndef COLOR_MAPPER_H
#define COLOR_MAPPER_H
#include "Color.h"
#include "../LandedTitlesScraper/LandedTitles.h"
#include "../DefinitionsScraper/Definitions.h"

class ColorMapper
{
public:
	void craftReplacementColorMatrix(const LandedTitles& landedTitles, const Definitions& definitions);

	[[nodiscard]] const auto& getReplacementMatrix() const { return replacementMatrix; }
	void exportDefinitions() const;

private:
	std::map<std::string, std::map<int, commonItems::Color>> countyBaronies; // we ping off the LAST barony in map (largest key).
	std::vector<std::pair<commonItems::Color, commonItems::Color>> replacementMatrix;
	std::map<int, std::pair<std::string, commonItems::Color>> countyColors; // provinceID/pair<c_county, color>
};

#endif // COLOR_MAPPER_H
