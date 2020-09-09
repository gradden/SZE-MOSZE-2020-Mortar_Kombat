#include "Character.h"

Character::Character(string characterName, int hitPoints, int damagePoints)
{
	setName(characterName);
	setHP(hitPoints);
	setDMG(damagePoints);
}

void Character::setName(string characterName)
{
	name = characterName;
}

string Character::getName()
{
	return name;
}

void Character::setHP(int hitPoints)
{
    if (hitPoints > 0)
    {
        HP = hitPoints;
    }
    else
    {
        HP = 0;
    }
}

void Character::setDMG(int damagePoints)
{
	DMG = damagePoints;
}

int Character::getHP()
{
	return HP;
}

int Character::getDMG()
{
	return DMG;
}
