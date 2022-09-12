#pragma once

#include <vector>

#include "Word.h"

class Line
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Line();

	/// <summary>
	/// Construct a line with the given space width, font size and maximum length.
	/// </summary>
	/// <param name='spaceWidth'> The default width of space. </param>
	/// <param name='fontSize'> The size of the font. </param>
	/// <param name='maxLength'> The maximum length of the line. </param>
	Line(double spaceWidth, double fontSize, double maxLength);

	/// <summary>
	/// Destructor.
	/// </summary>
	~Line();

	/// <summary>
	/// Attempts to add a word to the line if it fits.
	/// </summary>
	/// <param name='word'> The word to be added to the line. </param>
	bool AddWord(Word word);

	/// <summary>
	/// Get the words of the line.
	/// </summary>
	std::vector<Word> GetWords();

	/// <summary>
	/// Get the legth of the line.
	/// </summary>
	double GetLineLength();

private:

	std::vector<Word> words;

	double lineLength;
	double maxLength;
	double spaceWidth;
};

