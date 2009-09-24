#ifndef CAS_TRANSFORMS_H
#define CAS_TRANSFORMS_H
#include <stdint.h>

namespace CAS{
typedef uint32_t TransformType;

namespace Transforms
{
  //alle Typen
  const TransformType Simplify = 0;
  
  //Term
  const TransformType Ableitung = 1;
  
  //FunctionDefinition
  const TransformType UmkehrFunktion = 2;

};
};

#endif