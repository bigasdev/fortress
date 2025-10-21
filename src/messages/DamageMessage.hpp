#include "components/IMessage.hpp"

class DamageMessage : public Flag::IMessage {
public:
  ~DamageMessage() override = default;

  int giver_id;
  int amount;
};
