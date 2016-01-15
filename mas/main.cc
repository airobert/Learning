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

#include <bdd.h>



// #include "enviroment.h"

using namespace std;

typedef list<int> condition;
typedef list<int> world;


struct action {
	string name;
	condition pre;
  condition post;
} ;

struct safe_action
{
	string name;
	bdd pre;
	condition post;
};

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

condition get_post_condition (int n, int active, condition pre){
	// we consider restricted precondition only.
	// 
	condition l;
	// flip over the pre condition first
	for (std::list<int>::iterator it = pre.begin(); it != pre.end(); it++){
		l.push_back(*it *(-1));
	}
	
	while ((int)l.size() != active) {
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

void print_safe_action (safe_action a){
	cout << "The pre-codition in ROBDD is:" << endl;
	cout << bddtable << a.pre << endl;
	cout<<"\tThe post-condition is: ";
	print_condition(a.post);
}

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

action get_full_action (int domain, int active, int demand){
	if (active < demand) cout << "Demand too stict" <<endl;
	action a;
	condition precondition;
	precondition = get_pre_condition(domain, demand);
	a.pre = precondition;
	condition postcondition;
	postcondition = get_post_condition (domain, active, a.pre);
	a.post = postcondition;
	return a;
}


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


int action_learning (int domain, action act)
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
  cout<<"The action learnt is:"<<endl;
  print_action(learnt);

  cout<<"The original action is:"<<endl;
  print_action(act);
  // list<int>::iterator i = act.post.begin();
  // list<int>::iterator j = learnt.post.begin();
  // while(i != act.post.end() && j != learnt.post.end()){
  // 	if (*i != *j) cout<<"<<<<<<<<<<< error in learning!!! >>>>>>>>>>>>";
  // 	i++;
  // 	j++;
  // }

  return count;
}




// int test_example ()
// {
//   Smodels smodels;
//   Api api (&smodels.program);

//   // You'll have to keep track of the atoms not remembered yourself
//   api.remember ();

//   Atom *a = api.new_atom ();
//   Atom *b = api.new_atom ();
//   api.set_name (a, "a");      // You can give the atoms names.
//   api.set_name (b, "b");

//   api.begin_rule (BASICRULE);
//   api.add_head (a);
//   api.add_body (b, false);  // Add "not b" to the body.
//   api.end_rule ();

//   api.begin_rule(CHOICERULE);
//   api.add_head(a);
//   api.add_head(b);
//   api.end_rule();

//   // api.begin_rule (BASICRULE);
//   // api.add_head (b);
//   // api.add_body (a, true);
//   // api.end_rule ();

//   // You would add the compute statement here, e.g.,
//   // api.set_compute (a, true) demands that a is in the model.

//   // api.done ();  // After this you shouldn't change the rules.

//   smodels.program.print ();  // You can display the program.
  
//   smodels.init ();  // Must be called before computing any models.

//   // Compute all stable models.
//   while (smodels.model ())  // Returns 0 when there are no more models
//     smodels.printAnswer ();  // Prints the answer

//   // Of course, you can inspect the atoms directly.

//   cout<<"-----------------\n";


//   smodels.revert ();  // Forget everything that happened after init ().
//   // this was changed!!!!



  
// 	// api.done (); 
//   // Atom *b2 = api.new_atom ();
//   // api.set_name (b2, "b");
//   Atom *c = api.get_atom ("b");
//   api.set_compute (c, true);
//   // Alternatively, api.set_compute (b, false).
//   // api.reset_compute (Atom *, bool) removes atoms from the compute
//   // statement.
// 	smodels.program.print (); 

//   while (smodels.model ())  // Returns 0 when there are no more models
//     smodels.printAnswer ();  // Prints the answer


//   if (smodels.model ())  // There is a model.
//     {
//       Atom *c = api.get_atom ("a");
//       if (c->Bpos)
// 	cout << c->atom_name () << " is in the stable model" << endl;
//       if (c->Bneg)
// 	cout << c->atom_name () << " is not in the stable model" << endl;
//     }

//   return 0;
// }

// void testings(){


// 	// int x = 9;
// 	// printf("%i\n", x);
// 	int myints[] = {16, 15, 14, 12};
// 	cout << "the list is: ";

// 	for (int i = 0; i <= 3; i++){
// 		cout << myints[i] << " ";
// 	}

// 	int random1;
// 	srand (time(NULL)); /* initialize random seed: */
// 	random1 = rand() % 10;
// 	printf("%s %d\n", "\nthe number is: ", random1);
// 	if (random1 > 4) puts ("Greater than 4");

// 	// get an action 
// 	action a;
// 	a.pre = get_pre_condition(10, 4);
// 	a.post = get_post_condition(10, 4, a.pre);


//   cout << "the pre-codition is: ";
//   print_condition(a.pre);

//   // list<int>::iterator i_post;
//   cout << "\nthe post-codition is: ";
//   print_condition(a.post);

//   //get a before world

//   world w_before = get_pre_condition(10, 10);
	

//   if (action_validity(a, w_before)) cout<<"this action is valid!";
//   else {
//   	cout << "The world does not satisfy the Pre-condition of the action.";
//   	cout <<"Pre-condition is: ";
//   	print_condition(a.pre); 
//   	cout <<"But the world is: ";
//   	print_world(w_before);
//   };

//   world w0_before = generate_world_from_action(a, 10);

// 	world w0_after = perform_action(a, w0_before);

// 	stream s = generate_action_stream(a,10,4);
// 	print_stream(s);

// 	action a1;
// 	// a1.name = "pick_book";
// 	// cout<<a1.name;


// }

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

    int count = action_learning(domain, act);
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

    int count = action_learning(domain, act);
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
		<< "\tThe average No.iteration is: " << i->iterations <<endl;
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


// ---------------------- Action Learning with Pre-condition -----------

void vset_bdd_gbchandler(int pre, bddGbcStat *s) {
  // cout<<"test"<<endl;
}






// void test_bdd (){
// 	bdd x,y,z;

// 	bdd_init (1000, 100);
// 	bdd_setvarnum (5);

// 	x = bdd_ithvar (1);
// 	cout<<"print x"<<endl;
// 	bdd_printtable(x);
// 	y = bdd_ithvar (2);
// 	// z = bdd_addref(bdd_apply(x, y, bddop_and));
// 	z = x & y;

// 	cout << bddtable << z << endl;
// 	bdd_printtable(z);
// 	// bdd_delref (z);

// 	bdd_printtable (bdd_not (z));
// 	bdd_done();

// }


// bdd test_bdd2 (){
// 	bdd x, y, z;
// 	bdd a, b;

// 	bdd_init (1000, 100);
// 	bdd_setvarnum (100);

// 	x = bdd_ithvar (0);
// 	y = bdd_ithvar (1);
// 	z = bdd_nithvar (1);
	
// 	// a = bdd_apply(x, y, bddop_or);
// 	// b = bdd_apply(x, z, bddop_or);
// 	a = x;
// 	cout << "BDD before: a" << endl << bddtable << a << endl;
// 	a = a & z;

// 	// bdd c = bdd_apply(a, b, bddop_and);
// 	cout << "BDD after: a" << endl << bddtable << a << endl;
// 	// cout << "BDD: b" << endl << bddtable << b << endl;
// 	// cout << "BDD: c" << endl << bddtable << c << endl;

// 	return a;

// }

void print_bdd(bdd b){
	bdd_printtable(b);
}


bdd encode_world_bdd (condition c){
	// bdd_init (1000, 100);
	// bdd_setvarnum (100);
	// encode the pre-condition and return a BDD
	// list<bdd> stack;
	// cout << "encode preconditoin "<< endl;
	// print_condition (c);

  bdd b;
  bdd init = bddtrue;
	for (std::list<int>::iterator it = c.begin(); it != c.end(); it++){	
	  if (*it < 0){
	  	// cout<<"what?"<<endl;
	  	b = bdd_nithvar(*it * (-1));
	  	init = init & (b);
	  	
	  }else{
	  	b = bdd_ithvar(*it);
	  	init = init & (b);
	  }
		// acc = acc & b;
		// print_bdd(acc);
	}
	// bdd_done();
	return init;
}



int full_action_learning (int domain, action act)
{
	Smodels smodels;
  Api api (&smodels.program);

  // You'll have to keep track of the atoms not remembered yourself
  api.remember ();

	setup_prop_vars (&api, domain);

  smodels.init (); 

	int smodel_size = 0;
	int count = 0;
	safe_action a;
	a.pre = bddfalse;

  // cout << "<<<<<<<<<<<<<<<<<  I am looping  >>>>>>>>>>>>>>>>>>"<<endl;
	while (smodel_size != 1){
		// cout << "        <<------  This is the " << count << " iteration  ----->>     "<<endl;
		// print_action (act);
		action tmp;
		tmp.post = act.pre;
		world w_before = perform_action(tmp, get_before_world(domain));

		if (action_validity(act, w_before)){
			world w_after = perform_action(act, w_before);
			// print_before_after(w_before, w_after);
			encode_worlds(&api, w_before, w_after, domain);
			smodels.revert (); 
			if (only_one_model(&smodels)) smodel_size = 1;	
	// then learn the world ----- valid
			bdd w = encode_world_bdd(w_before);
			// cout<<" world is encoded as: "<<endl;
			// print_bdd(w);
			bdd  p = (a.pre) | w; 
			// print_bdd(p);
			a.pre = p;
			// w.bdd_delref();
			// cout<<"update BDD"<<endl;
			// print_bdd(a.pre);
			// cout<<"----"<<endl;
		}

		count ++;
	}


	smodels.revert();
	// count_and_print(&smodels); 

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

			if (ap->Bpos) a.post.push_back(index);
			if (an->Bpos) a.post.push_back(index * (-1));

  	}
  }
 
  // learning completed
  // cout<<"The precondition is:" <<endl;
  // print_bdd(a.pre);
  // cout<<"The postcondition is:"<<endl;
  // print_condition(a.post);

  return count;
}





effe full_effeciency_random_test (int domain, int repeat, int active, int demand) {
	
	
	// pre = post
 	
 	int iteration_acc = 0;
 	double time_acc = 0.0;
  
  for (int i = 0; i < repeat; i ++){
		action act = get_full_action(domain, active, demand);

  	clock_t start;
    double duration;
    start = clock();

    int count = full_action_learning(domain, act);
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


list<effe> full_effeciency_random_test_all (int range, int step, int repeat, bool balanced){
	list<effe> result;

	for (int i = step; i <= range; i += step){
		int domain  = i;
		int active = (rand() % domain) + 1;

		int demand;
		if (balanced){
			demand = active;
		}else{
			demand = (rand() % active) + 1;
		}
		effe e = full_effeciency_random_test(domain, repeat, active, demand);
		result.push_back(e);
	}
	return result;
} 


void full_scalability_random_test_all (){ // it is imbalanced by default ----the scalibility table -- final table
	list<effe> result;
	int repeat  = 20;
	for (int i = 50; i <= 100 ; i += 50){
		effe e;
		e.domain  = i;

		for (int j = 0; j < repeat; j++){
			int domain  = i;
			int active = (rand() % domain) + 1;
			int demand = (rand() % active) + 1;
			effe tmp = full_effeciency_random_test(domain, 1, active, demand);
			e.average_time += tmp.average_time;
			e.iterations += tmp.iterations;
		}
		cout << "Now I am at :" << i <<endl;
		
		e.average_time = e.average_time/repeat;
		e.iterations = e.iterations /repeat;

		result.push_back(e);
	}

	for(list<effe>::iterator i = result.begin(); i != result.end(); i++){
		cout<< "The domain is: " << i->domain
		<< "\tThe average time taken is: " << i->average_time 
		<< "\tThe average No.iteration is: " << i->iterations <<endl;
	}
} 



void full_test_print_effe_all (int range, int step, int repeat, bool balanced){

	list<effe> result = full_effeciency_random_test_all (range, step, repeat, balanced);

	list<effe>::iterator i;
	for(i = result.begin(); i != result.end(); i++){
		cout<< "The domain is: " << i->domain
		<< "\tThe average time taken is: " << i->average_time 
		<< "\tThe average No.iteration is: " << i->iterations <<endl;
	}
}

void eval3 (int total){
// balanced
	bdd_init (total*total/10, total);
	bdd_setvarnum (total*(total/10));
	// bdd_gbc_hook(vset_bdd_gbchandler);
	// --- 
	bdd_gbc_hook(vset_bdd_gbchandler);

	full_test_print_effe_all (total, total/20, 100, true);
	bdd_done();
}

void eval4 (int total){
// not balanced
	bdd_init (total*total/10, total);
	bdd_setvarnum (total*(total/10));
	bdd_gbc_hook(vset_bdd_gbchandler);
	full_test_print_effe_all (total, total/20, 100, false);
	bdd_done();
}


void eval5 (){
// not balanced
	int total = 1000;
	bdd_init (total*total/10, total);
	bdd_setvarnum (total*(total/10));
	bdd_gbc_hook(vset_bdd_gbchandler);
	for (int total = 100; total <= 1000; total += 100){
		full_test_print_effe_all (total, total/10, 20, false);
		cout<<"<<<<<<<<<<<--------------------------"<<endl;
	}
	
	bdd_done();
}

// void full_action_test(){
// 	action act = get_full_action(10, 4, 2);
// 	print_action(act);
// 	cout << "finished learning in ( " << full_action_learning(10, act) << " steps";
// 	// test_bdd2();
// }


//-----imbalanced section tests ---------

struct effe_imb_section
{
	int domain;
	int demand;
	int section_min;
	int section_max;
	double average_time;
	int iterations;
	/* data */
};


effe_imb_section effeciency_section_test_imb (int domain, int demand, int min, int max, int repeat){


	int iteration_acc = 0;
 	double time_acc = 0.0;

  for (int i = 0; i < repeat; i ++){
		int active = 0;
		// if min == max then ....
		if (min == max) active = min;
		else {
			while (active <= min || active > max){
				active = (rand() % domain) + 1;
			}		
		}
  	

		action act = get_full_action(domain, active, demand);

  	clock_t start;
    double duration;
    start = clock();

    int count = full_action_learning(domain, act);
    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;

    time_acc = time_acc + duration;
    iteration_acc = iteration_acc + count; 
  }

  effe_imb_section e;
  e.domain = domain;
  e.demand = demand;
  e.section_min = min;
  e.section_max = max;
  e.average_time = time_acc / repeat;
  e.iterations = iteration_acc / repeat;
  return e;
}


list<effe_imb_section> effeciency_all_sections_imb (int domain, int step, int repeat){
	list<effe_imb_section> result;
	for (int demand = step; demand <= domain; demand += step){
		for (int max = demand ; max <= domain; max += step){
				effe_imb_section e;
			if (max == demand) 
				e = effeciency_section_test_imb(domain, demand, max, max, repeat);
			else 
				e = effeciency_section_test_imb(domain, demand, max-step, max, repeat);

		result.push_back(e);
		}
	}
	return result;
} 



void test_print_sections_imb (int range, int step, int repeat){

	list<effe_imb_section> result = effeciency_all_sections_imb (range, step, repeat);

	list<effe_imb_section>::iterator i;
	for(i = result.begin(); i != result.end(); i++){
		cout<< "The domain is: " << i->domain
		<< "\tThe demand is: " << i->demand 
		<< "\tthis range is:" << i->section_min
		<< "\t to \t" << i->section_max
		<< "\tThe average time: " << i->average_time 
		<< "\tThe average N.iteration: " << i->iterations <<endl;
	}

}

void test5(){
	//colorful
	test_print_sections_imb(100, 10, 20);
}




int main (int argc, char * argv[]) {

	// int total = atoi(argv[1]);
	// eval2(total);
	
	// int total = atoi(argv[1]);
	// eval1(total);

	int total = atoi(argv[1]);
	
	// cout<<"-- Balanced Action Learning with Pre-condition --"<<endl;
	eval3(total);
	// cout<<"-- Imbalanced Action Learning with Pre-condition --"<<endl;
	// eval4(total);

//------colorful --- 
	// test5();
	//--scalability 
	// eval5();

	// full_scalability_random_test_all ();


  return 0;
};