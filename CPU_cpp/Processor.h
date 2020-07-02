#include <cstdint>
#define EXTRACT_FIELD_BITS(x) ((x >> 26) & 0x3F)
#define EXTRACT_INS_BROAD(x) ((x >> 21) & 0x1F)
#define EXTRACT_INS_SPEC(x) ((x >> 18) & 0x7)
#define EXTRACT_ADDR_MODE(x) ((x >> 16) & 0x3)
#define EXTRACT_OPERAND(x) (x & 0xFFFFFFFF)

typedef uint32_t word;
typedef word instruction;
typedef char byte;

struct ControlRegister{
    //CNZS Flags
    byte carryFlag : 1;
    byte zeroFlag : 1;
    byte negativeFlag : 1;
    byte signedOverflowFlag : 1;

    //! These flags are placed onto the control bus on clockFalling
    byte rwFlag : 1;        //Read / Write              0 / 1
    byte dataSize : 2;      //Controls the word length that is being request or stored  00 - Word, 01 - Three Quarters, 10 - Half, 11 - Quarter

    byte stall : 1;         // Whether an instruction has requested the next cpu cycle
    byte halt : 1;          // Whether the processor is halted, ALL execution of the processor stops until it is manually undone
    byte tickCounter : 6;   // Stores a counter determining the stage that an instruction is at
};

struct ControlBus{
    byte rwFlag : 1;
    byte dataSize : 2;
};

class Processor{
public:
    word MAR;
    word MDR;
    instruction CIR;
    ControlRegister CR;
    word registers[16] = { 0 };
    word* PC;
    word* SP;

    Processor();

    // Arguments:   -The base value to operate on           -word     
    //              -The value that is modifying the base   -word
    //              -The operation                          -byte
    //              -The result                             -word*
    void ALU(word base, word modifier, byte operation, word* output);
    void FPU(word base, word modifier, byte operation, word* output);

    //The address decode unit (ADU)
    //Arguments:    -The addrmode       -byte
    //              -The operand        -word (right half)
    //              -The output addr    -word*
    //              -Fullsize operand   -bool
    bool ADU(byte addrMode, word operand, word* addr, bool fullSize);    // Fullsize determines whether the operand is 16 or 12 bits

    void clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus);
    void clockHigh();
    void clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus);
};