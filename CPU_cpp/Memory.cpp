#include "Memory.h"

void Memory::clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus){
    //Stores the buses into the internal config
    this->addr = *_addrBus;
    this->data = *_dataBus;
    this->rwFlag = _controlBus->rwFlag;
    this->dataSize = _controlBus->dataSize;
}
void Memory::clockHigh(){
    if (this->addr > 4096) return;
    //Stores the value at the memory location
    if (this->rwFlag) this->StoreValue(this->addr, this->data);
    //Loads the data at the memory location into the return data
    else this->returnData = this->LoadValue(this->addr);
}
void Memory::clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus){
    // Writes the returnData value onto the data bus
    *_dataBus = this->returnData;
}


void Memory::StoreValue(word addr, word value){
    // Stores the datasize amount of the value at the address big endian
    // e.g. if datasize == quarter (11) then the last big endian byte is stored at the exact address location
    word baseAddr = (addr - this->dataSize);
    if (this->dataSize <= 0) this->memory[baseAddr] = value >> 24;
    if (this->dataSize <= 1) this->memory[baseAddr + 1] = value >> 16;
    if (this->dataSize <= 2) this->memory[baseAddr + 2] = value >> 8;
    this->memory[baseAddr + 3] = value;
}
word Memory::LoadValue(word addr){
    // Gets the datasize amount of the value at the address big endian
    // e.g. if datasize == quarter (11) then the last big endian byte is stored at the exact address location
    word baseAddr = (addr - this->dataSize);
    word byte_0 = 0;
    if (this->dataSize <= 0) byte_0 = (this->memory[baseAddr] & 0xFF) << 24;
    word byte_1 = 0;
    if (this->dataSize <= 1) byte_1 = (this->memory[baseAddr + 1] & 0xFF) << 16;
    word byte_2 = 0;
    if (this->dataSize <= 2) byte_2 = (this->memory[baseAddr + 2] & 0xFF) << 8;
    word byte_3 = this->memory[baseAddr + 3] & 0xFF;

    return byte_0 | byte_1 | byte_2 | byte_3;
    // return ((this->memory[addr] & 0xFF) << 24) | ((this->memory[addr + 1] & 0xFF) << 16) | ((this->memory[addr + 2] & 0xFF) << 8) | (this->memory[addr + 3] & 0xFF);
}
