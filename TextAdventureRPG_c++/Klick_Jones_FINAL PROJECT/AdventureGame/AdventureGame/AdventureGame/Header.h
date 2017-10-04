#include <iostream>
#include <string>
#include <vector>
#include <time.h>

using namespace std;

class Organism
{
protected:
	string name;
	int health;
	int strength;
	int defense;
	

public:
	Organism();				// Default Constructor
	Organism(string n, int h, int s, int d);			// Constructor
	int giveDamage();		// Calculates damage done to target
	void takeDamage(int d);	// Applies damage to organisms's health
	void setName(string n);		// Setter for name property
	string getName();		// Getter for name property
	int getHealth();		// Getter for the health property
	int getStr();			// Getter for strength property
	int getDef();			// Getter for defense property
	string die();					// What to output in case of death
	void heal(int amount);			// Heals the player by the specified amount
	void addStr(int bonus);			// Gives the player a bonus to strength property
	void addDef(int bonus);			// Gives the player a bonus to defense property
};

class Item
{
protected:
	string name;
	string description;

public:
	Item();						// Default Constructor
	Item(string n, string d);	// Constructor
	string getName();			// Getter for name property
	string getDescr();			// Getter for desription property
	virtual void doEffect(Organism* user);
	virtual ~Item();			// Virtual desctructor
};

class Player: public Organism
{
	// Subclass of Organism
private:
	string race;			// Race of the character	
	Organism* target;			// Current target of the character

public:
	vector<Item*> inventory;	// Inventory of the character
	Player(string n, int h, int s, int d, string r);	// Constructor
	void accessInventory(Player * player);			// Displays the items in the inventory vector, allows user to use items
	void setTarget(Organism* t);	// Setter for the target property
	Organism* getTarget();			// Getter for the target property
	string getRace();				// Getter for the race Property
	void addItem(Item* item);		// Adds the specified item to the inventory vector
	string die();					// Override Organism's die() method
};

class Weapon: public Item
{
	// Subclass of Item
private:
	bool equipped;		// Indicates if the player has already equipped the item
	int strBonus;		// Modifier to player's strength property
public:
	Weapon(string n, string d, int bonus);				// Constructor
	void doEffect(Organism* player);	// Add the bonus to the player's strength
	~Weapon();							//  destructor of Weapon class
};

class HealthPotion: public Item
{
	// Subclass of Item
private:
	bool used;
	int healthBonus;
public:
	HealthPotion(string n, string d, int bonus);		// Constrcutor
	void doEffect(Organism* player);	// Add the bonus to the player's health
	~HealthPotion();					//  destructor of HealthPotion class
};

class DefPotion: public Item
{
	// Subclass of Item
private:
	bool used;
	int defBonus;		// Amount to add to player's defense
public:
	DefPotion(string n, string d, int defBonus);	// Constructor
	void doEffect(Organism* player);	// Add bonus to player's defense
	~DefPotion();						//  destructor of DefPotion class
};

class Room
{
protected:
	string description;	// The description of this room
	string options;		// All available actions user can take in this room

public:
	Room();						// Default Constructor
	Room(string d);				// Constructor
	string getDescr();			// Getter for description property
	void setOptions(string o);	// Setter for the options property
	int getOptions();			// Displays the options for the user in this room and returns the user's choice
};

class CombatRoom: public Room
{
	// Subclass of Room
public:
	CombatRoom(string d);			// Constructor
	vector<Organism*> enemies;		// Vector of all the enemies in this room
	vector<Item*> loot;				// Vector of all loot in this room
	void addEnemy(Organism* enemy);	// Add a new enemy to the vector
	void removeEnemy(Organism* enemy);	// Effectively kill an enemy in this room
	bool combat(Player* player);	// Runs combat scene for the player and enemies in this room
	void lootRoom(Player* player);	// Adds the loot to the player's inventory
};


// User defined functions
Room* prepRoom1();			// Initializes fields for Room 1
CombatRoom* prepRoom2();	// Initializes fields for Room 2
CombatRoom* prepRoom3();	// Initializes fields for Room 3
CombatRoom* prepRoom4();	// Initializes fields for Room 4
CombatRoom* prepRoom5();	// Initialize fields for Room 5
void endGame(Player* player);		// Ends the game, the player achieved victory
void intro(string& n, string& r);	// Displays the intro text and prompts user for Name and Race values

// Meant to release all memory pulled from the heap during dynamic memory allocation
void freeMemory(Player* player, Room* r1, CombatRoom* r2, CombatRoom* r3, CombatRoom* r4, CombatRoom* r5);