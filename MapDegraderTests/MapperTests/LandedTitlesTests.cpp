#include "../../MapDegrader/MapDegrader/Source/Mappers/LandedTitlesScraper/LandedTitles.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(MapDegrader_LandedTitlesTests, titlePrimitivesDefaultToBlank)
{
	std::stringstream input;
	LandedTitles titles;
	titles.loadTitles(input);

	ASSERT_TRUE(titles.getDJLiege().empty());
	ASSERT_TRUE(titles.getName().empty());
	ASSERT_FALSE(titles.getProvinceID());
}

TEST(MapDegrader_LandedTitlesTests, titlePrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "province = 345\n";

	LandedTitles titles;
	titles.loadTitles(input, "c_roma", "b_rom");

	ASSERT_EQ("c_roma", titles.getDJLiege());
	ASSERT_EQ("b_rom", titles.getName());
	ASSERT_EQ(345, titles.getProvinceID());
}

TEST(MapDegrader_LandedTitlesTests, titlesDefaultToEmpty)
{
	std::stringstream input;
	LandedTitles titles;
	titles.loadTitles(input);

	ASSERT_TRUE(titles.getTitles().empty());
}

TEST(MapDegrader_LandedTitlesTests, titlesCanBeLoaded)
{
	std::stringstream input;
	input << "b_barony = { province = 12 }\n";
	input << "b_barony2 = { province = 13 }\n";

	LandedTitles titles;
	titles.loadTitles(input);

	const auto& barony = titles.getTitles().find("b_barony");
	const auto& barony2 = titles.getTitles().find("b_barony2");

	ASSERT_EQ(2, titles.getTitles().size());
	ASSERT_EQ(12, barony->second.getProvinceID());
	ASSERT_EQ(13, barony2->second.getProvinceID());
}

TEST(MapDegrader_LandedTitlesTests, titlesCanBeLoadedRecursively)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { province = 12 } } } }\n";
	input << "c_county5 = { b_barony6 = { province = 13 } }\n";

	LandedTitles titles;
	titles.loadTitles(input);

	const auto& barony4 = titles.getTitles().find("b_barony4");
	const auto& barony6 = titles.getTitles().find("b_barony6");

	ASSERT_EQ(6, titles.getTitles().size());
	ASSERT_EQ(12, barony4->second.getProvinceID());
	ASSERT_EQ(13, barony6->second.getProvinceID());
}
