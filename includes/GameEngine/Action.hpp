#pragma once
#include <string>

enum class ActionState { START, END };
class Action {
private:
  std::string m_name;  // Action name, e.g., "SHOOT", "JUMP"
  ActionState m_state; // State of the action

public:
  Action(const std::string &name, const ActionState &state);

  const std::string &getName() const;
  const ActionState &getState() const;
};
