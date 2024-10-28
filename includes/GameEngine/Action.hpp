#pragma once
#include "../Helpers/Vec2.hpp"
#include <string>

enum ActionState { START, END };
class Action {
private:
  std::string m_name;  // Action name, e.g., "SHOOT", "JUMP"
  ActionState m_state; // State of the action
  Vec2        m_pos;

public:
  Action(const std::string &name, const ActionState &state, const Vec2 &pos = {0, 0});

  const std::string &getName() const;
  const ActionState &getState() const;
  const Vec2        &getPos() const;
};
