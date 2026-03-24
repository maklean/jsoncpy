# jsoncpy
``jsoncpy`` is a JSON compiler frontend built entirely in C. It parses a JSON file and prints its Abstract Syntax Tree (AST). The program takes a single command-line argument: the path to the JSON file. This compiler frontend follows the JSON Grammar defined in [RFC 8259 (Internet Standard)](https://datatracker.ietf.org/doc/html/rfc8259).

## File Responsibilities
- ``parser.c`` – Contains all of the parsing logic, responsible for building the AST from tokens.
- ``scanner.c`` – Contains all of the scanning/tokenizing logic, converting raw JSON input into tokens.
- ``utils.c`` – Contains utility functions, such as freeing the AST and printing specific token types.
- ``debug.c`` – Contains debugging functions for printing the JSON file, scan results, and AST structures.

## Example Output
The following is the output from the program when given ``assets/test.json``:
```
OBJECT (Length: 7) {
  KEY: edge_cases
    OBJECT (Length: 6) {
      KEY: zero_float
        NUMBER (FLOAT): 0
      KEY: negative_zero
        NUMBER (FLOAT): -0
      KEY: lone_negative
        NUMBER (INT): -1
      KEY: exp_no_decimal
        NUMBER (FLOAT): 5000
      KEY: negative_exp
        NUMBER (FLOAT): -0.025000000000000001
      KEY: positive_exp_sign
        NUMBER (FLOAT): 100000
    }
  KEY: strings
    OBJECT (Length: 6) {
      KEY: empty
        STRING: ""
      KEY: only_spaces
        STRING: "   "
      KEY: newline_tab
        STRING: "a
b	c"
      KEY: escaped_backslash
        STRING: "a\b"
      KEY: escaped_quote
        STRING: "say "hi""
      KEY: forward_slash
        STRING: "a/b"
    }
  KEY: arrays
    OBJECT (Length: 5) {
      KEY: single_element
        ARRAY (Length: 1) [
            NUMBER (INT): 42
        ]
      KEY: nested_arrays
        ARRAY (Length: 3) [
            ARRAY (Length: 2) [
                NUMBER (INT): 1
                NUMBER (INT): 2
            ]
            ARRAY (Length: 2) [
                NUMBER (INT): 3
                NUMBER (INT): 4
            ]
            ARRAY (Length: 2) [
                NUMBER (INT): 5
                NUMBER (INT): 6
            ]
        ]
      KEY: all_nulls
        ARRAY (Length: 3) [
            NULL
            NULL
            NULL
        ]
      KEY: all_bools
        ARRAY (Length: 4) [
            BOOLEAN: true
            BOOLEAN: false
            BOOLEAN: true
            BOOLEAN: false
        ]
      KEY: mixed
        ARRAY (Length: 5) [
            NUMBER (INT): 1
            STRING: "two"
            NULL
            BOOLEAN: false
            NUMBER (FLOAT): 3.1400000000000001
        ]
    }
  KEY: objects
    OBJECT (Length: 3) {
      KEY: single_key
        OBJECT (Length: 1) {
          KEY: x
            NUMBER (INT): 1
        }
      KEY: all_nulls
        OBJECT (Length: 3) {
          KEY: a
            NULL
          KEY: b
            NULL
          KEY: c
            NULL
        }
      KEY: numeric_looking_keys
        OBJECT (Length: 2) {
          KEY: 1key
            STRING: "value"
          KEY: key1
            STRING: "value"
        }
    }
  KEY: deeply_nested_array
    ARRAY (Length: 1) [
        ARRAY (Length: 1) [
            ARRAY (Length: 1) [
                ARRAY (Length: 1) [
                    ARRAY (Length: 1) [
                        NUMBER (INT): 42
                    ]
                ]
            ]
        ]
    ]
  KEY: null_top_level_value
    NULL
  KEY: bool_top_level
    BOOLEAN: true
}
```

## How To Run

1. Clone the repository from GitHub and navigate to the project:
```bash
git clone https://github.com/maklean/jsoncpy.git
cd jsoncpy
```

2. Build the project with the `make` utility:
```bash
make
```

3. Run the project:
```bash
./jsoncpy <path-to-json>
```

## TODO
- [X] Support scanning exponential numbers (e.g., 2.9e-4)
- [X] Make a Makefile
- [X] Check for duplicate object keys
- [X] Use arena allocator to make freeing memory easier.
- [ ] Add a query language to traverse the AST.

