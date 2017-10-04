#include "Header.h"

//##### User-Functions #####// MKLICK IANJONES 12/8/13
void attack(Organism* attacker, Organism* target)
{
	// Input: Pointers to the attacker and target objects
	// Output: The result of the attack
	// Behavior: Calculates the random values for the attack and damage
	//			 Applies the damage to the target if the hit is successful

	int attack = rand() % 20 + 1;	// Attack value is random value from 1 to 20
	attack += attacker->getStr();	// Add the damage modifier of the attacker to the attack total

	if(attack > target->getDef())	// Check if the attack total is greater than the targets defense
	{
		int damage = attacker->giveDamage();	// Calculates the damage done by the attacker
		target->takeDamage(damage);				// Applies the damage to the target
		// Alert the user of the result
		cout<<"\t"<<target->getName()<<" has taken "<<damage<<" damage!  "<<target->getHealth()<<" health remaining.\n";
	}
	else
	{
		cout<<"\t"<<attacker->getName()<<" has missed!\n";
	}
	return;
}

Room* prepRoom1()
{
	// Input: none
	// Returns: A pointer to a Room object
	// Behavior: Define the description and options for this room.

	// This text will be what the user sees when they first enter the room
	Room* room = new Room("\n You see before you a small wooden door, creaky and rotting.\n Behind it you can hear low growls and the pattering of feet.\n You have come this far to slay the demon rumored to live here.\n No turning back now!\n");

	// This text will be displayed to the room after it has been cleared of enemies, if any
	room->setOptions("\t1 - Pass through the wooden door.\n\t2 - Use an item");
	return room;
}

CombatRoom* prepRoom2()
{
	// Input: none
	// Returns: A pointer to a Room object
	// Behavior: Define the description, options, loot, and enemies for this room.

	// This text will be what the user sees when they first enter the room
	CombatRoom* room = new CombatRoom(" As you pass through the door a vicious, smelly rat leaps out from a\n pile of garbage!  Behind it, you see two heavy, rusting metal doors.\n The rat lunges at you hungirly, defend yourself!\n\n");

	// This text will be displayed to the room after it has been cleared of enemies, if any
	room->setOptions("\t1 - Enter the metal door to your left.\n\t2 - Enter the metal door to your right.\n\t3 - Rest and heal while you can.\n\t4 - Use an item\n");

	// Define a Rat enemy for this room, add it to its enemies vector
	Organism* enemy = new Organism("Rat", 35, 10, 25);	
	room->addEnemy(enemy);

	// Define a potion and weapon as loot for this room, add it to its loot vector
	Item* healthPot = new HealthPotion("Health Potion", "Heals 50 hit points.", 50);
	room->loot.push_back(healthPot);
	Item* sword = new Weapon("Longsword", "Adds two to attack and damage.", 2);
	room->loot.push_back(sword);
	return room;
}

CombatRoom* prepRoom3()
{
	// Input: none
	// Returns: A pointer to a Room object
	// Behavior: Define the description, options, loot, and enemies for this room.

	// This text will be what the user sees when they first enter the room
	CombatRoom* room = new CombatRoom(" You shove the heavy door open and peer into the dimly lit room.\n All you can hear is short, quick breaths and low grunts.\n As your eyes adjust, you see a looming form moving towards you.\n Get ready, a dim-witted slack-jawed orc is charging with his rat friend!\n");

	// This text will be displayed to the room after it has been cleared of enemies, if any
	room->setOptions("\t1 - Head to the other door.\n\t2 - Rest and heal while you can.\n\t3 - Use an item.\n");

	// Define a Rat enemy for this room, add it to its enemies vector
	Organism* enemy = new Organism("Rat", 20, 7, 10);	
	room->addEnemy(enemy);

	// Define an Orc enemy for this room, add it to its enemies vector
	enemy = new Organism("Orc", 55, 20, 20);	
	room->addEnemy(enemy);

	// Define a potion, key and weapon as loot for this room, add it to its loot vector
	Item* axe = new Weapon("Great-Axe", "Adds 6 to attack and damage.", 4);
	room->loot.push_back(axe);	
	Item* healthPot = new HealthPotion("Health Potion", "Heals 70 hit points.", 70);
	room->loot.push_back(healthPot);		
	Item* key = new Item("Mysterious Key", "This will hopefully unlock something.");
	room->loot.push_back(key);	
	return room;
}

CombatRoom* prepRoom4()
{
	// Input: none
	// Returns: A pointer to a Room object
	// Behavior: Define the description, options, loot, and enemies for this room.

	// This text will be what the user sees when they first enter the room
	CombatRoom* room = new CombatRoom(" You unlock the worn metal door and discover a staircase winding down.\n It reeks of death and rot. At the bottom of the staircase, you find\n a small landing leading to a grandiose door, engraved with gold writing\n in a strange language.  Guarding this impressive door is an even\n more impressive ogre!\n");

	// This text will be displayed to the room after it has been cleared of enemies, if any
	room->setOptions("\t1 - Pass through the golden door.\n\t2 - Heal!!\n\t3 - Use an item.");

	// Define an Ogre enemy for this room, add it to its enemies vector
	Organism* enemy = new Organism("Ogre", 120, 22, 18);
	room->addEnemy(enemy);

	// Define a potion, weapon as loot for this room, add it to its loot vector
	room->loot.push_back(new Weapon("Vicious Greatsword", "Adds 10 to attack and damage.", 4));
	room->loot.push_back(new DefPotion("Defense Potion", "Adds 15 to defense.", 15));
	return room;
}

CombatRoom* prepRoom5()
{
	// Input: none
	// Returns: A pointer to a Room object
	// Behavior: Define the description, options, loot, and enemies for this room.

	// This text will be what the user sees when they first enter the room
	CombatRoom* room = new CombatRoom(" Just by examining the strange symbols on this massive gold door, you know\n that behind it lays the demon.  You have traveled far to vanquish it.\n As you thrust open the door and charge in you see a fiery throne,\n and upon it a grizzly looking beast, with blood red wings and cracked,\n twisted horns. It lets out a ferocious laugh before leaping from it's seat,\n charging directly for you. . .\n");

	// This text will be displayed to the room after it has been cleared of enemies, if any
	room->setOptions("\t1 - Proceed to your victory!");

	// Define a Demon enemy for this room, add it to its enemies vector
	Organism* enemy = new Organism("Demon", 200, 28, 30);
	room->addEnemy(enemy);

	// Define the Golden Goose as loot for this room, add it to its loot vector
	room->loot.push_back(new Item("The Golden Goose", "Provides an infinite amount of wealth!"));
	return room;
}

void endGame(Player * player)
{
	// This text will be displayed if the user completed the game
	cout<<" You have vanquished the demon and discovered a source of infinite wealth.\n You have brought great honor and power to your people "<<player->getName()<<" the "<<player->getRace()<<".\n\n\tVICTORY . . . GAME OVER\n\n\n\n";
}

void intro(string& n, string& r)
{
	// Input: Refernces to string name and race
	// Output: Initialized strings name and race
	// Behavior: Displays intro text, prompts the user for name and race values

	cout<<" Welcome!  You have travelled long and far to this dungeon on an\n important mission.  In case you die, please specify your name and race\n now, for history's sake.\n"<<endl;
	cout<<" What is your name: ";
	cin>>n;
	cout<<" What is your race: ";
	cin>>r;
	return;
}

void freeMemory(Player* player, Room* r1, CombatRoom* r2, CombatRoom* r3, CombatRoom* r4, CombatRoom* r5)
{
	// Input: All dynamically allocated memory that needs to be freed

	// Delete all objects in the player inventory
	// This simultaneously deletes all the rooms loot vector pointers
	for(int i = 0; i < player->inventory.size(); i++)
		delete player->inventory[i];

	// Delete the player
	delete player;

	// Delete room1
	delete r1;

	// Delete all enemies in room2
	for(int i = 0; i < r2->enemies.size(); i++)
		delete r2->enemies[i];

	// Delete room 2
	delete r2;

	// Delete all enemies in room3
	for(int i = 0; i < r3->enemies.size(); i++)
		delete r3->enemies[i];

	// Delete room 3
	delete r3;

	// Delete all enemies in room4
	for(int i = 0; i < r4->enemies.size(); i++)
		delete r4->enemies[i];

	// Delete room 4
	delete r4;

	// Delete all enemies in room5
	for(int i = 0; i < r5->enemies.size(); i++)
		delete r5->enemies[i];

	// Delete room 5
	delete r5;
}

//##### All organism method implementations #####// IANJONES 12/6/13
Organism::Organism()
{
	// Default constructor, needed to allow for inheritance // 
}

Organism::Organism(string n, int h, int s, int d)
{
	// Constructor for Organism, initializes all properties
	name = n;
	health = h;
	strength = s;
	defense = d;
}

int Organism::giveDamage()
{
	// Input: None
	// Output: Pseudorandom damage amount

	int damage = rand() % 20 + 1;	// damage is random value from 1 to 20
	damage += getStr();				// Add the organisms strength bonus
	return damage;
}

void Organism::takeDamage(int damage)
{
	// Input: Amount of damage done to the organism
	// Behavior: Updates the Organism's health

	health -= damage;	// Reduce the organisms health by the amount of damage taken
	return;
}

void Organism::setName(string n)
{
	name = n;	// Set the organism's name
	return;
}

string Organism::getName()
{
	return name;	// Get the organism's name
}

int Organism::getHealth()
{
	return health;	// Get the organism's health
}

int Organism::getStr()
{
	return strength;	// Get the organism's strength
}

int Organism::getDef()
{
	return defense;	// Get the organism's defense
}

string Organism::die()
{
	// Outputs text in case the organsim dies

	return "\t" + getName() + " has died by the sword!\n";
}

void Organism::heal(int amount)
{
	// Input: Amount to heal the player by
	// Check if health is greater than max, if so reset to 100

	health += amount;
	if(health > 100)
	{
		// User greater than max hit points
		health = 100;
		cout<<"\n Healed to maximum hit points."<<endl;
	}
	else
		// User simply healed by amount
		cout<<"\n Healed by "<< amount <<" hit points.  Now at " << health <<" hit points."<<endl;
	return;
}

void Organism::addStr(int bonus)
{
	strength += bonus;	// Apply the bonus
	return;
}

void Organism::addDef(int bonus)
{
	defense += bonus;	// Apply the bonus
	return;
}

//##### All Player method implementations #####// MKLICK 12/7/13
Player::Player(string n, int h, int s, int d, string r)
{
	// Player constrcutor, initializes all properties necessary

	name = n;
	health = h;
	strength = s;
	defense = d;
	race = r;
}

void Player::addItem(Item* item)
{
	// Input: item to add to inventory
	// Behavior: push the item onto the inventory vector

	inventory.push_back(item);
	return;
}

void Player::setTarget(Organism* t)
{
	// Input: new target for player
	// Behavior: Set the player's target to the new organism

	target = t;	
	return;
}

void Player::accessInventory(Player* player)
{
	// Input: pointer to the player's object
	// Behavior: Display the inventory, allow the user to choose an item to use

	// Display the inventory
	cout<<"\n Here is your current inventory:\n";
	int invSize = inventory.size();

	if(invSize == 0)	// If empty inventory
		cout<<"\n\tYour inventory is empty!";
	else
	{
		// For non-empty inventories, output each item in a numbered list
		for(int i = 1; i <= invSize; i++)
			cout<< "\n\t" << i << " - " << inventory[i-1]->getName() << "\t" << inventory[i-1]->getDescr();

		// Allow the user to choose an item to use
		int choice = 0;
		while(choice < 1 || choice > invSize)
		{
			cout<<"\n\tChoose an item: ";
			cin>>choice;
		}

		// Set range of choice to start at 0 to index item vector, run the effect of that item
		choice--;	
		inventory[choice]->doEffect(player);
	}
	return;
}

Organism* Player::getTarget()
{
	return target;	// Get the player's current target
}

string Player::die()
{
	// This text is ran if the player is killed during the game

	return "\n\t In a tragic mishap, you dropped your weapon.  Your enemies took\n\t advantage and quickly dispatched of you.\n\n\tGAME OVER\n\n";
}

string Player::getRace()
{
	return race;	// Get the player's race
}


//##### All Item method implementations #####// IANJONES 12/6/13
Item::Item()
{
	// Default constructor, needed to allow for inheritance // 
}

Item::Item(string n, string d)
{
	// Item constructor, initializes all properties

	name = n;
	description = d;
}

void Item::doEffect(Organism* user)
{
	// Input: pointer to user of the item
	// If an item is of type Item, it cannot be used directly by the user

	cout<<"\n\tThis item cannot be used like that."<<endl;
	return;
}

string Item::getName()
{
	return name;	// Get the item's name
}

string Item::getDescr()
{
	return description;	// Get the item's description
}

Item::~Item()
{
	// Virtual destructor
}



//##### All Weapon method implementations #####// MKLICK IANJONES 12/7/13
Weapon::Weapon(string n, string d, int bonus)
{
	// Weapon constructor, initializes all properties

	name = n;
	description = d;
	strBonus = bonus;	
}

void Weapon::doEffect(Organism* player)
{
	// Input: pointer to user of the item
	// Behavior: Check if the user has already received the strength bonus from this weapon
	//			 If they have, do nothing and alert the user, otherwise apply the bonus

	if(equipped != true)
	{
		// Apply bonus and set equipped to true, alert the user
		equipped = true;
		player->addStr(strBonus);
		cout<<"\n Your strength has been increased to "<< player->getStr() <<endl;

	}

	else
		// Already received the bonus, alert the user
		cout<<"\n You have already used this weapon."<<endl;
	return;
}

Weapon::~Weapon()
{
	// Destructor
}


//##### All HealthPotion method implementations #####// MKLICK IANJONES 12/7/13
HealthPotion::HealthPotion(string n, string d, int bonus)
{
	// HealthPotion constructor, initalizes all properties

	name = n;
	description = d;
	healthBonus = bonus;
}

void HealthPotion::doEffect(Organism * player)
{
	// Input: pointer to the user of the potion
	// Behavior: See if the user has already drank this potion
	//			 If so, do nothing and alert the user, otherwise apply the bonus

	if(used != true)
	{
		// Heal the player by the specified heal property amount, set used to true
		player->heal(healthBonus);	
		used = true;
		name = "Empty potion";
		description = "Useless piece of glass.";
	}
	else
		// Potion already used, alert the user
		cout<<"There is nothing you can do with this."<<endl;
}

HealthPotion::~HealthPotion()
{
	// Destructor
}


//##### All DefPotion method implementations #####// MKLICK IANJONES 12/7/13 
DefPotion::DefPotion(string n, string d, int bonus)
{
	// DefPotion constructor, initalizes all properties

	name = n;
	description = d;
	defBonus = bonus;
}

void DefPotion::doEffect(Organism * player)
{
	// Input: pointer to the user of the potion
	// Behavior: See if the user has already drank this potion
	//			 If so, do nothing and alert the user, otherwise apply the bonus

	if(used != true)
	{
		// Increase player's defense score
		cout<<"\tYour defense has been increased by 15 points.\n";
		player->addDef(defBonus);	
		used = true;
		name = "Empty potion";
		description = "Useless piece of glass.";
	}
	else
		// Potion already used, alert the use
		cout<<"There is nothing you can do with this."<<endl;
}

DefPotion::~DefPotion()
{
	// Destructor
}


//##### All Room method implementations #####// M KLICK / IAN JONES 12/7/13
Room::Room()
{
	// Default constructor, needed to allow for inheritance // 
}

Room::Room(string d)
{
	// Room constructor

	description = d;	// Set the description of the room
}

string Room::getDescr() 
{
	return description;	// Get the room's description
}

void Room::setOptions(string o)
{
	options = o;	// Set the room's options property
}

int Room::getOptions()
{
	// Input: None
	// Output: The option the user would like to take in this room
	// Behavior: Gets the options property for the room and outputs it in a numbered list format

	int choice;
	cout<<"\n\n What would you like to do?\n";
	cout<<options<<endl;
	cin>>choice;		// Lets the user choose an option
	return choice;
}


//##### All CombatRoom method implementations #####// M KLICK 12/8/13

CombatRoom::CombatRoom(string d)
{
	// CombatRoom constructor

	description = d;	// Set the room's description
}

void CombatRoom::addEnemy(Organism* enemy)
{
	// Input: Pointer to Enemy object to add to list
	// Behavior: Push the specified enemy pointer onto the vector

	enemies.push_back(enemy);
}

void CombatRoom::removeEnemy(Organism* enemy)
{
	// Input: pointer to the enemy that must be removed from the list

	// Find the correct Organism in the enemies vector to remove
	for(int i = 0; i < enemies.size(); i++)
	{
		if(enemies[i]->getName() == enemy->getName())
		{
			// If proper enemy found, call their die function and change name to DEAD
			cout<<enemy->die();
			enemies[i]->setName("DEAD");	//  indicates to combat function that this creature is dead.
			break;
		}
	}
}

bool CombatRoom::combat(Player* player)
{
	// Input: A pointer to the player
	// Output: The guides and results for each round of combat
	// Returns: True if the player has lived, false if the player has died
	// Behavior: Manages the attacks for each Organism, updates health accordingly, checks for deaths


	// ##### Begin by setting the player's target to the first enemy in the enemies vector for this room ##### //
	player->setTarget(enemies[0]);
	while(true)
	{
		//##### Inform the user who they are targeting, display possible combat options #####//
		cout<<"\n You are targeting the "<<player->getTarget()->getName()<<endl;
		cout<<" Would you like to do . . .\n";
		cout<<"\t1 - Attack the target?\n";
		cout<<"\t2 - Use an item?\n";
		cout<<"\t3 - Choose a new target?\n";

		// ##### Get the user's choice and run the appropriate method ##### //
		int choice = 0;
		while(choice < 1 || choice > 3)
		{
			cin>>choice;
		}

		switch(choice)
		{
			// Player chose to attack
		case 1: attack(player, player->getTarget());
			break;
			// Player chose to use an item
		case 2:	player->accessInventory(player);
			break;
			// Player chose to select a new target
		case 3:
			int newTarget = 0;
			cout<<" Select your target\n";
			for(int i = 0; i < enemies.size(); i ++)
			{
				cout<<"\t"<<i+1<<" - "<<enemies[i]->getName()<<endl;
			}
			// Prompt the user for the new target and update
			while(newTarget < 1 || newTarget > enemies.size())
			{
				cin>>newTarget;
			}
			player->setTarget(enemies[--newTarget]);
			break;
		}

		// ##### Check for enemy deaths ##### //
		if(player->getTarget()->getHealth() <= 0)	// Chaining implemented to check enemies health
		{
			removeEnemy(player->getTarget());

			// #### Check if all enemies are dead ##### //
			bool newRound = false;	// Will remain false if all enemies have been killed
			for(int i = 0; i < enemies.size(); i++)
			{
				if(enemies[i]->getName() != "DEAD")
				{
					// Living enemy remains player's target is set to that enemy
					newRound = true;
					player->setTarget(enemies[i]);
					break;
				}
			}
			if(newRound == false)
			{
				// No living enemy remains
				cout<<"\n You have achieved victory!\n\n";
				return true;
			}

		}

		// ##### Player's turn has ended, enemies' turn ##### //
		for(int i = 0; i < enemies.size(); i++)
		{
			if(enemies[i]->getName() != "DEAD")	// If enemy not already killed, attack
				attack(enemies[i], player);			
		}

		// ##### Check if player has died ##### //
		if(player->getHealth() < 0)
		{
			// Player has died
			cout<<player->die();			
			return false;
		}
		// ##### If program has reached this point then Player and at least one enemy have survived the round ##### //
		// ##### Move to next round (repeat the while loop body) ##### //
	}// End of While loop
}

void CombatRoom::lootRoom(Player* player)
{
	// Input: Pointer to the player looting the room

	cout<<" You have discovered new items after vanquishing the monster."<<endl;
	for(int i = 0; i < loot.size(); i++)
	{
		// Display to the player all items in the loot vector for this room
		cout<<"\n\t"<< i + 1 <<" - "<< loot[i]->getName() << "\t" << loot[i]->getDescr();
		player->addItem(loot[i]);
	}
}


