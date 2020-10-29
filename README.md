# MapDegrader

The latest and brandest in Paradox Game Converters Group portfolio of converters, this converter does one thing and does one thing alone:

It converts the CK3 map into "something usable".

## Features (what it does)

- Autodetection of CK3 install folder for Windows Steam Installations
- Autoscraping of CK3 provinces.png, rivers.png, definition.csv, 00_landed_titles.txt, and localization folders
- Loading of said data
- Export of workable provinces.bmp, rivers.bmp (note format change) and definition.csv so those can be used with our [Province Mapper](https://github.com/ParadoxGameConverters/provinceMapper). 

## What is being done exactly?

- For every county (a collection of baronies), a barony with the lowest provinceID is chosen to be the county's representative in terms of color and provinceID.
- All other baronies on the map (provinces.png) are recolored to that particular color.
- If localization is found for the county, it is loaded from english sources, and converted from UTF8 to WIN1252.
- In definition.csv the exports like these are done:

```
ProvID;r;g;b;title;x;
1;42;3;128;Vestrland;x;
4;168;12;2;Austrland;x;
...
```

- Finally, the recolored and exported files are saved into `export` directory. 

## Why is this being done?

CK3 baronies are inherently indivisible from their counties. Any mapping of said baronies to or from is senseless as only one entity can own the whole lot. 
Thus, it behooves us to map counties instead, and through this method, we give counties (a logical term in CK3) a link to actual clay. It is up to the converters 
themselves to translate that provinceID (belonging to a barony) into a link to the county by re-using 00_landed_titles.txt.

Also, provinces on map are now larger and easier to map to and from. Counties are approx 1:1 compared to CK2 or EU4 provinces so it's more manageable.

## Notes

- For non-steam installations or where autodetection fails, program will expect for all the input files to be present in running directory:

  * provinces.png
  * rivers.png
  * definition.csv
  * 00_landed_titles.txt
  * english/ directory copied from CK3's game/localization/ directory.

- Localization directory is optional and if not found, counties will be named by their canonical c_county names.
- For linux be sure to export LD_PATH to your imagemagick libraries (either installed by build_linux.sh script or preinstalled, so the program can find them.
