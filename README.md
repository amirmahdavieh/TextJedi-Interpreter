# TextJedi Interpreter

## Project Overview

TextJedi is a custom programming language designed for simplified text and integer manipulation. This repository contains the Lexer and Parser written in C, which interprets and executes .tj (TextJedi) files.

This is a refactored version of an earlier implementation: the main logic and language features are preserved, but the code has been reorganized to be more modular, readable, and maintainable (centralized error handling, helper functions for token and value resolution).


## Features & Capabilities

The TextJedi language supports the following core operations:
<br>

### 1. Variable Declaration

All variables must be declared using the `new` keyword and one of the two supported types:

| Keyword   | Description                       |
|-----------|-----------------------------------|
| `new int` | Declares a new integer variable.  |
| `new text`| Declares a new string (text) variable. |

#### **Example:**

```
new int myNumber;
new text myString;
```
<br>


### **2. Assignment & Arithmetic**

Assignment uses the `:=` operator.
Basic arithmetic (`+` / `-`) is supported for both types.


#### **Operator Behavior Table**

| Type   | Operator | Behavior                                                     |
| ------ | -------- | ------------------------------------------------------------ |
| `int`  | `+`      | Standard integer addition.                                   |
| `int`  | `-`      | Standard integer subtraction.                                |
| `text` | `+`      | String concatenation.                                        |
| `text` | `-`      | Removes the second string (substring) from the first string. |

#### **Example**

```
myNumber := 10 + 5;
myString := "Hello" + " World";
anotherString := myString - " World"; /* anotherString is "Hello" */
```
<br>


### **3. Built-in Functions**

TextJedi provides several functions for dynamic string manipulation.
These functions output results directly to the console in this implementation.


#### **Function Reference**

| Function                      | Arguments             | Description                                                                                                                |
| ----------------------------- | --------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| `size(var)`                   | `text` variable       | Returns the length of the string in the variable.                                                                          |
| `subs(var, start, end)`       | `text`, `int`, `int`  | Returns a substring from the `start` index to the `end` index (inclusive).                                                 |
| `locate(var, sub, start)`     | `text`, `text`, `int` | Returns the starting index of the first occurrence of `sub` within `var`, starting from `start`. Returns `0` if not found. |
| `insert(var, loc, insText)`   | `text`, `int`, `text` | Inserts `insText` into `var` at the specified `loc`.                                                                       |
| `override(var, loc, ovrText)` | `text`, `int`, `text` | Overwrites the content of `var` starting at `loc` with `ovrText`.                                                          |
| `asText(var)`                 | `text` variable       | Converts a numeric string (e.g., `"123"`) to an integer.                                                                   |
| `asString(var)`               | `int` variable        | Converts an integer value to a string (e.g., `123` → `"123"`).                                                             |


<br>

### **4. Input/Output (I/O)**


| Command            | Arguments                            | Description                                                              |
| ------------------ | ------------------------------------ | ------------------------------------------------------------------------ |
| `read from fileID` | `text` variable, Identifier          | Reads the contents of `fileID.txt` and stores it in the `text` variable. |
| `write to fileID`  | `int` or `text` variable, Identifier | Appends the variable's value to `fileID.txt`.                            |
| `input prompt`     | `text` variable                      | Prompts the user for input and stores the result in the `text` variable. |
| `output`           | `int` or `text` variable             | Prints the value of the variable to the console.                         |

<br>

## Theoretical Overview of the System

This project is built upon foundational concepts from compiler and interpreter theory. At its core, the system transforms raw source code into meaningful actions by breaking the process into well-defined stages. This mirrors the structure of most programming-language processors.

The first major component is the lexer, also known as the tokenizer. The purpose of lexical analysis is to convert a stream of characters into a sequence of atomic units called tokens. These tokens represent the smallest meaningful elements of the language—identifiers, keywords, constants, operators, and punctuation. Theoretical models describe the lexer as a finite state machine, which moves between states according to character classes (letters, digits, symbols, whitespace). By classifying characters and grouping them into categories, lexical analysis abstracts away the raw text and provides a structured foundation for the next stage.

Once tokens are produced, the parser performs syntactic analysis. Theoretical parsing is based on the grammar of the language, typically expressed in forms such as Backus–Naur Form (BNF). A parser checks whether the token stream follows the rules of this grammar. The project uses a style that corresponds to recursive descent parsing, where each grammar rule is represented by a separate function or logical block that calls other rules as needed. Recursive descent parsers are conceptually modeled as a set of mutually recursive recognizers for each non-terminal in the grammar. This parsing approach does not require complex parser generators; instead, it relies on the predictability of the language’s grammar and the ability to look ahead to the next token to decide which rule applies.

Behind the parser lies the semantic layer, which interprets the meaning of syntactically valid statements. The semantic model defines how variables are stored, how operations behave, and how expressions are evaluated. Theoretical semantics can be described in terms of abstract machines, where each statement transforms an internal program state. This state includes the symbol table (mapping variable names to values) and the environment that tracks types and runtime data. Operations such as arithmetic, string manipulation, and input/output correspond to state transitions defined by the language’s semantics.

Overall, the architecture of the project reflects a classical three-stage theoretical pipeline found in many language processors:

1. Lexical Analysis: Transforms characters into tokens using finite-state principles.

2. Parsing: Checks grammatical structure using recursive descent methods grounded in context-free grammar theory.

3. Semantic Evaluation: Interprets statements by applying meaning rules and updating program state.

These techniques collectively illustrate how high-level language constructs can be reduced to systematic, rule-driven processes, forming the theoretical foundation for interpreters and compilers of all sizes.


<br>

## How to Run
You can run the TextJedi interpreter either by compiling manually with GCC or using CLion/CMake.

### Running with GCC

Open a terminal inside the project directory:

```bash
cd path/to/TextJedi
```

Compile the interpreter:

```bash
gcc -o textjedi main.c
```

Make sure your myProg.tj file is located in the same directory (or update BASE_PATH in main.c accordingly).

Run the interpreter:

```bash
./textjedi
```

The program will automatically load and execute myProg.tj.
