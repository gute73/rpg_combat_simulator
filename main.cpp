#include <iostream>
#include <string>
#include <cstdlib> // for rand() and srand()
#include <ctime> // for time()

// Generate a random number between min and max (inclusive)
// Assumes srand() has already been called
int getRandomNumber(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);  // static used for efficiency, so we only calculate this value once
	// evenly distribute the random number across our range
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

class Creature
{
protected:
    std::string m_name;
    char m_symbol;
    int m_health;
    int m_damage;
    int m_gold;

public:
    Creature(std::string name, char symbol, int health, int damage, int gold)
    : m_name(name), m_symbol(symbol), m_health(health), m_damage(damage), m_gold(gold) {}

    std::string& getName() { return m_name; }

    char getSymbol() const { return m_symbol; }

    int getHealth() const { return m_health; }

    int getDamage() const { return m_damage; }

    int getGold() const { return m_gold; }

    void reduceHealth(int reduce)
    {
        m_health -= reduce;
    }

    bool isDead()
    {
        return (m_health <= 0);
    }

    void addGold(int gold)
    {
        m_gold += gold;
    }
};

class Player: public Creature
{
private:
    int m_level;

public:
    Player(std::string name)
    : Creature(name, '@', 10, 1, 0), m_level(1) {}

    void levelUp()
    {
        ++m_level;
        ++m_damage;
    }

    int getLevel() { return m_level; }

    bool hasWon()
    {
        return (m_level >= 20);
    }

};

class Monster: public Creature
{
public:
    enum Type
    {
        DRAGON,
        ORC,
        SLIME,
        MAX_TYPES
    };

    struct MonsterData
    {
        std::string name;
        char symbol;
        int health;
        int damage;
        int gold;
    };

    static MonsterData monsterData[MAX_TYPES];

    Monster(Type type)
    : Creature(monsterData[type].name, monsterData[type].symbol, monsterData[type].health, monsterData[type].damage, monsterData[type].gold) {}

    static Monster getRandomMonster()
    {
        return Monster(static_cast<Type>(getRandomNumber(0, MAX_TYPES-1)));
    }
};

Monster::MonsterData Monster::monsterData[Monster::MAX_TYPES]
{
	{ "dragon", 'D', 20, 4, 100 },
	{ "orc", 'o', 4, 2, 25 },
	{ "slime", 's', 1, 1, 10 }
};

void attackMonster(Player &p, Monster &m)
{
    m.reduceHealth(p.getDamage());
    std::cout << "You hit the " << m.getName() << " for " << p.getDamage() << " damage.\n";
}

void attackPlayer(Player &p, Monster &m)
{
    p.reduceHealth(m.getDamage());
    std::cout << "The " << m.getName() << " hit you for " << m.getDamage() << " damage.\n";
}

void fightMonster(Player &p)
{
    Monster m = Monster::getRandomMonster();
    std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ").\n";
    while(!p.isDead() && !m.isDead())
    {
        char fight;
        do
        {
            std::cout << "(R)un or (F)ight: ";
            std::cin >> fight;
        } while (((fight != 'F') && (fight != 'f')) && ((fight != 'R') && (fight != 'r')));

        if ((fight == 'R') || (fight == 'r'))
        {
            if (getRandomNumber(0, 1))
            {
                std::cout << "You successfully fled.\n";
                return;
            }
            else
                attackPlayer(p, m);
        }
        else
        {
            attackMonster(p, m);
            if (!m.isDead())
                attackPlayer(p, m);
        }
    }
    if (m.isDead())
    {
        std::cout << "You killed the " << m.getName() << ".\n";
        p.levelUp();
        std::cout << "You are now level " << p.getLevel() << ".\n";
        std::cout << "You found " << m.getGold() << " gold.\n";
        p.addGold(m.getGold());
    }
}


int main()
{
	srand(static_cast<unsigned int>(time(0))); // set initial seed value to system clock
	rand(); // get rid of first result

	// create character
	std::cout << "Enter your name: ";
	std::string name;
	std::cin >> name;
	std::cout << "\nWelcome, " << name << '\n';
    Player p(name);

    // fight!
    do
    {
        fightMonster(p);
    } while (!p.hasWon() && !p.isDead());

    // victory or defeat
    if (p.hasWon())
    {
        std::cout << "You are victorious!\n";
        std::cout << "You now possess " << p.getGold() << " gold. Your wealth grows along with your legend!\n";
    }
    else
    {
        std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
        std::cout << "Too bad you can't take it with you!";
    }

    return 0;
}
