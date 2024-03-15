#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

int codeOffset = 0; 
int dataOffset = 0;
struct code co;
struct data dat;

struct label {
    int memoryAddress;
    char* labelName;
};

struct code {
    char type[7];
    char label[100];
    //int lineNum;
    uint8_t opcode;
    int8_t byte;
    short shor;
    uint32_t mem;
    int ints;
    long long lon;
    float flo;
    double dou;
    char lab[100];
    uint8_t uByte;
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

void resetData() {
    strcpy(dat.type, "");
    dat.byte = 0;
    dat.shor = 0;
    dat.ints = 0;
    dat.lon = 0;
    dat.flo = 0.0f;
    dat.dou = 0;
}

void resetInstr() {
    strcpy(co.type, "");
    strcpy(co.label, "");
    co.opcode = 0;
    co.byte = 0;
    co.shor = 0;
    co.mem = 0;
    co.ints = 0;
    co.lon = 0;
    co.flo = 0.0f;
    co.dou = 0;
    co.uByte = 0;
    strcpy(co.lab, "");
}

bool checkData(char str[]) {
    if(strcmp(dat.type, "byte") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT8_MIN && foo <= INT8_MAX) {
            int8_t foos = (int8_t)foo;
            dat.byte = foos;
            return true;
            dataOffset += 1;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "ascii") == 0) {
        int8_t foo = str[0];
        dat.byte = foo;
        dataOffset += 1;
        return true;
    }
    else if(strcmp(dat.type, "short") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT16_MIN && foo <= INT16_MAX) {
            short foos = (short)foo;
            dat.shor = foos;
            dataOffset += 2;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "int") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT32_MIN && foo <= INT32_MAX) {
            int foos = (int)foo;
            dat.ints = foos;
            dataOffset += 4;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(dat.type, "long") == 0) {
        int64_t foo = atoll(str);
        if(foo >= INT64_MIN && foo <= INT64_MAX) {
            long long foos = (long long) foo;
            dat.lon = foos;
            dataOffset += 8;
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
            dataOffset += 4;
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
            dataOffset += 8;
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

bool checkCode(char token[]) {
    //printf("token: %s, co.type: %s, offset: %d\n", token, co.type, offset);
    //8
    if(strcmp(co.type, "byte") == 0) {
        int64_t foo = atoll(token);
        if(foo >= INT8_MIN && foo <= INT8_MAX) {
            co.byte = (int8_t) foo;
            // printf("co.byte: %d\n", co.byte);
            codeOffset += 2;
            return true;
        }
        else { 
            //printf("here?\n");
            return false;
        }
    }
    //16
    else if(strcmp(co.type, "short") == 0) {
        int64_t foo = atoll(token);
        if(foo >= INT16_MIN && foo <= INT16_MAX) {
            short foos = (short) foo;
            co.shor = foos;
            codeOffset += 3;
            return true;
        }
        else { 
            return false;
        }
    }
    //24
    else if(strcmp(co.type, "memory") == 0) {
        int64_t foo = atoll(token);
        if(foo >= 0 && foo <= 16777215) {
            uint32_t foos = (uint32_t) foo;
            co.mem = foos;
            codeOffset += 4;
            return true;
        }
        else {
            return false;
        }
    }
    //32
    else if(strcmp(co.type, "int") == 0) {
        int64_t foo = atoll(token);
        if(foo >= INT32_MIN && foo <= INT32_MAX) {
            int foos = (int) foo;
            co.ints = foos;
            codeOffset += 5;
            return true;
        }
        else { 
            return false;
        }
    }
    //64
    else if(strcmp(co.type, "long") == 0) {
        int64_t foo = atoll(token);
        if(foo >= INT64_MIN && foo <= INT64_MAX) {
            long long foos = (long long) foo;
            co.lon = foos;
            codeOffset += 9;
            return true;
        }
        else { 
            return false;
        }
    }
    //32
    else if(strcmp(co.type, "float") == 0) {
        float foo = atof(token);
        if(foo >= -__FLT_MAX__ && foo <= __FLT_MAX__) {
            co.flo = foo;
            codeOffset += 5;
            return true;
        }
        else { 
            return false;
        }
    }
    else if(strcmp(co.type, "double") == 0) {
        double foo = atof(token);
        if(foo >= -__DBL_MAX__ && foo <= __DBL_MAX__) {
            co.dou = foo;
            codeOffset += 9;
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

bool getOperand(struct code co, char str[]) {
    int offs = 0;
    char token[100];
    int index = 0;
    while (sscanf(str + offs, "%s", token) == 1) {
        if(index == 1) {
            return checkCode(token);
        }
        index++;
        offs += strlen(token) + 1; 
    }
}

int main(int argc, char** argv) {
    char fileName[strlen(argv[1]) + 2];
    strcpy(fileName, argv[1]);
    if(fileName[strlen(fileName) - 3] != 's' || fileName[strlen(fileName) - 2] != 'l' || fileName[strlen(fileName) - 1] != 'k') {
        fprintf(stderr, "Invalid slinker filepath\n");
        exit(1);
    }
    strcat(fileName, "o");

    FILE* file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Invalid slinker filepath\n");
        exit(1);
    }

    FILE* output = fopen(fileName, "wb");
    if(output == NULL) {
        fprintf(stderr, "Invalid slinker filepath\n");
        exit(1);
    }

    //8 bit: {0, 255} {-128, 127} [3]
    //16 bit: {0, 65535} {-32768, 32767} [5]
    //24 bit: {0, 16777215} {-8388608, 8388607} [8]
    //32 bit: {0, 4294967295} {-2147483648, 2147483647} [10]
    //64 bit: {0, 18446744073709551615} {-9223372036854775808, 9223372036854775807} [19, 20 if unsigned]
    char *regexInstructions[] = {
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
        "^\tpushbm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //address will be 24 bit, operation will be 8 bit
        //pushsm address 07
        "^\tpushsm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //16 bit
        //pushim address 08
        "^\tpushim ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //32 
        //pushlm address 09
        "^\tpushlm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //64
        //pushfm address 0a
        "^\tpushfm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //32 float
        //pushdm address 0b
        "^\tpushdm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //64 float
        //pushmm address items 0c
        "^\tpushmm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+) ([0-9]{1,3})\n$", //items is 8 bit, operation is 8 bit
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
        "^\tpopbm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //8 bit
        //popsm address 1a
        "^\tpopsm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //16 bit
        //popim address 1b
        "^\tpopim ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //32 bit
        //poplm address 1c
        "^\tpoplm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //64 bit integer
        //popfm address 1d 
        "^\tpopfm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //32 bit float 
        //popdm address 1e 
        "^\tpopdm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$", //64 bit float
        //popmm address items 1f
        "^\tpopmm ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+) ([0-9]{1,3})\n$", //items is 8 bit
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
        "^\tdivs\n$",
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
        "^\tjrpc (-?[0-9]{1,3})\n$", //signed 8 bit
        //jind 85
        "^\tjind\n$",
        //jz address 86
        "^\tjz ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //jnz address 87
        "^\tjnz ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //jgt address 88
        "^\tjgt ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //jlt address 89
        "^\tjlt ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //jge address 8a
        "^\tjge ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //jle address 8b
        "^\tjle ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //call address 8c
        "^\tcall ([0-9]{1,8}|:[a-zA-Z0-9_-]+|:[a-zA-Z0-9_-]+\\+?\\-?[0-9]+)\n$",
        //return 8d
        "^\treturn\n$",
        //halt 8e
        "^\thalt\n$",
        //labels 143
        "^:[a-zA-Z0-9_-]+\n$",
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
        "^\t(-?[0-9]+(\\.[0-9]+)?)\n$",
        //ascii data type 154
        "^\t[a-zA-Z]\n$"
    };

    /*regex_t regEx[155];
    for(int i = 0; i < 155; i++) {
        if (regcomp(&regEx[i], regexInstructions[i], REG_EXTENDED) != 0) {
            fprintf(stderr, "Could not compile regex pattern: %s\n", regexInstructions[i]);
            return 1;
        }
    }

    
    char test[100];
    strcpy(test, "\t32.7\n");
    for (int i = 0; i < 155; i++) {
        if (regexec(&regEx[i], test, 0, NULL, 0) == 0) {
            printf("regex val %d\n", i);
            break;
        }
    } */

    //pass 1: counting labels
    char str[100];
    int countLabels = 0;
    int countCode = 0;
    int countData = 0;
    bool hasCode = false;
    // int codeSection = 0;
    // int dataSection = 0;
    bool codeSection = false;
    bool dataSection = false;
    while(fgets(str, sizeof(str), file) != NULL) {
        if(str[0] == ':') {
            //printf("label\n");
            countLabels++;
        }
        else if(strcmp(str, ".code\n") == 0) {
            //printf(".code\n");
            hasCode = true;
            codeSection = true;
            dataSection = false;
        }
        else if(strcmp(str, ".data\n") == 0) {
            //printf(".data\n");
            hasCode = true;
            codeSection = false;
            dataSection = true;
        }
        else if(strcmp(str, ".byte\n") == 0 || strcmp(str, ".ascii\n") == 0 || strcmp(str, ".short\n") == 0 || strcmp(str, ".int\n") == 0 || strcmp(str, ".long\n") == 0 || strcmp(str, ".float\n") == 0 || strcmp(str, ".double\n") == 0 || str[0] == ';') {
            //printf("data specification or comment\n");
            continue;
        }
        else if(codeSection) {
            //printf("instruction\n");
            countCode++;
        }
        else if(dataSection) {
            //printf("data item str: %s", str);
            countData++;
        }
    }

    //printf("countLabels: %d, countCode: %d, countData: %d, str: %s\n", countLabels, countCode, countData, str);
    //make sure at least 1 .code
    if(!hasCode) {
        remove(fileName);
        fprintf(stderr, "Error on line 0\n");
        exit(1);
    }
    rewind(file);
    
    //printf("countLabels: %d, countCode: %d, countData: %d\n", countLabels, countCode, countData);
    //pass 2: adding in offsets
    struct label labelsArray[countLabels]; //created a label array
    struct code instrArray[countCode]; //array for instr
    struct data dataArray[countData]; //array for data

    //compiling regex
    regex_t regEx[155];
    for(int i = 0; i < 155; i++) {
        if (regcomp(&regEx[i], regexInstructions[i], REG_EXTENDED) != 0) {
            remove(fileName);
            fprintf(stderr, "Could not compile regex pattern: %s\n", regexInstructions[i]);
            exit(1);
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

    //index for the arrays
    int datIndex = 0;
    int codeIndex = 0;

    //int offset = 0;
    int lineNum = 1;
    while(fgets(str, sizeof(str), file) != NULL) {
        //printf("whilecount: %d, str: %s\n", countCode, str);
        uint8_t opcode;
        //identifying which line type it is 
        if(strncmp(str, ";", 1) != 0) { //ignoring if its a commment
            int value = -1;
            //printf("str: %s\n", str);
            for (int i = 0; i < 155; i++) {
                if (regexec(&regEx[i], str, 0, NULL, 0) == 0) {
                    value = i;
                    //printf("regex val %d\n", value);
                    break;
                }
            }
            //printf("str: %s value: %d\n", str, value);
            //making sure instructions are in .code section
            if(value >= 0 && value <= 142) {
                if(!codeSection) {
                    remove(fileName);
                    fprintf(stderr, "Error on line %d\n", lineNum);
                    // fprintf(stderr, "Error on line %d missing code section\n", lineNum);
                    exit(1);
                }

                //converts opcode to 1 byte binary
                opcode = value & 0xFF;
                //printf("value: %d opcode: %d\n", value, opcode);
            }

            char typ[7]; //to hold type of data
            

            int labelArrayIndex = 0;
            switch(value) {
                //directives
                case 144: 
                    codeSection = true;
                    dataSection = false;
                    break;
                case 145: 
                    dataSection = true;
                    codeSection = false;
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
                    if(typ == "" || strcmp(typ, "ascii") == 0) {
                        // fprintf(stderr, "Error on line %d data type doesn't match\n", lineNum);
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        exit(1);
                    }

                    strcpy(dat.type, typ);
                    //confirming that it is formatted correctly
                    if(!checkData(str)) {
                        // fprintf(stderr, "Error on line %d data formatted incorrectly\n", lineNum);
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        exit(1);
                    }
                    dataArray[datIndex] = dat; //assign to array (with initialized data type)
                    datIndex++;
                    //resetData(dat); //reset struct
                    break;
                //handling ascii
                case 154: 
                    //need to make sure it is ascii
                    if(typ == "" || strcmp(typ, "ascii") != 0) {
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        // fprintf(stderr, "Error on line %d data type doesn't match (ascii)\n", lineNum);
                        exit(1);
                    }

                    strcpy(dat.type, typ);
                    //confirming formatting
                    if(!checkData(str)) {
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        // fprintf(stderr, "Error on line %d data formatted incorrectly (ascii)\n", lineNum);1
                        exit(1);
                    }
                    dataArray[datIndex] = dat; //assign to array
                    datIndex++;
                    //resetData(dat); //reset struct
                    break;


                //instructions of various sizes
                //8 bit
                case 0: case 123: case 124: case 125: case 126: case 127: case 128: case 129: case 130: case 132: 
                    //TODO jrpcs should be signed so check whether it needs to move
                    strcpy(co.type, "byte");
                    if(!getOperand(co, str)) {
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        // fprintf(stderr, "Error on line %d operand wrong 8 bit\n", lineNum);
                        exit(1);
                    }
                    co.opcode = opcode; 
                    instrArray[codeIndex] = co;
                    //resetInstr();
                    codeIndex++;
                    //codeOffset += 2; //1 byte + 1 byte
                    break;
                //16 bit 
                case 1: 
                    strcpy(co.type, "short");
                    if(!getOperand(co, str)) {
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        // fprintf(stderr, "Error on line %d operand wrong 16 bit\n", lineNum);
                        exit(1);
                    }
                    co.opcode = opcode;
                    instrArray[codeIndex] = co;
                    //resetInstr();
                    codeIndex++;
                    //codeOffset += 3; //1 + 2 bytes
                    break;
                //case 24 bit
                case 6: case 7: case 8: case 9: case 10: case 11: case 25: case 26: case 27: case 28: case 29: case 30: case 131: case 134: case 135: case 136: case 137: case 138: case 139: case 140:
                    strcpy(co.type, "memory");
                    int off = 0;
                    char tok[100];
                    int ind = 0;
                    while (sscanf(str + off, "%s", tok) == 1) {
                        if(ind == 1) {
                            if(tok[0] == ':') {
                                co.mem = -1;
                                strcpy(co.lab, &tok[1]);
                                //co.lineNum = lineNum;
                            }
                        }
                        ind++;
                        off += strlen(tok) + 1; 
                    }
                    //printf("co.mem: %d\n", co.mem);
                    if(co.mem != -1) {
                        if(!getOperand(co, str)) {
                            // fprintf(stderr, "Error on line %d operand wrong 24 bit\n", lineNum);
                            remove(fileName);
                            fprintf(stderr, "Error on line %d\n", lineNum);
                            exit(1);
                        }
                    }
                    else {
                        codeOffset += 4;
                    }
                    co.opcode = opcode;
                    instrArray[codeIndex] = co;
                    //resetInstr();
                    codeIndex++;
                    break;
                    //codeOffset += 4; //1 + 3 
                //32 bit
                case 2: case 4: 
                    strcpy(co.type, "int");
                    if(!getOperand(co, str)) {
                        // fprintf(stderr, "Error on line %d operand wrong 32 bit\n", lineNum);
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        exit(1);
                    }
                    co.opcode = opcode;
                    instrArray[codeIndex] = co;
                    //resetInstr();
                    codeIndex++;
                    //codeOffset += 5; //1 + 4
                    break; 
                //64 bit
                case 3: case 5: 
                    strcpy(co.type, "double");
                    if(!getOperand(co, str)) {
                        // fprintf(stderr, "Error on line %d operand wrong 64 bit\n", lineNum);
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        exit(1);
                    }
                    co.opcode = opcode;
                    instrArray[codeIndex] = co;
                    //resetInstr();
                    codeIndex++;
                    //codeOffset += 9; //1 + 8
                    break;
                //24 bit + 8 bit items
                case 12: case 31:
                    strcpy(co.type, "specMem");
                    int offset = 0;
                    char to[100];
                    int index = 0;
                    while (sscanf(str + offset, "%s", to) == 1) {
                        if(index == 1) {
                            int64_t foo = atoll(to);
                            if(foo >= 0 && foo <= 16777215) {
                                uint32_t foos = (uint32_t) foo;
                                co.mem = foos;  
                            }
                            else {
                                // fprintf(stderr, "Error on line %d special memory p1\n", lineNum);
                                remove(fileName);
                                fprintf(stderr, "Error on line %d\n", lineNum);
                                exit(1);
                            }
                        }
                        if(index == 2) {
                            int64_t foo = atoll(to);
                            if(foo >= 0 && foo <= UINT8_MAX) {
                                co.uByte = (uint8_t) foo;
                            }
                            else {
                                // fprintf(stderr, "Error on line %d specmem p2\n", lineNum);
                                remove(fileName);
                                fprintf(stderr, "Error on line %d\n", lineNum);
                                exit(1);
                            }
                        }
                        index++;
                        offset += strlen(to) + 1; 
                    }
                    co.opcode = opcode;
                    codeOffset += 5; //1 + 3 + 1
                    instrArray[codeIndex] = co;
                    //resetInstr();
                    codeIndex++;
                    break;
                //no operands
                case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55:  case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88: case 89: case 90: case 91: case 92:  case 93: case 94: case 95: case 96: case 97: case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109: case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119: case 120: case 121: case 122: case 133: case 141: case 142:
                     
                    co.opcode = opcode;
                    codeOffset += 1; //1 byte + 0 operands
                    instrArray[codeIndex] = co;
                    //printf("co.opcode: %d, codeIndex: %d, instrArray[%d].opcode: %d\n", opcode, codeIndex, codeIndex, instrArray[codeIndex].opcode);
                    //resetInstr();
                    codeIndex++;
                    break;
                //label
                case 143: {
                    struct label tempLabel;
                    tempLabel.labelName = strdup(str + 1);
                    int len = strlen(tempLabel.labelName);
                    tempLabel.labelName[len-1] = '\0';
                    
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
                        // fprintf(stderr, "Error on line %d label\n", lineNum);
                        remove(fileName);
                        fprintf(stderr, "Error on line %d\n", lineNum);
                        exit(1);
                    }
                    //printf("countCode b: %d codeIndex: %d ", countCode, codeIndex);
                    //instrArray[codeIndex] = co;
                    //printf("countCode e: %d\n", countCode);
                    //resetInstr();
                    codeIndex++;
                    break;
                }
                default: 
                    // fprintf(stderr, "Error on line %d %s switch case\n", lineNum, str);
                    remove(fileName);
                    fprintf(stderr, "Error on line %d\n", lineNum);
                    exit(1);
                    break;
            }
        } 
        //printf("codeOffset: %d, lineNum: %d\n", codeOffset, lineNum);
        lineNum++;
        resetInstr();
        resetData();
    }
    rewind(file);

    //printf("countCode: %d\n", countCode);

    //run through file again, this time replacing labels with memory addresses, so i could just increment counter for code lines and access the element of the array when i get to it, replace label, update value in struct, and array will be updated
    for(int i = 0; i < countCode; i++) {
        struct code co = instrArray[i];
        //printf("instruction %d: ", i);
        if(co.mem == -1) {
            //printf("has a label, ");
            int indexOf = -1;
            for(int k = 0; k < strlen(co.lab); k++) {
                if(co.lab[k] == '+' || co.lab[k] == '-') {
                    indexOf = k;
                    //printf("indexOf: %d, ", indexOf);
                    break;
                }
            }
            int ari = 0;
            if(indexOf != -1) {
                //printf("has arithmetic, ");
                char arith[100];
                strcpy(arith, &co.lab[indexOf]);
                ari = atoi(arith);
            }
            co.lab[indexOf] = '\0';
            bool exists = false;
            for(int j = 0; j < countLabels; j++) {
                //printf("label: %s ", labelsArray[j].labelName);
                //printf(" co.label %s\n", co.lab);
                if(strcmp(labelsArray[j].labelName, co.lab) == 0) {
                    //printf("found a label match\n");
                    co.mem = labelsArray[j].memoryAddress + ari;
                    exists = true;
                    break;
                } 
            }
            if(!exists) {
                // fprintf(stderr, "Error on line %d label doesn't exist\n", lineNum);
                remove(fileName);
                fprintf(stderr, "Error on line %d\n", lineNum);
                exit(1);
            }
            instrArray[i] = co;
        }
        //printf("co.mem: %d\n", co.mem);
    } 
    //iterate through the .code array and the .data array to write to the slko file
    //.code offset
    uint32_t offse = 0;
    offse |= (8 & 0xFF) << 24;
    offse |= ((8 >> 8) & 0xFF) << 16;
    offse |= ((8 >> 16) & 0xFF) << 8;
    offse |= (8 >> 24) & 0xFF;
    //printf("offset: %d\n", offse);
    fwrite((const void *)&offse, sizeof(offse), 1, output);

    //.data offset
    int dOff = 8 + codeOffset;
    uint8_t num = (dOff >> 24) & 0xFF;
    fwrite((const void *)&num, sizeof(num), 1, output);
    num = (dOff >> 16) & 0xFF;
    fwrite((const void *)&num, sizeof(num), 1, output);
    num = (dOff >> 8) & 0xFF;
    fwrite((const void *)&num, sizeof(num), 1, output);
    num = dOff & 0xFF;
    fwrite((const void *)&num, sizeof(num), 1, output);

    //writing instructions
    uint8_t op = 0; 
    for(int i = 0; i < countCode; i++) {
        op = 0;
        op = instrArray[i].opcode;
        //printf("instr: %s op: %d\n", instrArray[i].type, op);
        fwrite((const void *)&op, sizeof(op), 1, output);
        if(strcmp(instrArray[i].type, "byte") == 0) {
            int8_t toWrite = instrArray[i].byte;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "short") == 0) {
            short toWrit = instrArray[i].shor;
            short toWrite = 0;
            toWrite |= (toWrit >> 8) & 0xFF; 
            toWrite |= (toWrit << 8) & 0xFF00; 
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "memory") == 0) {
            uint32_t toWrit = instrArray[i].mem;
            //printf("instruction %d co.mem in writing: %d\n", i, instrArray[i].mem);
            uint8_t toWrite = 0;
            toWrite = (toWrit >> 16) & 0xFF;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
            toWrite = (toWrit >> 8) & 0xFF; 
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
            toWrite = toWrit & 0xFF; 
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
            // toWrite |= (toWrit << 8) & 0xFF0000; 
            // toWrite |= (toWrit << 24) & 0xFF000000;
            // fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "int") == 0) {
            int toWrit = instrArray[i].ints;
            int toWrite = 0;
            toWrite |= (toWrit >> 24) & 0xFF;
            toWrite |= (toWrit >> 8) & 0xFF00; 
            toWrite |= (toWrit << 8) & 0xFF0000; 
            toWrite |= (toWrit << 24) & 0xFF000000;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "long") == 0) {
            long long toWrit = instrArray[i].lon;
            long long toWrite = 0;
            toWrite |= (toWrit >> 56) & 0xFFLL;
            toWrite |= (toWrit >> 40) & 0xFF00LL; 
            toWrite |= (toWrit >> 24) & 0xFF0000LL;
            toWrite |= (toWrit >> 8) & 0xFF000000LL; 
            toWrite |= (toWrit << 8) & 0xFF00000000LL;
            toWrite |= (toWrit << 24) & 0xFF0000000000LL;            
            toWrite |= (toWrit << 40) & 0xFF000000000000LL; 
            toWrite |= (toWrit << 56) & 0xFF00000000000000LL;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "float") == 0) {
            int toWrit = 0;
            memcpy(&toWrit, &instrArray[i].flo, sizeof(float));
            int toWrite = 0;
            toWrite |= (toWrit >> 24) & 0xFF;
            toWrite |= (toWrit >> 8) & 0xFF00; 
            toWrite |= (toWrit << 8) & 0xFF0000; 
            toWrite |= (toWrit << 24) & 0xFF000000;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "double") == 0) {
            int64_t toWrit = (uint64_t)instrArray[i].dou;
            int64_t toWrite = 0;
            toWrite |= (toWrit & 0xFF) << 56;
            toWrite |= ((toWrit >> 8) & 0xFF) << 48;
            toWrite |= ((toWrit >> 16) & 0xFF) << 40;
            toWrite |= ((toWrit >> 24) & 0xFF) << 32;
            toWrite |= ((toWrit >> 32) & 0xFF) << 24;
            toWrite |= ((toWrit >> 40) & 0xFF) << 16;
            toWrite |= ((toWrit >> 48) & 0xFF) << 8;
            toWrite |= (toWrit >> 56) & 0xFF;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(instrArray[i].type, "specMem") == 0) {
            uint32_t toWrit = instrArray[i].mem;
            uint8_t toWrite = 0;
            toWrite = (toWrit >> 16) & 0xFF;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
            toWrite = (toWrit >> 8) & 0xFF; 
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
            toWrite = toWrit & 0xFF; 
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);

            uint8_t toWrit2 = instrArray[i].uByte;
            fwrite((const void *)&toWrit2, sizeof(toWrit2), 1, output);
        }
        else {
            continue;
        }
    }

    //writing data
    for(int i = 0; i < countData; i++) {
        if(strcmp(dataArray[i].type, "byte") == 0) {
            int8_t toWrite = dataArray[i].byte;
            printf("byte %d\n", toWrite);
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(dataArray[i].type, "ascii") == 0) {
            int8_t toWrite = dataArray[i].byte;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(dataArray[i].type, "short") == 0) {
            short toWrit = dataArray[i].shor;
            printf("short %hd\n", toWrit);
            short toWrite = 0;
            toWrite |= (toWrit >> 8) & 0xFF; 
            toWrite |= (toWrit << 8) & 0xFF00; 
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(dataArray[i].type, "int") == 0) {
            int toWrit = dataArray[i].ints;
            printf("int %d\n", toWrit);
            int toWrite = 0;
            toWrite |= (toWrit >> 24) & 0xFF;
            toWrite |= (toWrit >> 8) & 0xFF00; 
            toWrite |= (toWrit << 8) & 0xFF0000; 
            toWrite |= (toWrit << 24) & 0xFF000000;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(dataArray[i].type, "long") == 0) {
            long long toWrit = dataArray[i].lon;
            printf("long %lld\n", toWrit);
            long long toWrite = 0;
            toWrite |= (toWrit >> 56) & 0xFFLL;
            toWrite |= (toWrit >> 40) & 0xFF00LL; 
            toWrite |= (toWrit >> 24) & 0xFF0000LL;
            toWrite |= (toWrit >> 8) & 0xFF000000LL; 
            toWrite |= (toWrit << 8) & 0xFF00000000LL;
            toWrite |= (toWrit << 24) & 0xFF0000000000LL;            
            toWrite |= (toWrit << 40) & 0xFF000000000000LL; 
            toWrite |= (toWrit << 56) & 0xFF00000000000000LL;
            fwrite((const void *)&toWrite, sizeof(toWrite), 1, output);
        }
        else if(strcmp(dataArray[i].type, "float") == 0) {
            struct data da = dataArray[i];
            int8_t *bytes = (int8_t *) &da.flo;
            for (int i = sizeof(float) - 1; i >= 0; i--) {
                fputc(bytes[i], output);
            }
        }
        else if(strcmp(dataArray[i].type, "double") == 0) {
            struct data da = dataArray[i];
            int8_t *bytes = (int8_t *) &da.dou;
            for (int i = sizeof(double) - 1; i >= 0; i--) {
                fputc(bytes[i], output);
            }
        }
    } 
}