# jsoncpy
``jsoncpy`` is a JSON compiler frontend built entirely in C. It parses a JSON file and prints its Abstract Syntax Tree (AST). The program takes a single command-line argument: the path to the JSON file. This compiler frontend follows the JSON Grammar defined in [RFC8259](https://datatracker.ietf.org/doc/html/rfc8259).

## File Responsibilities
- ``parser.c`` – Contains all of the parsing logic, responsible for building the AST from tokens.
- ``scanner.c`` – Contains all of the scanning/tokenizing logic, converting raw JSON input into tokens.
- ``utils.c`` – Contains utility functions, such as freeing the AST and printing specific token types.
- ``debug.c`` – Contains debugging functions for printing the JSON file, scan results, and AST structures.

## Example Output
The following is the output from the program when given ``assets/test.json``:
```
OBJECT (Length: 3) {
  KEY: user
    OBJECT (Length: 6) {
      KEY: id
        NUMBER (INT): 101
      KEY: name
        STRING: "John Doe"
      KEY: email
        STRING: "johndoe@example.com"
      KEY: isAdmin
        BOOLEAN: true
      KEY: roles
        ARRAY (Length: 3) [
            STRING: "user"
            STRING: "editor"
            OBJECT (Length: 3) {
              KEY: roleName
                STRING: "administrator"
              KEY: rolePermissions
                ARRAY (Length: 3) [
                    STRING: "*"
                    NULL
                    NUMBER (INT): 100
                ]
              KEY: coolNumbers
                ARRAY (Length: 5) [
                    NUMBER (FLOAT): 1000
                    NUMBER (FLOAT): 1000
                    NUMBER (FLOAT): 0.0051999999999999998
                    NUMBER (FLOAT): 6000
                    NUMBER (FLOAT): 0.0089999999999999993
                ]
            }
        ]
      KEY: profile
        OBJECT (Length: 4) {
          KEY: age
            NUMBER (INT): 29
          KEY: gender
            STRING: "male"
          KEY: location
            STRING: "New York"
          KEY: preferences
            OBJECT (Length: 3) {
              KEY: theme
                STRING: "dark"
              KEY: notifications
                BOOLEAN: false
              KEY: language
                STRING: "en"
            }
        }
    }
  KEY: posts
    ARRAY (Length: 2) [
        OBJECT (Length: 6) {
          KEY: id
            NUMBER (INT): 1
          KEY: title
            STRING: "Introduction to C"
          KEY: content
            STRING: "This post is about learning C programming."
          KEY: tags
            ARRAY (Length: 3) [
                STRING: "C"
                STRING: "programming"
                STRING: "tutorial"
            ]
          KEY: published
            BOOLEAN: false
          KEY: date
            STRING: "2025-12-15T12:00:00Z"
        }
        OBJECT (Length: 6) {
          KEY: id
            NUMBER (INT): 2
          KEY: title
            STRING: "Exploring JSON"
          KEY: content
            STRING: "This post delves into JSON data format."
          KEY: tags
            ARRAY (Length: 3) [
                STRING: "JSON"
                STRING: "data"
                STRING: "format"
            ]
          KEY: published
            BOOLEAN: false
          KEY: date
            STRING: "2025-12-20T08:30:00Z"
        }
    ]
  KEY: comments
    ARRAY (Length: 2) [
        OBJECT (Length: 4) {
          KEY: id
            NUMBER (INT): 1001
          KEY: userId
            NUMBER (INT): 102
          KEY: text
            STRING: "Great post! Looking forward to more tutorials."
          KEY: date
            STRING: "2025-12-15T14:00:00Z"
        }
        OBJECT (Length: 4) {
          KEY: id
            NUMBER (INT): 1002
          KEY: userId
            NUMBER (INT): 103
          KEY: text
            STRING: "JSON is amazing, thanks for explaining it so clearly."
          KEY: date
            STRING: "2025-12-21T10:00:00Z"
        }
    ]
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
- [ ] Add a query language to traverse the AST.
- [ ] Use arena allocator to make freeing memory easier.
