#include "Memory.h"

void Memory::clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus){
    //Stores the buses into the internal config
    this->addr = *_addrBus;
    this->data = *_dataBus;
    this->rwFlag = _controlBus->rwFlag;
}
void Memory::clockHigh(){
    if (this->addr > 4096) return;
    if (this->rwFlag) *(word*)&this->memory[this->addr] = this->data;
    else this->returnData = *(word*)&this->memory[this->addr];   //Stores the data at the memory location in the return data
}
void Memory::clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus){
    // Writes the returnData value onto the data bus
    *_dataBus = this->returnData;
}