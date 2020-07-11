#include <string>
#include <boost/filesystem.hpp>


namespace ia {
	std::string get_file_extension(const std::string& path)
	{
		std::string ext = boost::filesystem::extension(boost::filesystem::path(path));
		return ext;
	}
}