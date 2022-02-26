# CompilerConstructionProject
Project for the course CS F363 Compiler Construction.

Compile: 
```bash
make lexer
```

--------

How to use First and Follow sets automation:
(1) gcc -c FirstAndFollow.c
(1) gcc FirstAndFolllowDriver.c FirstAndFollow.o
(2) ./a.out finalgrammar.txt
(3) Results stored in firstSets.txt and followSets.txt
Note: grammar file in the form sym1 ===> sym2 sym3 sym4
FirstAndFollowDriver.c contains only the main() function for FirstAndFollow.c functions.

Invoke computeFirstAndFollowSets() (implemented in FirstAndFollow.c) to use the modules in other module
Also include the FirstAndFollow.h when doing so.

--------
