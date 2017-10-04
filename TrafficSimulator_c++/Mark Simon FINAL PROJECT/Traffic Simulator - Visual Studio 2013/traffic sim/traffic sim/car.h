#ifndef CAR_H_
#define CAR_H_
#include <stack>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Random.h"
#include <stack>

extern Random my_random;

struct car {
	int speed;
	int myDestination;
	int myHome;
	int currentTimeLeft;
	int startTime;
	int waitTime;
	int endTime;
	std::string name;
	std::stack<int> route;
	car(std::string NewName, int NewStartTime, int home)
	{
		speed = my_random.next_int(10) + 25;
		startTime = NewStartTime;
		name = NewName;
		myHome = home;
		myDestination = my_random.next_int(2);
		initRoute();
	}
	void initRoute()
	{
		switch (myHome)
		{
		case 0: //from Jackson
			switch (myDestination)
			{
			case 0: // to Bank
				route.push(0);
				route.push(1);
				route.push(1);
				route.push(1);
				route.push(1);
				break;
			case 1: //to School
				route.push(0);
				route.push(1);
				route.push(1);
				route.push(1);
				route.push(2);
				break;
			}
			break;
		case 1: //from Travis
			switch (myDestination)
			{
			case 0: // to Bank
				route.push(0);
				route.push(0);
				route.push(0);
				route.push(0);
				route.push(1);
				break;
			case 1: //to School
				route.push(0);
				route.push(0);
				route.push(0);
				route.push(0);
				route.push(2);
				break;
			}
			break;
		}
	}
	/*****************RULE OF THREE*****************/
	//DESTRUCTOR
	~car() 
	{
		delete this;
	}
	//COPY CONSTRUCTOR
	car(const car &other)
	{
		speed = other.speed;
		startTime = other.startTime;
		name = other.name;
		myDestination = other.myDestination;
	}
	//ASSIGNMENT OPERATOR
	// n/a
};

#endif