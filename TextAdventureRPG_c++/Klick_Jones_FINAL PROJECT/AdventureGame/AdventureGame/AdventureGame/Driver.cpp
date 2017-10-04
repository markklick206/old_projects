#include "Header.h"

int main()
{
	// Initialize seed for random values
	srand(time(NULL));

	// Initialize paramaters for Player constructor
	string name;
	string race;
	intro(name, race);
	int health = 100;
	int strength = 25;
	int defense = 25;

	// Define a dynamic Player object
	Player* player = new Player(name, health, strength, defense, race);

	// Define all the areas of the game  
	Room* room1 = prepRoom1();
	CombatRoom* room2 = prepRoom2();
	CombatRoom* room3 = prepRoom3();
	CombatRoom* room4 = prepRoom4();
	CombatRoom* room5 = prepRoom5();


	int choice = 0;						// Used to store the user's choices throughout the game
	
	// Display the description for room 1
	cout<<room1->getDescr();	

	while(choice != 1)					
	{
		// choice = 1 advances to the next area, loop until user decides to advance

		// Display the options for room 1
		choice = room1->getOptions();	// Returns the user's choice
		switch(choice)
		{
		case 1:
			// Player chose to advance
			break;	
		case 2: 
			// Player chose to view the inventory vector
			player->accessInventory(player);
			break;
		default: 
			break;
		}
	}

	// ##### Player has advanced to room 2 ##### //

	// Display the description for room 2
	cout<<room2->getDescr();

	if(room2->combat(player))		
	{
		// If the player survives combat in room 2 meaning if the combat method returns true 

		// Player loots the room
		room2->lootRoom(player);

		// Reset player's choice for next set of options
		choice = 0;	

		// Player must find key in room 3 before advancing
		bool haveKey = false;		
		
		while(choice != 1)			
		{
			// Choice of 1 moves to new area.  Choice of 2 leads to locked door

			choice = room2->getOptions();	// Returns the user's choice
			switch(choice)
			{
			case 1: 
				// ##### Player has advanced to room 3 ##### //

				// Display the description for room 3
				cout<<room3->getDescr();	

				if(room3->combat(player))	
				{
					// If the player survives combat in room 3

					// Player loots the room
					room3->lootRoom(player);
					haveKey = true;			// The player has found the key to progress to room 4

					// Reset player's choice for next set of options
					choice = 0;				
					while(choice != 1)		
					{
						// Choice of 1 sends player to room 4

						choice = room3->getOptions();	// Returns the user's choice
						switch(choice)
						{
						case 1:
							break;			// Player advances to room 4, breaks to case 2
						case 2:
							// Player chose to heal
							player->heal(100);
							break;
						case 3:
							// Player chose to use an item
							player->accessInventory(player);
							break;
						default: break;
						}

					}
				}
			case 2:		
				if(haveKey == true)
				{
					// ##### Player has advanced to room 4 ##### //

					// Display the description for room 4
					cout<<room4->getDescr();

					if(room4->combat(player))
					{
						// If the player survives combat in room 4

						// Reset player's choice for next set of options
						choice = 0;

						// Player loots the room
						room4->lootRoom(player);

						while(choice != 1)
						{
							// Choice of 1 sends player to room 5
							choice = room4->getOptions();	// Returns user's choice
							switch(choice)
							{
							case 1:
								// ##### Player has advanced to room 5 ##### //

								// Display the description for room 5
								cout<<room5->getDescr();

								if(room5->combat(player))
								{
									// If the player survives combat in room 4

									// Reset player's choice for next set of options
									choice = 0;

									// Player loots the room
									room5 ->lootRoom(player);
									while(choice != 1)
									{
										// Choice of 1 sends player to the end of the game
										choice = room5->getOptions();	// Returns the user's choice
									}
										switch(choice)
									{
										case 1:
											// Player has won!
											endGame(player);
											break;
										default: break;
									}
								}					
								break;
							case 2:
								// Player is in room 4, and has chosen to heal
								player->heal(100);
								break;
							case 3:
								// Player is in room 4, and has chosen to use an item
								player->accessInventory(player);
							default: break;
							}
						}
					}
				}

				// The user tried to enter room 4 before getting the key from room 3
				else cout<<"\n The door is locked.  Perhaps you should look around for a key?"<<endl;
				break;
			case 3:
				// The user is in room 2 and has chosen to heal
				player->heal(100);
				break;
			case 4:
				// The user is in room 2 and has chosen to use an item
				player->accessInventory(player);
				break;
			default: 
				break;
			}
			
		}		
	}
	// Realeases memory back into the heap
	freeMemory(player, room1, room2, room3, room4, room5);
	return 0;
}