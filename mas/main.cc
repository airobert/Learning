// basic file operations
#include <iostream>
#include <stdio.h>
#include <time.h> 
#include <stdlib.h> 
#include <string>
#include <list>
#include <algorithm>
#include <cmath> 
#include <exception>

#include "smodels.h"
#include "api.h"
#include "atomrule.h"

#include <ctime>


// #include "enviroment.h"

using namespace std;

typedef list<int> condition;
typedef list<int> world;


struct action {
	string name;
	condition pre;
  condition post;
} ;

struct event
{
	condition before;
	action action;
	condition after;
};

typedef list<event> stream;
// typedef list<>

typedef condition answer;
typedef list<answer> epistemicmodel;


// this function takes a domain n and return a list of literals less of length 
// less than n.
// the size of precondition and post-condition are both of size s

bool abs_compare (int x, int y){
	if (abs (x) > abs (y)) return false;
		else return true;
};

condition get_pre_condition (int n, int s) { 
	condition l;
	// get a random length < n
	while ((int)l.size() != s) {
		int v = (rand() % n) +1;
		int sign = (rand() % 2) * 2 -1;
		list<int>::iterator pos1 = find(l.begin(), l.end(), v); 
		list<int>::iterator pos2 = find(l.begin(), l.end(), v * (-1));
		if (pos1 == l.end() && pos2 == l.end()){
			l.push_back(v * sign);
			// cout << "the number is: " << v * sign << endl;;
		}
	}//end for	
  l.sort(abs_compare);
	return l;
};

world get_before_world (int domain){
	return (get_pre_condition(domain, domain));

}

condition get_post_condition (int n, int s, condition pre){
	condition l;
	// get a random length < n
	while ((int)l.size() != s) {
		int v = (rand() % n) +1;
		int sign = (rand() % 2) * 2 -1;
		list<int>::iterator pos1 = find(l.begin(), l.end(), v); 
		list<int>::iterator pos2 = find(l.begin(), l.end(), v * (-1));
		//make sure also this number doesn't occure in the precondition
		list<int>::iterator pos = find(pre.begin(), pre.end(), v*sign);
		if (pos1 == l.end() && pos2 == l.end() && pos == pre.end()){
			l.push_back(v * sign);
			// cout << "the number is: " << v * sign << endl;;
		}
	}//end for	
  l.sort(abs_compare);
	return l;
};

action get_precondition_free_action (int domain, int size){
	action a;
	// condition precondition;
	// a.pre = precondition; // only declare, do nothing else.
	a.post = get_pre_condition(domain, size);
	return a;
}

void print_condition(condition c){
	list<int>::iterator i;
	for(i = c.begin(); i != c.end(); i++) cout << *i << " ";
	cout<<endl;
};

void print_action(action a){
	cout<<"\tThe pre-codition is: ";
	print_condition(a.pre);
	cout<<"\tThe post-condition is: ";
	print_condition(a.post);
	cout<<"\n";
};

void print_world(world w){
	print_condition(w);
	cout<<endl;
};

// test if the world satisfies the precondition of the action
// that is to say, all the literals are in the world w.
// So the intersection of them is the same as the precondition.
bool action_validity (action a, world w) {
	bool result = true;

	if (a.pre.begin() == a.pre.end()) return true; // precondition free action.

	for (std::list<int>::iterator it = a.pre.begin(); result && it != a.pre.end(); it++){
		list<int>::iterator pos1 = find(w.begin(), w.end(), *it); 
		if (pos1 == w.end()) result = false; // some precondition is not satisfied.
	}
	return result;
};

struct ActionPreconditionException : public exception
{
  const char * what () const throw ()
  {
    return "The action can't be performed!\nThe world does not satisfy the precondition of the action!";
  }
};

world perform_action (action a, world w) {

	// try
	// {
	// 	throw ActionPreconditionException();
	// }
	// catch(ActionPreconditionException& e)
	// {
	// 	std::cout << "Exception caught while performing the action!" << std::endl;
	//     std::cout << e.what() << std::endl;
	// }


	world after;

	for (std::list<int>::iterator it = w.begin(); it != w.end(); it++){
		list<int>::iterator pos1 = find(a.post.begin(), a.post.end(), *it); 
		list<int>::iterator pos2 = find(a.post.begin(), a.post.end(), *it * (-1)); 
		if (pos1 != a.post.end()) 
			after.push_back(*it); // some post-condition is not satisfied.
		else if (pos2 != a.post.end())
			after.push_back(*it * (-1));
		else after.push_back(*it);
	}
	return after;
};

world generate_world_from_action(action a, int size){
	
	world after;
	for (int index = 1; index <= size; index ++){
		//generate a literal 
		int l = index * ((rand() % 2) * 2 - 1);
		list<int>::iterator pos1 = find(a.pre.begin(), a.pre.end(), l);
		list<int>::iterator pos2 = find(a.pre.begin(), a.pre.end(), l*(-1));
		if (pos1 != a.pre.end()){ // this generated literal is in precondition
			after.push_back(l);
		} else if (pos2 != a.pre.end()){ // the literal is not in, but its negation is.
			after.push_back(l*(-1));
		} else {
			after.push_back(l); // neither of them, 
		}
	}
	
	return after;
};

// generate steam (before world and after world) of a certain size of length amount.
stream generate_action_stream(action a, int size, int amount){
	stream s;
	for (int i =0; i<amount; i ++){
		//generate a world which satisfy the precondition of a world
		world before = generate_world_from_action(a, size);
		world after = perform_action(a, before);
		event e;
		e.before = before;
		e.after = after;
		e.action = a;
		s.push_back(e);
	}
	return s;
};



void print_before_after (world before, world after){
	cout<<"The world before is: ";
	print_world(before);
	cout<<"Thwe world after is: ";
	print_world(after);

}



void print_stream (stream s){
	int index = 1;
	for(list<event>::iterator i = s.begin(); i != s.end(); i++) 
		{
			cout << "************ ENTRY "<< index << " **************\n";
			cout<<"The action is: \n";
			print_action((*i).action);
			print_before_after ((*i).before, (*i).after);
			index++;
		}
	cout<<endl;
};

bool only_one_model (Smodels* smodels){

	if (smodels->model ()){
		if (not smodels->model())
			return true;
	}
	return false;
}

int count_models (Smodels* smodels){
  // Compute all stable models.
  int model_counter = 0;
  while (smodels->model ()){  // Returns 0 when there are no more models
  	model_counter ++;
    // smodels->printAnswer ();  // Prints the answer
  }
  // smodels->revert ();  // Forget everything that happened after init ().
	return model_counter;
}

int count_and_print (Smodels* smodels){
  // Compute all stable models.
  int model_counter = 0;
  while (smodels->model ()){  // Returns 0 when there are no more models
  	model_counter ++;
    smodels->printAnswer ();  // Prints the answer
  }
  // smodels->revert ();  // Forget everything that happened after init ().
	return model_counter;
}

void test_add_atom (Api *api, int i){

  Atom *ap = api->new_atom ();
  Atom *an = api->new_atom ();
  Atom *ax = api->new_atom ();
  // char name[5];
  // char buf[5];
	
	char s0[10];
	char s1[10];
	char s2[10];
  // int n = 25;

  sprintf(s0, "%d", i);
  sprintf(s1, "%d", i);
  sprintf(s2, "%d", i);
	
	strcat(s0, "+");
	strcat(s1, "-");
	strcat(s2, "x");

  api->set_name (ap, s0);      // You can give the atoms names.
  api->set_name (an, s1);
  api->set_name (ax, s2);


  api->begin_rule (BASICRULE);
  api->add_head (ap);
  api->add_body (an, false);  // Add "-b" to the body.
  api->add_body (ax, false);
  api->end_rule ();

  api->begin_rule (BASICRULE);
  api->add_head (ap);
  api->add_body (ax, false);  // Add "-b" to the body.
  api->add_body (an, false);
  api->end_rule ();

  api->begin_rule (BASICRULE);
  api->add_head (an);
  api->add_body (ap, false);  // Add "-b" to the body.
  api->add_body (ax, false);
  api->end_rule ();

  api->begin_rule (BASICRULE);
  api->add_head (an);
  api->add_body (ax, false);  // Add "-b" to the body.
  api->add_body (ap, false);
  api->end_rule ();

  api->begin_rule (BASICRULE);
  api->add_head (ax);
  api->add_body (ap, false);  // Add "-b" to the body.
  api->add_body (an, false);
  api->end_rule ();

  api->begin_rule (BASICRULE);
  api->add_head (ax);
  api->add_body (an, false);  // Add "-b" to the body.
  api->add_body (ap, false);
  api->end_rule ();


  api->begin_rule (CHOICERULE);
  api->add_head (an);
  api->add_head(ax);
  api->add_head(ap);
  api->add_body(an, false);
  api->add_body(ap, false); 
  api->add_body(ax, false);
  api->set_atleast_body(2);
  api->end_rule();

  // api->done (); 

}

void encode_worlds(Api *api, world w_before, world w_after, int domain){
	//domain is not used for now. 
	//add constrants to the solver!
	for(list<int>::iterator i = w_before.begin(); i != w_before.end(); i++){
		list<int>::iterator pos1 = find(w_after.begin(), w_after.end(), *i); //if it is there 
		list<int>::iterator pos2 = find(w_after.begin(), w_after.end(), (*i * (-1))); //if it's negation there 

		// list<int>::iterator pos2 = find(l.begin(), l.end(), v * (-1));
		if (pos2 != w_after.end()){
		// 	if it's negation is there.
			
			char s[10];
			Atom *c;
			if (*i > 0){

			  sprintf(s, "%d", *i);
				strcat(s, "-");
			
			} else {
				sprintf(s, "%d", *i *(-1));
				strcat(s, "+");

			}
			c = api->get_atom (s);
			api->set_compute (c, true);


		  // api->done ();  // After this you shouldn't change the rules.
		}

		for(list<int>::iterator i = w_after.begin(); i != w_after.end(); i++){
			char s[10];
			Atom *c;
			if (*i > 0){

			  sprintf(s, "%d", *i);
				strcat(s, "-");
			
			} else {
				sprintf(s, "%d", *i *(-1));
				strcat(s, "+");

			}
			c = api->get_atom (s);
			api->set_compute (c, false);		
		}
	}


	// smodels.program.print (); 
  // cout<<"I have just encoded the event"<<endl;
}


void setup_prop_vars(Api *api, int domain){
	for (int i = 1; i <= domain; i ++)
	test_add_atom(api, i);

  api->done ();  // After this you shouldn't change the rules.
	// cout<<"All propositional variables are initialized!" <<endl;
}


int action_learning (int domain, int actable, action act)
{
	Smodels smodels;
  Api api (&smodels.program);

  // You'll have to keep track of the atoms not remembered yourself
  api.remember ();




	// int domain = 10;
	// // int observable = ;
	// int actable = 5;

	setup_prop_vars (&api, domain);



	// cout <<"************************  Now, let's learn it!  *****************************" << endl;

  // cout<<"The action is: "<<endl;
	// action act = get_precondition_free_action(domain, actable); 
	// print_action (act);

  // smodels.program.print ();  // You can display the program.
	// api.done (); 
  smodels.init (); 
	// count_and_print(&smodels); 

  // smodels.init ();  // Must be called before computing any models.


	int smodel_size = 0;
	int count = 0;
  // cout << "<<<<<<<<<<<<<<<<<  I am looping  >>>>>>>>>>>>>>>>>>"<<endl;
	while (smodel_size != 1){
		// cout << "        <<------  This is the " << count << " iteration  ----->>     "<<endl;
		// print_action (act);
		world w_before = get_before_world(domain);
		world w_after = perform_action(act, w_before);
		// print_before_after (w_before, w_after);
		
		// smodels.program.print (); 
		// smodels.revert (); 

		encode_worlds(&api, w_before, w_after, domain);
		smodels.revert (); 

		// smodels.program.print (); 
		// --- the models
		// smodels.init (); 
		// cout <<"debug 1"<<endl;
		// smodel_size = count_models (&smodels);
		if (only_one_model(&smodels)) smodel_size = 1;
		// smodel_size = count_and_print(&smodels);
		// cout << "There are/is " << smodel_size << " models"<<endl;
		// smodel_size = 1; 
		count ++;
	}

	// if (smodel_size == 1) cout<< "  Congratulations!  \n"<<" Your agent learnt this action! "<<endl;


	// cout <<"****************  End of learning. You had " << count << " iternations ***************" <<endl;
  // a->setTrue ();

  // api.begin_rule (BASICRULE);
  // api.add_head (b);
  // api.add_body (a, false);
  // api.end_rule ();

  // You would add the compute statement here, e.g.,
  // api.set_compute (a, true) demands that a is in the model.


  // this was changed!!!!
	
	// cout<<"------"<<endl;

	smodels.revert();
	// count_and_print(&smodels);
  // b->computeFalse = true;  // compute [-b]
  // a->computeFalse = true;

  // api.set_compute (b, false);
  // api.set_compute (a, false);

  // Alternatively, api.set_compute (b, false).
  // api.reset_compute (Atom *, bool) removes atoms from the compute
  // statement.
  
  // while (smodels.model ())  // Returns 0 when there are no more models
  //   smodels.printAnswer ();  // Prints the answer


	// action learnt
	action learnt;  

  if (smodels.model ())  // There is a model.
  {
  	// for each number, we get positive/negative, then unknown.
  	for (int index = 1; index <= domain; index++){
  		char sp[10];
  		char sn[10];
			Atom *ap;
			Atom *an;

		  sprintf(sp, "%d", index);
		  sprintf(sn, "%d", index);
			strcat(sp, "+");
			strcat(sn, "-");

			ap = api.get_atom (sp);
			an = api.get_atom (sn);

			if (ap->Bpos) learnt.post.push_back(index);
			if (an->Bpos) learnt.post.push_back(index * (-1));

  	}
  }
  // cout<<"The action learnt is:"<<endl;
  // print_action(learnt);

  // cout<<"The original action is:"<<endl;
  // print_action(act);


  return count;
}




int test_example ()
{
  Smodels smodels;
  Api api (&smodels.program);

  // You'll have to keep track of the atoms not remembered yourself
  api.remember ();

  Atom *a = api.new_atom ();
  Atom *b = api.new_atom ();
  api.set_name (a, "a");      // You can give the atoms names.
  api.set_name (b, "b");

  api.begin_rule (BASICRULE);
  api.add_head (a);
  api.add_body (b, false);  // Add "not b" to the body.
  api.end_rule ();

  api.begin_rule(CHOICERULE);
  api.add_head(a);
  api.add_head(b);
  api.end_rule();

  // api.begin_rule (BASICRULE);
  // api.add_head (b);
  // api.add_body (a, true);
  // api.end_rule ();

  // You would add the compute statement here, e.g.,
  // api.set_compute (a, true) demands that a is in the model.

  // api.done ();  // After this you shouldn't change the rules.

  smodels.program.print ();  // You can display the program.
  
  smodels.init ();  // Must be called before computing any models.

  // Compute all stable models.
  while (smodels.model ())  // Returns 0 when there are no more models
    smodels.printAnswer ();  // Prints the answer

  // Of course, you can inspect the atoms directly.

  cout<<"-----------------\n";


  smodels.revert ();  // Forget everything that happened after init ().
  // this was changed!!!!



  
	// api.done (); 
  // Atom *b2 = api.new_atom ();
  // api.set_name (b2, "b");
  Atom *c = api.get_atom ("b");
  api.set_compute (c, true);
  // Alternatively, api.set_compute (b, false).
  // api.reset_compute (Atom *, bool) removes atoms from the compute
  // statement.
	smodels.program.print (); 

  while (smodels.model ())  // Returns 0 when there are no more models
    smodels.printAnswer ();  // Prints the answer


  if (smodels.model ())  // There is a model.
    {
      Atom *c = api.get_atom ("a");
      if (c->Bpos)
	cout << c->atom_name () << " is in the stable model" << endl;
      if (c->Bneg)
	cout << c->atom_name () << " is not in the stable model" << endl;
    }

  return 0;
}

void testings(){


	// int x = 9;
	// printf("%i\n", x);
	int myints[] = {16, 15, 14, 12};
	cout << "the list is: ";

	for (int i = 0; i <= 3; i++){
		cout << myints[i] << " ";
	}

	int random1;
	srand (time(NULL)); /* initialize random seed: */
	random1 = rand() % 10;
	printf("%s %d\n", "\nthe number is: ", random1);
	if (random1 > 4) puts ("Greater than 4");

	// get an action 
	action a;
	a.pre = get_pre_condition(10, 4);
	a.post = get_post_condition(10, 4, a.pre);


  cout << "the pre-codition is: ";
  print_condition(a.pre);

  // list<int>::iterator i_post;
  cout << "\nthe post-codition is: ";
  print_condition(a.post);

  //get a before world

  world w_before = get_pre_condition(10, 10);
	

  if (action_validity(a, w_before)) cout<<"this action is valid!";
  else {
  	cout << "The world does not satisfy the Pre-condition of the action.";
  	cout <<"Pre-condition is: ";
  	print_condition(a.pre); 
  	cout <<"But the world is: ";
  	print_world(w_before);
  };

  world w0_before = generate_world_from_action(a, 10);

	world w0_after = perform_action(a, w0_before);

	stream s = generate_action_stream(a,10,4);
	print_stream(s);

	action a1;
	// a1.name = "pick_book";
	// cout<<a1.name;


}

// ----------------<  Evaluation 1  >--------------

struct effe_section
{
	int domain;
	int section_max;
	double average_time;
	int iterations;
	/* data */
};


effe_section effeciency_section_test(int domain, int min, int max, int repeat){


	int iteration_acc = 0;
 	double time_acc = 0.0;

  for (int i = 0; i < repeat; i ++){
		int active = 0;
  	while (active <= min || active > max){
			active = (rand() % domain) + 1;
		}

		action act = get_precondition_free_action(domain, active); 
  	

  	clock_t start;
    double duration;
    start = clock();

    int count = action_learning(domain, active, act);
    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
    // cout<<"duration is: " << duration << endl;
    time_acc += duration;
    iteration_acc = iteration_acc + count; 
  }

  effe_section e;
  e.domain = domain;
  e.section_max = max;
  e.average_time = time_acc / repeat;
  e.iterations = iteration_acc / repeat;
  return e;
}


list<effe_section> effeciency_all_sections (int domain, int step, int repeat){
	list<effe_section> result;

	for (int i = step; i <= domain; i += step){
		effe_section e = effeciency_section_test(domain, i-step, i, repeat);
		result.push_back(e);
	}
	return result;
} 



void test_print_sections (int range, int step, int repeat){

	list<effe_section> result = effeciency_all_sections (range, step, repeat);

	list<effe_section>::iterator i;
	for(i = result.begin(); i != result.end(); i++){
		cout<< "The domain is: " << i->domain
		<< "\tThe max of this range is:" << i->section_max
		<< "\tThe average time taken is: " << i->average_time 
		<< "\tThe average N.iteration is: " << i->iterations <<endl;
	}

}


// ----------------<  Evaluation 2  >--------------

struct effe
{
	/* data */
	int domain;
	double average_time;
	int iterations;
};


effe effeciency_random_test (int domain, int repeat) {
	int active = (rand() % domain) + 1;
 	
 	int iteration_acc = 0;
 	double time_acc = 0.0;
  
  for (int i = 0; i < repeat; i ++){

		action act = get_precondition_free_action(domain, active);

  	clock_t start;
    double duration;
    start = clock();

    int count = action_learning(domain, active, act);
    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;

    time_acc = time_acc + duration;
    iteration_acc = iteration_acc + count; 
  }

  effe e;
  e.domain = domain;
  e.average_time = time_acc / repeat;
  e.iterations = iteration_acc / repeat;
  return e;

}


list<effe> effeciency_random_test_all (int range, int step, int repeat){
	list<effe> result;

	for (int i = step; i <= range; i += step){
		effe e = effeciency_random_test(i, repeat);
		result.push_back(e);
	}
	return result;
} 


void test_print_effe_all (int range, int step, int repeat){

	list<effe> result = effeciency_random_test_all (range, step, repeat);

	list<effe>::iterator i;
	for(i = result.begin(); i != result.end(); i++){
		cout<< "The domain is: " << i->domain
		<< "\tThe average time taken is: " << i->average_time 
		<< "\tThe average N.iteration is: " << i->iterations <<endl;
	}

}



void eval1(int total){
	int sections = 10;
	for (int i = total/sections; i <= total ; i += total/sections){
		cout<< " ------ " << "the domain is now :" << i <<" ------ " <<endl; 
		test_print_sections (i, i/sections, 20);
	}
}

void eval2 (int total){
	test_print_effe_all (total, total/20, 20);
}


int main (int argc, char * argv[]) {

	int total = atoi(argv[1]);
	eval2(total);
	
	// int total = atoi(argv[1]);
	// eval1(total);

  return 0;
};