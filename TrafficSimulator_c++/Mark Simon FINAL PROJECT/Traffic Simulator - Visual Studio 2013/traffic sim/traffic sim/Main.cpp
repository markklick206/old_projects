#include <iostream>
#include "Simulator.h"
using namespace std;

int main()
{
	Simulator* sim1 = new Simulator();
	sim1->run_simulation();
	sim1->show_stats();
	sim1->searchSequence();
	system("CLS");
	cout << "Simulation of Traffic complete!" << endl;
	cout << "Traffic Simulator 2014 by Mark Klick and Simon Puzankov" << endl;
	return 0;
}