#pragma once
#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include "States.h"
#include <vector>

class StateManager
{
public: // Public properties
	static std::vector<State*> s_states;

public: // Public methods
	static void Update();
	static void Render();
	static void PushState(State* pState); // Used when going to PauseState
	static void PopState(); // Used when leaving PauseState
	static void ChangeState(State* pState);
	static void Quit(); // Used for cleanup
	static std::vector<State*>& GetStates(); // returns a reference to our State* vector

private: // Private methods
	StateManager() {}
};

typedef StateManager STMA; // Creating a shorter alias

#endif