FINT - a simple formula interpreter implemented in ANTLR4 for C++.

To build this project you need ANTLR4 generator installed.
This is how to do it:

1. Copy this repository to some folder, for example /home/fint
2. Create subfolder build
3. Run ANTLR4 to generate C++ parser: "java -jar <ANTLR4 JAR> -Dlanguage=Cpp -visitor fint.g4
4. Change directory to build: cd build
5. Run cmake: cmake ..;make
6. Try running fint: fint -f test.fint.

