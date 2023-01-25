#include "MapDegrader.h"
#include "CommonFunctions.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <iomanip>

void MapDegrader::degradeMap(const std::string& gamePath)
{
	scrapeLandedTitles(gamePath);
	scrapeDefinitions(gamePath);
	localizationScraper.loadLocalizations(gamePath);
	loadMap(gamePath);
	definitions.loadPixelData(map);
	colorMapper.craftReplacementPixelList(landedTitles, definitions);
	alterMap(gamePath);
	colorMapper.exportDefinitions(localizationScraper);
}

void MapDegrader::scrapeLandedTitles(const std::string& gamePath)
{
	Log(LogLevel::Info) << "Scraping Landed Titles";
	if (gamePath.empty())
	{
		for (const auto& filename: commonItems::GetAllFilesInFolder("landed_titles/"))
			if (getExtension(filename) == "txt")
			{
				Log(LogLevel::Info) << "Parsing landed titles: landed_titles/" << filename;
				landedTitles.loadTitles("landed_titles/" + filename);
			}
	}
	else
	{
		for (const auto& filename: commonItems::GetAllFilesInFolder(gamePath + "common/landed_titles/"))
			if (getExtension(filename) == "txt")
			{
				Log(LogLevel::Info) << "Parsing landed titles:" << gamePath + "common/landed_titles/" << filename;
				landedTitles.loadTitles(gamePath + "common/landed_titles/" + filename);
			}
	}
	Log(LogLevel::Info) << landedTitles.getTitles().size() << " titles scraped.";
}

void MapDegrader::scrapeDefinitions(const std::string& gamePath)
{
	LOG(LogLevel::Info) << "Scraping Definitions";
	if (gamePath.empty())
	{
		Log(LogLevel::Info) << "Parsing definitions: definition.csv";
		definitions.loadDefinitions("definition.csv");
	}
	else
	{
		Log(LogLevel::Info) << "Parsing definitions:" << gamePath + "map_data/definition.csv";
		definitions.loadDefinitions(gamePath + "map_data/definition.csv");
	}
	Log(LogLevel::Info) << definitions.getDefinitions().size() << " definitions scraped.";
}

void MapDegrader::loadMap(const std::string& gamePath)
{
	if (gamePath.empty())
		if (commonItems::DoesFileExist("provinces.bmp"))
		{
			Log(LogLevel::Info) << "Loading local provinces.bmp";
			map.read("provinces.bmp");
		}
		else
		{
			Log(LogLevel::Info) << "Loading local provinces.png";
			map.read("provinces.png");
		}
	else if (commonItems::DoesFileExist(gamePath + "map_data/provinces.bmp"))
	{
		Log(LogLevel::Info) << "Loading " << gamePath + "map_data/provinces.bmp";
		map.read(gamePath + "map_data/provinces.bmp");
	}
	else
	{
		Log(LogLevel::Info) << "Loading " << gamePath + "map_data/provinces.png";
		map.read(gamePath + "map_data/provinces.png");
	}
	if (!map.isValid())
		throw std::runtime_error("Could not open provinces.png/bmp!");
}

void MapDegrader::alterMap(const std::string& gamePath)
{
	Log(LogLevel::Info) << "Altering Map.";

	// Prepare output directory.
	if (!commonItems::TryCreateFolder("export"))
		throw std::runtime_error("Cannot create export folder!");

	// And now, chroma by chroma, replace targeted pixels only.
	Log(LogLevel::Info) << "Swapping colors.";
	map.modifyImage();

	Magick::Pixels view(map);
	const auto width = static_cast<int>(map.size().width());
	const auto height = static_cast<int>(map.size().height());
	auto* rawPixels = view.get(0, 0, width, height);

	auto counter = 0;
	auto pixelCounter = 0;
	double progress;
	for (const auto& [chroma, pixels]: colorMapper.getReplacementPixels())
	{
		const auto [r, g, b] = pixelUnpack(chroma);
		for (const auto& pixel: pixels)
		{
			const auto offset = coordsToOffset(pixel.x, pixel.y, width);
			rawPixels[offset] = r;
			rawPixels[offset + 1] = g;
			rawPixels[offset + 2] = b;
			++pixelCounter;
		}
		if (counter % 100 == 0)
		{
			progress = counter * 100.0 / static_cast<double>(colorMapper.getReplacementPixels().size());
			Log(LogLevel::Progress) << std::fixed << std::setprecision(2) << progress << "% complete";
		}
		++counter;
	}
	view.sync();
	progress = counter * 100.0 / static_cast<double>(colorMapper.getReplacementPixels().size());
	Log(LogLevel::Progress) << std::fixed << std::setprecision(2) << progress << "% complete";

	map.write("export/provinces.png");
	Log(LogLevel::Info) << "Provinces exported, " << pixelCounter << " pixels out of " << map.size().width() * map.size().height()
							  << " starting pixels changed.";

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
