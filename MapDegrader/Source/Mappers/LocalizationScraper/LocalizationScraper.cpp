#include "LocalizationScraper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>

void LocalizationScraper::loadLocalizations(const std::string& gamePath)
{
	std::string locPath;
	if (gamePath.empty())
		locPath = "english";
	else
		locPath = gamePath + "/localization/english";

	LOG(LogLevel::Info) << "Scraping localizations from directory: " << locPath;

	if (!commonItems::DoesFolderExist(locPath))
	{
		Log(LogLevel::Error) << "Localization Mapper cannot find proper folder. You need either \"english\" in running dir or " << gamePath
									<< "localization/english.";
		Log(LogLevel::Error) << "Localizations for titles are disabled.";
		return;
	}

	auto fileNames = commonItems::GetAllFilesInFolderRecursive(locPath);
	for (const auto& file: fileNames)
	{
		std::ifstream fileStream(locPath + "/" + file);
		if (fileStream.is_open())
		{
			Log(LogLevel::Info) << "Localization scraping from: " << locPath + "/" + file;
			scrapeStream(fileStream);
		}
		fileStream.close();
	}

	LOG(LogLevel::Info) << localizations.size() << " words read.";
}

void LocalizationScraper::loadLocalizations(std::istream& theStream)
{
	scrapeStream(theStream);
}

void LocalizationScraper::scrapeStream(std::istream& theStream)
{
	while (!theStream.eof())
	{
		std::string line;
		getline(theStream, line);

		if (line[0] == '#' || line[1] == '#' || line.length() < 4)
			continue;

		const auto sepLoc = line.find_first_of(':');
		if (sepLoc == std::string::npos)
			continue;
		const auto key = line.substr(1, sepLoc - 1);
		const auto newLine = line.substr(sepLoc + 1, line.length());
		const auto quoteLoc = newLine.find_first_of('\"');
		const auto quote2Loc = newLine.find_last_of('\"');
		if (quoteLoc == std::string::npos || quote2Loc == std::string::npos || quote2Loc - quoteLoc == 0)
			continue;
		const auto value = newLine.substr(quoteLoc + 1, quote2Loc - quoteLoc - 1);

		localizations[key] = value;
	}
}

std::optional<std::string> LocalizationScraper::getLocForKey(const std::string& key) const
{
	const auto& keyItr = localizations.find(key);
	if (keyItr != localizations.end())
		return keyItr->second;
	return std::nullopt;
}