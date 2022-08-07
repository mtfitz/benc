# benc
A simple Bencode library, written in modern C++

## Usage
To demonstrate this, build and execute `benc-cli`, which displays a help message:

```
Usage: benc-cli [OPTIONS] input_text
Options:
	-i FILE		use input file, instead of STDIN
	-o FILE		use output file, instead of STDOUT
	-r FILE		give input text via terminal/STDIN
```

For example, to decode the example found in the project root directory:

```
% cat testinput.txt
i32e
% ./build/bin/benc-cli -i testinput.txt
32
```

To use this library in a C++ application, please refer to the header `benc.hpp`.

## Building
The library and program can be build using CMake and its supported outputs. From the project root directory, this looks like:

```
mkdir build && cd build
cmake ..
cmake --build .
```

Compilation should work out-of-the-box on POSIX-compatible systems. On Windows, you must ensure the `getopt` library exists to build the demo; you can grab it using `vcpkg`.
