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
#include <time.h>

#include "GitCommands.hpp"
#include "GitConfig.hpp"

namespace GitWrapper
{
    class GitWrapper
    {
    public:
        static bool CallCommand(GitCommands::GitCommandsEnum command,
            GitConfig::GitConfigData config,
            std::string& result,
            void* extraData = nullptr);
    };
}

#endif // !__GIT_OBJECT_HPP__
