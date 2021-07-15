#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <functional>
#include "Map.h"

enum MACHINE_STATE
{
  CONNECTING,
  CONNECTED,
  AP_MODE
};

class StateManager
{
public:
  StateManager();
  /**
   * @brief Associate the function with the given state
   * 
   * @param state The state to which associate the function
   * @param callback The callback function
   */
  void registerStateFunction(MACHINE_STATE state, std::function<void(void)> callback);
  /**
   * @brief Sets the current state
   * 
   * @param newState The new state
   */
  void setState(MACHINE_STATE newState);
  /**
   * @brief Gets the current state
   * 
   * @return MACHINE_STATE The current state
   */
  MACHINE_STATE getState() const;
  /**
   * @brief Executes the callback associated to the current state
   * 
   */
  void executeState() const;

private:
  MACHINE_STATE state = CONNECTING;
  Map<MACHINE_STATE, std::function<void(void)>, 3> callbacks;
};

#endif // STATE_MANAGER_H
