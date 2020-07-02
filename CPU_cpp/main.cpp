#include "Memory.h"
#include <iostream>

int main(){
    Device* mem = (Device*)(new Memory());
    Driver* driver = new Driver();
    driver->devices = mem;
    
//    instruction mov = 0x00800012;
//    *(instruction*)&(((Memory*)(mem))->memory[0]) = mov;
//
//    instruction sub = 0x02441009;
//    *(instruction*)&(((Memory*)(mem))->memory[4]) = sub;
//
//    driver->clock();
//    std::cout << driver->processor->registers[0] << std::endl;
//    driver->clock();
//    std::cout << driver->processor->registers[0] << std::endl;
//    driver->clock();
//    std::cout << driver->processor->registers[0] << std::endl;
//    std::cout << driver->processor->registers[1] << std::endl;
    
//    instruction mov = 0x00800014
//    *(instruction*)&(((Memory*)(mem))->memory[0]) = mov;
//    *(instruction*)&(((Memory*)(mem))->memory[4]) = add;
//    *(instruction*)&(((Memory*)(mem))->memory[8]) = jmp;
    
//     Fibonacci
    instruction ins_0 = 0x80840001; //MOV 0 1
    instruction ins_1 = 0x80802000; //MOVR 2 0
    instruction ins_2 = 0x82000010; //ADD 0 0 (1)
    instruction ins_3 = 0x80801200; //MOVR 1 2
    instruction ins_4 = 0x81000040; //STA 0 [64]
    instruction ins_5 = 0x80C00000; //CLR 0
    instruction ins_6 = 0x81800040; //LDA 0 [64]
    instruction ins_7 = 0x80400004; //JMP [4]

    ((Memory*)(mem))->StoreValue(0, ins_0);
    ((Memory*)(mem))->StoreValue(4, ins_1);
    ((Memory*)(mem))->StoreValue(8, ins_2);
    ((Memory*)(mem))->StoreValue(12, ins_3);
    ((Memory*)(mem))->StoreValue(16, ins_4);
    ((Memory*)(mem))->StoreValue(20, ins_5);
    ((Memory*)(mem))->StoreValue(24, ins_6);
    ((Memory*)(mem))->StoreValue(28, ins_7);

    while (true){
        driver->clock();
        std::cout << driver->processor->registers[0] << " ";
        std::cout << ((Memory*)(mem))->LoadValue(64) << std::endl;
    }
    
    // Store test
//    instruction mov = 0x00800014;
//    instruction sta = 0x01000040;
//
//    ((Memory*)(mem))->StoreValue(0, mov);
//    ((Memory*)(mem))->StoreValue(4, sta);
//
//    driver->clock();
//    driver->clock();
//    driver->clock();
//    driver->clock();
//    driver->clock();
    
    
    
    
    std::cin.get();
}
