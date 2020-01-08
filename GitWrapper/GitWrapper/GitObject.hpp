#ifndef __GIT_OBJECT_HPP__
#define __GIT_OBJECT_HPP__

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include <cstdio>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>

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

namespace GitCommands
{
    enum class GitCommandsEnum
    {
        Version,
        Log
    };

    class GitCommand
    {
    protected:
        static const std::string commandString;
        static std::string buildCommand(std::string GitPath = std::string(), std::vector<std::string> parameters = std::vector<std::string>())
        {
            std::string command = CommandString(GitPath);
            for (auto& item : parameters)
            {
                // todo integrate homemeade trim or boost::
                command += SPACE_SEPARATOR + item;
            }
            return command;
        }

    public:
        static std::string CommandString(std::string GitPath)
        {
            return __buildPath(GitPath, commandString);
        }

        static bool CallCommand(std::string GitPath, std::string &result, std::vector<std::string> parameters = std::vector<std::string>())
        {
            std::string command = buildCommand(GitPath, parameters);

            return __shellCall(command, result);
        }

    };

    class Version : public GitCommand
    {
    public:
    };

    class Log : public GitCommand
    {
    public:
    };

    const std::string Version::commandString = "version";
    const std::string Log::commandString = "log";

};

namespace GitHelper
{
    struct GitConfigData
    {
    public:
        GitConfigData() = delete;

        ~GitConfigData() = default;

        static GitConfigData& CreateConfig(std::string GitPath)
        {
            bool successful = true;
            std::string result;

            successful &= __fileExists(GitPath);
            if (!successful)
            {
                throw std::runtime_error("Specified path does not exist");
            }

            successful &= GitCommands::Version::CallCommand(GitPath, result);
            if (!successful)
            {
                throw std::runtime_error("Unknown git path error");
            }

            successful &= (result.rfind("git version") == 0);
            if (!successful)
            {
                throw std::runtime_error("Specified path does not contain expected executable");
            }

            return GitConfigData(GitPath);
        }

        std::string getPath() { return gitPath; };
    private:

        GitConfigData(std::string GitPath) :
            gitPath(GitPath) { };
        std::string gitPath;

    };
};

namespace GitWrapper
{
    class GitWrapper
    {
    public:
        static bool CallCommand(GitCommands::GitCommandsEnum command, GitHelper::GitConfigData config, std::string &result)
        {
            std::string gitPath = config.getPath();
            switch (command)
            {
            case GitCommands::GitCommandsEnum::Version:
                return GitCommands::Version::CallCommand(gitPath, result);
            case GitCommands::GitCommandsEnum::Log:
                return GitCommands::Version::CallCommand(gitPath, result, {"--oneline"});
            default:
                break;
            }
        }
    };
}

void call_git(GitCommands::GitCommandsEnum command, GitHelper::GitConfigData config, std::string &result)
{
    if (!GitWrapper::GitWrapper::CallCommand(command, config, result))
    {
        throw std::runtime_error("Could not call command");
    }
}

#endif // !__GIT_OBJECT_HPP__
