#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include "Color.h"
#include <map>
#include "Magick++.h"

struct Pixel
{
	Pixel(const int ix, const int iy)
	{
		x = ix;
		y = iy;
	}
	int x = 0;
	int y = 0;	
};

struct ProvinceDefinition
{
	int provinceID = 0;
	unsigned int chroma = 0;
	std::vector<Pixel> pixels;
};

class Definitions
{
  public:
	void loadDefinitions(const std::string& fileName);
	void loadDefinitions(std::istream& theStream);
	void loadPixelData(Magick::Image& map);

	[[nodiscard]] std::optional<unsigned int> getChromaForProvinceID(int provinceID) const;
	[[nodiscard]] const auto& getDefinitions() const { return definitions; }
	[[nodiscard]] std::vector<Pixel> getPixelsForProvinceID(int provinceID) const;

  private:
	void parseStream(std::istream& theStream);

	std::map<int, int> chromaCache; // chroma, provinceID
	std::map<int, ProvinceDefinition> definitions;
};

unsigned int pixelPack(unsigned char r, unsigned char g, unsigned char b);
std::tuple<unsigned char, unsigned char, unsigned char> pixelUnpack(unsigned int chroma);
	 int coordsToOffset(int x, int y, int width);

#endif // DEFINITIONS_H
