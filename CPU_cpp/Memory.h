#include "Driver.h"

class Memory: Device{
public:
    //------Variables------
    byte rwFlag;        //Stores whether the memory should be reading or writing
    byte dataSize;      //Stores the size of data to be written or read, two bits representing fractions of a word
    word addr;          //Stores the address in the address bus
    word data;          //Stores the data in the data bus
    word returnData;    //Stores the output data

    byte memory[4096];

    // Stores and loads a value into and from memory, all values are stored as big endian in memory
    // Stores and loads 4 bytes, starting at the address given
    void StoreValue(word addr, word value);
    word LoadValue(word addr);

    //------Overrides------
    void clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus) override;
    void clockHigh() override;
    void clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus) override;

};