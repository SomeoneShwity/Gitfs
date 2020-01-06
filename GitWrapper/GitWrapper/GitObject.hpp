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
      catch (std::exception& ex)
      {
        //err += ex + '\n' todo: some semblance of error handling
      }
    }
  }
  catch (std::runtime_error& ex)
  {
    return false;
  }

  return true;
}

namespace GitCommands
{
  enum class GitCommandsEnum
  {
    Version,
  };

  class GitCommand
  {
  private:
    static const std::string commandString;
  public:
    virtual std::string CommandString(std::string GitPath = std::string(), std::vector<std::string> parameters = std::vector<std::string>()) = 0;
  };

  class Version : public GitCommand
  {
    static const std::string commandString;
  public:
    static std::string CommandString(std::string GitPath = std::string(), std::vector<std::string> parameters = std::vector<std::string>())
    {
      return "\"" + GitPath + "\"" + commandString;
    }
  };
  const std::string Version::commandString = "version";

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

      successful &= __shellCall(GitCommands::Version::CommandString(GitPath), result);
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

  private:

    GitConfigData(std::string GitPath) :
      gitPath(GitPath) { };
    std::string gitPath;

  };
};

void call_git()
{

}

#endif // !__GIT_OBJECT_HPP__
