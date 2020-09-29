#include "../../MapDegrader/MapDegrader/Source/Mappers/DefinitionsScraper/Definitions.h"
#include "gtest/gtest.h"
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

	ASSERT_EQ(2, definitions.getDefinitions().size());
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{1, 2, 3}), d1->second.color);
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{5, 6, 7}), d2->second.color);
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

TEST(MapDegrader_DefinitionsTests, matchOnIDReturnsColor)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;1;2;3;c_title1;x;\n";
	definitions.loadDefinitions(input);

	ASSERT_EQ(commonItems::Color(std::array<int, 3>{1, 2, 3}), definitions.getColorForProvinceID(1));
}

TEST(MapDegrader_DefinitionsTests, mismatchOnIDReturnsNothing)
{
	std::stringstream input;
	Definitions definitions;
	input << "comment\n";
	input << "1;1;2;3;c_title1;x;\n";
	definitions.loadDefinitions(input);

	ASSERT_FALSE(definitions.getColorForProvinceID(2));
}
