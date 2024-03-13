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
        "^\tpushf (\d*\.?\d*)\n$", //32 bit float //TODO: can be negative?
        //pushd value 05
        "^\tpushd (\d*\.?\d*)\n$", //64 bit double 
        //pushbm address 06
        "^\tpushbm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //address will be 24 bit, operation will be 8 bit
        //pushsm address 07
        "^\tpushsm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //16 bit
        //pushim address 08
        "^\tpushim ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //32 
        //pushlm address 09
        "^\tpushlm ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //64
        //pushfm address 0a //TODO fix this regex
        "^\tpushfm (\d*\.?\d*|:([a-zA-Z0-9_-]+))\n$", //32 float
        //pushdm address 0b
        "^\tpushdm (\d*\.?\d*|:([a-zA-Z0-9_-]+))\n$", //64 float
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
        "^\tpopfm (\d*\.?\d*|:([a-zA-Z0-9_-]+))\n$", //32 bit float 
        //popdm address 1e 
        "^\tpopdm (\d*\.?\d*|:([a-zA-Z0-9_-]+))\n$", //64 bit float
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
        "^\tjmp ([0-9]{1,8}|:([a-zA-Z0-9_-]+))\n$", //TODO CHECK WHETHER OPERATIONS ARE LEGAL (im p sure they are)
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
        //labels 143 //TODO ADD IN LABELS W OPERATION 
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
    };

    //pass 1: counting labels
    char str[100];
    int countLabels = 0;
    bool hasCode = false;
    bool codeSection = false;
    bool dataSection = false;
    // int lineNum = 0;
    while(fgets(str, sizeof(str), file) != NULL) {
        // if(lineNum == 0 && strcmp(str, ".code\n") != 0) { //TODO check whether it has to start with .code or just has to have one in there?
        //     fprintf(stderr, "Error on line 0\n");
        //     exit(1);
        // }
        if(str[0] == ':') {
            countLabels++;
        }
        if(strcmp(str, ".code") == 0) {
            hasCode = true;
            codeSection = true;
            dataSection = false;
        }
        if(strcmp(str, ".data") == 0) {
            hasCode = true;
            codeSection = false;
            dataSection = true;
        }
        //lineNum++;
    }
    //make sure at least 1 .code
    if(!hasCode) {
        fprintf(stderr, "Error on line 0");
        exit(1);
    }
    rewind(file);
        
    //pass 2: adding in offsets
    struct label labelsArray[countLabels]; //created a label array

    //part 1: going through and assigning flag to help identify size of instruction (1 byte for opcode + size of operand)

    //compiling regex
    regex_t regEx[41];
    for(int i = 0; i < 41; i++) {
        if (regcomp(&regEx[i], regexInstructions[i], REG_EXTENDED) != 0) {
            fprintf(stderr, "Could not compile regex pattern: %s\n", regexInstructions[i]);
            return 1;
        }
    }

    //code, data types of line
    bool code = false; //all after contain instructions
    bool data = false; //all after contain data
    bool byte = false;
    bool ascii = false; //all after contain type ascii: signed 8 bit
    bool shor = false; //short: signed 16 bit
    bool ints = false; //int: signed 32 bit
    bool lon = false; //long: signed 64 bit 
    bool flo = false; //float: 32 bit float 
    bool dou = false; //double: 64 bit float

    //instruction sizes
    bool oneb = false;
    bool twob = false;
    bool threeb = false;
    bool fourb = false;
    bool eightb = false;
    int offset = 0;
    int lineNum = 1;
    //TODO: check ranges here for the operands
    while(fgets(str, sizeof(str), file) != NULL) {
        //identifying which instruction it is 
        if(strncmp(str, ";", 1) != 0) { //ignoring if its a commment
            int value = -1;
            for (int i = 0; i < 41; i++) {
                if (regexec(&regEx[i], str, 0, NULL, 0) == 0) {
                    value = i;
                    break;
                }
            }
            
            if(value >= 0 && value <= 142) {
                if(!code) {
                    fprintf(stderr, "Error on line %d", lineNum);
                    exit(1);
                }

                //TODO convert opcode to 1 byte binary but should i even do it here
            }

            //assigning boolean flags based on what kind of instruction it is
            switch(value) {
                case 144: 
                    code = true;
                    break;
                case 145: 
                    data = true;
                    code = false;
                    break;
                case 146:
                    byte = true;
                    ascii = false;
                    shor = false;
                    ints = false;
                    lon = false;
                    flo = false;
                    dou = false;
                    break;
                case 147:
                    ascii = true;
                    byte = false;
                    shor = false;
                    ints = false;
                    lon = false;
                    flo = false;
                    dou = false;
                    break;
                case 148:
                    shor = true;
                    byte = false;
                    ascii = false;
                    ints = false;
                    lon = false;
                    flo = false;
                    dou = false;
                    break;
                case 149:
                    ints = true;
                    byte = false;
                    ascii = false;
                    shor = false;
                    lon = false;
                    flo = false;
                    dou = false;
                    break;
                case 150:
                    lon = true;
                    byte = false;
                    ascii = false;
                    shor = false;
                    ints = false;
                    flo = false;
                    dou = false;
                    break;
                case 151:
                    flo = true;
                    byte = false;
                    ascii = false;
                    shor = false;
                    ints = false;
                    lon = false;
                    dou = false;
                    break;
                case 152:
                    dou = true;
                    byte = false;
                    ascii = false;
                    shor = false;
                    ints = false;
                    lon = false;
                    flo = false;
                    break;
                case 0: case 123: 
                    oneb = true;
                    break;
            }
        } 
        lineNum++;
        //okay so now based on the flag, write to the file do i need to separate this to a different pass actually yes the writing part definitely has to be its own part because i need to do all the data first and then the code part so then maybe i assign to an array but is the array dynamically resizing?????? maybe i should do this while i go through the first time to like determine the size of each section
    }
    rewind(file);
}