Sure! Let's create a structured documentation for your custom programming language.

---

# cBudgie Language Documentation

## Introduction

This language is designed for fun. It includes features such as custom numeric methods, basic control flow, and input/output functions.

## Language Features

### Variables

- **Declaration and Initialization**: Use `=` to declare and initialize variables.
- **Global Variables**: Must be defined before main() is called.

### Functions

- **Definition**: Use `def` to define a function.
- **Return**: Use `return` to return values from functions.
- Example:
  ```plaintext
  def functionName(parameters)
      // function body
  return value
  ```
- **Main Function**: Call main() to start execution.

### Comments

- Use # for single-line comments.

### Operators

#### Relational Operators

- **>**: Greater than
- **>=**: Greater than or equal to
- **<**: Less than
- **<=**: Less than or equal to

#### Logical Operators

- **&**: And
- **|**: Or
- **^**: XOR

#### Equality Operators

- **=**: Equal
- **==**: Equivalent
- **!=**: Not equivalent
- **!**: Not

#### Arithmetic Operators

- **Addition**: +
- **Subtraction**: -
- **Multiplication**: *
- **Division**: /
- **Modulus**: %
- **Power**: **

### Control Flow

- **While Loop**: 
  ```plaintext
  while (condition) {
      // code block
  }
  ```

- **If-Else Statements**:
  ```plaintext
  if (condition) {
      // code block
  } else if (condition) {
      // code block
  } else {
      // code block
  }
  ```

- **For Loop**:
  ```plaintext
  for (i=start:end:step) {
      // code block
  }
  ```

### Input/Output

- **Print**: `print("message")`
- **Read**: 
  ```plaintext
  variable = read("Prompt: ")
  ```

### Arrays

- **Declaration**: Use square brackets to declare arrays.
- **Access and Modify**: Access via index and modify directly.
- Example:
  ```plaintext
  name = ["Tom", "Tim"]
  name[1] = "Hugo"
  ```

### String Interpolation

- Use `f"Your message {variable}"` for formatted strings.

### Numeric Methods

- **Numeric Operations**: `numeric("operation", precision)`
  - `"floor"`: Rounds down.
  - `"ceil"`: Rounds up.
  - `"round"`: Rounds to the nearest value.

## Example Scripts

[Refer to Examples](../Examples/)

## Installation and Setup

To build and install the `cbudgie` interpreter, follow these steps:

1. **Clone the Repository**:
    ```sh
    git clone https://github.com/Hugodonotexit/CBudgie
    cd CBudgie
    ```

2. **Create a Build Directory**:
    ```sh
    mkdir build
    cd build
    ```

3. **Run CMake**:
    ```sh
    cmake ..
    ```

4. **Build the Project**:
    ```sh
    make
    ```

5. **Install the Executable**:
    ```sh
    sudo make install .
    ```

---