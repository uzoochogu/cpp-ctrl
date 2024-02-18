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
* `CMake`
* Clang tools - `clang-tidy` and `clang-format`
* `vcpkg`

## Dependency management
`vcpkg` is used for dependency management. Some sections use external libraries pulled in from their repositaries. You can comment out these sections in the TopLevel [`CMakelist.txt file`](/CMakeLists.txt). If you want to build these sections you would need to install the libraries specified in their `vcpkg.json` file. To do this, you can run:
```shell
# run this in any folder that vcpkg.json is in.
# triplet is optional, but useful on windows
path_to_vcpkg install --triplet triplet_name
```
Dependency management will be covered in a future section to further address this. Note you can also use Conan or any robust package manager or even `CMake` itself. Sections may have a markdown document detailing some dependencies for the section. 
### Some useful vcpkg links
* [`vcpkg` getting started](https://vcpkg.io/en/getting-started)
* [`Cpp Weekly vcpkg guide`](https://www.youtube.com/watch?v=KOeOLOu6nHw)
* [`Microsoft Developer long form video on vcpkg`](https://www.youtube.com/watch?v=3vXOKkv3ND0)
* [`Gamefromscratch - using vcpkg`](https://www.youtube.com/watch?v=iZeK3Ie5Fz0)

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