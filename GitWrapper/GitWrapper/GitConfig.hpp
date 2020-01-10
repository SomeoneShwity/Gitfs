#pragma once

#include <string>

#include "Utils.hpp"
#include "GitCommands.hpp"

namespace GitConfig
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