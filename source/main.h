/* 
 * main header file 
 * Jonatan Waern 7/3 -15
 * Defines the linearProblem class that is used in main.cpp
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_
#include "RLF.cpp"
#include <utility>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

using namespace RLF;

//A linearProblem is moving across positions on a line untill
//you come to one that is a goal state
class linearProblem :
  public RLframework<int,int> {
public:
 linearProblem() : RLframework() {};

  linearProblem(int size,
		std::vector<int> steps,
		std::vector<int> goals,
		std::vector<int> traps);

  bool isGoalState(int);
  
  void print();
  
private:
  int size;
  std::vector<int> steps, goals, traps;
  
  std::vector<int> getPossibleActions(int state) {
    std::vector<int> actualPossibleActions;
    //std::cout << "Actions: ";
    for (std::vector<int>::iterator start = steps.begin();
	 start != steps.end();
	 ++start) {
      if ((state + *start) >= 0 && (state + *start) < size) {
	actualPossibleActions.push_back(*start);
	//std::cout << *start << " ";
      }
    }
    return actualPossibleActions;
  }

  std::pair<int,float> getNextState(int state, int step) {
    float reward = 0;
    int newState = state+step;
    //reset if we are in a trap or a goal
    if (std::find(goals.begin(),goals.end(),state) != goals.end() ||
	std::find(traps.begin(),traps.end(),state) != traps.end()) {
      std::vector<int> possibleStartStates = getInitialStates();
      //disallow starting in winning or losing states
      for (std::vector<int>::iterator s = goals.begin();
	   s != goals.end(); ++s) {
	possibleStartStates.erase(std::find(possibleStartStates.begin(),
					    possibleStartStates.end(),*s));
      }
      for (std::vector<int>::iterator s = traps.begin();
	   s != traps.end(); ++s) {
	possibleStartStates.erase(std::find(possibleStartStates.begin(),
					    possibleStartStates.end(),*s));
      }
      int randnum = rand() % possibleStartStates.size();
      newState = possibleStartStates[randnum];

    }
    assert(newState >= 0 && newState < size);
    //Check for rewards
    if (std::find(goals.begin(),goals.end(),newState) != goals.end()) {
      reward = 1;
    }
    if (std::find(traps.begin(),traps.end(),newState) != traps.end()) {
      reward = -1;
    }
    return std::make_pair(newState,reward);
  }
  
  std::vector<int> getInitialStates() {
    std::vector<int> positions;
    for (int i = 0; i < size; ++i) {
      positions.push_back(i);
    }
    return positions;
  }
};

#endif //_MAIN_H_
