# Section Dependencies 

* CTPG
* Boost
* ANTLR


## `CTPG`
CTPG is a single header library - It was just copied to the src directory and compiled against in the Cmake file. 

Note that it can also be installed using `conan` or `vcpkg` or any other package manager. In this case, you can use the `find_package()` command in the `CMakeLists.txt` file. 

If you don't want to use a package manager, the release version of repo can be downloaded, built from source and installed. After this, the `find_package()` command should work.


## `Boost.Spirit X3`
This comes with the a Boost installation. `vcpkg` is used to manage this dependency. A `vcpkg.json` file was provided. Check [`here`](../README.MD#dependency-management) for a little guide on using `vcpkg`.


## `ANTLR4`:
Yet to be installed and used.
