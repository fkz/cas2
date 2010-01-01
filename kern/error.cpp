#include "error.h"


CAS::Error::Error(CAS::Error::State st)
: state (st)
{

}

void CAS::Error::ToString(std::ostream& stream) const
{
  switch (state)
  {
    case Unknown:
      stream << "Unknown";
      return;
    case Infinity:
      stream << "Infinity";
      return;
    case minusInfinity:
      stream << "-Infinity";
      return;
    default:
      assert (0);
  }
}
