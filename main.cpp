#include "Memory.h"
#include <iostream>

int main(){
    Device* mem = (Device*)(new Memory());
    Driver* driver = new Driver();
    driver->devices = mem;
    
    int32_t ins = 0x00200012;
    Instruction* mov = new Instruction();
    mov->instructionBroad = 0b00100;
    mov->rightHalf = 0x0012;
    *(Instruction*)&(((Memory*)(mem))->memory[0]) = *mov;
    
    Instruction* bar = (Instruction*)&ins;
    
    Instruction* sub = new Instruction();
    sub->instructionBroad = 0b10010;
    sub->instructionSpecific = 0b001;
    sub->rightHalf = 0x1009;
    *(Instruction*)&(((Memory*)(mem))->memory[4]) = *sub;
    
    driver->clock();
    std::cout << driver->processor->registers[0] << std::endl;
    driver->clock();
    std::cout << driver->processor->registers[0] << std::endl;
    driver->clock();
    std::cout << driver->processor->registers[0] << std::endl;
    std::cout << driver->processor->registers[1] << std::endl;
    
    std::cin.get();
}
