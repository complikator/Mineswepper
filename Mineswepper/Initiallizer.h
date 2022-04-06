#pragma once

#include "Field.h"

class Initializer {
public:
	virtual Field* Next() = 0;
};

class RandomInitializer : public Initializer {
	private:
		int maxFieldsLeft;
		int bombFieldsLeft;
public:
	RandomInitializer(int maxFields, int bombFields);
	virtual Field* Next();
};