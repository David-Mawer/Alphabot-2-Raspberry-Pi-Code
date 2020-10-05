#include "appState.h"

//ctor
appState::appState()
{
}

 //dtor
appState::~appState()
{
}


appState* appState::myInstance()
{
    // Enforce single instance by Local Static variable
    static appState _stateInstance;
    return &_stateInstance;
}

bool appState::isBotActive() {
    return ((botInitialised == true) && (botActive == true));
}


