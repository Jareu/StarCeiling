#pragma once

#include <string>
#include <utility>
#include <vector>
#include "types.h"

int main();
void handleEvents();
void handleUserInput();
void update();
void render();
void readCSV(std::string filename, bool has_header = true);
void calculateCeilingSize();
void populateConstellations();