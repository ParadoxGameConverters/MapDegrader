#include "MapDegrader.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"

void MapDegrader::degradeMap(const std::string& gamePath)
{	
	scrapeLandedTitles(gamePath);
	scrapeDefinitions(gamePath);
	localizationScraper.loadLocalizations(gamePath);
	loadMap(gamePath);
	definitions.loadPixelData(map);
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

void MapDegrader::loadMap(const std::string& gamePath)
{
	if (gamePath.empty())
		map.read("provinces.png");
	else
		map.read(gamePath + "map_data/provinces.png");
	if (!map.isValid())
		throw std::runtime_error("Could not open provinces.png!");
}


void MapDegrader::alterMap(const std::string& gamePath)
{
	Log(LogLevel::Info) << "Altering Map.";

	// Prepare output directory.
	if (!commonItems::TryCreateFolder("export"))
		throw std::runtime_error("Cannot create export folder!");

	// And now, chroma by chroma, replace targeted pixels only.
	Log(LogLevel::Info) << "Swapping colors. This section takes about 10 minutes on a reasonable machine and cannot be rushed.";
	map.modifyImage();

	Magick::Pixels view(map);
	const auto width = static_cast<int>(map.size().width());
	const auto height = static_cast<int>(map.size().height());
	auto* rawPixels = view.get(0, 0, width, height);

	auto counter = 0;
	for (const auto& [chroma, pixels]: colorMapper.getReplacementPixels())
	{
		const auto [r, g, b] = pixelUnpack(chroma);
		for (const auto& pixel: pixels)
		{
			const auto offset = coordsToOffset(pixel.x, pixel.y, width);
			rawPixels[offset] = r;
			rawPixels[offset + 1] = g;
			rawPixels[offset + 2] = b;
		}
		++counter;
		if (counter % 100 == 0)
			Log(LogLevel::Progress) << std::to_string(counter * 100.0 / static_cast<double>(colorMapper.getReplacementPixels().size())) << "% complete";
	}
	view.sync();

	map.write("export/provinces.png");
	Log(LogLevel::Info) << "Provinces exported.";

	// Part 2. We'll read and re-write the rivers to fix potential sub-format issues.

	Log(LogLevel::Info) << "Loading rivers.";
	if (gamePath.empty())
		map.read("rivers.png");
	else
		map.read(gamePath + "map_data/rivers.png");
	if (!map.isValid())
		throw std::runtime_error("Could not open rivers.png!");
	map.write("export/rivers.png");
	Log(LogLevel::Info) << "Rivers Exported.";
}
