#pragma once

#include "resource.h"

#include <memory>

#include "Board.h"
#include "BoardDrawer.h"
#include "Referee.h"

#define S 10

const int panelHeight = 30;
const int fieldSize = 25;

const int defaultFieldsX = 20;
const int defaultFieldsY = 25;

const int defaultBombCount = 40;

const double defaultTime = 600 * S;

const int minHeight = 10;
const int maxHeight = 30;

const int minWidth = 10;
const int maxWidth = 24;

const int minBombs = 10;
