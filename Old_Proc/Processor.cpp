// #include "Processor.h"

// Processor::Processor(){
//     //Creates the pc and sp aliases
//     this->PC = &this->registers[15];
//     this->SP = &this->registers[14];
// }

// word Processor::GetAddress(byte addrMode, word operand, bool fullSize){
//     operand = operand & (fullSize ? 0xFFFF : 0xFFF); // Converts the operand to either the 16 or 12 bits depedning on the size it should be (fullSize)
//     if (addrMode & 0x2) { // Register / Register Offset
//         if (addrMode & 0x1) { // Register Offset
//             if (fullSize) return this->registers[(operand >> 12) & 0xF] + operand & 0xFFF;
//             else return this->registers[(operand >> 8) & 0xF] + operand & 0xFF;
//         }
//         else{ // Register
//             return this->registers[(operand >> (fullSize ? 12 : 8)) & 0xF]; // The value in the register defined is the address, the register is in the first 4 bits of the operand
//         }
//     }
//     else { // Absolute / Indirect
//         if (addrMode & 0x1) { // Indirect
//             if (this->CR.tickCounter == 0){
//                 this->MAR = operand;
//                 this->CR.rwFlag = 0;
//                 this->CR.idFlag = 1;
//                 this->CR.tickCounter = 1;
//                 this->CR.stall = 1;
//                 return 0;
//             }
//             else if (this->CR.tickCounter == 1){
//                 return this->MDR;
//             }
//             else{
//                 return 0;
//             }
//         }
//         else{ // Absolute
//             return operand; //The operand is the address to be used
//         }
//     }
// }

// word Processor::ALU(word base, word modifier, byte operation){
//     word result = 0;
//     //Decodes the operation
//     if (operation & 0x8) {  //Bitwise
//         if (operation & 0x4) {  //Gates
//             if (operation & 0x2) { //Or / XOr
//                 if (operation & 0x1)    result = base ^ modifier; //XOr
//                 else                    result = base | modifier; //Or
//             }
//             else{  //And / Not
//                 if (operation & 0x1)    result = ~base;           //Not
//                 else                    result = base & modifier; //And
//             }
//         }
//         else{   //Shifts
//             if (operation & 0x2) { //Rot
//                 if (operation & 0x1)    result = base ^ modifier; //XOr
//                 else                    result = base | modifier; //Or
//             }
//             else{  //Shift
//                 if (operation & 0x1)    result = base >> modifier;//Shift Right
//                 else                    result = base << modifier;//Shift Left
//             }
//         }
//     }
//     else{   //Mathematical
//         if (operation & 0x4) {  //Mult
//             if (operation & 0x2) { //Div
//                 if (!(operation & 0x1)) result = base / modifier; //Div
//             }
//             else {
//                 if (!(operation & 0x1)) result = base * modifier; //Mult
//             }
//         }
//         else{   //Add / Sub
//             if (operation & 0x2) { //Sub
//                 if (operation & 0x1)    result = base - modifier - 1 + this->CR.carryFlag; //Subc
//                 else                    result = base - modifier; //Sub
//             }
//             else{  //Add
//                 if (operation & 0x1)    result = base + modifier + this->CR.carryFlag;//Addc
//                 else                    result = base + modifier;//Add
//                 if (result < base) this->CR.carryFlag = 1;  //Sets the carry bit if there was an overflow  
//             }
//         }
//     }
//     if (result == 0) this->CR.zeroFlag = 1; //Sets the zero flag if the result was zero
//     if (result & 0x80000000) this->CR.negativeFlag = 1; //Sets the negative flag if the result is negative
//     if (this->CR.negativeFlag && !(base & 0x80000000)) this->CR.signedOverflowFlag = 1; //Sets the signed overflow flag if there was a change in sign
//     return result;
// }

// word Processor::FPU(word base, word modifier, byte operation){
//     float result = 0;
//     if (operation ^ 0x10)      result = *(float*)&base + *(float*)&modifier;    //Add
//     else if (operation ^ 0x12) result = *(float*)&base - *(float*)&modifier;    //Sub
//     else if (operation ^ 0x14) result = *(float*)&base * *(float*)&modifier;    //Mult
//     else if (operation ^ 0x16) result = *(float*)&base / *(float*)&modifier;    //Div
//     return *(word*)&result;
// }

// void Processor::clockRising(word* _addrBus, word* _dataBus, byte* _controlBus){
//     if (!this->CR.rwFlag) { //Specific read mode
//         this->MDR = *_dataBus;  //Stores the value of the data bus into the MDR
//         if (!this->CR.idFlag) { //Specific instruction mode
//             //Shifts the instruction queue left
//             if (!this->CR.filling) insQueue[0] = insQueue[1];
//             //Moves the MDR value to the instruction queue
//             insQueue[1] = *_dataBus;
//             //Increments the program counter by a word
//             *this->PC += sizeof(word);
//             //Ensures that the insQueue is always filled
//             if (insQueue[0] == 0){
//                 insQueue[0] == insQueue[1];
//                 insQueue[1] = 0;
//                 this->CR.stall = 1;
//                 this->CR.filling = 1;
//             }
//         }
//     }
//     this->MAR = *this->PC;  //Moves the program counter to the MAR
// }

// void Processor::clockHigh(){
//     if (this->CR.stall) {
//         this->CR.stall = 0;
//         return;
//     }
//     //Gets the instruction
//     Instruction* ci = (Instruction*)(void*)&this->insQueue[0];
//     //! Conditions checks
//     // Checks if the instruction is an operator
//     if (ci->instructionBroad & 0x10) {
//         byte Rout = (ci->rightHalf & 0xF000) >> 12; //Extracts the output register
//         byte Rin  = (ci->rightHalf & 0xF00)  >> 8;  //Extracts the input register
//         word base = this->registers[Rin];           //Gets the base value from the register
//         word modifier = 0;
//         //Gets the modifier of the operation, i.e. the second value. Only if the operation isn't not
//         if (!(ci->instructionBroad ^ 0x1F)) {
//             if (ci->instructionSpecific & 0x1) modifier = ci->rightHalf & 0xFF;   // Manipulator is operand value
//             else modifier = this->registers[(ci->rightHalf & 0xF0 >> 4)];        // Manipulator is value in register
//         }

//         word result;
//         //Checks if the operation is ALU or FPU and executes the instruction
//         if (ci->instructionSpecific & 0x4) result = this->FPU(base, modifier, ci->instructionBroad);
//         else result = this->ALU(base, modifier, ci->instructionBroad);
//         //Stores the result in the Rout register
//         this->registers[Rout] = result;
//         // Resets flags
//         this->CR.idFlag = 0;
//         this->CR.rwFlag = 0;
//     }
//     else{ //Register Manipulation
//         if (ci->instructionBroad & 0x8) { // Store / Load
//             byte Rbase = (ci->rightHalf & 0xF000) >> 12; //Extracts the output register
//             if (ci->instructionBroad & 0x4) { // Load
//                 if (this->CR.tickCounter == 2){ // The value is in the mdr
//                     this->registers[Rbase] = this->MDR;
                    
//                     this->CR.rwFlag = 0;
//                     this->CR.idFlag = 0;
//                     this->CR.tickCounter = 0;
//                 }else{
//                     // Gets the address, setting fullsize to false as the Address section is only 12 bits
//                     word addr = this->GetAddress(ci->addrMode, ci->rightHalf & 0xFFF, false);
//                     // If the cr is in stall, then the address is still being retrieved
//                     if (!this->CR.stall){
//                         // Stores the addr into the MAR
//                         this->MAR = addr;
//                         // Sets flags
//                         this->CR.rwFlag = 0;
//                         this->CR.idFlag = 1;
//                         this->CR.tickCounter = 2;
//                         this->CR.stall = 1;
//                     }
//                 }
//             }
//             else{ // Store
//                 if (this->CR.tickCounter == 3){ // During this clock cycle the next ins is being read
//                     this->CR.idFlag = 0;
//                     this->CR.tickCounter = 0;
//                 }
//                 else{
//                     if (this->CR.tickCounter == 2){ // The data is being handled by the device at this stage
//                         this->CR.rwFlag = 0;
//                         this->CR.idFlag = 1;
//                         this->CR.stall = 1;
//                     }
//                     else{
//                         // Gets the address, setting fullsize to false as the Address section is only 12 bits
//                         word addr = this->GetAddress(ci->addrMode, ci->rightHalf & 0xFFF, false);
//                         // If the cr is in stall, then the address is still being retrieved
//                         if (!this->CR.stall){
//                             this->MDR = this->registers[Rbase];
//                             this->CR.rwFlag = 1;
//                             this->CR.idFlag = 1;
//                             this->CR.tickCounter = 2;
//                         }
//                     }
//                 }
//             }
//         }
//         else{ // Jumps / Move
//             if (ci->instructionBroad & 0x4){ // Mov
//                 if (ci->instructionBroad & 0x2) { // Other
//                     if (!ci->instructionBroad & 0x1) { // Clear
//                         byte Rout = (ci->rightHalf >> 12) & 0xF; //Extracts the output register
//                         this->registers[Rout] = 0;
//                     }
//                 }
//                 else{ // Mov
//                     byte Rout = (ci->rightHalf >> 12) & 0xF; //Extracts the output register
//                     if (ci->instructionBroad & 0x1) { // Register move
//                         byte Rin = (ci->rightHalf >> 8) & 0xF; //Extracts the input register
//                         this->registers[Rout] = this->registers[Rin];
//                     }
//                     else{ // Value move
//                         this->registers[Rout] = ci->rightHalf & 0xFFF;
//                     }
//                 }
//             }
//             else{ // Jumps
//                 if (ci->instructionBroad & 0x2){ // Jump
//                     if (this->CR.tickCounter == 2){ // The next instruction is being fetched during this cycle
//                         this->CR.idFlag = 0;
//                         this->CR.tickCounter = 0;
//                         this->insQueue[0] = 0;
//                         this->insQueue[1] = 0;
//                         //Increments the pc and mar
//                         *this->PC++;
//                         //Sets the mar to the new address
//                         this->MAR++;
//                     }
//                     else{
//                         // Gets the address, setting fullsize based on the last bit, if it is off, then a register isn't used and so a full address is used
//                         word addr = this->GetAddress(ci->addrMode, ci->rightHalf & 0xFFF, (ci->instructionBroad & 0x1) ? false : true);
//                         if (!this->CR.stall){
//                             if (ci->instructionBroad & 0x1){ // Stores the last address in a register if a register is given
//                                 this->registers[(ci->rightHalf >> 12) & 0xF] = *this->PC;
//                             }
//                             //Sets the pc to the new address
//                             *this->PC = addr;
//                             //Sets the mar to the new address
//                             this->MAR = addr;
                            
//                             this->CR.rwFlag = 0;
//                             this->CR.idFlag = 1;
//                             this->CR.tickCounter = 2;
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

// void Processor::clockFalling(word* _addrBus, word* _dataBus, byte* _controlBus){
//     //Moves the MAR to the address bus
//     *_addrBus = this->MAR;
//     //Sets the mdr to the data bus if in write mode
//     if (this->CR.rwFlag){
//         *_dataBus = this->MDR;
//         //Sets the write flag, highest bit
//         *_dataBus |= 0x80;
//     }else{
//         //Sets the read flag, highest bit
//         *_dataBus &= 0x7F;
//     }
// }