#ifndef LANDED_TITLES_H
#define LANDED_TITLES_H
#include "Parser.h"

class LandedTitles: commonItems::parser
{
public:
	void loadTitles(std::istream& theStream, const std::string& theDJLiege, const std::string& theName);
	void loadTitles(const std::string& fileName);
	void loadTitles(std::istream& theStream);

	[[nodiscard]] const auto& getTitles() const { return titles; }
	[[nodiscard]] const auto& getDJLiege() const { return djLiege; }
	[[nodiscard]] const auto& getName()const { return name; }
	[[nodiscard]] auto getProvinceID() const { return provinceID; }

  private:
	void registerKeys();

	std::string djLiege;
	int provinceID = 0;
	std::string name;
	std::map<std::string, LandedTitles> titles;

};

#endif // LANDED_TITLES_H
