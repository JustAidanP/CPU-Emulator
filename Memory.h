#include "Driver.h"

class Memory: Device{
public:
    //------Variables------
    byte rwFlag;        //Stores whether the memory should be reading or writing
    word addr;          //Stores the address in the address bus
    word data;          //Stores the data in the data bus
    word returnData;    //Stores the output data

    byte memory[4096];

    void clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus) override;
    void clockHigh() override;
    void clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus) override;
};