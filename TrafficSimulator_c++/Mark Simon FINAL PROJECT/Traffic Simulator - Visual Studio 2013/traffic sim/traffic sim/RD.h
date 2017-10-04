#ifndef RD_H_
#define RD_H_
#include <map>
#include <utility>
#include "car.h"
#include <queue>

class RD
{
public:
	int MAXTRAFFIC;				//ROAD CAPACITY
	int RoadLength;				//ROAD LENGTH
	std::multimap<int*, car*> RoadMap;		//CARS ON THE ROAD
	std::queue<car*>* departure_queue1;		//CARS MOVING ON TO THE NEXT ROAD
	std::queue<car*>* departure_queue2;		//CARS MOVING ON TO THE NEXT ROAD
	std::queue<car*>* feeding_queue1;		//CARS ENTERING THE ROAD
	std::queue<car*>* feeding_queue2;		//CARS ENTERING THE ROAD
	RD(int max_capacity, int length)
	{
		RoadLength = length;
		MAXTRAFFIC = max_capacity;
		departure_queue1 = new std::queue<car*>();
		departure_queue2 = new std::queue<car*>();
		feeding_queue1   = new std::queue<car*>();
		feeding_queue2   = new std::queue<car*>();
	}
	void hookRoads(std::queue<car*>* feed1, std::queue<car*>* feed2)
	{
		this->feeding_queue1 = feed1;
		this->feeding_queue2 = feed2;
	}
	void Show()
	{
		std::multimap<int*, car*>::iterator itr;
		for (itr = RoadMap.begin(); itr != RoadMap.end(); itr++)
		{
			std::cout << itr->second->name << "\t\t\t" << itr->second->myDestination << std::endl;
		}
		showQueue(departure_queue1);
		showQueue(departure_queue2);
	}
	void showQueue(std::queue<car*>* toShow)
	{
		std::queue<car*>* temp = new std::queue<car*>();
		while (!toShow->empty())
		{
			std::cout << "- " << toShow->front()->name << '\n';
			temp->push(toShow->front());
			toShow->pop();
		}
		while (!temp->empty())
		{
			toShow->push(temp->front());
			temp->pop();
		}
		delete temp;
	}
	std::queue<car*>* FullestQueue(std::queue<car*>* one, std::queue<car*>* two)
	{
		if (one->size() >= two->size()) 
			return one;
		else 
			return two;
	}
	int countDepartures()
	{
		return departure_queue1->size() + departure_queue2->size();
	}
	virtual void Update(int clock)
	{
		// place cars until road is full
		while ((RoadMap.size() + countDepartures()) < MAXTRAFFIC && ((!(*feeding_queue1).empty()) || (!(*feeding_queue2).empty())))
		{
			std::queue<car*>* largest = FullestQueue(feeding_queue1, feeding_queue2);
			if (!(*largest).empty()){
				// obtain car from entery queue
				car* car = largest->front();
				if (RoadLength == -1) // bank
					car->currentTimeLeft = my_random.next_int(11) + 10;
				else if (RoadLength == -2) //school
					car->currentTimeLeft = my_random.next_int(6) + 5;
				else
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
					car->route.pop();
					break;
				case 1:
					departure_queue2->push(car);
					car->route.pop();
					break;
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