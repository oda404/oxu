// Copyright (c) Olaru Alexandru <olarualexandru404@gmail.com>
// Licensed under the MIT license found in the LICENSE file in the root of this repository.

#include"logger.hpp"

#include<spdlog/sinks/stdout_color_sinks.h>

void oxu::Logger::init()
{
    spdlog::set_pattern("%^[%T] %v%$");
    spdlog::set_level(spdlog::level::debug);
}
