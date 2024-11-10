#include <utility>

#include "../../includes/GameEngine/Action.hpp"
#include "../../includes/Helpers/Vec2.hpp"

Action::Action(std::string name, const ActionState &state, const std::optional<Vec2> &pos) :
    m_name(std::move(name)), m_state(state), m_pos(pos) {}

const std::string &Action::getName() const {
  return m_name;
}

const ActionState &Action::getState() const {
  return m_state;
}

const std::optional<Vec2> &Action::getPos() const {
  return m_pos;
}
