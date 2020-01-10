#pragma once

#include <string>
#include <memory>
#include <array>

#ifdef _WIN32
constexpr char PATH_SEPARATOR = '\\';
constexpr char QUOTE_MARKS = '\"';
constexpr char SPACE_SEPARATOR = ' ';
#endif

bool inline __fileExists(std::string& filePath)
{
    struct stat buffer;
    return stat(filePath.c_str(), &buffer);
}

//note: reference https://stackoverflow.com/questions/478898
bool inline __shellCall(std::string& command, std::string& out)
{
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);

    try
    {
        if (!pipe)
        {
            throw std::runtime_error("Process call failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            try
            {
                out += buffer.data();
            }
            catch (std::exception & ex)
            {
                //err += ex + '\n' todo: some semblance of error handling
            }
        }
    }
    catch (std::runtime_error & ex)
    {
        return false;
    }

    return true;
}

template <typename ...Args>
std::string inline __buildPath(std::string first, std::string second, Args... other)
{
    if (first.back() != PATH_SEPARATOR)
        first += PATH_SEPARATOR;
    return __buildPath(first + second, other);
}

std::string inline __buildPath(std::string first, std::string second)
{
    if (first.back() != PATH_SEPARATOR)
        first += PATH_SEPARATOR;
    return QUOTE_MARKS + first + second + QUOTE_MARKS;
}