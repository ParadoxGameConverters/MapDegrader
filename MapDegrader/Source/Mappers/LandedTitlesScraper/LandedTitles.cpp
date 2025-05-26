#include "LandedTitles.h"
#include <CommonRegexes.h>
#include <Log.h>
#include <ParserHelpers.h>

void LandedTitles::loadTitles(std::istream& theStream, const std::string& theDJLiege, const std::string& theName)
{
	djLiege = theDJLiege;
	name = theName;
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void LandedTitles::loadTitles(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void LandedTitles::loadTitles(const std::filesystem::path& fileName)
{
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
}

void LandedTitles::registerKeys()
{
	registerRegex(R"((e|k|d|c|b)_[A-Za-z0-9_\-\']+)",
		 [this](const std::string& titleName, std::istream& theStream)
		 {
			 LandedTitles newTitle;
			 newTitle.loadTitles(theStream, name, titleName);

			 // Store ourselves
			 titles.insert(std::pair(titleName, newTitle));

			 // Pull the titles beneath this one and add them to the lot.
			 const auto& locatedTitles = newTitle.getTitles();
			 titles.insert(locatedTitles.begin(), locatedTitles.end());
		 });
	registerKeyword("province",
		 [this](const std::string& unused, std::istream& theStream)
		 {
			 provinceID = commonItems::singleInt(theStream).getInt();
		 });
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
