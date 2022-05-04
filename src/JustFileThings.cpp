#include "JustFileThings.h"
#include <array>
#include <fstream>
#include <ios>


MoreFileThings::CollectedRom MoreFileThings::open_rom(const std::string &filename) {
		return open_rom(filename.c_str());
}

MoreFileThings::CollectedRom MoreFileThings::open_rom(const char *filename) {

		std::ifstream stream{filename, std::ifstream::binary};

		MoreFileThings::CollectedRom ret_val{{}, ErrorCode::NoError};

		if(stream.is_open()) {
				std::vector<uint8_t> payload{};

				constexpr int BUFFER_SIZE = 24;
				std::array<char, BUFFER_SIZE> buffer;

				while( stream.read(buffer.data(), buffer.size()) ) {
						payload.insert(payload.end(), buffer.begin(), buffer.begin()+stream.gcount());
				}

				if (!stream.eof()) {
					ret_val.result = ErrorCode::Unknown;
				}
				else {
					payload.insert(payload.end(), buffer.begin(), buffer.begin() + stream.gcount());
					ret_val.data.emplace(std::move(payload));
				}
		}
		else {
				ret_val.result = ErrorCode::FileDidNotOpen;
		}

		
		return ret_val;
}
