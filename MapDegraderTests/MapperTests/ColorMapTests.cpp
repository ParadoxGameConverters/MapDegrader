#include "../../MapDegrader/MapDegrader/Source/Mappers/ColorMapper/ColorMapper.h"
#include "../../MapDegrader/MapDegrader/Source/Mappers/LandedTitlesScraper/LandedTitles.h"
#include "../../MapDegrader/MapDegrader/Source/Mappers/DefinitionsScraper/Definitions.h"
#include "../../MapDegrader/MapDegrader/Source/Mappers/LocalizationScraper/LocalizationScraper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(MapDegrader_ColorMapperTests, matrixCanBeBuilt)
{
	std::stringstream titleStream;
	// We'll be replacing barony4's and barony6's color on map with barony5's as barony5 has highest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n"; // ignoring useless titles.
	definitionStream << "13;4;5;6;c_title2;x;\n";
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	colorMapper.craftReplacementColorMatrix(titles, definitions);

	const auto& matrix = colorMapper.getReplacementMatrix(); // matrix tells us which colors to replace with which others.

	ASSERT_EQ(2, matrix.size());
	
	const auto& pair1 = matrix[0];
	const auto& pair2 = matrix[1];

	ASSERT_EQ(commonItems::Color(std::array<int, 3>{1, 2, 3}), pair1.first);
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{7, 8, 9}), pair1.second);
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{4, 5, 6}), pair2.first);
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{7, 8, 9}), pair2.second);
}

TEST(MapDegrader_ColorMapperTests, matrixWillThrowExceptionOnMissingDefinitions)
{
	std::stringstream titleStream;
	// We'll be replacing barony4's and barony6's color on map with barony5's as barony5 has highest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n";
	definitionStream << "#13;4;5;6;c_title2;x;\n";  // <- missing definition.
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	ASSERT_THROW(colorMapper.craftReplacementColorMatrix(titles, definitions), std::runtime_error);
}

TEST(MapDegrader_ColorMapperTests, matrixWillNotThrowExceptionOnExtraDefinitions)
{
	std::stringstream titleStream;
	// We'll be replacing barony4's and barony6's color on map with barony5's as barony5 has highest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n";
	definitionStream << "13;4;5;6;c_title2;x;\n";
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	definitionStream << "19;4;5;6;c_title2;x;\n";  // <- extra definition, leftovers and such.
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	ASSERT_NO_THROW(colorMapper.craftReplacementColorMatrix(titles, definitions));
}

TEST(MapDegrader_ColorMapperTests, newDefinitionsCanBeExported)
{
	std::stringstream titleStream;
	// We'll be replacing barony4's and barony6's color on map with barony5's as barony5 has highest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n";
	definitionStream << "13;4;5;6;c_title2;x;\n";
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	colorMapper.craftReplacementColorMatrix(titles, definitions);

	std::stringstream locStream;
	locStream << "l_english:\n";
	locStream << " c_county3:0 \"The County\" # comment\n";
	LocalizationScraper localizations;
	localizations.loadLocalizations(locStream);

	// This is expected result. County3 mapped to highest barony # (17), using that barony's colors.
	const auto* output = "ProvID;r;g;b;title;x;\n17;7;8;9;The County;x;\n";

	ASSERT_EQ(output, colorMapper.exportDefinitionsToString(localizations));
}

TEST(MapDegrader_ColorMapperTests, newDefinitionsCanBeExportedWithoutLocalizationsLoaded)
{
	std::stringstream titleStream;
	// We'll be replacing barony4's and barony6's color on map with barony5's as barony5 has highest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n";
	definitionStream << "13;4;5;6;c_title2;x;\n";
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	colorMapper.craftReplacementColorMatrix(titles, definitions);

	std::stringstream locStream;
	LocalizationScraper localizations;
	localizations.loadLocalizations(locStream);

	// Locs will default to title name
	const auto *output = "ProvID;r;g;b;title;x;\n17;7;8;9;c_county3;x;\n";

	ASSERT_EQ(output, colorMapper.exportDefinitionsToString(localizations));
}
