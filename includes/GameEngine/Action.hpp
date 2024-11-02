#pragma once
#include "../Helpers/Vec2.hpp"
#include <optional>
#include <string>

enum ActionState { START, END };
class Action {
private:
  std::string         m_name;  // Action name, e.g., "SHOOT", "JUMP"
  ActionState         m_state; // State of the action
  std::optional<Vec2> m_pos;

public:
  Action(const std::string &name, const ActionState &state, const std::optional<Vec2> &pos);

  const std::string         &getName() const;
  const ActionState         &getState() const;
  const std::optional<Vec2> &getPos() const;
};
