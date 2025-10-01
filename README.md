# Limit Order Book with Iceberg Order support

## Overview

This project is a implementation of a Limit Order Book (LOB) with support for Iceberg Orders. The LOB is a fundamental component of modern electronic trading systems, responsible for managing and matching buy and sell orders based on specified limits and quantities.

The design focuses on optimizing time complexity and efficiency, ensuring rapid insertion and matching operations.

## Building & Testing

The project uses CMake as a build system and Google Test for unit testing. To build and test the project, follow these steps:

1. Clone the repository to your local machine:
    ```bash 
    git clone https://github.com/JovanMarkovic99/iceberg-order-book
    ```

2. Navigate to the project's root directory and create a build directory:
    ```bash
    cd iceberg-order-book
    mkdir build
    ```

3. Build the project by running `cmake` in the build directory:
    ```bash
    cd build
    cmake ..
    cmake --build .
    ```

4. (Optional) Run the provided unit tests to ensure the project functions as expected:
    ```bash
    ctest -C Debug
    ```

5. Run the program `main.exe`.
