#include <format>
#include <string>

int main() {
		string a = std::format("Test {:2x}", 0xff);
		return 0;
}
