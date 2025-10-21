#include "components/IMessage.hpp"

class CollisionMessage : public Flag::IMessage {
public:
  ~CollisionMessage() override = default;

  int dir;
};
