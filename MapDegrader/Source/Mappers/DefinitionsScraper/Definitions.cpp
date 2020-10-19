#include "Definitions.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>

void Definitions::loadDefinitions(std::istream& theStream)
{
	parseStream(theStream);
}

void Definitions::loadDefinitions(const std::string& fileName)
{
	if (!commonItems::DoesFileExist(fileName))
		throw std::runtime_error("Definitions file cannot be found!");

	std::ifstream definitionsFile(fileName);
	parseStream(definitionsFile);
	definitionsFile.close();
}

void Definitions::parseStream(std::istream& theStream)
{
	std::string line;
	getline(theStream, line); // discard first line.

	while (!theStream.eof())
	{
		getline(theStream, line);
		if (line[0] == '#' || line[1] == '#' || line.length() < 4)
			continue;

		auto r = 0;
		auto g = 0;
		auto b = 0;

		ProvinceDefinition definition;
		try
		{
			auto sepLoc = line.find(';');
			if (sepLoc == std::string::npos)
				continue;
			auto sepLocSave = sepLoc;
			definition.provinceID = std::stoi(line.substr(0, sepLoc));
			sepLoc = line.find(';', sepLocSave + 1);
			if (sepLoc == std::string::npos)
				continue;
			r = std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1));
			sepLocSave = sepLoc;
			sepLoc = line.find(';', sepLocSave + 1);
			if (sepLoc == std::string::npos)
				continue;
			g = std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1));
			sepLocSave = sepLoc;
			sepLoc = line.find(';', sepLocSave + 1);
			if (sepLoc == std::string::npos)
				continue;
			b = std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1));
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Line: |" + line + "| is unparseable! Breaking. (" + e.what() + ")");
		}

		definition.color = commonItems::Color(std::array<int, 3>{r, g, b});
		definitions.insert(std::pair(definition.provinceID, definition));
	}
}

std::optional<commonItems::Color> Definitions::getColorForProvinceID(int provinceID) const
{
	const auto& definitionItr = definitions.find(provinceID);
	if (definitionItr != definitions.end())
		return definitionItr->second.color;
	else
		return std::nullopt;
}
