#include <cstdint>
#define EXTRACT_FIELD_BITS(x) (x >> 26) && 0x3F
#define EXTRACT_ADDR_MODE(x) (x >> 24) & 0x3
#define EXTRACT_INS_BROAD(x) (x >> 19) & 0x1F
#define EXTRACT_INS_SPEC(x) (x >> 16) & 0x7
#define EXTRACT_OPERAND(x) x & 0xFF

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

    byte stall : 1;         // Whether an instruction has requested the next cpu cycle
    byte tickCounter : 6;   // Stores a counter determining the stage that an instruction is at
};

struct Instruction{
    //!The fields are ordered so that the bits get fitted into whole bytes and there are no overflows
    //Field comparison bits
    byte xa : 1;    // XOr / And    0 / 1   - Determines comparison used for field bits
    byte zn : 1;    // Zero / Not   0 / 1   - Determines whether the comparison result should be zero
    byte carryFlag : 1;
    byte zeroFlag : 1;
    byte negativeFlag : 1;
    byte signedOverflowFlag : 1;
    //Addressing mode
    byte addrMode : 2;

    //Instruction definition
    byte instructionBroad : 5;
    byte instructionSpecific : 3;

    //Right Half Word
    word rightHalf : 16;
};

struct ControlBus{
    byte rwFlag : 1;
};

class Processor{
public:
    word MAR;
    word MDR;
    Instruction CIR;
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