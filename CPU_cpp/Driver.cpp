#include "Driver.h"

Driver::Driver(){
    this->processor = new Processor();
}

void Driver::clockRising(){
    //Clocks the processor
    if (!this->processor->CR.halt) this->processor->clockRising(&this->addrBus, &this->dataBus, &this->controlBus);
    //Loops through all of the devices and performs a clock rising
    Device* currentDevice = this->devices;
    while (currentDevice != nullptr){
        currentDevice->clockRising(&this->addrBus, &this->dataBus, &this->controlBus);
        //Gets the next device in the chain
        currentDevice = currentDevice->nextDevice;
    }
}
void Driver::clockHigh(){
    //Clocks the processor
    if (!this->processor->CR.halt) this->processor->clockHigh();
    //Loops through all of the devices and performs a clock high
    Device* currentDevice = this->devices;
    while (currentDevice != nullptr){
        currentDevice->clockHigh();
        //Gets the next device in the chain
        currentDevice = currentDevice->nextDevice;
    }
}
void Driver::clockFalling(){
    //Loops through all of the devices and performs a clock falling
    Device* currentDevice = this->devices;
    while (currentDevice != nullptr){
        currentDevice->clockFalling(&this->addrBus, &this->dataBus, &this->controlBus);
        //Gets the next device in the chain
        currentDevice = currentDevice->nextDevice;
    }
    //Clocks the processor
    if (!this->processor->CR.halt) this->processor->clockFalling(&this->addrBus, &this->dataBus, &this->controlBus);
}

void Driver::clock(){
    this->clockRising();
    this->clockHigh();
    this->clockFalling();
}