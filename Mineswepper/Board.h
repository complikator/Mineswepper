#pragma once

#include<vector>

#include"Field.h"
#include "Initiallizer.h"
#include "Referee.h"

class Board {
private:
	int sizeX;
	int sizeY;
	std::vector<std::vector<Field*>> board;
	Referee* ref;

	void propagateActivation(int x, int y);
	bool isValidPos(int x, int y);
public:
	Board(int sizeX, int sizeY, Initializer* initializer, Referee* ref);

	// returns type of activated field
	FieldType Activate(int posX, int posY);

	void Mark(int posX, int posY);

	void Unmark(int posX, int posY);

	// returns number of neighbours
	int GetNeighborsValues(int posX, int posY);

	FieldState GetFieldState(int posX, int posY);

	FieldType GetFieldType(int posX, int posY);

	bool IsFieldDirty(int posX, int posY);

	void CleanUpField(int posX, int posY);

	int GetSizeX();
	
	int GetSizeY();

	int GetBombsCount();

	~Board();
};
