#include <vr/Version.h>
#include <sstream>

std::string vr::getVersion()
{
    std::stringstream str;
    str << VRLIB_MAJOR_VERSION << "." << VRLIB_MINOR_VERSION << std::ends;
    return str.str();
}

std::string vr::getName()
{
    return "VRLIB";
}

