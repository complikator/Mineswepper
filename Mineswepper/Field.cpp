
#include "Field.h"

void Field::SetState(FieldState state) {
	isDirty = true;
	this->state = state;
}

FieldState Field::GetState() {
	return state;
}

bool Field::IsDirty() {
	return isDirty;
}

void Field::ClearDirty() {
	isDirty = false;
}

int Field::GetNeighborValue() {
	return neighborValue;
}

BombField::BombField() {
	neighborValue = 1;
}

FieldType BombField::GetType() {
	return FieldType::Bomb;
}

FieldType EmptyField::GetType() {
	return FieldType::Empty;
}


