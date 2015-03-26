/* 
 * main running file
 * Jonatan Waern 7/3 -15
 * Implements the running interface and some functions of the linearproblem
 * class.
 *
 * Note: "interactive" mode currently does not work properly (only having steps
 * of 2 and -1)
 */

#include "main.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>

using namespace RLF;

enum PrintMode {VALUE,MOVE,NONE};
enum RunMode {INTERACTIVE,OFFLINE};
enum EndCondition {STEPS, CONDITION};
enum ProbInstance {ENDPOINTS,MIDDLE,CUSTOM};
PrintMode pm;
RunMode rm;
ProbInstance pi;
EndCondition ec;
int numsteps = 10000;

linearProblem problem;
void interactive()
{
  int in = 1;
  while (in != 0) {
    int state = problem.getState();
    std::cout << state << ": ";
    std::cout << "{-1;" << problem.getQValue(state,-1) << "}";
    std::cout << "{2;" << problem.getQValue(state,2) << "}\n";
    std::cout << "Prompt? (-1/2 or 0 to quit): " << std::flush;
    std::cin >> in;
    if (in != 0 && in != -1 && in != 2) {
      std::cout << "Invalid" << std::endl;
    } else if (in != 0) {
      problem.perform(in);
    }
  } 
}

const int numAfterToEnd = 10;
int currentGoals = 0;
bool endAfterNumGoals(int state) {
  if (problem.isGoalState(state)) {
    currentGoals++;
  }
  return !(currentGoals < numAfterToEnd);
}


int main(int argc, char *argv[])
{
  int probsize = 5;
  ec = STEPS;
  pm = MOVE;
  rm = OFFLINE;
  pi = ENDPOINTS;
  std::vector<int> goals;
  std::vector<int> steps;
  std::vector<int> traps;
  steps.push_back(2);
  steps.push_back(-1);
  for (int argindex = 1; argindex < argc; ++argindex) {
    if (strcmp(argv[argindex],"--interactive") == 0)
      rm = INTERACTIVE;
    else if (strcmp(argv[argindex],"--offline") == 0)
      rm = OFFLINE;
    else if (strcmp(argv[argindex],"--pmoves") == 0)
      pm = MOVE;
    else if (strcmp(argv[argindex],"--pvalues") == 0)
      pm = VALUE;
    else if (strcmp(argv[argindex],"--pnone") == 0)
      pm = NONE;
    else if (strcmp(argv[argindex],"--size") == 0)
      probsize = atoi(argv[++argindex]);
    else if (strcmp(argv[argindex],"--endafter") == 0) {
      numsteps = atoi(argv[++argindex]);
      ec = STEPS;
    } else if (strcmp(argv[argindex],"--endcondition") == 0)
      ec = CONDITION;
    else if (strcmp(argv[argindex],"--goals") == 0) {
      if (strcmp(argv[argindex+1],"endpoints") == 0) {
	pi = ENDPOINTS;
	      ++argindex;
      } else if (strcmp(argv[argindex+1],"middle") == 0) {
	pi = MIDDLE;
	++argindex;
      } else {
	pi = CUSTOM;
	int numgoals = atoi(argv[++argindex]);
	goals = std::vector<int>();
	for (int n = 0; n < numgoals; n++) {
	  goals.push_back(atoi(argv[++argindex]));
	}
      }
    } else if (strcmp(argv[argindex],"--steps") == 0) {
	int numsteps = atoi(argv[++argindex]);
	steps = std::vector<int>();
	for (int n = 0; n < numsteps; n++) {
	  steps.push_back(atoi(argv[++argindex]));
	}
    } else if (strcmp(argv[argindex],"--traps") == 0) {
	int numtraps = atoi(argv[++argindex]);
	traps = std::vector<int>();
	for (int n = 0; n < numtraps; n++) {
	  traps.push_back(atoi(argv[++argindex]));
	}
    } else
      std::cout << "Unrecognized argument " << argindex << std::endl;
  }
  
  
  if (pi == ENDPOINTS) {
    goals.push_back(0);
    goals.push_back(probsize-1);
  }
  if (pi == MIDDLE) {
    goals.push_back(probsize/2);
  }

  problem = linearProblem(probsize,steps,goals,traps);
  problem.start();
  switch (rm) {
  case INTERACTIVE:
    interactive();
    break;
  case OFFLINE:
    switch (ec) {
    case STEPS:
      problem.tick(numsteps);
      break;
    case CONDITION:
      problem.tickUntill(&endAfterNumGoals);
      break;
    }
  default:
    break;
  }
  if (pm != NONE)
    problem.print();
}

linearProblem::linearProblem(int size_,
			     std::vector<int> steps_,
			     std::vector<int> goals_,
			     std::vector<int> traps_)
  : RLframework(0.3,0.7,1.0)
{
  size=size_;
  steps=steps_;
  goals=goals_;
  traps=traps_;
}

bool linearProblem::isGoalState(int state) {
  return std::find(goals.begin(),goals.end(),state) != goals.end();	
}

void linearProblem::print()
{
  std::cout << "[";
  for (int i = 0; i < size; ++i)
    {
      if (i != 0)
      	std::cout << ",";
      //std::cout << i << "> ";
      if (std::find(goals.begin(),goals.end(),i) != goals.end())
	std::cout << "G";
      else  if (std::find(traps.begin(),traps.end(),i) != traps.end())
	std::cout << "T";
      else {
	if (pm == VALUE) {
	  std::vector<int> step = getPossibleActions(i);
	  std::cout << "{";
	  std::vector<int>::iterator curr = step.begin();
	  std::cout << *curr << ";" << getQValue(i,*curr);
	  curr++;
	  while (curr != step.end()) {
	    std::cout << ", " << *curr << ";" << getQValue(i,*curr);
	    curr++;
	  }
	  std::cout << "}";
	} else if (pm == MOVE) {
	  std::vector<int> step = getPossibleActions(i);
	  std::vector<int>::iterator curr = step.begin();
	  int bestaction = *curr;
	  float highq = getQValue(i,bestaction);
	  curr++;
	  while (curr != step.end()) {
	    float newq = getQValue(i,*curr);
	    if (newq > highq) {
	      bestaction = *curr;
	      highq = newq;
	    }
	    curr++;
	  }
	  std::cout << bestaction;
	}
      }
    }
  std::cout << "]\n";
}
