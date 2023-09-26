# cpp-ctrl
***C++ Concepts, Tools, Rabbit holes, and Libraries.*** 

The C++ community is getting bigger and the language feels very modern. This is a useful guide for beginners and junior engineers to scale the gap from intermediate level on their journey to expert level (or at least Professional Level).
It introduces common C++ practices, concepts, paradigms, libraries and tools.

## Structure
This repository uses CMake as its build tool. It is organized in folders. Each folder is designed to be a single project and may have at least one (1) target. It also uses Compiler Explorer by Godbolt and Coliru to host sections that can be fiddled with. The goal is to learn and provide a structured way to do so. 

## Contribution Guide
Each project/folder should be a comprehensive guide on a particular pattern, concept, feature or practice. You can talk about how to use new tools as well. They can typically contain a `CMakeLists.txt` file. This file should be added as a subdirectory to the top level CMakeFile so that it can all be built with one command. Thus it should expose a single target (Executable, Module, Shared Lib or Static lib).

Dependencies should also be specified so that the top level `CMakeLists.txt` can expose different flags to build different folders.

If a topic already exists on what you want to talk about, don’t be discouraged. You can write on the application of the particular concept. For example as exciting as talking about the development that led to `std::variant`, you can discuss it's utility in exception handling and polymorphism without vtables.

## Tools Required
* C++20 compatible compiler
* CMake
* Clang tools - `clang-tidy` and `clang-format`

### Building Project
```cmd
$ mkdir build && cd build
$ cmake ..
$ cmake --build . 
```
Support for optional building of different sections will be added later.
### Running clang-tidy
Pass the path of the section you want to test, and the path to the include directory if any.
```cmd
clang-tidy --format-style=file 1-callables-and-callbacks/src/*.cpp -extra-arg=-std=c++20 -- -I./
```

## Useful Section Tags

***<details><summary><code>Rabbit Hole</code></summary>***
</details>
You'll see this warning scattered about the sections. Computer science is vast and there are various intricacies. This tag collapses possibly useful information not essential to understand the concept at hand. If you are curious, they usually point to articles that explore those concepts in details.