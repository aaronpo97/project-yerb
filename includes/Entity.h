#ifndef ENTITY_H
#define ENTITY_H

#include "Components.h"
#include <memory>
#include <string>

class Entity {
private:
  friend class EntityManager;
  bool        m_active = true;
  size_t      m_id     = 0;
  std::string m_tag    = "default";

  Entity(const size_t id, const std::string &tag) :
      m_id(id), m_tag(tag) {}

public:
  // component pointers
  std::shared_ptr<CTransform> cTransform;
  std::shared_ptr<CShape>     cShape;
  std::shared_ptr<CInput>     cInput;
  std::shared_ptr<CLifespan>  cLifespan;
  std::shared_ptr<CEffects>   cEffects;

  // private member access functions
  bool               isActive() const;
  const std::string &tag() const;
  size_t             id() const;
  void               destroy();
};

#endif // ENTITY_H