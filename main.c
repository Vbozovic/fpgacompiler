#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "stretchy_buffer.h"

//POTENCIJALNI OVERFLOW
static char err_buf[1024 * 1024];

void fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("FATAL: ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
    exit(1);
}

typedef enum TokenKind {
    INT_TOKEN = 128,
    NAME_TOKEN

} TokenKind;


typedef struct Token {
    TokenKind kind;
    const char *start;
    const char *end;
    union {
        int intval; //ukoliko je token int literal cuvamo vrednost samog literal
        const char *name; //ime koje ce se internovati
    };
} Token;


typedef struct IStr {
    size_t len;
    const char *str;
} Istr;

static Istr *interns;

const char *str_intern_range(const char *start, const char *end) {

    size_t len = end - start;
    size_t i;
    for (i = 0; i < buf_len(interns); i++) {
        if (interns[i].len == len && strncmp(interns[i].str, start, len) == 0) {
            //Nasli smo string
            return interns[i].str;
        }
    }

    char *str = malloc(len + 1);
    memcpy(str, start, len);
    str[len] = 0; //null terminate
    buf_push(interns, ((Istr) {len, str}));

    return str;
}


const char *str_intern(const char *str) {
    return str_intern_range(str, str + strlen(str));
}

const char *stream;
Token currToken;

void next_token() {
    currToken.start = stream;
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
            int val = 0;
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

            const char *begining = stream;
            while (isalnum(*stream) || *stream == '_') {
                //idemo na kraj strima
                stream++;
            }
            currToken.kind = NAME_TOKEN;
            currToken.name = str_intern_range(begining, stream);
        }
            break;
        default:
            currToken.kind = *stream;
            stream++;
            break;
    }

    currToken.end = stream;

}


void printToken(Token tok) {
    switch (tok.kind) {

        case INT_TOKEN: {
            printf("TOKEN INT: %d\n", tok.intval);
            break;
        }
        case NAME_TOKEN: {
            printf("NAME TOKEN %.*s\n", (int) (tok.end - tok.start), tok.start);
            break;
        }
        default: {
            printf("U:%c\n", tok.kind);
            break;
        }
    }

}

const char *get_token_kind_name(TokenKind tok) {

    switch (tok) {
        case INT_TOKEN:
            return "INT_TOKEN";
        case NAME_TOKEN:
            return "NAME_TOKEN";
    }

    return "NONE";
}

bool is_token(TokenKind kind) {
    return currToken.kind == kind;
}

inline bool is_token_name(const char *name) {
    return currToken.name == name;
}


//ukoliko je trenutni token ono sto trazimo (kind) onda idemo na sledeci token
bool match_token(TokenKind kind) {
    if (is_token(kind)) {
        next_token();
        return true;
    } else {
        return false;
    }
}

//isto kao match token samo sto umesto da vratimo false mi cmeo da obustavimo sve
bool expect_token(TokenKind kind) {
    if (match_token(kind)) {
        return true;
    } else {
        fatal("Expected %s , got %s", get_token_kind_name(kind), get_token_kind_name(currToken.kind));
        return false;
    }
}

int parse_expr();

int parse_expr3() {

    if (is_token(INT_TOKEN)) {
        int val = currToken.intval;
        next_token(); //samo ga prepoznajemo
        return val;
    } else if (match_token('(')) {
        int val = parse_expr(); //pravimo petlju
        expect_token(')');
        return val;
    } else {
        fatal("Expected integer or ( , got %s", get_token_kind_name(currToken.kind));
    }


}

int parse_expr2() {
    if (match_token('-')) {
        return -parse_expr3();
    } else {
        return parse_expr3();
    }
}

int parse_expr1() {
    int val = parse_expr2();
    int rval;
    while (is_token('*') || is_token('/')) {
        char operater = currToken.kind;
        next_token();
        rval = parse_expr2(); // ovde kaze parse_expr2() ???

        if (operater == '*') {
            val *= rval;
        } else {
            assert(operater == '/');
            assert(rval != 0);

            val /= rval;
        }

    }
    return val;
}

int parse_expr0() {
    int val = parse_expr1();
    int rval;
    while (is_token('+') || is_token('-')) {
        char operater = currToken.kind;
        next_token();
        rval = parse_expr1();// ovde kaze parse_expr2() ???

        if (operater == '+') {
            val += rval;
        } else {
            assert(operater == '-');
            val -= rval;
        }

    }
    return val;
}

void init_stream(const char *init) {
    stream = init;
    next_token();
}

int parse_expr() {
    return parse_expr0();
}

int test_parse_expresion(const char* str){
    init_stream(str);
    return parse_expr();
}


void testParse() {

    assert(test_parse_expresion("1") == 1);
    assert(test_parse_expresion("(1)") == 1);
    assert(test_parse_expresion("1+2+3") == 6);
    assert(test_parse_expresion("1+2*3") == 7);
    assert(test_parse_expresion("(1+2)*3") == 9);
    assert(test_parse_expresion("2*(1+2)*3") == 18);
    assert(test_parse_expresion("2*3+4*5") == 26);

}

void testLex() {

    char *input = "() 1234+923 iden neki iden";
    stream = input;
    next_token();
    while (currToken.kind) {
        printToken(currToken);
        next_token();
    }

}

void intrn_test() {

    const char *x = "Vuk";
    const char *y = "Vuk";
    const char *z = "Vuk!";

    const char *px = str_intern(x);
    const char *py = str_intern(y);
    const char *pz = str_intern(z);

    assert(px == py);
    assert(py != pz);


}


int main() {

    testLex();
    intrn_test();
    testParse();

    return 0;
}