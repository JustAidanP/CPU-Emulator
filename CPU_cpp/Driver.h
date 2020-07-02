#include "Processor.h"

class Device{
public:
    //------Variables------
    //Devices are stored as a linked list
    Device* nextDevice = nullptr;

    //------Procedures/Functions------
    virtual void clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus) {}
    virtual void clockHigh() {}
    virtual void clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus) {}
};

class Driver{
public:
    word addrBus;
    word dataBus;
    ControlBus controlBus;

    Processor* processor;
    Device* devices = nullptr;

    Driver();

    void clockRising();
    void clockHigh();
    void clockFalling();

    void clock();
};