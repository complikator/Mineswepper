
#include "Referee.h"

Referee::Referee(int bombs, int maxFields, double maxTime) :maxBombs(bombs), maxFields(maxFields), maxTime(maxTime), bombsMarked(0), markedCount(0), marksLeft(bombs), emptyFieldsLeft(maxFields - bombs), bombsActivated(0) { }


void Referee::StateChanged(FieldState oldState, FieldState newState, FieldType type) {
	switch (oldState) {
	case FieldState::Empty:
		switch (newState)
		{
		case FieldState::Empty:
			break;
		case FieldState::Active:
			switch (type)
			{
			case FieldType::Empty:
				emptyFieldsLeft--;
				break;
			case FieldType::Bomb:
				bombsActivated++;
				break;
			}
			break;
		case FieldState::Marked:
			marksLeft--;
			if (type == FieldType::Bomb)
				bombsMarked++;
		}
		break;
	case FieldState::Marked:
		if (newState == FieldState::Empty) {
			marksLeft++;
			if (type == FieldType::Bomb)
				bombsMarked--;
		}
		break;
	}

	checkGameState();
}

int Referee::GetMarksLeft() {
	return marksLeft;
}

bool Referee::GameFinished() {
	return isGameFinished;
}

bool Referee::IsWin() {
	return win;
}

void Referee::checkGameState() {
	if (bombsActivated) {
		isGameFinished = true;
		win = false;
		return;
	}

	if (emptyFieldsLeft == 0) {
		isGameFinished = true;
		win = true;
		return;
	}

	if (bombsMarked == maxBombs) {
		isGameFinished = true;
		win = true;
		return;
	}

}

void Referee::StopGame() {
	isGameFinished = true;
	win = false;
}

void Referee::TimerChanged(double time) {
	if (time > maxTime) {
		isGameFinished = true;
		win = false;
	}
}