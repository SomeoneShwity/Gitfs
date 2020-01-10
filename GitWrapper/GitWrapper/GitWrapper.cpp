// GitWrapper.cpp : Defines the exported functions for the DLL application.
//

#include "GitWrapper.hpp"

bool GitWrapper::GitWrapper::CallCommand(GitCommands::GitCommandsEnum command,
    GitConfig::GitConfigData config,
    std::string& result,
    void* extraData = nullptr)
{
    std::string gitPath = config.getPath();
    switch (command)
    {
    case GitCommands::GitCommandsEnum::Version:
        return GitCommands::Version::CallCommand(gitPath, result);

    case GitCommands::GitCommandsEnum::Log:
        return GitCommands::Version::CallCommand(gitPath, result, { "--oneline" });

    case GitCommands::GitCommandsEnum::Branch:
    {
        std::vector<std::string> branchParameters = { "-b" };
        if (extraData != nullptr)
        {
            try
            {
                std::string branchName = *static_cast<std::string*>(extraData);
                branchParameters.push_back(branchName);
            }
            catch (std::exception & ex)
            {
                //todo: idk
            }
        }
        return GitCommands::Checkout::NewBranch(gitPath, result, branchParameters);
    }

    case GitCommands::GitCommandsEnum::Remove:
    {
        std::vector<std::string> branchParameters;
        if (extraData != nullptr)
        {
            try
            {
                std::string branchName = *static_cast<std::string*>(extraData);
                branchParameters.push_back(branchName);
            }
            catch (std::exception & ex)
            {
                //todo: idk
            }
        }
        return GitCommands::Branch::DeleteBranch(gitPath, result, branchParameters);
    }
    default:
        break;
    }
}

void __cdecl call_git(GitCommands::GitCommandsEnum command, GitConfig::GitConfigData config, std::string& result, void* extraData = nullptr)
{
    if (!GitWrapper::GitWrapper::CallCommand(command, config, result, extraData))
    {
        throw std::runtime_error("Could not call command");
    }
}