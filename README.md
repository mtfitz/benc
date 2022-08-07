# benc
A simple Bencode library, written in modern C++

## Usage
To demonstrate this, build and execute `benc-cli`, which displays a help message:

```
Usage: benc [OPTIONS] input_text
Options:
	-i FILE		use input file, instead of STDIN
	-o FILE		use output file, instead of STDOUT
	-r FILE		give input text via terminal/STDIN
```



To use this library from a C++ application, please refer to the header `benc.hpp`.

## Building
The library and program can be build using CMake and its supported outputs. Compilation should work out-of-the-box on POSIX-compatible systems. On Windows, you must ensure the `getopt` library exists; you can grab it using `vcpkg`.
