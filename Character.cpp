#include "Character.h"

Character::Character(const std::string _name, float _maxHP, float _DMG, float _ACD)
: name(_name)
{
	HP = maxHP = _maxHP;
	DMG = _DMG;
	ACD = _ACD;
}

const std::string Character::getName() const
{
	return name;
}

const float Character::getHealthPoints() const
{
	return HP;
}

const float Character::getMaxHealthPoints() const
{
	return maxHP;
}

const float Character::getDamage() const
{
	return DMG;
}

const float Character::getAttackCoolDown() const
{
    return ACD;
}


float Character::gotHit(Character* attacker)
{
	float potentialXP = attacker->getDamage();
	if (HP - potentialXP > 0)
	{
		HP -= potentialXP;
		return potentialXP;
	}
	else
	{
		potentialXP = HP;
		HP = 0;
		return potentialXP;
	}
}

void Character::doHit(Character& victim)
{
	victim.gotHit(this);
}

bool Character::isAlive()
{
	return HP > 0;
}

void Character::fightTilDeath(Character &enemy)
{
    float ACDTimer1 = 0, ACDTimer2 = 0;

    while (enemy.getHealthPoints() > 0 && HP > 0)
    {
        if (ACDTimer1 <= ACDTimer2)
        {
			this->doHit(enemy);
			ACDTimer1 += ACD;
        }
        else
        {
			enemy.doHit(*this);
			ACDTimer2 += enemy.getAttackCoolDown();
        }
    }
}

float RONAF(const float& num)
{
	if (num < 0)
	{
		throw std::runtime_error("Negative numeric value is not acceptable here!");
	}
	return num;
}

