#include "FileUtils.h"
#include <filesystem>

namespace Minecraft {
	namespace Utils {

		std::string getFilePath(const std::string& relativeFilePath) {
			return std::filesystem::absolute(std::filesystem::path(relativeFilePath)).string();
		}

		std::string getProjectPath() {
			std::string currentPath = std::filesystem::current_path().string();
			return currentPath.substr(0, currentPath.find("Utils"));
		}

	}
}