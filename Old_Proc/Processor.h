#include <cstdint>
typedef uint32_t word;
typedef char byte;

struct ControlRegister{
    //CNZS Flags
    byte carryFlag : 1;
    byte zeroFlag : 1;
    byte negativeFlag : 1;
    byte signedOverflowFlag : 1;

    //! These flags are placed onto the control bus on clockFalling
    byte rwFlag : 1;        //Read / Write              0 / 1
    byte idFlag : 1;        //Instruction / Data mode   0 / 1

    byte stall : 1;     // If 1 then don't execute clockHigh
    byte halt : 1;      //If 1 then the program has finished executing
    byte filling: 1;    //If 1 then the instruction queue is being filled
    byte tickCounter : 6;
};

struct Instruction{
    //Field comparison bits
    byte xa : 1;    // XOr / And    0 / 1   - Determines comparison used for field bits
    byte zn : 1;    // Zero / Not   0 / 1   - Determines whether the comparison result should be zero
    byte carryFlag : 1;
    byte zeroFlag : 1;
    byte negativeFlag : 1;
    byte signedOverflowFlag : 1;

    //Instruction definition
    byte instructionBroad : 5;
    byte instructionSpecific : 3;
    
    //Addressing mode
    byte addrMode : 2;

    //Right Half Word
    word rightHalf : 16;
};

class Processor{
    word MAR;
    word MDR;
    ControlRegister CR;
    word registers[16] = { 0 };
    word* PC;
    word* SP;
    word insQueue[2] = { 0 };

    Processor();

    word ALU(word base, word modifier, byte operation);
    word FPU(word base, word modifier, byte operation);

    word GetAddress(byte addrMode, word operand, bool fullSize);    // Fullsize determines whether the operand is 16 or 12 bits

    void clockRising(word* _addrBus, word* _dataBus, byte* _controlBus);
    void clockHigh();
    void clockFalling(word* _addrBus, word* _dataBus, byte* _controlBus);
};