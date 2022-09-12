#include "Word.h"

Word::Word() : fontSize(1), width(0) { }

Word::Word(double fontSize) : fontSize(fontSize), width(0) { }

Word::~Word() { }

void Word::AddCharacter(Character character)
{
	characters.push_back(character);
	width += character.GetAdvance() * fontSize;
}

std::vector<Character> Word::GetCharacters()
{
	return characters;
}

double Word::GetWidth()
{
	return width;
}
