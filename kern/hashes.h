#ifndef CAS_HASHES_H
#define CAS_HASHES_H

namespace CAS {
namespace hashes {

enum Hashes {
#include <GenerateHashes/hashes.out.h>
  Error = 0
};

  
};
};

#endif