#include "Character.h"

Character::Character() { }

Character::Character(int id, double xTextureCoord, double yTextureCoord, double xTexSize, double yTexSize, double xOffset, double yOffset, double sizeX, double sizeY, double advance) : id(id), xTextureCoord(xTextureCoord), yTextureCoord(yTextureCoord), xTexSize(xTexSize), yTexSize(yTexSize), xOffset(xOffset), yOffset(yOffset), sizeX(sizeX), sizeY(sizeY), advance(advance)
{
	xMaxTextureCoord = xTextureCoord + xTexSize;
	yMaxTextureCoord = yTextureCoord + yTexSize;
}

Character::~Character() { }

int Character::GetId()
{
	return id;
}

double Character::GetXTextureCoord()
{
	return xTextureCoord;
}

double Character::GetYTextureCoord()
{
	return yTextureCoord;
}

double Character::GetXMaxTextureCoord()
{
	return xMaxTextureCoord;
}

double Character::GetYMaxTextureCoord()
{
	return yMaxTextureCoord;
}

double Character::GetXTextureSize()
{
	return xTexSize;
}

double Character::GetYTextureSize()
{
	return yTexSize;
}

double Character::GetXOffset()
{
	return xOffset;
}

double Character::GetYOffset()
{
	return yOffset;
}

double Character::GetSizeX()
{
	return sizeX;
}

double Character::GetSizeY()
{
	return sizeY;
}

double Character::GetAdvance()
{
	return advance;
}
