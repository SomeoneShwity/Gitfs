#pragma once

#include <string>
#include <vector>

#include "Utils.hpp"
#include <time.h>

namespace GitCommands
{
    enum class GitCommandsEnum
    {
        Version,
        Log,
        Branch, // branch create
        Remove, //branch remove
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

        static bool CallCommand(std::string GitPath, std::string& result, std::vector<std::string> parameters = std::vector<std::string>())
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

    class Checkout : public GitCommand
    {
        static const size_t defaultRandomBranchNameSize;

        static std::string& GenerateRandomString(size_t size)
        {
            static const auto charSet = "0123456789abcdefghijklmnopqrstuvwxyz";
            std::string randomString;
            randomString.resize(size);

            srand(time(0));

            for (size_t i = 0; i < size; i++)
            {
                randomString[i] = charSet[rand() % 37];
            }

            return randomString;
        }
    public:
        static bool NewBranch(std::string gitPath, std::string& result, std::vector<std::string> parameters = std::vector<std::string>())
        {
            bool success = true;
            std::string newBranchName;

            if (parameters.empty())
            {
                newBranchName = GenerateRandomString(defaultRandomBranchNameSize);
            }
            else
            {
                newBranchName = parameters.front();
            }

            success &= CallCommand(gitPath, result, { "-b", newBranchName });
            if (!success || result.find("fatal") != std::string::npos)
                return false;

            result = newBranchName;
            return true;
        }
    };

    class RevParse : public GitCommand
    {
    };

    class Branch : public GitCommand
    {
    public:
        static bool DeleteBranch(std::string gitPath, std::string& result, std::vector<std::string> parameters)
        {
            bool success = false;
            if (parameters.empty())
            {
                return false;
            }

            std::string currentBranchName;
            
            success = RevParse::CallCommand(gitPath, currentBranchName, { "--abbrev", "-ref", "HEAD" });
            if (success && currentBranchName == parameters.front())
            {
                success = Checkout::CallCommand(gitPath, result, { "-f", "master" });
                if (!success)
                {
                    throw std::runtime_error("not implemented");
                    //todo : implement branch change
                }
            }

            return Branch::CallCommand(gitPath, result, { "-D", parameters.front() });
        }
    };

    const std::string Version::commandString = "version";
    const std::string Log::commandString = "log";
    const std::string Checkout::commandString = "checkout";
    const std::string Branch::commandString = "branch";
    const std::string RevParse::commandString = "rev-parse"; // --abbrev - ref HEAD

    const size_t Checkout::defaultRandomBranchNameSize = 16;

};