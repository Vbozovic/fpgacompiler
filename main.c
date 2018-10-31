#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "stretchy_buffer.h"


typedef enum TokenKind {
    INT_TOKEN = 128,
    NAME_TOKEN

} TokenKind;


typedef struct Token {
    TokenKind kind;
    union {
        uint64_t intval; //ukoliko je token int literal cuvamo vrednost samog literala
        struct {
            const char *start;
            const char *end;
        };
    };
} Token;


const char *stream;

Token currToken;

void next_token() {

    switch (*stream) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            uint64_t val = 0;
            while (isdigit(*stream)) {
                val *= 10;
                val += *stream - '0';
                stream++;
            }
            currToken.kind = INT_TOKEN;
            currToken.intval = val;
            break;
        }

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_': {

            char *begining = stream;
            while (isalnum(*stream) || *stream == '_') {
                //idemo na kraj strima
                stream++;
            }

            currToken.kind = NAME_TOKEN;
            currToken.start = begining;
            currToken.end = stream;
        }
            break;
        default:
            currToken.kind = *stream;
            stream++;
            break;
    }

}


void printToken(Token tok) {

    switch (tok.kind){

        case INT_TOKEN:{
            printf("TOKEN INT: %d\n",tok.intval);
            break;
        }
        case NAME_TOKEN:{
            printf("NAME TOKEN %.*s\n",(int)(tok.end-tok.start),tok.start);
            break;
        }
        default:{
            printf("U:%c\n",tok.kind);
            break;
        }
    }



}

void testLex() {

    char *input = "() 1234+923 iden";
    stream = input;
    next_token();
    while (currToken.kind) {
        printToken(currToken);
        next_token();
    }

}


int main() {

    testLex();

    return 0;
}