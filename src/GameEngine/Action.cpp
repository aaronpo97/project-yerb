#include "../../includes/GameEngine/Action.hpp"

Action::Action(const std::string &name, const ActionState &state) :
    m_name(name), m_state(state) {}

const std::string &Action::getName() const {
  return m_name;
}

const ActionState &Action::getState() const {
  return m_state;
}
