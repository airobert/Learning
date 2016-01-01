#include <iostream>
#include <stdlib.h> 
#include <string>
#include <list>
#include <algorithm>
#include <cmath> 
#include <exception>
using namespace std;
#include "agent.h"

// define condition, 
typedef list<int> world;

struct event
{
	condition before;
	action action;
	condition after;
};

typedef list<event> stream;

class environment{
private:
	
public:

	void print_world(world w);
};