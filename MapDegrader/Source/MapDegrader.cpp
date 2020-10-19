#include "MapDegrader.h"
#include "Log.h"
#include "Magick++.h"
#include "OSCompatibilityLayer.h"

void MapDegrader::degradeMap(const std::string& gamePath)
{
	scrapeLandedTitles(gamePath);
	scrapeDefinitions(gamePath);
	localizationScraper.loadLocalizations(gamePath);
	colorMapper.craftReplacementColorMatrix(landedTitles, definitions);
	alterMap(gamePath);
	colorMapper.exportDefinitions(localizationScraper);
}

void MapDegrader::scrapeLandedTitles(const std::string& gamePath)
{
	LOG(LogLevel::Info) << "Scraping Landed Titles";
	if (gamePath.empty())
		landedTitles.loadTitles("00_landed_titles.txt");
	else
		landedTitles.loadTitles(gamePath + "common/landed_titles/00_landed_titles.txt");
	Log(LogLevel::Info) << landedTitles.getTitles().size() << " titles scraped.";
}

void MapDegrader::scrapeDefinitions(const std::string& gamePath)
{
	LOG(LogLevel::Info) << "Scraping Definitions";
	if (gamePath.empty())
		definitions.loadDefinitions("definition.csv");
	else
		definitions.loadDefinitions(gamePath + "map_data/definition.csv");
	Log(LogLevel::Info) << definitions.getDefinitions().size() << " definitions scraped.";
}

void MapDegrader::alterMap(const std::string& gamePath)
{
	Log(LogLevel::Info) << "Altering Map.";
	Magick::Image map;
	if (gamePath.empty())
		map.read("provinces.png");
	else
		map.read(gamePath + "map_data/provinces.png");
	if (!map.isValid())
		throw std::runtime_error("Could not open provinces.png!");

	// Prepare output directory.
	if (!commonItems::TryCreateFolder("export"))
		throw std::runtime_error("Cannot create export folder!");

	// Quarter the map if too huge.
	if (map.size().width() > 4096)
	{
		map.sample("50%");
		Log(LogLevel::Info) << "Scaling down image to a more manageable " << map.size().width() << "x" << map.size().height() << ".";
	}

	// And now, color by color (doing pixel by pixel would require hours)
	Log(LogLevel::Info) << "Swapping colors. This section takes about 10 minutes on a reasonable machine and cannot be rushed.";
	auto counter = 0;
	for (const auto& colorPair: colorMapper.getReplacementMatrix())
	{
		auto swap = Magick::Color(static_cast<Magick::Quantum>(colorPair.first.r()),
			 static_cast<Magick::Quantum>(colorPair.first.g()),
			 static_cast<Magick::Quantum>(colorPair.first.b()));
		auto with = Magick::Color(static_cast<Magick::Quantum>(colorPair.second.r()),
			 static_cast<Magick::Quantum>(colorPair.second.g()),
			 static_cast<Magick::Quantum>(colorPair.second.b()));
		map.opaque(swap, with);
		if (counter % 100 == 0)
			Log(LogLevel::Progress) << std::lround(static_cast<double>(counter) * 100 / static_cast<double>(colorMapper.getReplacementMatrix().size()))
											<< "% complete.";
		counter++;
	}

	map.write("export/provinces.png");
	Log(LogLevel::Info) << "Provinces exported.";

	// Part 2.

	Log(LogLevel::Info) << "Loading rivers.";
	if (gamePath.empty())
		map.read("rivers.png");
	else
		map.read(gamePath + "map_data/rivers.png");
	if (!map.isValid())
		throw std::runtime_error("Could not open rivers.png!");
	if (map.size().width() > 4096)
	{
		map.sample("50%");
		Log(LogLevel::Info) << "Scaling down rivers to a more manageable " << map.size().width() << "x" << map.size().height() << ".";
	}
	map.write("export/rivers.png");
	Log(LogLevel::Info) << "Rivers Exported.";
}
