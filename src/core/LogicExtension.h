#ifndef LOGIC_EXTENSION_H
#define LOGIC_EXTENSION_H

#include "Context.h"
#include <memory>

class LogicExtension
{
public:
  virtual void do_logic(std::shared_ptr<Context> context, std::shared_ptr<Sprite> trigger) = 0;
};

#endif
