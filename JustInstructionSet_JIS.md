# JIS Instruction Set, yes the 'Just Instruction Set Instruction Set'

## Syntax
- `LABEL label_name:` - This specifices a label in the program which can be jumped to  
- `INS arg_0 arg_1 ...` - The first part is the instruction name, for the most part this is one to one to the cpi instruction set, exceptions would be jump. The other parts are associated values  
- `#value` - A plain value  
- `value` - Refers to a register, e.g. R0, PC  
- `DATA[size] data_name data` - Defines a *variable*. It allocates the specified space in memory and fills it with the defined data. If no size is provided, it is determined from the data size

**Addressing:**  
- `value` - An absolute address  
- `@value` - An indirect address  
- `[value]` - A register address, value is the register  
- `[value,offset]` - A register offset address, value is the register, offset is the offset  
- If `value` is given as a string, then it is checked against DATA or LABEL to find an associated address value there  

## Instructions
- `ADD` `Rout Rbase operand` - Adds the Rbase and the operand (can be a register or a value) and stores it in Rout  
- `ADDC` `Rout Rbase operand` - Adds the Rbase and the operand (can be a register or a value) with the carry flag and stores it in Rout  
- `FADD` `Rout Rbase operand` - Floating point Adds the Rbase and the operand (can be a register or a value) and stores it in Rout  
- `SUB` `Rout Rbase operand` - Subtracts the operand from the Rbase (can be a register or a value) and stores it in Rout  
- `SUBC` `Rout Rbase operand` - Subtracts the operand from the Rbase (can be a register or a value) with the carry flag and stores it in Rout
- `FSUB` `Rout Rbase operand` - Floating point Subtracts the operand from the Rbase (can be a register or a value) and stores it in Rout  
- `MUL` `Rout Rbase operand` - Multiplies the Rbase and the operand (can be a register or a value) and stores it in Rout  
- `FMUL` `Rout Rbase operand` - Floating point Multiplies the Rbase and the operand (can be a register or a value) and stores it in Rout  
- `DIV` `Rout Rbase operand` - Divides the Rbase by the operand (can be a register or a value) and stores it in Rout  
- `FDIV` `Rout Rbase operand` - Floating point Divides the Rbase by the operand (can be a register or a value) and stores it in Rout  

- `SHTL` `Rout Rbase operand` - Shifts the Rbase left by the operand (can be a register or a value) and stores it in Rout  
- `SHTR` `Rout Rbase operand` - Shifts the Rbase right by the operand (can be a register or a value) and stores it in Rout  
- `ROTL` `Rout Rbase operand` - Rotates the Rbase left by the operand (can be a register or a value) and stores it in Rout  
- `ROTR` `Rout Rbase operand` - Rotates the Rbase right by the operand (can be a register or a value) and stores it in Rout  

- `AND` `Rout Rbase operand` - Ands the Rbase and the operand (can be a register or a value) and stores it in Rout  
- `NOT` `Rout Rbase` - Nots the Rbase and stores it in Rout
- `OR` `Rout Rbase operand` - Ors the Rbase and the operand (can be a register or a value) and stores it in Rout  
- `XOR` `Rout Rbase operand` - Exclusive ors the Rbase and the operand (can be a register or a value) and stores it in Rout  

- `STQ` `Rin Address` - Stores a quarter word of the value (big endian, from right side) in Rin at the given address, the address can be one of the four addressing modes
- `STH` `Rin Address` - Stores a half word of the value (big endian, from right side) in Rin at the given address, the address can be one of the four addressing modes
- `STT` `Rin Address` - Stores a three quarter word of the value (big endian, from right side) in Rin at the given address, the address can be one of the four addressing modes
- `STW` `Rin Address` - Stores a word of the value (big endian, from right side) in Rin at the given address, the address can be one of the four addressing modes

- `LDQ` `Rout Address` - Loads a quarter word from the given address (big endian, from right side) and stores it in Rout, the address can be one of the four addressing modes
- `LDH` `Rout Address` - Loads a half word from the given address (big endian, from right side) and stores it in Rout, the address can be one of the four addressing modes
- `LDT` `Rout Address` - Loads a three quarter word from the given address (big endian, from right side) and stores it in Rout, the address can be one of the four addressing modes
- `LDW` `Rout Address` - Loads a word from the given address (big endian, from right side) and stores it in Rout, the address can be one of the four addressing modes

- `MOV` `Rout operand` - Moves the value of operand (can be a register or a value) into Rout

- `JMP`

- `CLR` `Rout` - Sets all of the bits to 0 in Rout
- `HLT` - Sets the halt flag for the control unit

## Example Usage
```
LABEL main:
{CZS X1}    MOV R0 #20
            ADD R1 R0 #4
            STH R1 output

            LDH R0 [R1,5]

DATA[4] output 2
DATA[1024] heap 0
```