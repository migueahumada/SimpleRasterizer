#include "Kernel.h"
#include <fstream>

Vector<char> Kernel::GetSource(const String& filePath)
{
  std::fstream file(filePath, std::ios::in | std::ios::ate);

  if (!file.is_open())
  {
    return Vector<char>(0);
  }

  auto fileSize = file.tellg();

  file.seekg(std::ios::beg);

  Vector<char> source(fileSize,0);

  file.read(source.data(), source.size());

  file.close();

  return source;
  
}
