
#pragma once
#include <string>
#include <sys/stat.h>
#include <vector>
namespace vr {

	struct FileSystem {

#ifdef _MSC_VER
		typedef struct _stat StatStruct;
#else
		typedef struct stat StatStruct;
#endif

		/// Separator between directories in a file path: on unix "/", on windows "\\"
		static const char* PATH_SEPARATOR;

		/// Path separator under UNIX "/"
		static const char* UNIX_PATH_SEPARATOR;

		/// Path separator under Windows "\\"
		static const char* WINDOWS_PATH_SEPARATOR;

		/// Separator between paths in a list of paths, on unix: ":", on windows ";"
		static const char* PATH_DELIMITER;

		static std::string findFile(const std::string& filename, const char* environmentVariable = "VR_PATH");

		/**
		\param unixStyle - Path in unix style (with / separators)
		\return path in Windows style (with \\)
		*/
		static std::string convertUnixFilePathToWindows(const std::string& unixStyle);

		/**
		\param windowsStyle - Path in windows style (with \ separators)
		\return path in Unix style (with \)
		*/
		static std::string convertWindowsFilePathToUnix(const std::string& windowsStyle);

		/// \return \p filepath converted to the current platforms filepath specification
		static std::string convertToNativeFilePath(const std::string& filePath);

		/**
		Take a filepath, convert it to Unix format (using UNIX_PATH_SEPARATOR) and finally convert it
		to a format that Collada DAE api accepts.

		For windows this means that "c:\path" -> "/c:/path"

		If the string "://"   can be found in \p filePath then no conversion will take place, as it
		assumes that the path is already in the correct format.
		\return filePath converted to collada compliant file path format.
		*/
		static std::string convertToColladaFilePath(const std::string& filePath);

		/// \return the extension (without .) of the file
		static std::string getFileExtension(const std::string& filePath);

		/// \return the filename only (no extension no path)
		static std::string getFileWithoutExtension(const std::string& filePath);

		/// \return the filename including file extension (no path)
		static std::string getFileWithExtension(const std::string& filePath);

		/// \return the file system directory containing the file
		static std::string getDirectory(const std::string& filePath);

		///\ return the path without any ending filename (only the directory/disk) in native format
		static std::string getPath(const std::string& path);

		/**
		If \p is a relative path (isAbsolute() returns false), the path will be expanded into a full path
		using current working directory.
		\return \p relativePath expanded into an absolute path
		*/
		static std::string getAbsolutePath(const std::string& relativePath);

		/**
		* \returns true if the given path is an absolute path.
		*/
		static bool isAbsolute(const std::string& path);

		/**
		Wrapper around the stat-function to work around a bug in Visual Studio
		when passing an empty string to stat.
		\param path The path to the file (c-string in original stat)
		\param buf The pointer to a preallocated stat-struct (see original stat-function).
		*/
		static int stat(const std::string& path, StatStruct* buf);

		/**
		Test if a path exists.
		\return true if file exists.
		*/
		static bool exists(const std::string& path);

    static std::string getEnv(const std::string& variable);

		// Split directories and files into separate elements in a vector
		static std::vector<std::string> splitPath(const std::string& path);
	};
}    // end of namespace

