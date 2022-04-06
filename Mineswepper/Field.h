#pragma once

enum class FieldType {
	Empty,
	Bomb,
};

enum class FieldState {
	Empty,
	Active,
	Marked,
};

class Field {
protected:
	FieldState state = FieldState::Empty;
	int neighborValue = 0;
	bool isDirty = false;
public:
	virtual void SetState(FieldState);
	virtual FieldState GetState();

	bool IsDirty();
	void ClearDirty();

	virtual FieldType GetType() = 0;
	virtual int GetNeighborValue();
};

class BombField : public Field {
public:
	BombField();
	virtual FieldType GetType();
};

class EmptyField : public Field {
public:
	virtual FieldType GetType();
};