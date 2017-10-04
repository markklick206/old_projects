#ifndef SIMULATE_H_
#define SIMULATE_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <ios>
#include <vector>
#include "Random.h"
#include <fstream>
#include <map>
#include <utility>
#include "car.h"

#include "RD.h"
#include "JacksonQueue.h"
#include "TravisQueue.h"
#include "Jackson.h"
#include "Travis.h"

Random my_random;  // Global variable 

class Simulator
{
private:
	int total_time;  // total time to run simulation
	int clock;       // current time
	Jackson* JacksonRD;
	Travis* TravisRD;
	RD* AmberRD;
	RD* TulipRD;
	RD* JamesRD;
	RD* BirchRD;
	RD* Bank;
	RD* School;
	int AmberRDcapacity;
	int TulipRDcapacity;
	int JamesRDcapacity;
	int BirchRDcapacity;
	int TravisRDcapacity;
	int JacksonRDcapacity;
	int arrivalRateEntry;
	double arrival_rate;
	JacksonQueue* JQ1;
	TravisQueue* TQ1;
	std::queue<car*>* directory;
	std::multimap<std::string, car*>* CityRecord;
	// Remember me from HW3
	int read_int(const std::string &prompt)
	{
		std::cin.exceptions(std::ios_base::failbit);
		int num = 0;
		while (true) {
			try {
				std::cout << prompt;
				std::cin >> num;
				std::cout << std::endl;
				return num;
			}
			catch (std::ios_base::failure) {
				std::cout << "Bad numeric string -- try again\n";
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<int>::max(), '\n');
			}
		}
	}

public:

	Simulator() {
		enter_data();
		//INITIALIZING DATA FOR SIMULATION
		CityRecord = new std::multimap<std::string, car*>();
		TQ1 = new TravisQueue();
		TQ1->set_arrival_rate(arrival_rate);
		JQ1 = new JacksonQueue();
		JQ1->set_arrival_rate(arrival_rate);
		directory = new std::queue<car*>();
		///////////////////////
		int simfactor = 8;
		JacksonRD = new Jackson(JacksonRDcapacity, 3);
		TravisRD  = new Travis(TravisRDcapacity, 3);
		AmberRD   = new RD(AmberRDcapacity, 6);
		TulipRD   = new RD(TulipRDcapacity, 3);
		JamesRD   = new RD(JamesRDcapacity, 4);
		BirchRD   = new RD(BirchRDcapacity, 5);
		Bank      = new RD(50, -1);
		School    = new RD(50, -2);
		//////////////////
		//Road hookups
		JacksonRD->HookRoads(&JQ1->the_queue, AmberRD->departure_queue1, TulipRD->departure_queue1);
		TravisRD->HookRoads(&TQ1->the_queue, JamesRD->departure_queue2, BirchRD->departure_queue2);

		AmberRD->hookRoads(JacksonRD->departure_queue2, Bank->departure_queue1);
		TulipRD->hookRoads(JacksonRD->departure_queue3, School->departure_queue1);
		JamesRD->hookRoads(Bank->departure_queue2, TravisRD->departure_queue2);
		BirchRD->hookRoads(School->departure_queue2, TravisRD->departure_queue3);
		Bank   ->hookRoads(AmberRD->departure_queue2, JamesRD->departure_queue1);
		School ->hookRoads(TulipRD->departure_queue2, BirchRD->departure_queue1);
		AmberRD->feeding_queue2 = Bank->departure_queue1;
		JamesRD->feeding_queue1 = Bank->departure_queue2;
		TulipRD->feeding_queue2 = School->departure_queue1;
		BirchRD->feeding_queue1 = School->departure_queue2;
		JacksonRD->feeding_queue2 = AmberRD->departure_queue1;
		JacksonRD->feeding_queue3 = TulipRD->departure_queue1;
		TravisRD->feeding_queue2 = JamesRD->departure_queue2;
		TravisRD->feeding_queue3 = BirchRD->departure_queue2;
		JamesRD->feeding_queue2 = TravisRD->departure_queue2;
		BirchRD->feeding_queue2 = TravisRD->departure_queue3;

		JacksonRD->departure_queue1 = directory;
		TravisRD->departure_queue1 = directory;
		///////////////////////
		clock = 0;
	}

	void enter_data()
	{
		//SIMULATION INITIATION DATA
		std::cout << "Welcome CS273 to the Simple  Traffic Simulator!\n";

		arrivalRateEntry = read_int("Please enter the car arrival rate (car/hour): ");
		arrival_rate = arrivalRateEntry / 60.0;

		total_time = read_int("Please enter the simulation time (hours): ");
		total_time *= 60;

		AmberRDcapacity   = read_int("Please enter capacity for Amber road: ");
		TulipRDcapacity	  =	read_int("Please enter capacity for Tulip road: ");
		JamesRDcapacity	  =	read_int("Please enter capacity for James road: ");
		BirchRDcapacity	  =	read_int("Please enter capacity for Birch road: ");
		TravisRDcapacity  =	read_int("Please enter capacity for Travis road: ");
		JacksonRDcapacity =	read_int("Please enter capacity for Jackson road: ");
		system("CLS");
	}

	void run_simulation()
	{
		// run the simulation
		for (clock = 0; clock < total_time; ++clock)
		{
			Update(clock);
			system("CLS");
		}
		while (!directory->empty())
		{
			CityRecord->insert(make_pair(directory->front()->name, directory->front()));
			directory->pop();
		}
		std::cout << "Simulation done" << std::endl;
	}
	void Update(int clock)
	{
		JQ1->Update(clock);
		JacksonRD->Update(clock);
		TQ1->Update(clock);
		TravisRD->Update(clock);
		AmberRD->Update(clock);
		TulipRD->Update(clock);
		JamesRD->Update(clock);
		BirchRD->Update(clock);
		Bank->Update(clock);
		School->Update(clock);
		//////////////////////////////
		//UNCOMMENT THIS SECTION TO SEE THE SIMULATION IN PROCESS
		//////////////////////////////
		
		/*std::cout << "*******Jackson" << std::endl;
		JacksonRD->ShowMain();
		std::cout << "*******Travis" << std::endl;
		TravisRD->ShowMain();
		std::cout << "*******Amber" << std::endl;
		AmberRD->Show();
		std::cout << "*******Tulip" << std::endl;
		TulipRD->Show();
		std::cout << "*******James" << std::endl;
		JamesRD->Show();
		std::cout << "*******Birch" << std::endl;
		BirchRD->Show();
		std::cout << "*******Bank" << std::endl;
		Bank->Show();
		std::cout << "*******School" << std::endl;
		School->Show();
		JacksonRD->ShowOutside();
		TravisRD->ShowOutside();*/

		std::cout << "Running simulation " << clock << " minutes." << std::endl << std::endl;
	}
	double visitAVG()
	{
		int totalWait = 0;
		int countCars = 0;
		std::multimap<std::string, car*>::iterator itr;
		for (itr = CityRecord->begin(); itr != CityRecord->end(); itr++)
		{
			totalWait += (itr->second->endTime - itr->second->startTime);
			countCars++;
		}
		return totalWait / static_cast<double>(countCars);
	}
	void show_stats()
	{
		//OUTPUT RESULTS
		std::cout << "Total simulation time: " << total_time/60.0 << " hours" << std::endl;
		std::cout << "Total cars that left the city: " << CityRecord->size() << std::endl;
		std::cout << "Average city visit time: " << visitAVG() << " minutes" << std::endl;
		system("pause");
	}
	void searchSequence()
	{
		//ALLOWS THE SIMULATOR TO SEARCH FOR NAMES OF VISITORS
		std::string userinput = " ";
		std::stack<car*>* results = new std::stack<car*>();
		while (userinput != "-1")
		{
			system("CLS");
			std::cout << "Search for a visitor (-1 to exit): ";
			std::cin >> userinput;
			if (userinput == "-1")
				break;
			std::multimap<std::string, car*>::iterator itr;
			for (itr = CityRecord->begin(); itr != CityRecord->end(); itr++)
			{
				if (itr->second->name == userinput)
					results->push(itr->second);
			}
			if (results->size() > 0)
			{
				while (!results->empty()){
					std::cout << "Name: " << results->top()->name << std::endl;
					std::cout << "Speed: " << results->top()->speed << std::endl;
					switch (results->top()->myDestination)
					{case 0: std::cout << "Destination: Bank" << std::endl; break;
					default: std::cout << "Destination: School" << std::endl; break;}
					switch (results->top()->myHome)
					{case 0: std::cout << "Home: Jackson" << std::endl; break;
					default: std::cout << "Home: Travis" << std::endl; break;}
					std::cout << "Travel time: " << results->top()->endTime - results->top()->startTime << " minutes" << std::endl;
					std::cout << std::endl;
					results->pop();
				}
			}
			else 
				std::cout << "\"" << userinput << "\" not found." << std::endl;
			system("pause");
		}
	}
};

#endif