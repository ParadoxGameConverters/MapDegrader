#include "../../MapDegrader/MapDegrader/Source/Mappers/LocalizationScraper/LocalizationScraper.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(MapDegrader_LocalizationsTests, matchReturnsCorrectLoc)
{
	std::stringstream input;
	input << "l_english:\n";
	input << " key:0 \"value\"\n";
	LocalizationScraper localizations;
	localizations.loadLocalizations(input);

	ASSERT_EQ("value", *localizations.getLocForKey("key"));
}

TEST(MapDegrader_LocalizationsTests, mismatchReturnsNothing)
{
	std::stringstream input;
	input << "l_english:\n";
	input << " key:0 \"value\"\n";
	LocalizationScraper localizations;
	localizations.loadLocalizations(input);

	ASSERT_FALSE(localizations.getLocForKey("key2"));
}

TEST(MapDegrader_LocalizationsTests, commentsAreIgnored)
{
	std::stringstream input;
	input << "l_english:\n";
	input << " key:0 \"value\" # comment\n";
	input << " #key:0 \"value2\"\n";

	LocalizationScraper localizations;
	localizations.loadLocalizations(input);

	ASSERT_EQ("value", *localizations.getLocForKey("key"));
}

TEST(MapDegrader_LocalizationsTests, laterLocsOverridePrevious)
{
	std::stringstream input;
	input << "l_english:\n";
	input << " key: \"value\"\n";
	input << " key: \"value2\"\n";

	LocalizationScraper localizations;
	localizations.loadLocalizations(input);

	ASSERT_EQ("value2", *localizations.getLocForKey("key"));
}

TEST(MapDegrader_LocalizationsTests, brokenLocsAreNotHandledNotOurFaultLocalizationScraperDidNothingWrong)
{
	std::stringstream input;
	input << "l_english:\n";
	input << " key: \"value\" key2: \"value2\"\n";

	LocalizationScraper localizations;
	localizations.loadLocalizations(input);

	ASSERT_EQ("value\" key2: \"value2", *localizations.getLocForKey("key"));
}
