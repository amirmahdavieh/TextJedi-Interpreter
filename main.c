#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define BASE_PATH "C:\\Users\\Amir\\CLionProjects\\untitled\\"
#define MAIN_FILE_NAME "myProg.tj"

// Types
typedef enum {
    Idenfitier,
    IntegerConst,
    StringConst,
    Operator,
    Keyword,
    Bracket,
    EndOfLine,
    Comma,
    Unknown
} TokenType;

typedef struct {
    TokenType type;
    char* name;
    union {
        char* strConst;
        int intConst;
    } value;
} Token;


char *keywords[] = {
        "new", "int", "text", "size", "subs", "locate", "insert", "override",
        "read", "write", "from", "to", "input", "output", "asText", "asString"
};


Token getToken(FILE *program);
void parseLine(Token token, Token *varsarray, int *vrcounter, char *varTypes, FILE* program);


void errorExit(const char* msg) {
    printf("Error: %s\n", msg);
    exit(0);
}

void syntaxError(const char* msg) {
    printf("Syntax Error! %s\n", msg);
    exit(0);
}

// Lexer Utilities

char ungetChar(FILE *point) {           // Sees the forward character while reading and puts it back to stream
    char cha;
    cha = fgetc(point);
    ungetc(cha,point);

    return cha;
}

int isKeyword(char *str) {       // Decides if the string is in keywords' array

    for (int a = 0; a < 16; ++a) {

        if(strcmp(str, keywords[a]) == 0 ) {
            return 1;
        }
    }
    return 0;
}

Token identifier(Token tk, char* word, char ch, int count, int notfound, FILE *program) {
    char character_peek;
    word[count] = ch;
    while (notfound) {
        character_peek = ungetChar(program);
        if (isalpha(character_peek) || isdigit(character_peek) || character_peek == '_') {
            ch = fgetc(program);
            word[++count] = ch;
        } else {
            notfound = 0;
            word[++count] = '\0';
        }
    }

    if (isKeyword(word) == 1) {
        tk.type = Keyword;
        tk.name = word;
    }
    else {
        tk.type = Idenfitier;
        tk.name = word;
        tk.value.strConst = "\0";
    }
    return tk;
}

Token operator(Token tk, char* word, char ch, int count, FILE *program) {

    if (ch == ':' && ungetChar(program) == '=') {
        word[count++] = ch;
        ch = fgetc(program);
        word[count++] = ch;
    }
    else {
        word[count++] = ch;
    }

    word[count] = '\0';

    if (word[0] == ':' && word[1] != '=') {
        errorExit("':' is not a defined operator!"); // Centralized error handling
    } else {
        tk.type = Operator;
        tk.name = word;
        tk.value.strConst = "\0";
    }
    return tk;
}

Token strConst(Token tk, char* str_arr, char ch, int count, FILE *program) {

    ch = fgetc(program);
    str_arr[count] = ch;

    while (ungetChar(program) != EOF && ungetChar(program) != '"') {
        ch = fgetc(program);
        str_arr[++count] = ch;
    }

    if (ungetChar(program) == EOF) {
        errorExit("String is not closed.");
    } else {
        str_arr[++count] = '\0';
        fgetc(program); // consume closing quote

        tk.type = StringConst;
        tk.value.strConst = str_arr;
    }
    return tk;
}

Token intConst(Token tk, char* word, char ch, int count, FILE *program) {

    word[count++] = ch;
    while(isdigit(ungetChar(program))) {
        ch = fgetc(program);
        word[count++] = ch;
    }

    word[count] = '\0';

    tk.type = IntegerConst;
    tk.name = "\0";
    tk.value.strConst = word;

    return tk;
}

Token basicToken(TokenType type, char* name) {
    Token tk;
    tk.type = type;
    tk.name = name;
    return tk;
}

void comment(char ch, FILE *program) {

    int cmtCounter = 0;

    while ((ch = ungetChar(program)) != EOF) {
        ch = fgetc(program);
        if (ch == '*' && ungetChar(program) == '/') {
            cmtCounter = 1;
            fgetc(program);     // takes '/' from file stream
            break;
        }
    }

    if (cmtCounter == 0) {
        errorExit("Comment is not closed!");
    }
}

// Consolidated Lexer
Token getToken(FILE *program) {
    Token tk = {Unknown, " ", ""};
    char character;
    int counter;
    int notfound;
    char *str_arr;

    while (!feof(program)) {
        char *lexeme = (char *) malloc(500);
        if (!lexeme) errorExit("Memory allocation failed in getToken.");

        notfound = 1;
        counter = 0;
        character = fgetc(program);

        if (character == EOF) { free(lexeme); break; }
        if (isspace(character)) { free(lexeme); continue; }

        if (isalpha(character)) {
            tk = identifier(tk, lexeme, character, counter, notfound, program);
            return tk;
        } else if( character == '+' || character == '-' || character == ':') {
            tk = operator(tk, lexeme, character, counter, program);
            return tk;
        } else if (character == '"') {
            str_arr = (char*) malloc(500);
            if (!str_arr) errorExit("Memory allocation failed for string.");
            tk = strConst(tk, str_arr, character, counter, program);
            free(lexeme);
            return tk;
        } else if (isdigit(character)) {
            tk = intConst(tk, lexeme, character, counter, program);
            return tk;
        } else if (character == '(') {
            tk = basicToken(Bracket, "(");
            free(lexeme);
            return tk;
        } else if (character == ')') {
            tk = basicToken(Bracket, ")");
            free(lexeme);
            return tk;
        } else if (character == ';') {
            tk = basicToken(EndOfLine, ";");
            free(lexeme);
            return tk;
        } else if (character == '/' && ungetChar(program) == '*') {
            comment(character, program);
            free(lexeme);
            continue;
        } else if (character == ',') {
            tk = basicToken(Comma, ",");
            free(lexeme);
            return tk;
        }
        free(lexeme);
    }
    return tk;
}

// Variable and Token Helpers

// Replaces findIndex and incorporates error handling
int getExistingVarIndex(Token* vars, int count, char* name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(vars[i].name, name) == 0) return i;
    }
    // If not found, exit (replaces repeated check blocks)
    printf("Error: You have not defined variable '%s' before!\n", name);
    exit(0);
}

// Ensures the next token matches type and name, and consumes it
Token requireToken(FILE* program, TokenType type, const char* nameMatch) {
    Token t = getToken(program);
    if (t.type != type) {
        syntaxError("Unexpected token type.");
    }
    if (nameMatch != NULL && strcmp(t.name, nameMatch) != 0) {
        char msg[100];
        sprintf(msg, "Expected '%s' but got '%s'.", nameMatch, t.name);
        syntaxError(msg);
    }
    return t;
}

// Resolves an integer value (either Literal or Variable)
int resolveInt(Token t, Token* vars, int count, char* types) {
    if (t.type == IntegerConst) {
        return atoi(t.value.strConst);
    } else if (t.type == Idenfitier) {
        int idx = getExistingVarIndex(vars, count, t.name);
        if (types[idx] != 'i') errorExit("Expected integer variable (type mismatch).");
        return vars[idx].value.intConst;
    }
    syntaxError("Expected integer or integer variable.");
    return 0;
}

// Resolves a string value (either StringConst or Variable)
char* resolveString(Token t, Token* vars, int count, char* types) {
    if (t.type == StringConst) {
        return t.value.strConst;
    } else if (t.type == Idenfitier) {
        int idx = getExistingVarIndex(vars, count, t.name);
        if (types[idx] != 's') errorExit("Expected text variable (type mismatch).");
        return vars[idx].value.strConst;
    }
    syntaxError("Expected string constant or text variable.");
    return NULL;
}

// String Functions

char* removeSubstring(char *string1, char *string2) {
    if (!string1 || !string2) return string1;
    int length1 = strlen(string1);
    int length2 = strlen(string2);
    char* result = strstr(string1,string2);

    if (length2 > length1) {
        errorExit("Second string is longer than first string!");
    }

    if(result == NULL){
        return string1;
    }
    int newLength = length1-length2;
    char* newText = malloc((newLength+1)*sizeof (char));
    if (!newText) errorExit("Memory allocation failed.");

    // Improved substring removal using strncpy and pointer arithmetic
    strncpy(newText,string1,result-string1);
    strcpy(newText+(result-string1),result+length2);
    newText[newLength]='\0';
    return newText;
}

int sizeFunction(Token tk) {
    char* str = tk.value.strConst;
    return str ? strlen(str) : 0;
}

char* subsFunction(Token tk, int start, int end) {
    char* str = tk.value.strConst;
    // Replaced fixed 200 size with dynamic based on required length
    int len = end - start + 1;
    char* substring = (char*)malloc(sizeof(char) * (len + 1));
    if (!substring) errorExit("Memory allocation failed.");

    int substringIndex = 0;
    for (int i = start; i <= end && str[i] != '\0'; i++) {
        substring[substringIndex++] = str[i];
    }
    substring[substringIndex] = '\0';
    return substring;
}

int locateFunction(const char* bigText, const char* smallText, int start) {

    int firstLengh = strlen(bigText);
    int secondLength = strlen(smallText);

    if (start >= firstLengh) {
        errorExit("Start is greater than text length!");
    }

    for (int i = start; i <= firstLengh - secondLength; i++) {
        int j;
        for (j = 0; j < secondLength; j++) {
            if (bigText[i + j] != smallText[j]) {
                break;
            }
        }
        if (j == secondLength) {
            return i;
        }
    }
    return 0;
}

char* insertFunction(char* str, int location, char* insertText) {

    int strLength = strlen(str);
    int insertLength = strlen(insertText);
    int resultLength = strLength + insertLength;

    char* result = malloc((resultLength + 1) * sizeof(char));
    if (!result) errorExit("Memory allocation failed.");

    strncpy(result, str, location);
    result[location] = '\0';
    strcat(result, insertText);
    strcat(result, str + location);

    return result;
}

char* overrideFunction(char* str, int location, char* ovrText) {
    int ovrLength = strlen(ovrText);
    int strLen = strlen(str);

    if (location > strLen) return str;

    // Allocate exactly what is needed for the result
    int newLen = (location + ovrLength > strLen) ? location + ovrLength : strLen;
    char* result = malloc((newLen + 1) * sizeof(char));
    if (!result) errorExit("Memory allocation failed.");

    // Copy original string part before location
    strncpy(result, str, location);
    result[location] = '\0';

    // Copy overwrite text
    strncat(result, ovrText, ovrLength);

    // Copy remaining original text
    if (location + ovrLength < strLen) {
        strcat(result, str + location + ovrLength);
    }

    result[newLen] = '\0';
    return result;
}

char* asStringFunction(Token *variablesArray, int index) {

    char *str = (char *) malloc(500 * sizeof(char));
    if (!str) errorExit("Memory allocation failed.");
    int integerValue = variablesArray[index].value.intConst;
    sprintf(str, "%d", integerValue);

    return str;
}

int asTextFunction(char *myString) {
    int result = 0;
    if (*myString == '\0') {
        errorExit("Empty array!");
    }

    while (*myString != '\0') {
        if (*myString < '0' || *myString > '9') {
            errorExit("Input is not a number!");
        }
        result = result * 10 + (*myString - '0');
        myString++;
    }
    return result;
}

char* readString(char* file) {

    char path[256] = BASE_PATH; // Use define
    strcat(path, file);
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *result = (char *)malloc((size + 1) * sizeof(char));
    if (!result) errorExit("Memory allocation failed.");

    fread(result, 1, size, f); // Changed sizeof(char*) to 1
    result[size] = '\0';
    fclose(f);

    return result;
}

void writeString(char* file, char* str) {

    char path[256] = BASE_PATH; // Use define
    strcat(path, file);

    FILE* f = fopen(path, "a+");
    if (f == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return;
    }
    fprintf(f, "%s", str);
    fclose(f);
}

// Parse Logic

void parseLine(Token token, Token *varsarray, int *vrcounter, char *varTypes ,FILE* program) {
    int currentVarCount = *vrcounter; // local copy

    if (token.type == Keyword) {

        if (strcmp(token.name, "new")==0) {
            Token typeToken = requireToken(program, Keyword, NULL);

            if (strcmp(typeToken.name, "int") == 0) {
                varTypes[currentVarCount] = 'i';
            } else if (strcmp(typeToken.name, "text") == 0) {
                varTypes[currentVarCount] = 's';
            } else {
                syntaxError("Use 'int' or 'text' keyword after 'new'.");
            }

            Token varToken = requireToken(program, Idenfitier, NULL);
            int index = -1;
            for (int i= 0; i < currentVarCount; i++) {
                if (strcmp(varToken.name, varsarray[i].name) == 0) index = i;
            }
            if (index != -1) {
                errorExit("Variable already defined!");
            }

            varsarray[currentVarCount] = varToken;
            (*vrcounter)++; // Update counter

            requireToken(program, EndOfLine, ";");
            parseLine(getToken(program), varsarray, vrcounter, varTypes, program);
        }

        else if (strcmp(token.name, "size") == 0 || strcmp(token.name, "subs") == 0 ||
                 strcmp(token.name, "locate") == 0 || strcmp(token.name, "insert") == 0 ||
                 strcmp(token.name, "override") == 0 || strcmp(token.name, "asString") == 0 ||
                 strcmp(token.name, "asText") == 0)
        {
            char* command = token.name;

            requireToken(program, Bracket, "(");
            Token var1 = requireToken(program, Idenfitier, NULL);
            int idx1 = getExistingVarIndex(varsarray, currentVarCount, var1.name);

            char* resultStr = NULL;
            int resultInt = 0;

            if (strcmp(command, "size") == 0) {
                if (varTypes[idx1] != 's') errorExit("Size expects text variable.");
                resultInt = sizeFunction(varsarray[idx1]);
                printf("Size of String: %d\n", resultInt);
            }
            else if (strcmp(command, "asText") == 0) {
                if (varTypes[idx1] != 's') errorExit("AsText expects text variable.");
                resultInt = asTextFunction(varsarray[idx1].value.strConst);
                printf("As Integer: %d\n", resultInt);
            }
            else if (strcmp(command, "asString") == 0) {
                if (varTypes[idx1] != 'i') errorExit("AsString expects int variable.");
                resultStr = asStringFunction(varsarray, idx1);
                printf("As String: %s\n", resultStr);
            }
            else if (strcmp(command, "subs") == 0) {
                requireToken(program, Comma, ",");
                int start = resolveInt(getToken(program), varsarray, currentVarCount, varTypes);
                requireToken(program, Comma, ",");
                int end = resolveInt(getToken(program), varsarray, currentVarCount, varTypes);

                if (varTypes[idx1] != 's') errorExit("Subs expects text variable.");
                resultStr = subsFunction(varsarray[idx1], start, end);
                printf("Subs of String: %s\n", resultStr);
            }
            else if (strcmp(command, "locate") == 0) {
                requireToken(program, Comma, ",");
                char* searchStr = resolveString(getToken(program), varsarray, currentVarCount, varTypes);
                requireToken(program, Comma, ",");
                int start = resolveInt(getToken(program), varsarray, currentVarCount, varTypes);

                if (varTypes[idx1] != 's') errorExit("Locate expects text variable.");
                resultInt = locateFunction(varsarray[idx1].value.strConst, searchStr, start);
                printf("Locate Result: %d\n", resultInt);
            }
            else if (strcmp(command, "insert") == 0) {
                requireToken(program, Comma, ",");
                int loc = resolveInt(getToken(program), varsarray, currentVarCount, varTypes);
                requireToken(program, Comma, ",");
                char* insStr = resolveString(getToken(program), varsarray, currentVarCount, varTypes);

                if (varTypes[idx1] != 's') errorExit("Insert expects text variable.");
                resultStr = insertFunction(varsarray[idx1].value.strConst, loc, insStr);
                printf("Insert Result: %s\n", resultStr);
            }
            else if (strcmp(command, "override") == 0) {
                requireToken(program, Comma, ",");
                int loc = resolveInt(getToken(program), varsarray, currentVarCount, varTypes);
                requireToken(program, Comma, ",");
                char* ovrStr = resolveString(getToken(program), varsarray, currentVarCount, varTypes);

                if (varTypes[idx1] != 's') errorExit("Override expects text variable.");
                resultStr = overrideFunction(varsarray[idx1].value.strConst, loc, ovrStr);
                printf("Override Result: %s\n", resultStr);
            }

            requireToken(program, Bracket, ")");
            requireToken(program, EndOfLine, ";");
            parseLine(getToken(program), varsarray, vrcounter, varTypes, program);
        }

            // --- IO COMMANDS ---
        else if (strcmp(token.name, "read") == 0) {
            Token var = requireToken(program, Idenfitier, NULL);
            int idx = getExistingVarIndex(varsarray, currentVarCount, var.name);

            if (varTypes[idx] != 's') errorExit("Read requires text variable.");

            requireToken(program, Keyword, "from");
            Token fileTk = requireToken(program, Idenfitier, NULL);

            char filename[100];
            strcpy(filename, fileTk.name);
            strcat(filename, ".txt");

            varsarray[idx].value.strConst = readString(filename);

            requireToken(program, EndOfLine, ";");
            parseLine(getToken(program), varsarray, vrcounter, varTypes, program);
        }
        else if (strcmp(token.name, "write") == 0) {
            Token var = requireToken(program, Idenfitier, NULL);
            int idx = getExistingVarIndex(varsarray, currentVarCount, var.name);

            requireToken(program, Keyword, "to");
            Token fileTk = requireToken(program, Idenfitier, NULL);
            char filename[100];
            strcpy(filename, fileTk.name);
            strcat(filename, ".txt");

            if (varTypes[idx] == 'i') {
                char buffer[50];
                sprintf(buffer, "%d", varsarray[idx].value.intConst);
                writeString(filename, buffer);
            } else {
                writeString(filename, varsarray[idx].value.strConst);
            }

            requireToken(program, EndOfLine, ";");
            parseLine(getToken(program), varsarray, vrcounter, varTypes, program);
        }
        else if (strcmp(token.name, "input") == 0) {
            Token var = requireToken(program, Idenfitier, NULL);
            int idx = getExistingVarIndex(varsarray, currentVarCount, var.name);

            if (varTypes[idx] != 's') errorExit("Input requires text variable.");

            requireToken(program, Keyword, "prompt"); // New keyword prompt

            char* inputArray = (char*) malloc(sizeof(char) * 101);
            if (!inputArray) errorExit("Memory allocation failed.");
            printf("Input: ");
            fgets(inputArray, 101, stdin);
            inputArray[strcspn(inputArray, "\n")] = 0; // Remove newline

            varsarray[idx].value.strConst = inputArray;
            printf("Input value: %s\n", varsarray[idx].value.strConst);

            requireToken(program, EndOfLine, ";");
            parseLine(getToken(program), varsarray, vrcounter, varTypes, program);
        }
        else if (strcmp(token.name, "output") == 0) {
            Token var = requireToken(program, Idenfitier, NULL);
            int idx = getExistingVarIndex(varsarray, currentVarCount, var.name);

            char* outputValue;
            if (varTypes[idx] == 'i') {
                outputValue = asStringFunction(varsarray, idx); // Use your function
                printf("Output: %s\n", outputValue);
            } else {
                outputValue = varsarray[idx].value.strConst;
                printf("Output: %s\n", outputValue);
            }

            requireToken(program, EndOfLine, ";");
            parseLine(getToken(program), varsarray, vrcounter, varTypes, program);
        }
        else {
            syntaxError("Keyword not recognized at start of line.");
        }
    }


    else if (token.type == Idenfitier) {
        int targetIdx = getExistingVarIndex(varsarray, currentVarCount, token.name);

        requireToken(program, Operator, ":=");
        Token op1 = getToken(program);

        // INTEGER LOGIC
        if (varTypes[targetIdx] == 'i') {
            int val1 = resolveInt(op1, varsarray, currentVarCount, varTypes);
            int result = val1;

            Token opToken = getToken(program);

            if (opToken.type == Operator) {
                if (strcmp(opToken.name, "+") == 0 || strcmp(opToken.name, "-") == 0) {

                    Token op2 = getToken(program);
                    int val2 = resolveInt(op2, varsarray, currentVarCount, varTypes);

                    if (strcmp(opToken.name, "+") == 0) result = val1 + val2;
                    else result = val1 - val2;

                    requireToken(program, EndOfLine, ";");
                } else {
                    syntaxError("Expected '+' or '-' for arithmetic operation.");
                }
            } else {
                // Must be EndOfLine
                if (opToken.type != EndOfLine) syntaxError("Expected ';' or operator after assignment value.");
            }

            varsarray[targetIdx].value.intConst = result;
        }
            // String
        else if (varTypes[targetIdx] == 's') {
            char* val1 = resolveString(op1, varsarray, currentVarCount, varTypes);
            char* result = val1;

            Token opToken = getToken(program);

            if (opToken.type == Operator) {
                if (strcmp(opToken.name, "+") == 0 || strcmp(opToken.name, "-") == 0) {

                    Token op2 = getToken(program);
                    char* val2 = resolveString(op2, varsarray, currentVarCount, varTypes);

                    if (strcmp(opToken.name, "+") == 0) {
                        // Concatenation
                        char* buf = malloc(strlen(val1) + strlen(val2) + 1);
                        if (!buf) errorExit("Memory allocation failed.");
                        strcpy(buf, val1);
                        strcat(buf, val2);
                        result = buf;
                    }
                    else {
                        // Subtraction (Remove Substring)
                        result = removeSubstring(val1, val2);
                    }

                    requireToken(program, EndOfLine, ";");
                } else {
                    syntaxError("Expected '+' or '-' for string operation.");
                }
            } else {
                // Must be EndOfLine
                if (opToken.type != EndOfLine) syntaxError("Expected ';' or operator after assignment value.");
            }
            varsarray[targetIdx].value.strConst = result;
        }
        else {
            errorExit("Variable type is undefined.");
        }

        parseLine(getToken(program), varsarray, vrcounter, varTypes, program);

    } else if (token.type != Unknown) {
        syntaxError("Line must start with Keyword or Identifier.");
    }
}

int main() {
    char fullPath[256] = BASE_PATH;
    strcat(fullPath, MAIN_FILE_NAME);

    FILE *programFile = fopen(fullPath, "r");

    Token* variables = (Token*) malloc(200 * sizeof (Token));
    char* variablesType = (char*) malloc(200 * sizeof (char));
    int variablesIndex = 0;

    if (!variables || !variablesType) {
        errorExit("Initial memory allocation failed.");
    }

    if (programFile == NULL) {
        printf("File could not be opened at: %s\n", fullPath);
    } else {
        Token token = getToken(programFile);
        parseLine(token, variables, &variablesIndex, variablesType, programFile);
        fclose(programFile);
    }

    free(variables);
    free(variablesType);
    return 0;
}