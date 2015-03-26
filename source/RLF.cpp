/* 
 * RLF Implementation File
 * Jonatan Waern 5/3 -15
 * Implements the RLF class
 *
 */

//#include "RLF.h"
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include <utility>
#include <assert.h>
#include <algorithm>
#include <iostream>

namespace RLF {
  template <class State, class Action>
  class RLframework {
  public:
    //Constructs a framework
    RLframework(float learningRate_ = 0.5,
		float discountFactor_ = 0.8,
		float exploreFactor_ = 0.8)
    {
      learningRate=learningRate_;
      discountFactor=discountFactor_;
      exploreFactor=exploreFactor_;
    };
    
    //Start (restart) simulation
    State start();

    //Perform n ticks
    State tick(int n);

    //Perform untill condition is true
    //confition should be a function:
    // bool cond(State)
    State tickUntill(bool (*)(State));

    //Perform a specific action from the current state
    State perform(Action a);


    float getQValue(State state, Action action, float def = 0);

    State getState() const {return currentState;};

  private:

    Action getNextAction();
    void doAction(Action);
    
    void tickonce(void);

    //Current state in simulation
    State currentState;
    
    //q-values for state-action pairs
    typedef std::unordered_map<State,std::unordered_map<Action, float> > qmap; 
    qmap qvalue;

    float discountFactor, learningRate, exploreFactor;
    
    bool running;

    //Functions to be implemented by state-action systems
    //Given a state, find all actions possible in it
    virtual std::vector<Action> getPossibleActions(State) = 0;

    //Given a state and an action, find a new state and a reward
    virtual std::pair<State, float> getNextState(State,Action) = 0;

    //Find a list of possible starting states
    virtual std::vector<State> getInitialStates() = 0;

  };

  //Wrapper fun that let's you shorthand the search for q-values
  //using this instead of qvalue[state][map] avoids adding qvalues not yet added
  //and also gives correct default value
  template <class State, class Action>
  float RLframework<State,Action>::getQValue(const State state, const Action action, float def)
  {
    std::unordered_map<Action, float> stateMap;
    if (qvalue.find(state) != qvalue.end()) {
      stateMap = qvalue[state];
      if (stateMap.find(action) != stateMap.end()) {
	return stateMap[action];
      }	
    }
    return def;
  }

  template <class State, class Action>
  State RLframework<State,Action>::start()
  {
    //If we did this in constructor, start() would not be necessary
    running = true;
    std::vector<State> possibleStartStates = getInitialStates();
    int randnum = rand() % possibleStartStates.size();
    currentState = possibleStartStates[randnum];
    qvalue = qmap();
  }

  template <class State, class Action>
  State RLframework<State,Action>::tick(int n)
  {
    for (int i = 0; i < n; ++i)
      tickonce();
    return currentState;
  }

  template <class State, class Action>
  State RLframework<State,Action>::tickUntill(bool (*condition)(State))
  {
    while (!condition(currentState)) {
      tickonce();
    }
    return currentState;
  }

  template <class State, class Action>
  State RLframework<State,Action>::perform(Action a)
  {
    //sanity check, only perform actions we can do
    std::vector<Action> possibleMoves = getPossibleActions(currentState);
    assert(std::find(possibleMoves.begin(),possibleMoves.end(),a)
	   != possibleMoves.end() && "Impossible action to perform");
    doAction(a);
  }

  template <class State, class Action>
  Action RLframework<State,Action>::getNextAction() {
    //std::cout << "CurrState " << currentState << "; ";
    //get all possible moves
    std::vector<Action> possibleMoves = getPossibleActions(currentState);
    //std::cout << "; ";
    assert(possibleMoves.size() > 0 && "No potential actions");
  
    Action chosenMove;
    //choose if you explore
    if ((float)(((double) rand()) / RAND_MAX) < exploreFactor) {
      //choose random move
      int randnum = rand() % possibleMoves.size();
      chosenMove = possibleMoves[randnum];
      //std::cout << "Chose " << chosenMove << " at random" << std::endl;
    } else {
      typename std::vector<Action>::iterator currMove = possibleMoves.begin();
      chosenMove = *currMove;
      float maxq = getQValue(currentState,*(currMove++));
      while (currMove != possibleMoves.end()) {
	if (getQValue(currentState,*currMove) > maxq) {
	  maxq = getQValue(currentState,*currMove);
	  chosenMove = *currMove;
	}
	currMove++;
      }
      //std::cout << "Chose " << chosenMove << std::endl; 
    }
    return chosenMove;
  }

  template<class State, class Action>
  void RLframework<State,Action>::doAction(Action todo) {
    //Now, we have chosen a move. Perform it
    std::pair<State,float> result = getNextState(currentState,todo);

    //Update Q-value
  
    //Get best next move
    State nextState = result.first;
    //std::cout << "NextState " << nextState << "; ";
    std::vector<Action> nextMoves = getPossibleActions(nextState);
    //std::cout << std::endl;
    typename std::vector<Action>::iterator currNextMove = nextMoves.begin();
    float maxq = getQValue(nextState,*currNextMove);
    currNextMove++;
    for (;currNextMove != nextMoves.end();++currNextMove) {
      if (getQValue(nextState,*currNextMove) > maxq) {
	maxq = getQValue(nextState,*currNextMove);
      }  
    }

    //Write new value
    qvalue[currentState][todo] +=
      learningRate*(result.second+
		    discountFactor*maxq-getQValue(currentState,todo));
    currentState=nextState;
  }

  //Meat of algorithm
  template <class State, class Action>
  void RLframework<State,Action>::tickonce() {
    doAction(getNextAction());
  }
}
