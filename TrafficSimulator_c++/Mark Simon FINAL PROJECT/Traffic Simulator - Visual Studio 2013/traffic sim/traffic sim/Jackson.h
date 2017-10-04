#ifndef JACKSON_H_
#define JACKSON_H_

#include <map>
#include <utility>
#include "car.h"
#include <queue>
#include "RD.h"

class Jackson : public RD
{
public:
	std::queue<car*>* feeding_queue3;
	std::queue<car*>* departure_queue3;
	Jackson(int max_capacity, int length) : RD(max_capacity, length)
	{
		feeding_queue3 = new std::queue<car*>();
		departure_queue3 = new std::queue<car*>();
	}
	int countIncomingDepartures()
	{
		return departure_queue2->size() + departure_queue3->size();
	}
	void HookRoads(std::queue<car*>* feed1, std::queue<car*>* feed2, std::queue<car*>* feed3)
	{
		feeding_queue1 = feed1;
		feeding_queue2 = feed2;
		feeding_queue3 = feed3;
	}
	std::queue<car*>* FullestQueue(std::queue<car*>* one, std::queue<car*>* two, std::queue<car*>* three)
	{
		//checks for the most congested entry point
		if ((*one).size() >= (*two).size() && (*one).size() >= (*three).size()) return one;
		else if ((*two).size() > (*one).size() && (*two).size() >= (*three).size()) return two;
		else return three;
	}
	void ShowMain()
	{
		std::multimap<int*, car*>::iterator itr;
		for (itr = RoadMap.begin(); itr != RoadMap.end(); itr++)
		{
			std::cout << itr->second->name << "\t\t\t" << itr->second->myDestination << std::endl;
		}
		showQueue(departure_queue2);
		showQueue(departure_queue3);
	}
	void ShowOutside()
	{
		//cars entering the city
		std::cout << "Awaiting to enter the city via Jackson:\n";
			showQueue(feeding_queue1);
	}
	void Update(int clock)
	{
		// place cars until road is full
		while ((RoadMap.size() + countIncomingDepartures()) < MAXTRAFFIC && (!(*feeding_queue1).empty() || !(*feeding_queue2).empty() || !(*feeding_queue3).empty()))
		{
			std::queue<car*>* largest = FullestQueue(feeding_queue1, feeding_queue2, feeding_queue3);
			if (!(*largest).empty()){
				// obtain car from entery queue
				car* car = largest->front();
				car->currentTimeLeft = static_cast<double>(RoadLength) / car->speed * 60;
				int* carTimePTR = &car->currentTimeLeft;
				(*largest).pop();

				//place car into the road
				RoadMap.insert(std::make_pair(carTimePTR, car));
			}
		}
		std::multimap<int*, car*>::iterator itr;
		for (itr = RoadMap.begin(); itr != RoadMap.cend();)
		{
			itr->second->currentTimeLeft--;
			if (itr->second->currentTimeLeft == 0)
			{
				// get the car off the road
				car* car = itr->second;
				RoadMap.erase(itr++);

				//place car into departure
				switch (car->route.top())
				{
				case 0:
					departure_queue1->push(car);
					car->endTime = clock;
					car->route.pop();
					break;
				case 1:
					departure_queue2->push(car);
					car->route.pop();
					break;
				case 2:
					departure_queue3->push(car);
					
					car->route.pop();
				}
			}
			else
			{
				++itr;
			}
		}
	}
};

#endif