#include <fstream>
#include <iostream>
#include <string>

int main (int argc, char **argv)
{
  if (argc != 3)
    return 1;
  std::ifstream input (argv[1]);
  std::ofstream output (argv[2]);
  std::ifstream random ("/dev/urandom");
  while (true)
  {
    std::string str;
    if (!std::getline(input, str, '\n'))
      break;
    output << str << " = ";
    size_t size;
    random.read((char*)&size, sizeof(size));
    output << size << ",\n";    
  }
}