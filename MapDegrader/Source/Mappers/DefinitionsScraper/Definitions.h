#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include "Color.h"
#include <map>

struct ProvinceDefinition
{
	int provinceID;
	commonItems::Color color;
};

class Definitions
{
  public:
	void loadDefinitions(const std::string& fileName);
	void loadDefinitions(std::istream& theStream);

	[[nodiscard]] std::optional<commonItems::Color> getColorForProvinceID(int provinceID) const;
	[[nodiscard]] const auto& getDefinitions() const { return definitions; }

  private:
	void parseStream(std::istream& theStream);

	std::map<int, ProvinceDefinition> definitions;
};

#endif // DEFINITIONS_H
