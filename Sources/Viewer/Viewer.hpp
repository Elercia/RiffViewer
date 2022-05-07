#pragma once

#include "Riff/Riff.hpp"

#include <string>

bool LoadRiff(const std::string& path, Riff& out);
void ShowRiff(const Riff& riff);
