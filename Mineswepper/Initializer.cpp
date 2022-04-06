
#include "Initiallizer.h"

#include <time.h>
#include <stdlib.h>

RandomInitializer::RandomInitializer(int maxFields, int bombFields) : maxFieldsLeft(maxFields), bombFieldsLeft(bombFields) {
	srand(time(NULL));
}

Field* RandomInitializer::Next() {
	int chooser = rand() % maxFieldsLeft--;

	if (chooser < bombFieldsLeft) {
		bombFieldsLeft--;
		return new BombField();
	}
	else {
		return new EmptyField();
	}
}