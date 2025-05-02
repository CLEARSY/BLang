# BLang Library

## Overview

BLang is a C++ library designed to represent the B language. It is thread-safe and implements maximal sharing of subtypes. It also includes a visitor pattern and a formatter suitable for the `fmt` library.

## Features

- Type system:
    - Basic Types: Integer, Boolean, Float, Real, String
    - Complex Types: Product, Power Set, Abstract Set, Enumerated Set, Struct
    - Type Comparisons: Equality, Inequality, Less than, Greater than, etc.
    - Undefined type
- Expressions: 
  - Boolean literals: `TRUE` and `FALSE`
  - `bool` operator
  - Integer literals
- Predicates: Conjunction, Disjunction, Implication, Equivalence, Negation, Equality
- Maximal Sharing: Efficient memory usage by sharing common subtypes, sub-expressions, sub-predicates
- Visitor Pattern
- Thread Safety: Ensures safe operations in multi-threaded environments
- Formatter: Provides a formatter suitable for the `fmt` library

## Installation

To build and install the BLang library, follow these steps:

Clone the repository:

```sh
git clone https://github.com/CLEARSY/BLang.git
cd blang
```

Create a build directory and navigate to it:
```sh
mkdir build
cd build
```

Run CMake to configure the project:

```sh
cmake ..
```

Build the project:

```sh
make
```

(Optional) Run tests to verify the build:

```sh
ctest
```

## Example
Here is a simple example demonstrating the creation and usage of various types:

```
#include "blang_type.h"
#include <iostream>

int main() {
    auto integerType = BLang::TypeFactory::Integer();
    auto booleanType = BLang::TypeFactory::Boolean();
    auto productType = BLang::TypeFactory::Product(integerType, booleanType);
    auto powerType = BLang::TypeFactory::PowerSet(integerType);

    std::cout << "Integer Type: " << integerType->getKind() << std::endl;
    std::cout << "Boolean Type: " << booleanType->getKind() << std::endl;
    std::cout << "Product Type: " << productType->getKind() << std::endl;
    std::cout << "Power Type: " << powerType->getKind() << std::endl;

    return 0;
}
```

## Testing

The BLang library includes a comprehensive test suite to ensure the correctness and reliability of the types and their operations. The tests are located in the tests directory and can be run using ctest.


## License

BLang is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/.

## Contributing

Contributions are welcome! Please carefully read the CONTRIBUTING.md file in this repository in case you consider contributing.

## Contact

For any questions or issues, please open an issue on the GitHub repository.

