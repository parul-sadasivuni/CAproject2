#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct label {
    int memoryAddress;
    char* labelName;
};

struct code {
    char type[7];
    int8_t byte;
    short shor;
    int ints;
    long long lon;
    float flo;
    double dou;
};

struct data {
    char type[7];
    int8_t byte;
    short shor;
    int ints;
    long long lon;
    float flo;
    double dou;
};

void resetData(struct data dat) {
    dat.type = NULL;
    dat.byte = NULL;
    dat.shor = NULL;
    dat.ints = NULL;
    dat.lon = NULL;
    dat.flo = NULL;
    dat.dou = NULL;
}

void resetInstr(struct code co) {
    co.type = NULL;
    co.byte = NULL;
    co.shor = NULL;
    co.ints = NULL;
    co.lon = NULL;
    co.flo = NULL;
    co.dou = NULL;
}

bool checkData(struct data dat, char str[], int offset) {
    if(strcmp(dat.type, "byte") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT8_MIN && foo <= INT8_MAX) {
            int8_t foos = int8_t(foo);
            dat.byte = foos;
            return true;
            offset += 1;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "ascii") == 0) {
        int8_t foo = str[0];
        offset += 1;
        return true;
    }
    else if(strcmp(dat.type, "short") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT16_MIN && foo <= INT16_MAX) {
            short foos = short(foo);
            dat.shor = foos;
            offset += 2;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "int") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT32_MIN && foo <= INT32_MAX) {
            int foos = int(foo);
            dat.ints = foos;
            offset += 4;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "long") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT64_MIN && foo <= INT64_MAX) {
            short foos = short(foo);
            dat.shor = foos;
            offset += 8;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "float") == 0) {
        float foo = atof(str);
        if(foo >= -__FLT_MAX__ && foo <= __FLT_MAX__) {
            dat.flo = foo;
            offset += 4;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "double") == 0) {
        double foo = atof(str);
        if(foo >= -__DBL_MAX__ && foo <= __DBL_MAX__) {
            dat.dou = foo;
            offset += 8;
            return true;
        }
        else { 
            return false;
        }
    }
    else {
        return false;
    }
}

int duplicateExists(struct label labelArray[], char* str, int size) {
    for(int i = 0; i < size; i++) {
        if (strcmp(labelArray[i].labelName, str) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    //addresses are 24 bit (unsigned, BIG ENDIAN) or operations with labels
    //first 4 bytes of slko are offset of .code, second 4 bytes are offset of .data section (8+P)
    //next P bytes are instructions, Q bytes after that are the .data sections (which should always have a type)
    //passes: count labels, memory address and duplicate checking, validate with regex, 

    char fileName[strlen(argv[1]) + 3];
    strcpy(fileName, argv[1]);
    if(fileName[strlen(fileName) - 3] != 's' || fileName[strlen(fileName) - 2] != 'l' || fileName[strlen(fileName) - 1] != 'k') {
        fprintf(stderr, "Invalid slinker filepath 1\n");
        exit(1);
    }
    strcat(fileName, "o");

    FILE* file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Invalid slinker filepath 2\n");
        exit(1);
    }

    FILE* output = fopen(fileName, "wb");
    if(output == NULL) {
        fprintf(stderr, "Invalid slinker filepath 3\n");
        exit(1);
    }

    //8 bit: {0, 255} {-128, 127} [3]
    //16 bit: {0, 65535} {-32768, 32767} [5]
    //24 bit: {0, 16777215} {-8388608, 8388607} [8]
    //32 bit: {0, 4294967295} {-2147483648, 2147483647} [10]
    //64 bit: {0, 18446744073709551615} {-9223372036854775808, 9223372036854775807} [19, 20 if unsigned]
    char *regexInstructions[] = {
        //TODO: ADD IN LABELS WITH ARITHMETIC
        //pushb value 00
        "^\tpushb (-?[0-9]{1,3})\n$", //range should be 8 bit integer
        //pushs value 01
        "^\tpushs (-?[0-9]{1,5})\n$", //range should be a 16 bit integer
        //pushi value 02
        "^\tpushi (-?[0-9]{1,10})\n$", //32 bit integer
        //pushl value 03
        "^\tpushl (-?[0-9]{1,19})\n$", //64 bit integer
        //pushf value 04
        "^\tpushf (\\d*\\.?\\d*)\n$", //32 bit float 
        //pushd value 05
        "^\tpushd (\\d*\\.?\\d*)\n$", //64 bit double 
        //pushbm address 06
        "^\tpushbm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //address will be 24 bit, operation will be 8 bit
        //pushsm address 07
        "^\tpushsm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //16 bit
        //pushim address 08
        "^\tpushim ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //32 
        //pushlm address 09
        "^\tpushlm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //64
        //pushfm address 0a //TODO verify if this actually works buddy
        "^\tpushfm (\\d*\\.?\\d*|:([a-zA-Z0-9_-]+))\n$", //32 float
        //pushdm address 0b
        "^\tpushdm (\\d*\\.?\\d*|:([a-zA-Z0-9_-]+))\n$", //64 float
        //pushmm address items 0c
        "^\tpushmm ([0-9]{1,8}|:([a-zA-Z0-9_-]+)) ([0,9]{1,3})\n$", //items is 8 bit, operation is 8 bit
        //dupb 0e
        "^\tdupb\n$",
        //dups 0e
        "^\tdups\n$",
        //dupi 0f
        "^\tdupi\n$",
        //dupl 10
        "^\tdupl\n$",
        //dupf 11
        "^\tdupf\n$",
        //dupd 12
        "^\tdupd\n$",
        //popb 13
        "^\tpopb\n$",
        //pops 14
        "^\tpops\n$",
        //popi 15
        "^\tpopi\n$",
        //popl 16
        "^\tpopl\n$",
        //popf 17
        "^\tpopf\n$",
        //popd 18
        "^\tpopd\n$",
        //popbm address 19
        "^\tpopbm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //8 bit
        //popsm address 1a
        "^\tpopsm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //16 bit
        //popim address 1b
        "^\tpopim ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //32 bit
        //poplm address 1c
        "^\tpoplm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //64 bit integer
        //popfm address 1d 
        "^\tpopfm (\\d*\\.?\\d*|:([a-zA-Z0-9_-]+))\n$", //32 bit float 
        //popdm address 1e 
        "^\tpopdm (\\d*\\.?\\d*|:([a-zA-Z0-9_-]+))\n$", //64 bit float
        //popmm address items 1f
        "^\tpopmm ([0-9]{1,8}|:([a-zA-Z0-9_-]+) ([0,9]{1,3}))\n$", //items is 8 bit
        //swapb 20  
        "^\tswapb\n$",
        //swaps 21
        "^\tswaps\n$",
        //swapi 22
        "^\tswapi\n$",
        //swapl 23
        "^\tswapl\n$",
        //swapf 24
        "^\tswapf\n$",
        //swapd 25
        "^\tswapd\n$",
        //convbs 26
        "^\tconvbs\n$",
        //convbi 27
        "^\tconvbi\n$",
        //convbl 28
        "^\tconvbl\n$",
        //convbf 29
        "^\tconvbf\n$",
        //convbd 2a
        "^\tconvbd\n$",
        //convsb 2b
        "^\tconvsb\n$",
        //convsi 2c
        "^\tconvsi\n$",
        //convsl 2d
        "^\tconvsl\n$",
        //convsf 2e
        "^\tconvsf\n$",
        //convsd 2f
        "^\tconvsd\n$",
        //convib 30
        "^\tconvib\n$",
        //convis 31
        "^\tconvis\n$",
        //convil 32
        "^\tconvil\n$",
        //convif 33
        "^\tconvif\n$",
        //convid 34
        "^\tconvid\n$",
        //convlb 35
        "^\tconvlb\n$",
        //convls 36
        "^\tconvls\n$",
        //convli 37
        "^\tconvli\n$",
        //ADD A RANDOM 38
        "random",
        //convlf 39
        "^\tconvlf\n$",
        //convld 3a
        "^\tconvld\n$",
        //convfb 3b
        "^\tconvfb\n$",
        //convfs 3c
        "^\tconvfs\n$",
        //convfi 3d
        "^\tconvfi\n$",
        //convfl 3e
        "^\tconvfl\n$",
        //convfd 3f
        "^\tconvfd\n$",
        //convdb 40
        "^\tconvdb\n$",
        //convds 41
        "^\tconvds\n$",
        //convdi 42
        "^\tconvdi\n$",
        //convdl 43
        "^\tconvdl\n$",
        //convdf 44
        "^\tconvdf\n$",
        //inch 45
        "^\tinch\n$",
        //inb 46
        "^\tinb\n$",
        //ins 47
        "^\tins\n$",
        //ini 48
        "^\tini\n$",
        //inl 49
        "^\tinl\n$",
        //inf 4a
        "^\tinf\n$",
        //ind 4b
        "^\tind\n$",
        //outch 4c
        "^\toutch\n$",
        //outb 4d
        "^\toutb\n$",
        //outs 4e
        "^\touts\n$",
        //outi 4f
        "^\touti\n$",
        //outl 50
        "^\toutl\n$",
        //outf 51
        "^\toutf\n$",
        //outd 52
        "^\toutd\n$",
        //addb 53
        "^\taddb\n$",
        //adds 54
        "^\tadds\n$",
        //addi 55
        "^\taddi\n$",
        //addl 56
        "^\taddl\n$",
        //addf 57
        "^\taddf\n$",
        //addd 58
        "^\taddd\n$",
        //subb 59
        "^\tsubb\n$",
        //subs 5a
        "^\tsubs\n$",
        //subi 5b 
        "^\tsubi\n$",
        //subl 5c
        "^\tsubl\n$",
        //subf 5d
        "^\tsubf\n$",
        //subd 5e
        "^\tsubd\n$",
        //mulb 5f
        "^\tmulb\n$",
        //muls 60
        "^\tmuls\n$",
        //muli 61
        "^\tmuli\n$",
        //mull 62
        "^\tmull\n$",
        //mulf 63
        "^\tmulf\n$",
        //muld 64
        "^\tmuld\n$",
        //divb 65
        "^\tdivb\n$",
        //divs 66
        "^\tdivb\n$",
        //divi 67
        "^\tdivi\n$",
        //divl 68
        "^\tdivl\n$",
        //divf 69
        "^\tdivf\n$",
        //divd 6a
        "^\tdivd\n$",
        //and8 6b
        "^\tand8\n$",
        //and16 6c
        "^\tand16\n$",
        //and32 6d
        "^\tand32\n$",
        //and64 6e
        "^\tand64\n$",
        //or8 6f
        "^\tor8\n$",
        //or16 70
        "^\tor16\n$",
        //or32 71
        "^\tor32\n$",
        //or64 72
        "^\tor64\n$",
        //xor8 73
        "^\txor8\n$",
        //xor16 74
        "^\txor16\n$",
        //xor32 75
        "^\txor32\n$",
        //xor64 76
        "^\txor64\n$",
        //not8 77
        "^\tnot8\n$",
        //not16 78
        "^\tnot16\n$",
        //not32 79
        "^\tnot32\n$",
        //not64 7a
        "^\tnot64\n$",
        //shftrb value 7b
        "^\tshftrb (-?[0-9]{1,3})\n$", //8 bit
        //shftrs value 7c
        "^\tshftrs (-?[0-9]{1,3})\n$", //16 bit 
        //shftri value 7d
        "^\tshftri (-?[0-9]{1,3})\n$", //32 bit 
        //shftrl value 7e
        "^\tshftrl (-?[0-9]{1,3})\n$", //64 bit 
        //shftlb value 7f
        "^\tshftlb (-?[0-9]{1,3})\n$", //8 bit
        //shftls value 80
        "^\tshftls (-?[0-9]{1,3})\n$", //16 bit
        //shftli value 81
        "^\tshftli (-?[0-9]{1,3})\n$", //32 bit
        //shftll value 82
        "^\tshftll (-?[0-9]{1,3})\n$", //64 bit
        //jmp address 83
        "^\tjmp ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", 
        //jrpc offset 84
        "^\tjrpc (-?[0-9]{1,3})\n$", //signed 8 bit //TODO MAKE SURE CANT HAVE LABEL HERE
        //jind 85
        "^\tjind\n$",
        //jz address 86
        "^\tjz ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //jnz address 87
        "^\tjz ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //jgt address 88
        "^\tjgt ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //jlt address 89
        "^\tjlt ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //jge address 8a
        "^\tjge ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //jle address 8b
        "^\tjle ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //call address 8c
        "^\tcall ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$",
        //return 8d
        "^\treturn\n$",
        //halt 8e
        "^\thalt\n$",
        //labels 143 //TODO ADD IN LABELS W OPERATION FOR INSTRUCTIONS 
        "^:([a-zA-Z0-9_-]+\n$)",
        //.code 144
        "^.code\n$",
        //.data 145
        "^.data\n$",
        //.byte 146
        "^.byte\n$",
        //.ascii 147
        "^.ascii\n$",
        //.short 148
        "^.short\n$",
        //.int 149
        "^.int\n$",
        //.long 150
        "^.long\n$",
        //.float 151
        "^.float\n$",
        //.double 152
        "^.double\n$",
        //any other number data type 153
        "\t[0-9]+\\.?[0-9]\n$",
        //ascii data type 154
        "^\t[a-zA-Z]\n$"
    };

    //pass 1: counting labels
    char str[100];
    int countLabels = 0;
    int countCode = 0;
    int countData = 0;
    bool hasCode = false;
    bool codeSection = false;
    bool dataSection = false;
    while(fgets(str, sizeof(str), file) != NULL) {
        if(str[0] == ':') {
            countLabels++;
        }
        if(strcmp(str, ".code") == 0) {
            hasCode = true;
            codeSection = true;
            dataSection = false;
            typ = NULL;
        }
        else if(strcmp(str, ".data") == 0) {
            hasCode = true;
            codeSection = false;
            dataSection = true;
        }
        else if(strcmp(str, ".byte") == 0 || strcmp(str, ".ascii") == 0 || strcmp(str, ".short") == 0 || strcmp(str, ".int") == 0 || strcmp(str, ".long") == 0 || strcmp(str, ".float") == 0 || strcmp(str, ".double") == 0 || str[0] == ';') {
            continue;
        }
        if(codeSection) {
            countCode++;
        }
        else if(dataSection) {
            countData++;
        }
    }
    //make sure at least 1 .code
    if(!hasCode) {
        fprintf(stderr, "Error on line 0");
        exit(1);
    }
    rewind(file);
        
    //pass 2: adding in offsets
    struct label labelsArray[countLabels]; //created a label array
    struct code instrArray[countCode]; //array for instr
    struct data dataArray[countData]; //array for data

    //compiling regex
    regex_t regEx[155];
    for(int i = 0; i < 155; i++) {
        if (regcomp(&regEx[i], regexInstructions[i], REG_EXTENDED) != 0) {
            fprintf(stderr, "Could not compile regex pattern: %s\n", regexInstructions[i]);
            return 1;
        }
    }

    //code, data types of line
    codeSection = false; //all after contain instructions
    dataSection = false; //all after contain data
    //byte: signed 8 bit
    //ascii: signed 8 bit
    //short: signed 16 bit
    //int: signed 32 bit
    //long: signed 64 bit 
    //float: 32 bit float 
    //double: 64 bit float

    int offset = 0;
    int lineNum = 1;
    while(fgets(str, sizeof(str), file) != NULL) {
        //identifying which line type it is 
        if(strncmp(str, ";", 1) != 0) { //ignoring if its a commment
            int value = -1;
            for (int i = 0; i < 41; i++) {
                if (regexec(&regEx[i], str, 0, NULL, 0) == 0) {
                    value = i;
                    break;
                }
            }
            
            //making sure instructions are in .code section
            if(value >= 0 && value <= 142) {
                if(!codeSection) {
                    fprintf(stderr, "Error on line %d", lineNum);
                    exit(1);
                }

                //TODO convert opcode to 1 byte binary but should i even do it here
            }

            //creating a struct based on instruction type
            struct code co;
            struct data dat;
            char typ[7]; //to hold type of data
            //separate offsets
            int codeOffset = 0; 
            int dataOffset = 0;
            //index for the arrays
            int datIndex = 0;
            int codeIndex = 0;

            int labelArrayIndex = 0;
            switch(value) {
                //directives
                case 144: 
                    code = true;
                    data = false;
                    break;
                case 145: 
                    data = true;
                    code = false;
                    break;
                //subdirectives (sets type for data to follow)
                case 146:
                    strcpy(typ, "byte");
                    break;
                case 147:
                    strcpy(typ, "ascii");
                    break;
                case 148:
                    strcpy(typ, "short");
                    break;
                case 149:
                    strcpy(typ, "int");
                    break;
                case 150:
                    strcpy(typ, "long");
                    break;
                case 151:
                    strcpy(typ, "float");
                    break;
                case 152:
                    strcpy(typ, "double");
                    break;
                //handling number data
                case 153:
                    //need to make sure its not ascii
                    if(typ == NULL || strcmp(typ, "ascii") == 0) {
                        fprintf(stderr, "Error on line %d", lineNum);
                        exit(1);
                    }

                    dat.type = typ;
                    //confirming that it is formatted correctly
                    if(!checkData(dat, str, dataOffset)) {
                        fprintf(stderr, "Error on line %d", lineNum);
                        exit(1);
                    }
                    dataArray[datIndex] = dat; //assign to array (with initialized data type)
                    datIndex++;
                    resetData(dat); //reset struct
                    break;
                //handling ascii
                case 154: 
                    //need to make sure it is ascii
                    if(typ == NULL || strcmp(typ, "ascii") != 0) {
                        fprintf(stderr, "Error on line %d", lineNum);
                        exit(1);
                    }

                    dat.type = typ;
                    //confirming formatting
                    if(!checkData(dat, str, dataOffset)) {
                        fprintf(stderr, "Error on line %d", lineNum);
                        exit(1);
                    }
                    dataArray[datIndex] = dat; //assign to array
                    datIndex++;
                    resetData(dat); //reset struct
                    break;
                //instructions of various sizes
                //8 bit
                case 0: case 123: case 124: case 125: case 126: case 127: case 128: case 129: case 130: case 132: 
                    codeOffset += 2; //1 byte + 1 byte
                    break;
                //16 bit 
                case 1: 
                    codeOffset += 3; //1 + 2 bytes
                    break;
                //case 24 bit
                case 6: case 7: case 8: case 9: case 10: case 11: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 131: case 134: case 135: case 136: case 137: case 138: case 139: case 140:
                    codeOffset += 4; //1 + 3 
                //32 bit
                case 2: case 4: 
                    codeOffset += 5; //1 + 4
                    break; 
                //64 bit
                case 3: case 5: 
                    codeOffset += 9; //1 + 8
                    break;
                //24 bit + 8 bit items
                case 12: case 31:
                    codeOffset += 5; //1 + 3 + 1
                    break;
                //no operands
                case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55:  case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90: case 91: case 92:  case 93: case 94: case 95: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120: case 121: case 122: case 133: case 141: case 142: 
                    codeOffset += 1; //1 byte + 0 operands
                    break;
                //label
                case 143: 
                    struct label tempLabel;
                    tempLabel.labelName = strdup(str + 1);
                    int dupe = duplicateExists(labelsArray, tempLabel.labelName, countLabels);
                    if(dupe == 0) {
                        if(codeSection) {
                            tempLabel.memoryAddress = codeOffset;    
                        }
                        else if(dataSection) {
                            tempLabel.memoryAddress = 0x10000 + dataOffset;
                        }
                        labelsArray[labelArrayIndex] = tempLabel;
                        labelArrayIndex++;
                    }
                    else {
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        exit(1);
                    }
                    break;
                default: 
                    fprintf(stderr, "Error on line %d\n", lineNum);
                    exit(1);
            }
        } 
        lineNum++;
    }
    rewind(file);
}