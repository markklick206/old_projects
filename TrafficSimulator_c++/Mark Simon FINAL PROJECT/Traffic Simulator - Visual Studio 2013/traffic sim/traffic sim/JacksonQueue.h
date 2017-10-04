#ifndef JACKSON_Q_H_
#define JACKSON_Q_H_

#include <iostream>
#include <string>
#include <queue>
#include "car.h"
#include "Random.h"
#include <fstream>
#include <vector>

extern Random my_random;

class JacksonQueue
{
private:
	double arrival_rate;			// car arrival rate per minute
	int total_wait;					// total accumulated wait time in the arrival queue
	int num_arrived;				// number of cars entered through the arrival queue
	std::vector<std::string>*names;	// names, yo!
	int currentNAME;
public:
	std::queue<car*> the_queue;	// queue of cars arriving
	JacksonQueue() : total_wait(0), num_arrived(0) {
		names = new std::vector<std::string>;
		std::string line;
		int numlines = 0;
		std::ifstream myfile("names.txt");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				(*names).push_back(line);
				numlines++;
			}
			myfile.close();
		}
		else std::cout << "Unable to open file";
		currentNAME = 0;
	}

	void set_arrival_rate(double arrival_rate) {
		this->arrival_rate = arrival_rate - arrival_rate/2;
	}

	int get_total_wait() {
		return total_wait;
	}

	int get_num_arrived() {
		return num_arrived;
	}

	void Update(int clock)
	{
		if (my_random.next_double() < arrival_rate) {
			std::string name = (*names)[currentNAME];
			the_queue.push(new car(name, clock, 0));
			// uses only even numbered names, the other queue will use odd
			currentNAME++;
			currentNAME++;
			currentNAME %= 6507;
		}
	}
	friend class Simulator;
	friend class JacksonRD;
};

#endif