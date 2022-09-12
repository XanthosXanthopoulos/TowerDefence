#include "Line.h"

Line::Line() { }

Line::Line(double spaceWidth, double fontSize, double maxLength) : spaceWidth(spaceWidth * fontSize), maxLength(maxLength), lineLength(0) { }

Line::~Line() { }

bool Line::AddWord(Word word)
{
	if (lineLength + word.GetWidth() + spaceWidth > maxLength) return false;

	words.push_back(word);
	lineLength += word.GetWidth() + spaceWidth;
	return true;
}

std::vector<Word> Line::GetWords()
{
	return words;
}

double Line::GetLineLength()
{
	return lineLength;
}