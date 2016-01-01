#include <iostream>
#include <stdlib.h> 
#include <string>
#include <list>
#include <algorithm>
#include <cmath> 
#include <exception>
using namespace std;

typedef list<int> condition;
typedef list<int> world;

struct action {
	string name;
	condition pre;
  condition post;
} ;

list<int> action_model;


class agent {
string name


}