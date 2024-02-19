#include <vr/FileSystem.h>
#include <limits>
#include <iostream>
using namespace vr;


#if defined(_WIN32)
const char *FileSystem::PATH_SEPARATOR = "\\";
#else
const char *FileSystem::PATH_SEPARATOR = "/";
#endif

const char *FileSystem::UNIX_PATH_SEPARATOR = "/";
const char* FileSystem::WINDOWS_PATH_SEPARATOR = "\\";


#if defined(_WIN32)
const char *FileSystem::PATH_DELIMITER = ";";
#else
const char *FileSystem::PATH_DELIMITER = ":";
#endif






#ifdef _MSC_VER
#define NOMINMAX
#include <direct.h>

#else
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h> 
#include <stdlib.h> 

#endif

/*----------------------------------------------------------------------------
Extract directory from path (remove filename)
----------------------------------------------------------------------------*/
std::string vr::FileSystem::getPath(const std::string& path)
{
	std::string p = convertToNativeFilePath(path);
	std::string::size_type slash = p.find_last_of(PATH_SEPARATOR);

	if (slash == std::string::npos)
		return std::string("");

	return std::string(p.begin(), p.begin() + slash + 1);

}



std::string vr::FileSystem::convertUnixFilePathToWindows(const std::string &unixStyle)
{
	std::string windowsStyle(unixStyle);

	
	std::string::size_type separator = 0;
	while ((separator = windowsStyle.find_first_of(UNIX_PATH_SEPARATOR, separator)) != std::string::npos)
	{
		windowsStyle[separator] = WINDOWS_PATH_SEPARATOR[0];
	}

	// Make sure we dont have starting backslash if absolute path
	if (windowsStyle.size() > 0 && (windowsStyle.substr(0,1) == PATH_SEPARATOR) && windowsStyle.size() > 3 && windowsStyle[2] == ':')
		windowsStyle = windowsStyle.substr(1);

	return windowsStyle;
}

std::string vr::FileSystem::convertWindowsFilePathToUnix(const std::string& windowsStyle)
{
	std::string unixStyle(windowsStyle);

	std::string::size_type separator = 0;
	while ((separator = unixStyle.find_first_of(WINDOWS_PATH_SEPARATOR, separator)) != std::string::npos)
	{
		unixStyle[separator] = UNIX_PATH_SEPARATOR[0];
	}

	return unixStyle;
}

std::string vr::FileSystem::convertToNativeFilePath(const std::string& filePath)
{
#if defined(_WIN32)
	return convertUnixFilePathToWindows(filePath);
#else
	return convertWindowsFilePathToUnix(filePath);
#endif
}



std::string vr::FileSystem::getFileExtension(const std::string& fileName)
{
	std::string::size_type dot = fileName.find_last_of('.');
	if (dot == std::string::npos) return std::string("");
	return std::string(fileName.begin() + dot + 1, fileName.end());
}


std::string vr::FileSystem::getFileWithoutExtension(const std::string& fileName)
{
	std::string nPath = convertToNativeFilePath(fileName);
	std::string::size_type slash = nPath.find_last_of(PATH_SEPARATOR);

	if (slash != std::string::npos)
	{

		if (slash != nPath.length() - 1)  // Make sure the path does not end with a delimiter
			nPath = nPath.substr(slash + 1, nPath.length());
		else
			return ""; // Path ends with a directory delimiter->no filename
	}

	// Now remove everything beyond the dot
	std::string::size_type dot = nPath.find_last_of('.');
	if (dot == std::string::npos) return nPath;

	std::string path = nPath.substr(0, dot);
	return path;
}



std::string vr::FileSystem::getFileWithExtension(const std::string& inFileName)
{
	std::string path = convertToNativeFilePath(inFileName);

	std::string::size_type slash = path.find_last_of(PATH_SEPARATOR);

	if (slash == std::string::npos)
		return path;

	return std::string(path.begin() + slash + 1, path.end());
}

std::string vr::FileSystem::getDirectory(const std::string& filePath)
{
	std::string path = convertToNativeFilePath(filePath);
	std::string::size_type slash = path.find_last_of(PATH_SEPARATOR);

	return path.substr(0, slash);
}

/**
Remove double delimiters, ../ and ./ sections
*/
static std::string cleanupPath(const std::string& in)
{
	std::string path = FileSystem::convertWindowsFilePathToUnix(in);
	

	std::string result;

	// Ok what is this?
	// A network disk on windows is \\address\path
	// THe first \\ is important, the code below would remove one of these, so we want to keep them
	// So just keep the two first // (which is converted to \\ later) if we are running on a windows machine
	size_t pos = 0;
	while (pos != std::string::npos)
	{
		// Go to first character after, strip multiple redundant path delimiters, eg foo///bar -> foo/bar
		while (pos != std::string::npos && pos < path.size() && path[pos] == '/')
			pos++;

		// Handle .. section, eg foo/bar/../hej -> foo/hej
		if (path.compare(pos, 2, "..") == 0 && (pos + 2 == path.size() || path[pos + 2] == '/'))
		{
			// Remove last component from result path
			size_t prev = result.rfind("/");
			
			result = result.substr(0, prev);
			pos += 2;
			continue;
		}

		// Skip ./ sections, eg foo/bar/././hej -> foo/bar/hej
		if (path.compare(pos, 1, ".") == 0 && (pos + 1 == path.size() || path[pos + 1] == '/'))
		{
			pos += 1;
			continue;
		}


		size_t next = path.find("/", int(pos));
#ifdef _MSC_VER
		if (pos == 2) {
			result = path.substr(0, next);
		}
		else {
#endif
			if (next != std::string::npos)
				result += "/" + path.substr(pos, next - pos);
			else if (pos < path.size())
				result += "/" + path.substr(pos);
#ifdef _MSC_VER
		}
#endif
		pos = next;
	}

	// std::cout << "path: " << path << ", result: " << result << std::endl;
	return FileSystem::convertToNativeFilePath(result);
}

bool FileSystem::isAbsolute(const std::string& path)
{
#ifdef _MSC_VER
	bool netPath = path.size() > 1 && path.substr(0,2) == "\\\\";
	bool withDisk = (path.length() > 1 && path[1] == ':');
	bool justRoot = path.size() > 0 && path.substr(0,1)  == "\\" || path.substr(0,1) == "/";
	return netPath || withDisk || justRoot;
#else
	return path.size() > 1 && path.substr(0,1) == "/";
#endif
}


std::string FileSystem::getAbsolutePath(const std::string& relativePath)
{
	if (isAbsolute(relativePath))
		return cleanupPath(relativePath);

	std::string fullPath;
#if 0
#ifdef _MSC_VER
	char full[_MAX_PATH];
	if (_fullpath(full, relativePath.c_str(), _MAX_PATH) != NULL)
		fullPath = full;
#else
#endif
#endif

#ifdef _MSC_VER
	char work[_MAX_PATH];
	char *tmp = _getcwd(work, _MAX_PATH);
	fullPath = (tmp ? tmp : "") + std::string("/") + relativePath;
#elif defined(__APPLE__)
	char work[MAXPATHLEN];
	char *tmp = getcwd(work, MAXPATHLEN);
	fullPath = (tmp ? tmp : "") + std::string("/") + relativePath;
#else
	char *currdir = get_current_dir_name();
	if (currdir)
	{
		fullPath = currdir;
		free(currdir);
		currdir = 0;
	}
	fullPath = fullPath + std::string("/") + relativePath;
#endif

	return cleanupPath(fullPath);
}


/// \todo This will not work for windows. We need to use some wstat method and then struct stat* is not the right type.
int vr::FileSystem::stat(const std::string& name, StatStruct* buf)
{
	// Need this test due to bug in stat in VS:
	// http://code.google.com/p/drmemory/issues/detail?can=2&start=0&num=100&q=&colspec=ID%20Type%20Status%20Priority%20Milestone%20Owner%20Summary&groupby=&sort=&id=1298
	if (name.length() == 0)
		return -1;

	int ret = -1;

#ifdef _MSC_VER
	ret = _stat(name.c_str(), buf);
#else
	ret = ::stat(name.c_str(), buf);
#endif

	return ret;
}


std::string vr::FileSystem::findFile(const std::string& filename, const char* environmentVariable)
{
	// First check if the specified file exists
	bool exist = vr::FileSystem::exists(filename);
	
	// If it exists, just return it
	if (exist)
		return filename;

	std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
	if (vrPath.empty()) {
		std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;	
		return "";
	}


	std::string filepath = std::string(vrPath) + "/" + filename;
	exist = vr::FileSystem::exists(filepath);
	if (!exist)
	{
		std::cerr << "Unable to located file: " << filename << std::endl;
		return "";
	}

	// Return the found file
	return convertToNativeFilePath(filepath);
}

bool vr::FileSystem::exists(const std::string& path)
{
	StatStruct temp;
	return vr::FileSystem::stat(path, &temp) == 0;
}

#ifdef _MSC_VER
std::string vr::FileSystem::getEnv(const std::string& variable)
{
  char *pValue;
  size_t len;
  errno_t err = _dupenv_s(&pValue, &len, variable.c_str());
  if (err) return "";
  std::string value(pValue);
  free(pValue);

  return value;
}
#else
#include <stdlib.h>
std::string vr::FileSystem::getEnv(const std::string& variable)
{
  std::string result;

  const char *val = ::getenv(variable.c_str());
  if (val)
    result = val;

  return result;
}

#endif

static void split_whitespace(const std::string& str, std::vector< std::string >& result, int maxsplit)
{
	size_t i, j, len = str.size();
	for (i = j = 0; i < len; )
	{

		while (i < len && ::isspace(static_cast<unsigned char>(str[i]))) i++;
		j = i;

		while (i < len && !::isspace(static_cast<unsigned char>(str[i]))) i++;



		if (j < i)
		{
			if (maxsplit-- <= 0) break;

			result.push_back(str.substr(j, i - j));

			while (i < len && ::isspace(static_cast<unsigned char>(str[i]))) i++;
			j = i;
		}
	}
	if (j < len)
	{
		result.push_back(str.substr(j, len - j));
	}
}

static void split(const std::string& str, std::vector< std::string >& result, const std::string& sep, int maxsplit)
{
	result.clear();

	if (maxsplit < 0) maxsplit = std::numeric_limits<int>::max();


	if (sep.size() == 0)
	{
		split_whitespace(str, result, maxsplit);
		return;
	}

	size_t i, j, len = str.size(), n = sep.size();

	i = j = 0;

	while (i + n <= len)
	{
		if (str[i] == sep[0] && str.substr(i, n) == sep)
		{
			if (maxsplit-- <= 0) break;

			result.push_back(str.substr(j, i - j));
			i = j = i + n;
		}
		else
		{
			i++;
		}
	}

	result.push_back(str.substr(j, len - j));
}

std::vector<std::string> vr::FileSystem::splitPath(const std::string& path)
{
	auto nativePath = convertToNativeFilePath(path);

	std::vector<std::string> components;
	split(nativePath, components, PATH_SEPARATOR, -1);
	return components;
}

