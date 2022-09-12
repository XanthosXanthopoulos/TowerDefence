#pragma once

#include <vector>

#include "Character.h"

class Word
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Word();

	/// <summary>
	/// Constructo a word with the given font size.
	/// </summary>
	/// <param name='fonstSize'> the size of the font. </param>
	Word(double fontSize);

	/// <summary>
	/// Destructor.
	/// </summary>
	~Word();

	/// <summary>
	/// Add the given characer to the end of the word.
	/// </summary>
	/// <param name='character'> The character to add. </param>
	void AddCharacter(Character character);

	/// <summary>
	/// Get the characters of the word.
	/// </summary>
	std::vector<Character> GetCharacters();

	/// <summary>
	/// Get the width of the word.
	/// </summary>
	double GetWidth();

private:

	std::vector<Character> characters;
	double width;
	double fontSize;
};

