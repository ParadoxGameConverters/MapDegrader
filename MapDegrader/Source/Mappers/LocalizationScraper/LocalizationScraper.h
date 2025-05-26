#ifndef LOCALIZATION_SCRAPER
#define LOCALIZATION_SCRAPER
#include <filesystem>
#include <map>
#include <optional>
#include <string>

class LocalizationScraper
{
  public:
	void loadLocalizations(const std::filesystem::path& gamePath);
	void loadLocalizations(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getLocForKey(const std::string& key) const;

  private:
	void scrapeStream(std::istream& theStream);

	std::map<std::string, std::string> localizations; // key/loc.
};

#endif // LOCALIZATION_SCRAPER