#include "../../MapDegrader/MapDegrader/Source/Mappers/ColorMapper/ColorMapper.h"
#include "../../MapDegrader/MapDegrader/Source/Mappers/DefinitionsScraper/Definitions.h"
#include "../../MapDegrader/MapDegrader/Source/Mappers/LandedTitlesScraper/LandedTitles.h"
#include "../../MapDegrader/MapDegrader/Source/Mappers/LocalizationScraper/LocalizationScraper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(MapDegrader_ColorMapperTests, matrixCanBeBuilt)
{
	std::stringstream titleStream;
	// We'll be replacing barony5's and barony6's color on map with barony4's as barony4 has lowest provinceID.
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

	Magick::Image testImage(Magick::Geometry(3, 2), "white");
	testImage.modifyImage();
	Magick::Pixels view(testImage);
	auto* pixels = view.get(0, 1, 3, 1); // we'll color lower half to our provinces, one pixel per province.
	*pixels++ = 1;
	*pixels++ = 2;
	*pixels++ = 3;
	*pixels++ = 4;
	*pixels++ = 5;
	*pixels++ = 6;
	*pixels++ = 7;
	*pixels++ = 8;
	*pixels++ = 9;
	view.sync();

	definitions.loadPixelData(testImage); // assign that lower half to our defined provinces

	ColorMapper colorMapper;
	colorMapper.craftReplacementPixelList(titles, definitions);

	const auto& replacementPixels = colorMapper.getReplacementPixels(); // matrix tells us which pixels to replace with a chroma.

	// there should be 2 pixels inside a single map key, (2, 1) and (1, 1), responding to provIDs 17 and 13, respectively
	ASSERT_EQ(1, replacementPixels.size());

	const auto chroma = pixelPack(1, 2, 3);
	const auto calculatedPixels = replacementPixels.find(chroma)->second;

	EXPECT_EQ(2, calculatedPixels.size());
	EXPECT_EQ(1, calculatedPixels[0].x);
	EXPECT_EQ(1, calculatedPixels[0].y);
	EXPECT_EQ(2, calculatedPixels[1].x);
	EXPECT_EQ(1, calculatedPixels[1].y);
}

TEST(MapDegrader_ColorMapperTests, matrixWillThrowExceptionOnMissingDefinitions)
{
	std::stringstream titleStream;
	// We'll be replacing barony5's and barony6's color on map with barony4's as barony4 has lowest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n";
	definitionStream << "#13;4;5;6;c_title2;x;\n"; // <- missing definition.
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	ASSERT_THROW(colorMapper.craftReplacementPixelList(titles, definitions), std::runtime_error);
}

TEST(MapDegrader_ColorMapperTests, matrixWillNotThrowExceptionOnExtraDefinitions)
{
	std::stringstream titleStream;
	// We'll be replacing barony5's and barony6's color on map with barony4's as barony4 has lowest provinceID.
	titleStream << "c_county3 = { b_barony4 = { province = 12 } b_barony5 = { province = 17 } b_barony6 = { province = 13 } }\n";
	LandedTitles titles;
	titles.loadTitles(titleStream);

	std::stringstream definitionStream;
	definitionStream << "comment\n";
	definitionStream << "12;1;2;3;c_title1;x;\n";
	definitionStream << "13;4;5;6;c_title2;x;\n";
	definitionStream << "17;7;8;9;IRRELEVANT;x;\n";
	definitionStream << "19;4;5;6;c_title2;x;\n"; // <- extra definition, leftovers and such.
	Definitions definitions;
	definitions.loadDefinitions(definitionStream);

	ColorMapper colorMapper;
	ASSERT_NO_THROW(colorMapper.craftReplacementPixelList(titles, definitions));
}

TEST(MapDegrader_ColorMapperTests, newDefinitionsCanBeExported)
{
	std::stringstream titleStream;
	// We'll be replacing barony5's and barony6's color on map with barony4's as barony4 has lowest provinceID.
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
	colorMapper.craftReplacementPixelList(titles, definitions);

	std::stringstream locStream;
	locStream << "l_english:\n";
	locStream << " c_county3:0 \"The County\" # comment\n";
	LocalizationScraper localizations;
	localizations.loadLocalizations(locStream);

	// This is expected result. County3 mapped to lowest barony # (12), using that barony's colors.
	const auto* output =
		 "ProvID;r;g;b;title;x;\n"
		 "12;1;2;3;The County;x;\n";

	ASSERT_EQ(output, colorMapper.exportDefinitionsToString(localizations));
}

TEST(MapDegrader_ColorMapperTests, newDefinitionsCanBeExportedWithoutLocalizationsLoaded)
{
	std::stringstream titleStream;
	// We'll be replacing barony5's and barony6's color on map with barony4's as barony4 has lowest provinceID.
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
	colorMapper.craftReplacementPixelList(titles, definitions);

	std::stringstream locStream;
	LocalizationScraper localizations;
	localizations.loadLocalizations(locStream);

	// Locs will default to title name
	const auto* output =
		 "ProvID;r;g;b;title;x;\n"
		 "12;1;2;3;c_county3;x;\n";

	ASSERT_EQ(output, colorMapper.exportDefinitionsToString(localizations));
}
