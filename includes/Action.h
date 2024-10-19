#ifndef ACTION_H
#define ACTION_H
#include <string>

enum class ActionState { START, END }; // Ensure ActionState is defined

class Action {
private:
  std::string m_name;  // Action name, e.g., "SHOOT", "JUMP"
  ActionState m_state; // State of the action

public:
  Action(const std::string &name, const ActionState &state);

  const std::string &getName() const;
  const ActionState &getState() const;
};

#endif // ACTION_H