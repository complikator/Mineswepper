
#include "Board.h"

#include<queue>
#include <memory>

Board::Board(int sizeX, int sizeY, Initializer* initializer, Referee* ref) :sizeX(sizeX), sizeY(sizeY), ref(ref) {
	board.resize(sizeX);

	for (auto& row : board) {
		for (int i = 0; i < sizeY; i++) {
			row.push_back(initializer->Next());
		}
	}
}

FieldType Board::Activate(int posX, int posY) {
	auto type = GetFieldType(posX, posY);

	if (GetFieldType(posX, posY) == FieldType::Empty && GetNeighborsValues(posX, posY) == 0)
		propagateActivation(posX, posY);
	else {
		ref->StateChanged(GetFieldState(posX, posY), FieldState::Active, GetFieldType(posX, posY));
		board[posX][posY]->SetState(FieldState::Active);
	}
	return type;
}

int Board::GetNeighborsValues(int posX, int posY) {
	int value = 0;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			value += isValidPos(posX + i, posY + j) ? board[posX + i][posY + j]->GetNeighborValue() : 0;
		}
	}

	value -= board[posX][posY]->GetNeighborValue();

	return value;
}

FieldState Board::GetFieldState(int posX, int posY) {
	return board[posX][posY]->GetState();
}

FieldType Board::GetFieldType(int posX, int posY) {
	return board[posX][posY]->GetType();
}

void Board::propagateActivation(int x, int y) {
	std::queue<std::pair<int, int>> q;
	q.push({ x,y });

	std::unique_ptr<bool[]> visited(new bool[sizeX * sizeY]);

	for (int i = 0; i < sizeX * sizeY; i++) {
		visited[i] = false;
	}

	while (!q.empty()) {
		auto f = q.front();
		q.pop();

		visited[f.second * sizeX + f.first] = true;

		ref->StateChanged(GetFieldState(f.first, f.second), FieldState::Active, GetFieldType(f.first, f.second));

		board[f.first][f.second]->SetState(FieldState::Active);

		if (GetNeighborsValues(f.first, f.second) != 0)
			continue;

		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				auto xx = f.first + i;
				auto yy = f.second + j;
				auto pos = ((f.second + j) * sizeX) + (f.first + i);

				if (!isValidPos(xx, yy))
					continue;

				if (GetFieldType(xx, yy) == FieldType::Empty && !visited[pos] && GetFieldState(xx, yy) == FieldState::Empty) {
					q.push({ xx, yy });
					visited[pos] = true;
				}
			}
		}
	}
}

bool Board::isValidPos(int x, int y) {
	return x >= 0 && x < sizeX&& y >= 0 && y < sizeY;
}

void Board::Mark(int posX, int posY) {
	ref->StateChanged(GetFieldState(posX, posY), FieldState::Marked, GetFieldType(posX, posY));
	board[posX][posY]->SetState(FieldState::Marked);
}

Board::~Board() {
	for (auto row : board) {
		for (auto& f : row) {
			delete f;
		}
	}
}

bool Board::IsFieldDirty(int posX, int posY) {
	return board[posX][posY]->IsDirty();
}

void Board::CleanUpField(int posX, int posY) {
	board[posX][posY]->ClearDirty();
}

void Board::Unmark(int posX, int posY) {
	ref->StateChanged(GetFieldState(posX, posY), FieldState::Empty, GetFieldType(posX, posY));
	board[posX][posY]->SetState(FieldState::Empty);
}

int Board::GetSizeX() {
	return sizeX;
}

int Board::GetSizeY() {
	return sizeY;
}

int Board::GetBombsCount() {

	int ans = 0;
	for (int i = 0; i < sizeY; i++) {
		for (int j = 0; j < sizeX; j++) {
			if (GetFieldType(j, i) == FieldType::Bomb)
				ans++;
		}
	}
	return ans;
}