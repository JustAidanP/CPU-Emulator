#include "Processor.h"

Processor::Processor(){
    //Creates the pc and sp aliases
    this->PC = &this->registers[15];
    this->SP = &this->registers[14];
}

void Processor::ALU(word base, word modifier, byte operation, word* output){
    word result = 0;
    //Decodes the operation
    if (operation & 0x40) {  //Bitwise
        if (operation & 0x20) {  //Gates
            if (operation & 0x10) { //Or / XOr
                if (operation & 0x8)    result = base ^ modifier; //XOr
                else                    result = base | modifier; //Or
            }
            else{  //And / Not
                if (operation & 0x8)    result = ~base;           //Not
                else                    result = base & modifier; //And
            }
        }
        else{   //Shifts
            if (operation & 0x10) { //Rot
                // Checks the first special bit for the direction
                if (operation & 0x4)    result = (base >> modifier) | (base << (32 - modifier)); //XOr
                else                    result = (base << modifier) | (base >> (32 - modifier)); //Rotate Right
            }
            else{  //Shift
            // Checks the first special bit for the direction
                if (operation & 0x4)    result = base >> modifier;//Shift Right
                else                    result = base << modifier;//Shift Left
            }
        }
    }
    else{   //Mathematical
        if (operation & 0x20) {  //Mult
            if (operation & 0x10) { //Div
                if (!(operation & 0x8)) result = base / modifier; //Div
            }
            else {
                if (!(operation & 0x8)) result = base * modifier; //Mult
            }
        }
        else{   //Add / Sub
            if (!(operation & 0x10)) { //Add / Sub
                if (operation & 0x8){ //Sub
                    // Checks the first special bit to see if there is a carry
                    if (operation & 0x4)    result = base - modifier - 1 + this->CR.carryFlag; //Subc
                    else                    result = base - modifier; //Sub
                }
                else{ //Add
                    // Checks the first special bit to see if there is a carry
                    if (operation & 0x4)    result = base + modifier + this->CR.carryFlag;//Addc
                    else                    result = base + modifier;//Add
                    if (result < base) this->CR.carryFlag = 1;  //Sets the carry bit if there was an overflow
                }
            }
        }
    }
    if (result == 0) this->CR.zeroFlag = 1; //Sets the zero flag if the result was zero
    if (result & 0x80000000) this->CR.negativeFlag = 1; //Sets the negative flag if the result is negative
    if (this->CR.negativeFlag && !(base & 0x80000000)) this->CR.signedOverflowFlag = 1; //Sets the signed overflow flag if there was a change in sign
    *output = result;
}

void Processor::FPU(word base, word modifier, byte operation, word* output){
    float result = 0;
    if ((operation >> 3) ^ 0x10)      result = *(float*)&base + *(float*)&modifier;    //Add
    else if ((operation >> 3) ^ 0x12) result = *(float*)&base - *(float*)&modifier;    //Sub
    else if ((operation >> 3) ^ 0x14) result = *(float*)&base * *(float*)&modifier;    //Mult
    else if ((operation >> 3) ^ 0x16) result = *(float*)&base / *(float*)&modifier;    //Div
    *output = *(word*)&result;
}

bool Processor::ADU(byte addrMode, word operand, word* addr, bool fullSize){
    operand = operand & (fullSize ? 0xFFFF : 0xFFF); // Converts the operand to either the 16 or 12 bits depedning on the size it should be (fullSize)
    if (addrMode & 0x2) { // Register / Register Offset
        if (addrMode & 0x1) { // Register Offset
            if (fullSize) *addr = this->registers[(operand >> 12) & 0xF] + operand & 0xFFF;
            else *addr = this->registers[(operand >> 8) & 0xF] + operand & 0xFF;
            return true;
        }
        else{ // Register
            *addr = this->registers[(operand >> (fullSize ? 12 : 8)) & 0xF]; // The value in the register defined is the address, the register is in the first 4 bits of the operand
            return true;
        }
    }
    else { // Absolute / Indirect
        if (addrMode & 0x1) { // Indirect
            if (this->CR.tickCounter == 0){
                this->MAR = operand;
                this->CR.rwFlag = 0;
                this->CR.tickCounter = 1;
                this->CR.stall = 1;
                return false;
            }
            else if (this->CR.tickCounter == 1){    //During this cycle, the data is being retrieved from memory
                this->CR.stall = 1;
                return false;
            }
            else if (this->CR.tickCounter == 2){
                *addr = this->MDR;
                this->CR.stall = 0;
                return true;
            }
            else{
                return true;
            }
        }
        else{ // Absolute
            *addr = operand; //The operand is the address to be used
            return true;
        }
    }
}

void Processor::clockRising(word* _addrBus, word* _dataBus, ControlBus* _controlBus){
    this->MAR = *this->PC;  //Move the program counter value to the mar     - First Move
    this->CR.dataSize = 0b00; //Sets the datasize to a full word
    //Extracts the datasize from the databus
    this->MDR = *_dataBus;  //Moves the data bus value into the mdr
    if (this->CR.stall) return; //The instruction has requested this cycle and so don't execute anything
    this->CIR = *(instruction*)&this->MDR;  //Moves the mdr to the cir
    (*this->PC) += sizeof(instruction);    //Increments the program counter
    this->MAR = *this->PC;  //Move the program counter value to the mar     - Second Move
}

void Processor::clockHigh(){
    //If the instruction is 0, then it is invalid so skip clockHigh
    if (*(word*)&this->CIR == 0) return;
    //! Conditions checks
    // Gets the required field bits
    byte fieldBits = EXTRACT_FIELD_BITS(this->CIR);
    // Gets the processor's field bits
    byte controlBits = this->CR.carryFlag << 3 | this->CR.zeroFlag << 2 | this->CR.negativeFlag << 1 | this->CR.signedOverflowFlag;
    // If the first bit is set, then the operator is and otherwise xor
    byte fieldState = (fieldBits & 0x80) ? (fieldBits & 0xF) & controlBits : (fieldBits & 0xF) ^ controlBits;
    // If the second bit is set, then the fieldState can't be 0
    bool conditionsMet = (fieldBits & 0x40) ? fieldState : !fieldState;
    if (!conditionsMet) return;

    // Checks if the instruction is an operator
    if (EXTRACT_INS_BROAD(this->CIR) & 0x10) {
        byte Rout = (EXTRACT_OPERAND(this->CIR) & 0xF000) >> 12; //Extracts the output register
        byte Rin  = (EXTRACT_OPERAND(this->CIR) & 0xF00)  >> 8;  //Extracts the input register
        word base = this->registers[Rin];           //Gets the base value from the register
        word modifier = 0;
        //Gets the modifier of the operation, i.e. the second value. Only if the operation isn't not
        if (EXTRACT_INS_BROAD(this->CIR) != 0x1F) {
            if (EXTRACT_INS_SPEC(this->CIR) & 0x1) modifier = EXTRACT_OPERAND(this->CIR) & 0xFF;   // Modifier is operand value
            else modifier = this->registers[((EXTRACT_OPERAND(this->CIR) & 0xF0) >> 4)];        // Modifier is value in register
        }

        word result;
        //Checks if the operation is ALU or FPU and executes the instruction
        //The operation is the combination of the broad and specific instruction part
        if (EXTRACT_INS_SPEC(this->CIR) & 0x2) this->FPU(base, modifier, (EXTRACT_INS_BROAD(this->CIR) << 3) | EXTRACT_INS_SPEC(this->CIR), &result);
        else this->ALU(base, modifier, (EXTRACT_INS_BROAD(this->CIR) << 3) | EXTRACT_INS_SPEC(this->CIR), &result);
        //Stores the result in the Rout register
        this->registers[Rout] = result;
        // Resets flags
        this->CR.rwFlag = 0;
    }
    else{ //Register Manipulation
        if (EXTRACT_INS_BROAD(this->CIR) & 0x8) { // Store / Load
            byte Rbase = (EXTRACT_OPERAND(this->CIR) & 0xF000) >> 12; //Extracts the output register
            if (EXTRACT_INS_BROAD(this->CIR) & 0x4) { // Load
                if (this->CR.tickCounter < 3){  // The address is being retrieved
                    // Gets the address, setting fullsize to false as the Address section is only 12 bits
                    word addr;
                    //If the ADR returns true, the address has been retrieved, otherwise, it is waiting
                    if (this->ADU(EXTRACT_ADDR_MODE(this->CIR), EXTRACT_OPERAND(this->CIR) & 0xFFF, &addr, false)){
                        //Stores the address in the mdr
                        this->MAR = addr;
                        //Sets the datasize in the cr
                        this->CR.dataSize = EXTRACT_INS_SPEC(this->CIR) >> 1;   //The datasize is the first 2 bits
                        //Sets flags
                        this->CR.rwFlag = 0;
                        this->CR.tickCounter = 3;
                        this->CR.stall = 1;
                    }
                }
                else if (this->CR.tickCounter == 3) this->CR.tickCounter = 4;    //Waits another clock cycle as the data is being retrieved during this cycle
                else if (this->CR.tickCounter == 4){
                    this->registers[Rbase] = this->MDR; //The value is now in the mdr
                    this->CR.stall = 0;
                    this->CR.tickCounter = 0;
                }
            }
            else{ // Store
                if (this->CR.tickCounter < 3){  // The address is being retireved
                    // Gets the address, setting fullsize to false as the Address section is only 12 bits
                    word addr;
                    //If the ADR returns true, the address has been retrieved, otherwise, it is waiting
                    if (this->ADU(EXTRACT_ADDR_MODE(this->CIR), EXTRACT_OPERAND(this->CIR) & 0xFFF, &addr, false)){
                        //Stores the address in the mdr
                        this->MAR = addr;
                        //Sets the datasize in the cr
                        this->CR.dataSize = EXTRACT_INS_SPEC(this->CIR) >> 1;   //The datasize is the first 2 bits
                        this->MDR = this->registers[Rbase]; //Stores the register into the mdr
                        //Sets flags
                        this->CR.rwFlag = 1;
                        this->CR.tickCounter = 3;
                        this->CR.stall = 1;
                    }
                }
                else if (this->CR.tickCounter == 3){
                    this->CR.rwFlag = 0;
                    this->CR.tickCounter = 4;   //Waits another clock cycle as the data is being stored in memory
                }
                else if (this->CR.tickCounter == 4){
                    this->CR.tickCounter = 0;
                    this->CR.stall = 0;
                }
            }
        }
        else{ // Jumps / Move
            if (EXTRACT_INS_BROAD(this->CIR) & 0x4){ // Mov
                if (EXTRACT_INS_BROAD(this->CIR) & 0x2) { // Other
                    if (EXTRACT_INS_BROAD(this->CIR) & 0x1) this->CR.halt = 1;  // Halt
                    else{ // Clear
                        byte Rout = (EXTRACT_OPERAND(this->CIR) >> 12) & 0xF; //Extracts the output register
                        this->registers[Rout] = 0;
                    }
                }
                else{ // Mov
                    byte Rout = (EXTRACT_OPERAND(this->CIR) >> 12) & 0xF; //Extracts the output register
                    if (EXTRACT_INS_SPEC(this->CIR) & 0x1) { // Value move
                        this->registers[Rout] = EXTRACT_OPERAND(this->CIR) & 0xFFF;
                    }
                    else{ // Register move
                        byte Rin = (EXTRACT_OPERAND(this->CIR) >> 8) & 0xF; //Extracts the input register
                        this->registers[Rout] = this->registers[Rin];
                    }
                }
            }
            else{ // Jumps
                if (EXTRACT_INS_BROAD(this->CIR) & 0x2){ // Jump
                    if (this->CR.tickCounter < 3){  // The address is being retireved
                        // Gets the address, setting fullsize based on the last bit, if it is off, then a register isn't used and so a full address is used
                        word addr;
                        //If the ADR returns true, the address has been retrieved, otherwise, it is waiting
                        if (this->ADU(EXTRACT_ADDR_MODE(this->CIR), EXTRACT_OPERAND(this->CIR) & 0xFFF, &addr, (EXTRACT_INS_BROAD(this->CIR) & 0x1) ? false : true)){
                            if (EXTRACT_INS_BROAD(this->CIR) & 0x1){ // Stores the last address in a register if a register is given
                                this->registers[(EXTRACT_OPERAND(this->CIR) >> 12) & 0xF] = *this->PC;
                            }
                            *this->PC = addr;   //Sets the PC to the address
                            this->MAR = addr;   //Sets the MAR to the address
                            // Sets flags
                            this->CR.rwFlag = 0;
                            // Takes ownership of the next cycle as the instruction is being retrieved during this cycle
                            this->CR.tickCounter = 3;
                            this->CR.stall = 1;
                        }
                    }
                    else if (this->CR.tickCounter == 3){    //During the cycle, the next instruction is being retrieved
                        //Manually increments the program counter so that the instructions get fetched as usual
                        *this->PC = *this->PC + sizeof(instruction);
                        this->MAR = *this->PC;
                        this->CR.tickCounter = 0;
                        this->CR.stall = 0;
                    }
                }
            }
        }
    }
}

void Processor::clockFalling(word* _addrBus, word* _dataBus, ControlBus* _controlBus){
    //Moves the value of the mar onto the address bus
    *_addrBus = this->MAR;
    //If the cr is write mode, then the mdr is moved to the mar and a status bit is set
    _controlBus->rwFlag = this->CR.rwFlag;
    if (this->CR.rwFlag) *_dataBus = this->MDR;
}
