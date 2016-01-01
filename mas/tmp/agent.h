#include <iostream>
#include <stdlib.h> 
#include <string>
#include <list>
#include <algorithm>
#include <cmath> 
#include <exception>
using namespace std;

// define condition, 
typedef list<int> condition;

// struct action {
// 	string name;
// 	condition pre;
//   condition post;
// } ;

struct eventprocessor
{
	
};

list<int> action_model;

// definition of the agent class
class agent {
private:
	string name;
	condition get_pre_condition (int n, int s);
	condition get_post_condition (int n, int s, condition pre);

public:
	void print_condition(condition c);
	void print_action(action a);
	condition* world;

};