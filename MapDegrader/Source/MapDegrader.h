#ifndef MAP_DEGRADER_H
#define MAP_DEGRADER_H
#include "Mappers/LandedTitlesScraper/LandedTitles.h"
#include "Mappers/DefinitionsScraper/Definitions.h"
#include "Mappers/ColorMapper/ColorMapper.h"
#include "Mappers/LocalizationScraper/LocalizationScraper.h"

class MapDegrader
{
public:
	void degradeMap(const std::string& gamePath);
	
private:
	void scrapeLandedTitles(const std::string& gamePath);
	void scrapeDefinitions(const std::string& gamePath);
	void alterMap(const std::string& gamePath);
	
	LandedTitles landedTitles;
	Definitions definitions;
	ColorMapper colorMapper;
	LocalizationScraper localizationScraper;
	
};

#endif // MAP_DEGRADER_H