#include "Definitions.h"
#include <Log.h>
#include <Magick++.h>
#include <OSCompatibilityLayer.h>
#include <fstream>
#include <iomanip>

void Definitions::loadDefinitions(std::istream& theStream)
{
	parseStream(theStream);
}

void Definitions::loadDefinitions(const std::filesystem::path& fileName)
{
	if (!commonItems::DoesFileExist(fileName))
		throw std::runtime_error("Definitions file cannot be found!");

	std::ifstream definitionsFile(fileName);
	parseStream(definitionsFile);
	definitionsFile.close();
}

void Definitions::parseStream(std::istream& theStream)
{
	std::string line;
	getline(theStream, line); // discard first line.

	while (!theStream.eof())
	{
		getline(theStream, line);
		if (line[0] == '#' || line[1] == '#' || line.length() < 4)
			continue;

		unsigned char r;
		unsigned char g;
		unsigned char b;

		ProvinceDefinition definition;
		try
		{
			auto sepLoc = line.find(';');
			if (sepLoc == std::string::npos)
				continue;
			auto sepLocSave = sepLoc;
			definition.provinceID = std::stoi(line.substr(0, sepLoc));
			sepLoc = line.find(';', sepLocSave + 1);
			if (sepLoc == std::string::npos)
				continue;
			r = static_cast<unsigned char>(std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1)));
			sepLocSave = sepLoc;
			sepLoc = line.find(';', sepLocSave + 1);
			if (sepLoc == std::string::npos)
				continue;
			g = static_cast<unsigned char>(std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1)));
			sepLocSave = sepLoc;
			sepLoc = line.find(';', sepLocSave + 1);
			if (sepLoc == std::string::npos)
				continue;
			b = static_cast<unsigned char>(std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1)));
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Line: |" + line + "| is unparseable! Breaking. (" + e.what() + ")");
		}

		definition.chroma = pixelPack(r, g, b);
		definitions.insert(std::pair(definition.provinceID, definition));
		chromaCache.insert(std::pair(definition.chroma, definition.provinceID));
	}
}

std::optional<unsigned int> Definitions::getChromaForProvinceID(const int provinceID) const
{
	const auto& definitionItr = definitions.find(provinceID);
	if (definitionItr != definitions.end())
		return definitionItr->second.chroma;
	else
		return std::nullopt;
}

std::vector<Pixel> Definitions::getPixelsForProvinceID(const int provinceID) const
{
	const auto& definitionItr = definitions.find(provinceID);
	if (definitionItr != definitions.end())
		return definitionItr->second.pixels;
	else
		return std::vector<Pixel>();
}

void Definitions::loadPixelData(Magick::Image& map)
{
	auto map2 = map;
	Magick::Pixels view(map2);
	const auto width = static_cast<int>(map.size().width());
	const auto height = static_cast<int>(map.size().height());
	auto counter = 0;
	const auto pixelCount = static_cast<double>(width) * static_cast<double>(height);
	double progress;
	auto* pixels = view.get(0, 0, width, height);
	Log(LogLevel::Info) << "Loading Pixel Data: " << width << "x" << height;
	auto pixelCounter = 0;
	auto pixelFail = 0;
	std::set<unsigned int> failChromas;
	for (auto y = 0; y < height; ++y)
		for (auto x = 0; x < width; ++x)
		{
			const auto offset = coordsToOffset(x, y, width);
			const auto r = pixels[offset];
			const auto g = pixels[offset + 1];
			const auto b = pixels[offset + 2];
			const auto chroma = pixelPack(r, g, b);

			if (const auto& chromaItr = chromaCache.find(chroma); chromaItr != chromaCache.end())
			{
				definitions[chromaItr->second].pixels.emplace_back(Pixel(x, y));
				++pixelCounter;
			}
			else
			{
				++pixelFail;
				failChromas.emplace(chroma);
			}

			if (counter % 1000000 == 0)
			{
				progress = counter * 100.0 / pixelCount;
				Log(LogLevel::Progress) << std::fixed << std::setprecision(2) << progress << "% complete";
			}
			++counter;
		}
	progress = counter * 100.0 / pixelCount;
	Log(LogLevel::Progress) << std::fixed << std::setprecision(2) << progress << "% complete";
	Log(LogLevel::Info) << "Loaded " << counter << " pixels, " << pixelCounter << " pixels recognized, " << pixelFail << " were not assigned in "
							  << failChromas.size() << " failed chromas.";
}

unsigned int pixelPack(const unsigned char r, const unsigned char g, const unsigned char b)
{
	return r << 16 | g << 8 | b;
}

std::tuple<unsigned char, unsigned char, unsigned char> pixelUnpack(const unsigned int chroma)
{
	const unsigned char r = chroma >> 16 & 0xFF;
	const unsigned char g = chroma >> 8 & 0xFF;
	const unsigned char b = chroma & 0xFF;
	return std::tuple(r, g, b);
}

unsigned int coordsToOffset(const int x, const int y, const int width)
{
	return (y * width + x) * 3;
}
