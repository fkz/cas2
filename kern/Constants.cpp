#include "Constants.h"


size_t CAS::UniqueTypes::GetNextId()
{
  static size_t id = 0;
  return ++id;
}
