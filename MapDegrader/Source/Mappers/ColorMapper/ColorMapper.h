#ifndef COLOR_MAPPER_H
#define COLOR_MAPPER_H
#include "Color.h"

struct Pixel;
class LandedTitles;
class Definitions;
class LocalizationScraper;
class ColorMapper
{
  public:
	void craftReplacementPixelList(const LandedTitles& landedTitles, const Definitions& definitions);
	void exportDefinitions(const LocalizationScraper& localization) const;
	[[nodiscard]] std::string exportDefinitionsToString(const LocalizationScraper& localization) const;
	[[nodiscard]] const auto& getReplacementPixels() const { return replacementPixels; }

  private:
	std::map<unsigned int, std::vector<Pixel>> replacementPixels;
	std::map<int, std::pair<std::string, unsigned int>> countyColors; // provinceID/pair<c_county, chroma>
};

#endif // COLOR_MAPPER_H
