#ifndef COLOR_MAPPER_H
#define COLOR_MAPPER_H
#include "Color.h"

class LandedTitles;
class Definitions;
class LocalizationScraper;
class ColorMapper
{
public:
	void craftReplacementColorMatrix(const LandedTitles& landedTitles, const Definitions& definitions);
	void exportDefinitions(const LocalizationScraper& localization) const;
	[[nodiscard]] std::string exportDefinitionsToString(const LocalizationScraper& localization) const;

	[[nodiscard]] const auto& getReplacementMatrix() const { return replacementMatrix; }

private:
	std::vector<std::pair<commonItems::Color, commonItems::Color>> replacementMatrix;
	std::map<int, std::pair<std::string, commonItems::Color>> countyColors; // provinceID/pair<c_county, color>
};

#endif // COLOR_MAPPER_H
