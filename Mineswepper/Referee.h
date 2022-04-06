#pragma once

#include "Field.h"

class Referee {
private:
	int maxBombs;
	int maxFields;
	double maxTime;

	int bombsMarked;
	int markedCount;
	int marksLeft;

	int emptyFieldsLeft;
	int bombsActivated;

	bool isGameFinished = false;
	bool win = false;

	void checkGameState();
public:
	Referee(int bombs, int maxFields, double maxTime);
	void StateChanged(FieldState oldState, FieldState newState, FieldType type);
	int GetMarksLeft();
	bool GameFinished();
	bool IsWin();

	void StopGame();

	void TimerChanged(double time);
};