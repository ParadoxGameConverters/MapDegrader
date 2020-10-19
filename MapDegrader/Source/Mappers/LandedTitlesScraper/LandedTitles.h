#ifndef LANDED_TITLES_H
#define LANDED_TITLES_H
#include "Parser.h"

class LandedTitles: commonItems::parser
{
  public:
	void loadTitles(std::istream& theStream, const std::string& theDJLiege, const std::string& theName);
	void loadTitles(const std::string& fileName);
	void loadTitles(std::istream& theStream);

	[[nodiscard]] auto getProvinceID() const { return provinceID; }
	[[nodiscard]] const auto& getDJLiege() const { return djLiege; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getTitles() const { return titles; }

  private:
	void registerKeys();

	int provinceID = 0;
	std::string djLiege;
	std::string name;
	std::map<std::string, LandedTitles> titles;
};

#endif // LANDED_TITLES_H
