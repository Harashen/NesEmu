#include "cpu.hpp"

int main()
{
  Cpu cpu;

  bool quit = false;
  
  cpu.LoadRom("..\\rom\\nestest.nes");

  while(!quit){
    cpu.NextOpcode();
  }

  return 0;
}