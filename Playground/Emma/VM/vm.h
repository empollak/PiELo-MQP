#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <fstream>

extern std::map<std::string, std::vector<uint8_t>> heap;
extern std::stack<std::vector<uint8_t>> stack;
extern std::ifstream code;