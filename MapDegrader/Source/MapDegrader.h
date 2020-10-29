#ifndef MAP_DEGRADER_H
#define MAP_DEGRADER_H
#include "Mappers/ColorMapper/ColorMapper.h"
#include "Mappers/DefinitionsScraper/Definitions.h"
#include "Mappers/LandedTitlesScraper/LandedTitles.h"
#include "Mappers/LocalizationScraper/LocalizationScraper.h"
#include "Magick++.h"

class MapDegrader
{
  public:
	void degradeMap(const std::string& gamePath);

  private:
	void scrapeLandedTitles(const std::string& gamePath);
	void scrapeDefinitions(const std::string& gamePath);
	void alterMap(const std::string& gamePath);
	void loadMap(const std::string& gamePath);

	Magick::Image map;
	LandedTitles landedTitles;
	Definitions definitions;
	ColorMapper colorMapper;
	LocalizationScraper localizationScraper;
};

#endif // MAP_DEGRADER_H