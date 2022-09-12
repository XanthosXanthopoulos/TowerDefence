#pragma once

#include "glm/glm.hpp"

class Character
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Character();

	/// <summary>
	/// Construct a character with the given inforamtion.
	/// </summary>
	/// <param name='id'> The ASCII value of the character. </param>
	/// <param name='xTextureCoord'> The x texture coordinate for the top left corner of the character in the texture atlas. </param>
	/// <param name='yTextureCoord'> The y texture coordinate for the top left corner of the character in the texture atlas. </param>
	/// <param name='xTexSize'> The width of the character in the texture atlas. </param>
	/// <param name='yTexSize'> The height of the character in the texture atlas. </param>
	/// <param name='xOffset'> The x distance from the curser to the left edge of the character's quad. </param>
	/// <param name='yOffset'> The y distance from the curser to the top edge of the character's quad. </param>
	/// <param name='sizeX'> The width of the character's quad in screen space. </param>
	/// <param name='sizeY'> The height of the character's quad in screen space. </param>
	/// <param name='advance'> How far in pixels the cursor should advance after adding this character. </param>
	Character(int id, double xTextureCoord, double yTextureCoord, double xTexSize, double yTexSize, double xOffset, double yOffset, double sizeX, double sizeY, double advance);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Character();

	/// <summary>
	/// Get the ASCII value of the character.
	/// </summary>
	int GetId();

	/// <summary>
	/// Get the x texture coordinate for the top left corner of the character in the texture atlas.
	/// </summary>
	double GetXTextureCoord();

	/// <summary>
	/// Get the y texture coordinate for the top left corner of the character in the texture atlas.
	/// </summary>
	double GetYTextureCoord();

	/// <summary>
	/// Get the x texture coordinate for the bottom right corner of the character in the texture atlas.
	/// </summary>
	double GetXMaxTextureCoord();

	/// <summary>
	/// Get the y texture coordinate for the bottom right corner of the character in the texture atlas.
	/// </summary>
	double GetYMaxTextureCoord();

	/// <summary>
	/// Get the width of the character in the texture atlas.
	/// </summary>
	double GetXTextureSize();

	/// <summary>
	/// Get the height of the character in the texture atlas.
	/// </summary>
	double GetYTextureSize();

	/// <summary>
	/// Get the x distance from the curser to the left edge of the character's quad.
	/// </summary>
	double GetXOffset();

	/// <summary>
	/// Get the y distance from the curser to the top edge of the character's quad.
	/// </summary>
	double GetYOffset();

	/// <summary>
	/// Get the width of the character's quad in screen space.
	/// </summary>
	double GetSizeX();

	/// <summary>
	/// Get the height of the character's quad in screen space.
	/// </summary>
	double GetSizeY();

	/// <summary>
	/// Get how far in pixels the cursor should advance after adding this character.
	/// </summary>
	double GetAdvance();

private:

	int id;
	double xTextureCoord;
	double yTextureCoord;
	double xMaxTextureCoord;
	double yMaxTextureCoord;
	double xTexSize;
	double yTexSize;
	double xOffset;
	double yOffset;
	double sizeX;
	double sizeY;
	double advance;
};

