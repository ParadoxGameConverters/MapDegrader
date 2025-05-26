#include "../../MapDegrader/MapDegrader/Source/Mappers/DefinitionsScraper/Definitions.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(MapDegrader_DefinitionsTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	Definitions definitions;
	definitions.loadDefinitions(input);

	ASSERT_TRUE(definitions.getDefinitions().empty());
}

TEST(MapDegrader_DefinitionsTests, firstLineIsIgnored)
{
	std::stringstream input;
	input << "comment\n";

	Definitions definitions;
	definitions.loadDefinitions(input);

	ASSERT_TRUE(definitions.getDefinitions().empty());
}

TEST(MapDegrader_DefinitionsTests, definitionsCanBeLoaded)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;1;2;3;c_title1;x;\n"; // titles are ignored.
	input << "2;5;6;7;c_title2;x;\n";
	definitions.loadDefinitions(input);

	const auto d1 = definitions.getDefinitions().find(1);
	const auto d2 = definitions.getDefinitions().find(2);

	const auto chroma1 = pixelPack(1, 2, 3);
	const auto chroma2 = pixelPack(5, 6, 7);

	ASSERT_EQ(2, definitions.getDefinitions().size());
	ASSERT_EQ(chroma1, d1->second.chroma);
	ASSERT_EQ(chroma2, d2->second.chroma);
}

TEST(MapDegrader_DefinitionsTests, commentsAndEmptyLinesAreIgnored)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;1;2;3;c_title1;x;\n";
	input << "# comment\n";
	input << "\n";
	input << "2;5;6;7;c_title2;x; # commentmore\n";
	input << "3;1;3;4;c_title3;x; # commentmore;x;x;1;2;3;x;x;\n";
	definitions.loadDefinitions(input);

	ASSERT_EQ(3, definitions.getDefinitions().size());
}

TEST(MapDegrader_DefinitionsTests, chromaMatchOnIDReturnsChroma)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;1;2;3;c_title1;x;\n";
	definitions.loadDefinitions(input);

	const auto chroma = pixelPack(1, 2, 3);

	ASSERT_EQ(chroma, definitions.getChromaForProvinceID(1));
}

TEST(MapDegrader_DefinitionsTests, chromaMismatchOnIDReturnsNothing)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;1;2;3;c_title1;x;\n";
	definitions.loadDefinitions(input);

	ASSERT_FALSE(definitions.getChromaForProvinceID(2));
}

TEST(MapDegrader_DefinitionsTests, pixelsCanBeLoaded)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;255;0;0;c_title1;x;\n";
	definitions.loadDefinitions(input);

	Magick::Image testImage(Magick::Geometry(2, 2), "white");
	testImage.modifyImage();
	Magick::Pixels view(testImage);
	auto* pixels = view.get(0, 1, 2, 1); // we'll color lower half to red - starting at 0x1, 2 columns, 1 row
	for (auto column = 0; column < 2; ++column)
	{
		*pixels++ = 255;
		*pixels++ = 0;
		*pixels++ = 0;
	}
	view.sync();

	definitions.loadPixelData(testImage); // assign that lower half to our defined province

	const auto& provincePixels = definitions.getPixelsForProvinceID(1); // should be (0, 1) and (1, 1) as those are red

	ASSERT_EQ(2, provincePixels.size());
	ASSERT_EQ(0, provincePixels[0].x);
	ASSERT_EQ(1, provincePixels[0].y);
	ASSERT_EQ(1, provincePixels[1].x);
	ASSERT_EQ(1, provincePixels[1].y);
}
