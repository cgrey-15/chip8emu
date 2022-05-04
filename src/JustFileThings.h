#ifndef JUST_FILE_THINGS_H
#define JUST_FILE_THINGS_H

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class JustFileThings {
		public:
				JustFileThings(const char* name);
				std::size_t scoop(std::uint8_t* data, std::size_t size);

		private:
				char filename[32];
				//std::unique_ptr<std::ifstream> fileHandle;
};

namespace MoreFileThings {
		enum class ErrorCode : char {
				NoError=0, FileDidNotOpen, BadFileStream, Unknown
		};

		struct CollectedRom {
				std::optional<const std::vector<std::uint8_t>> data;
				ErrorCode result;
		};

		CollectedRom open_rom(const std::string &filename);
		CollectedRom open_rom(const char *filename);
}

#endif // JUST_FILE_THINGS_H
