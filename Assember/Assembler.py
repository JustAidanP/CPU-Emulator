import sys, struct

class Assembler:
    #------Static Variables------
    # Stores the machine code identifiers of each instruction mnemonic
    # The code for each is the identifier, the controlBits is specific controlbits that the mnemonic uses, only used if not none
    InstructionLookupTable = {
        "ADD": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10000000,
            "controlBits": None
        },
        "ADDC": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10000100,
            "controlBits": None
        },
        "FADD": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10000010,
            "controlBits": None
        },
        "SUB": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10001000,
            "controlBits": None
        },
        "SUBC": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10001100,
            "controlBits": None
        },
        "FSUB": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10001010,
            "controlBits": None
        },
        "MUL": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10100000,
            "controlBits": None
        },
        "FMUL": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10100010,
            "controlBits": None
        },
        "DIV": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10110000,
            "controlBits": None
        },
        "FDIV": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b10110010,
            "controlBits": None
        },
        "SHTL": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11000000,
            "controlBits": None
        },
        "SHTR": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11000100,
            "controlBits": None
        },
        "ROTL": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11001000,
            "controlBits": None
        },
        "ROTR": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11001100,
            "controlBits": None
        },
        "AND": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11100000,
            "controlBits": None
        },
        "NOT": {
            "code": 0b11101000,
            "controlBits": None
        },
        "OR": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11110000,
            "controlBits": None
        },
        "XOR": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b11111000,
            "controlBits": None
        },
        "STQ": {
            "code": 0b01000000,
            "controlBits": None
        },
        "STH": {
            "code": 0b01000010,
            "controlBits": None
        },
        "STT": {
            "code": 0b01000100,
            "controlBits": None
        },
        "STW": {
            "code": 0b01000110,
            "controlBits": None
        },
        "LDQ": {
            "code": 0b01100000,
            "controlBits": None
        },
        "LDH": {
            "code": 0b01100010,
            "controlBits": None
        },
        "LDT": {
            "code": 0b01100100,
            "controlBits": None
        },
        "LDW": {
            "code": 0b01100110,
            "controlBits": None
        },
        "MOV": {
            # The last bit needs to be set depending on whether the operand is a value or a register
            "code": 0b00100000,
            "controlBits": None
        },
        "JMP": {
            "code": 0b00010000,
            "controlBits": None
        },
        "CLR": {
            "code": 0b00110000,
            "controlBits": None
        },
        "HLT": {
            "code": 0b00111000,
            "controlBits": None
        },
    }

    #------Initialiser------
    def __init__(self):
        self.instructionList = []
        self.symbolTable = []

    #------Procedures/Functions------
    def separateElements(self, line):
        """
        Separates elements of a line from whitespace into an array

        Args:
            line (String): The line to separate elements from
        """
        # Stores the elements
        elements = []
        # Stores a series of valid characters
        validChars = ""
        # Stores whether the line is in a quotation
        inQuotation = False
        # Stores whether the next character should be escaped
        isEscape = False
        for char in line:
            # If there is a whitespace character, add valid chars into the elements array
            if (char == ' ' or char == '\t') and not inQuotation:
                if len(validChars) > 0:
                    elements.append(validChars)
                    validChars = ""
            else:
                if not isEscape and char == "\"": inQuotation = not inQuotation # Toggles inQuotation
                # Checks if the next character should be escaped
                if char == "\\": 
                    isEscape = True
                    continue
                if isEscape: isEscape = False

                validChars = validChars + char
        # Appends the last word
        if len(validChars) > 0: elements.append(validChars)
        return elements
    def parseArgs(self, args, instructionCode):
        def extractRegister(register):
            """Takes in a value and parses it to a register, performing validation

            Args:
                register (string): The register code

            Returns:
                [Int]: The register, can be None
            """
            # Special Case registers
            if register.lower() == "pc": return 15
            elif register.lower() == "sp": return 14

            if register[0].lower() != "r": return None
            register = register[1:]
            if not register.isnumeric(): return None
            register = int(register)
            if register > 15 or register < 0: return None
            return register
        def extractOperand(operand, operandStruct): 
            """Takes in a value and parses it to an operand, performing validation

            Args:
                operand (string): The value
                operandStruct (dict): The operand storage
            """
            # Attempts to interpret the argument as a register
            operandRegister = extractRegister(operand)
            if operandRegister:
                # The argument was a register
                operandStruct["operand"] = operandRegister
                operandStruct["operandType"] = "register"
                return
            # If the argument begins with hash, then it is a value
            if operand[0] == "#":
                operand = operand[1:]
                # Checks if the operand is a float
                if operand[-1] == "f":
                    if not operand[:-1].isnumeric(): raise Exception("Invalid arguments for instruction")
                    operandStruct["operand"] = float(operand[:-1])
                    operandStruct["operandType"] = "float"
                # The operand is an int
                else: 
                    if not operand.isnumeric(): raise Exception("Invalid arguments for instruction")
                    operandStruct["operand"] = int(operand)
                    operandStruct["operandType"] = "int"
                return
            # The argument is likely DATA and so a symbol is stored for the instruciton compiler
            operandStruct["awaiting"].append([{"DATA": {"symbol": args[2], "output":"operand"}}])
        def extractAddress(address, operandStruct):
            """Takes in a value and parses it to an address, performing validation

            Args:
                address (string): The address
                operandStruct (dict): The operand storage
            """
            # Handles addressing mode for register or register offset
            if address[0] == "[" and address[-1] == "]":
                registerOffset = address[1:-1].split(",")
                if len(registerOffset) == 0 or len(registerOffset) > 2: raise Exception("Invalid arguments for instruction")
                # Sets the Rin component
                Rin = extractRegister(registerOffset[0])
                if Rin == None: raise Exception("Invalid arguments for instruction")
                operandStruct["Rin"] = Rin
                # Sets the addressing mode
                operandStruct["addressingMode"] = "Register"
                if len(registerOffset) == 2:
                    if registerOffset[1].isnumeric(): 
                        operandStruct["offset"] = int(registerOffset[1])
                    else:
                        # Otherwise the offset is DATA and so a symbol is stored for the instruciton compiler
                        operandStruct["awaiting"].append({"DATA": {"symbol": registerOffset[1], "output":"offset"}})
                    # Sets the addressing mode to registeroffset
                    operandStruct["addressingMode"] = "RegisterOffset"
                return
            # Handles indirect addressing mode
            if address[0] == "@":
                if len(address) == 1: raise Exception("Invalid arguments for instruction")
                indrectAddress = address[1:]
                if not indrectAddress.isnumeric(): raise Exception("Invalid arguments for instruction")
                operandStruct["indirectAddress"] = int(indrectAddress)
                # Sets the addressing mode to indirect
                operandStruct["addressingMode"] = "Indirect"
                return
            # Handles absolute addressing mode
            if address.isnumeric():
                operandStruct["address"] = int(address)
                operandStruct["addressingMode"] = "Absolute"
                return
            # Otherwise the address is a LABEL and so a symbol is stored for the instruciton compiler
            operandStruct["awaiting"].append({"LABEL": {"symbol": address, "output":"address"}})
            operandStruct["addressingMode"] = "Absolute"

        """Parses the arguments given to an operand for an instruction

        Args:
            args ([string]): The arguments given to an instruction
            instructionCode (int): The instruction code that the args are for
        """
        if type(instructionCode) != int: raise TypeError("Invalid instructionCode")
        # Stores the operand structure
        operandStruct = {
            "awaiting": [], # Any data structures in here will be evaluated when the instruction is finally compiled at the end
        }
        # Parses the arguments differently depending on the instruction
        if instructionCode & 0x80: # Operator
            if len(args) < 2: raise Exception("Invalid arguments for instruction")
            # The first argument is the output register
            outRegister = extractRegister(args[0])
            # The second argument is the base register
            baseRegister = extractRegister(args[1])
            # Ensures that the regsiters are valid
            if outRegister == None or baseRegister == None: raise Exception("Invalid arguments for instruction")
            # Sets values to the operand structure
            operandStruct["Rout"] = outRegister
            operandStruct["Rin"] = baseRegister
            # Parses the other argument if the instruction isn't NOT
            if instructionCode == 0b11101000: return operandStruct
            if len(args) < 3: raise Exception("Invalid arguments for instruction")
            # Evaluates the argument into an operand
            extractOperand(args[2], operandStruct)
            
            return operandStruct

        else: # Other
            if instructionCode >> 6 == 1: # The instruction is a store or a load
                if len(args) < 2: raise Exception("Invalid arguments for instruction")
                # The first argument is the output register
                outRegister = extractRegister(args[0])
                if outRegister == None: raise Exception("Invalid arguments for instruction")
                # Sets values to the operand structure
                operandStruct["Rout"] = outRegister
                # Gets the address from the operand
                extractAddress(args[1], operandStruct)

                # If an awaiting has been set in the operand, it will move the await type to DATA rather than LABEL (it is set to LABEL by address resolution)
                if len(operandStruct["awaiting"]) == 1 and "LABEL" in operandStruct["awaiting"][0]:
                    operandStruct["awaiting"].append({"DATA": operandStruct["awaiting"][0]["LABEL"]})
                    operandStruct["awaiting"].pop(0)
                
                return operandStruct
            elif instructionCode == 0b100000: # The instruction is a move
                if len(args) < 2: raise Exception("Invalid arguments for instruciton")
                # The first argument is the output register
                outRegister = extractRegister(args[0])
                if outRegister == None: raise Exception("Invalid arguments for instruction")
                # Sets values to the operand structure
                operandStruct["Rout"] = outRegister
                # Gets the source data from the final argument
                extractOperand(args[1], operandStruct)

                return operandStruct
            elif instructionCode == 0b00110000: # The instruction is clr
                if len(args) < 2: raise Exception("Invalid arguments for instruction")
                # The first argument is the output register
                outRegister = extractRegister(args[0])
                if outRegister == None: raise Exception("Invalid arguments for instruction")
                # Sets values to the operand structure
                operandStruct["Rout"] = outRegister

                return operandStruct
            elif instructionCode == 0b00010000: # The instruction is a jump
                # Gets the address from the operand
                extractAddress(args[0], operandStruct)

                return operandStruct
        pass
    def compileInstruction(self, ins):
        """Compiles an instruction into it's machine code

        Args:
            ins (Json): The instruction
        """
        pass
    def parseLine(self, line):
        """
        Parses a single line of the assembly

        Args:
            line (string): The line to parse
        """
        # Removes surrounding whitespace
        line = self.separateElements(line)
        if len(line) == 0: return
        # Checks if the line is a label declaration
        if line[0].lower() == "label":
            # --Validates the line
            if len(line) != 2: raise Exception("Invalid Label")
            if len(line[1]) < 2: raise Exception("Invalid Label") 
            if line[1][-1] != ':': raise Exception("Invalid Label")
            # Gets the label name
            labelName = line[1][:-1]

            # Creates a new symbol entry for the label, the pointer refers to the memory location of the label
            # It defaults to the location of the label in the instruction sequence
            self.symbolTable.append({ "type": "LABEL", "name": labelName, "pointer": len(self.instructionList) * 4})
        # Checks if the line is data declaration
        elif line[0].lower() == "data" or (line[0].lower()[:4] == "data" and line[0][4] == "["):
            # Removes the DATA tag from the data
            line[0] = line[0][4:]
            # --Validates the line
            if len(line) < 2: raise Exception("Invalid DATA")
            # Gets the data name
            dataName = line[1]
            # Stores the data length
            dataLength = 4 # A word
            # Gets any default data
            defaultData = 0
            # Stores the data type
            dataType = "int"
            if len(line) == 3:
                if line[2][0] == "\"" and line[2][-1] == "\"":
                    dataType = "string"
                    defaultData = line[2][1:-1]
                    dataLength = len(defaultData)
                elif line[2].isnumeric():
                    defaultData = line[2]
                elif line[2][-1] == 'f' and line[2][:-1].isnumeric():
                    dataType = "float"
                    defaultData = line[2][0]
            # Checks if a data length was given
            if len(line[0]) > 2 and (line[0][0] == "[" and line[0][-1] == "]"):
                data = line[0][1:-1]
                if not data.isnumeric(): raise TypeError("Invalid data length type")
                dataLength = int(data)

            # Creates a new symbol entry for the data
            self.symbolTable.append({ "type": "DATA", "name": dataName, "default": defaultData, "dataType": dataType, "length": dataLength})
        # The line is most likely an instruction
        else:
            # --Validates the line
            #Stores the control bits
            controlBits = 1 << 5 # Sets it to 0b100000
            # Checks if the first element is control bits
            if line[0][0] == "{" and line[0][-1] == "}": # First element is control bits
                # Separates the two sections of the control bits
                controlSections = line[0].split(':')
                #Goes through the characters and constructs the control bits for the instruction
                carryBits = controlSections[0].lower()
                carryFlag = int('c' in carryBits)
                zeroFlag = int('z' in carryBits)
                negativeFlag = int('n' in carryBits)
                signedOverflowFlag = int('s' in carryBits)
                #Gets the conditions bits
                if len(controlSections) == 2:
                    conditionBits = controlSections[1].lower()
                    isAnd = int('x' in conditionBits)
                    isOne = int('1' in conditionBits)
                    #Sets the last two bits on controlBits to the conditionBits
                    controlBits ^= isAnd << 1
                    controlBits ^= isOne
                # Constructs the control bits section
                controlBits ^= carryFlag << 5
                controlBits ^= zeroFlag << 4
                controlBits ^= negativeFlag << 3
                controlBits ^= signedOverflowFlag << 2
                # Removes the control bits section from the line
                line.pop(0)
            # Performs this check as the controlbits element gets removed (if it existed) and so the length of the elments could be zerp
            if len(line) == 0: raise Exception("Invalid Instruction")
            # --The first element is the instruction
            # Identifies the instruction from the mnemonic using the lookup table
            if line[0] in self.InstructionLookupTable:
                ins = self.InstructionLookupTable[line[0]]
                insCode = ins["code"]
                insControlBits = ins['controlBits'] if ins['controlBits'] else controlBits
                # Creates a representation of the instruction, this is stored in the instructionList and is assembled later
                instrucitonRepr = {
                    "code": insCode,
                    "controlBits": insControlBits,
                }
                # Parses the arguments given and stores the operandStruct returned in the instruciton representation
                if len(line) > 1: instrucitonRepr["operand"] = self.parseArgs(line[1:], insCode)
                self.instructionList.append(instrucitonRepr)

    def parseAssembly(self, assembly):
        """
        Parses an assembly

        Args:
            assembly (string): The assembly program to parse
        """
        for line in assembly.splitlines():
            self.parseLine(line)

    def assemble(self):
        """
        Takes the internal buffers and assembles every instruction
        """
        machineCodeLength = len(self.instructionList)
        # Adds all of the data lengths to the length
        for symbol in self.symbolTable:
            if symbol["type"] == "DATA":
                machineCodeLength += symbol["length"]
        # Stores the machine code instructions
        machineCode = [0 for i in range(machineCodeLength)]
        # Adds all DATA symbols to the machineCode
        dataOffset = len(self.instructionList) # Stores the offset into the machine code for the current data symbol
        for symbol in self.symbolTable:
            if symbol["type"] == "DATA":
                # Stores the operand into the memory

                # Stores the memory location of the data
                symbol["pointer"] = dataOffset
                dataOffset += symbol["length"]

        # Assembles every instruction
        for i in range(len(self.instructionList)):
            ins = self.instructionList[i]
            # Constructs the machine code instruction
            machineCode[i] |= (ins['controlBits'] & 0x3F) << 26
            machineCode[i] |= (ins['code'] & 0xFF) << 18
            # Looks through all of the awaiting in the operand and fills in the output for each
            for sym in ins['operand']['awaiting']:
                symType = "DATA" if "DATA" in sym else "LABEL"
                symbolName = sym[symType]['symbol']
                destination = sym[symType]['output']
                # Searches through the symbol table for the symbol
                for symbol in self.symbolTable:
                    # Checks if it is a valid symbol
                    if symbol["type"] == symType and symbol["name"] == symbolName:
                        if symbol["type"] == "LABEL":
                            ins["operand"][destination] = symbol["pointer"]
                        elif symbol["type"] == "DATA":
                            ins["operand"][destination] = symbol["pointer"]
            ins['operand']['awaiting'] = []
            print(ins)
            # Gets the main operand value
            if ins['operand']:
                if 'operand' in ins['operand']:
                    if ins['operand']['operandType'] == 'int':
                        machineCode[i] |= (1 << 18) # Sets value mode for the operand
                        value = ins['operand']['operand'].to_bytes(4, "big")
                        machineCode[i] |= value[0] << 12
                        machineCode[i] |= value[1] << 8
                        machineCode[i] |= value[2] << 4
                        machineCode[i] |= value[3]
                    elif ins['operand']['operandType'] == 'float':
                        machineCode[i] |= (1 << 18) # Sets value mode for the operand
                        value = struct.pack('>f', ins['operand']['operand'])
                        machineCode[i] |= value[0] << 12
                        machineCode[i] |= value[1] << 8
                        machineCode[i] |= value[2] << 4
                        machineCode[i] |= value[3]
                    elif ins['operand']['operandType'] == 'register':
                        machineCode[i] |= (ins['operand']['operand'] & 0xF) << 4
                    if 'Rin' in ins['operand']: 
                        # Clears the bits at the location
                        machineCode[i] &= 0xFFFFF0FF
                        machineCode[i] |= (ins['operand']['Rin'] & 0xF) << 8
                elif 'address' in ins['operand']:
                    if ins['operand']['addressingMode'] == "Absolute" or ins['operand']['addressingMode'] == "Indirect":
                        addr = ins['operand']['address'].to_bytes(4, "big")
                        machineCode[i] |= addr[0] << 12
                        machineCode[i] |= addr[1] << 8
                        machineCode[i] |= addr[2] << 4
                        machineCode[i] |= addr[3]
                        if ins['operand']['addressingMode'] == "Absolute": machineCode[i] |= 0x0 << 16
                        elif ins['operand']['addressingMode'] == "Indirect": machineCode[i] |= 0x1 << 16

                    if ins['operand']['addressingMode'] == "Register":
                        machineCode[i] |= 0x2 << 16
                        machineCode[i] |= ins['operand']['offset']
                        if 'Rout' in ins['operand']:
                            # Clears the bits at the location
                            machineCode[i] &= 0xFFFFF0FF
                            machineCode[i] |= (ins['operand']['Rin'] & 0xF) << 8
                        else:
                            # Clears the bits at the location
                            machineCode[i] &= 0xFFFF0FFF
                            machineCode[i] |= (ins['operand']['Rin'] & 0xF) << 12
                    elif ins['operand']['addressingMode'] == "RegisterOffset": 
                        machineCode[i] |= 0x3 << 16

                if 'Rout' in ins['operand']:
                    # Clears the bits at the location
                    machineCode[i] &= 0xFFFF0FFF
                    machineCode[i] |= (ins['operand']['Rout'] & 0xF) << 12
            print(machineCode[i])
if __name__=="__main__":
    prog = open("testProg.jis", 'r')
    testProg = prog.read()
    prog.close()

    assembler = Assembler()
    assembler.parseAssembly(testProg)
    assembler.assemble()