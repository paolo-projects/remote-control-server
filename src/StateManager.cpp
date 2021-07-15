#include "StateManager.h"

StateManager::StateManager()
{
}

void StateManager::setState(MACHINE_STATE newState)
{
  state = newState;
}

MACHINE_STATE StateManager::getState() const
{
  return state;
}

void StateManager::registerStateFunction(MACHINE_STATE state, std::function<void(void)> callback)
{
  callbacks.put(state, callback);
}

void StateManager::executeState() const
{
  auto callback = callbacks[state];
  if (callback != nullptr)
  {
    (*callback)();
  }
}