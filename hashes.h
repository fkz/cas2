#ifndef CAS_HASHES_H
#define CAS_HASHES_H

namespace CAS {
namespace hashes {

enum Hashes {
  Number = 0xA2D3Ef,
  Frac = 0xa1463187,
  Variable = 0x1367187,
  Add = 0x7129FF,
  Mul = 0x000a1cb,
  SimpleTerm = 0x273,
  Exp = 0x013,
  Ln = 0x123AB,
  FunctionDefinition = 0x2137,
  NormalFunctionCall = 0x4619,
  BuildInFunctionDefinition = 0x634781,
  Relation = 0x01639F,
  Boolean = 0x64178,
  SimpleUniqueTerm = 0x4781,
  Derive = 0x648126,
  Extended = 0x623481
};

  
};
};

#endif