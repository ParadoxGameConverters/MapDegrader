#ifndef MAP_DEGRADER_H
#define MAP_DEGRADER_H
#include "Mappers/ColorMapper/ColorMapper.h"
#include "Mappers/DefinitionsScraper/Definitions.h"
#include "Mappers/LandedTitlesScraper/LandedTitles.h"
#include "Mappers/LocalizationScraper/LocalizationScraper.h"
#include <Magick++.h>

class MapDegrader
{
  public:
	void degradeMap(const std::filesystem::path& gamePath);

  private:
	void scrapeLandedTitles(const std::filesystem::path& gamePath);
	void scrapeDefinitions(const std::filesystem::path& gamePath);
	void alterMap(const std::filesystem::path& gamePath);
	void loadMap(const std::filesystem::path& gamePath);

	Magick::Image map;
	LandedTitles landedTitles;
	Definitions definitions;
	ColorMapper colorMapper;
	LocalizationScraper localizationScraper;
};

#endif // MAP_DEGRADER_H