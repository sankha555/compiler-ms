[Y] An identifier cannot be declared multiple times in the same scope

[Y] An identifier must be declared before its use

[Y] An identifier declared globally cannot be declared anywhere else in function definitions

[Y] The types and the number of parameters returned by a function must be the same as that of the parameters used in invoking the function.

[Y] The parameter passed of union data type is reported as error

[] The parameter of variant record type (for variant field only) is type checked at run type as described above.

[] A variant record passed as a parameter must have a discriminator named as tagvalue

[Y] The parameters being returned by a function must be assigned a value. If a parameter does not get a value assigned within the function definition, it should be reported as an error

[Y] The function that does not retum any value, must be invoked appropriately

[Y] The types of formal and actual parameters must be same. Number of actual parameters must be same as that of formal parameters

[Y] Function overloading is not allowed

[Y] The function cannot be invoked recursively

[Y] An identifier used beyond its scope must be viewed as undefined

[Y] A record type definition is visible anywhere in the program.

[Y] The right hand side expression of an assignment statement must be of the same type as that of the left hand side identifier

[N] A function definition for a function being used (say F1) by another (say F2) must precede the definition of the function using it(e. F2)

[Y] A while statement must redefine the variable that participates in the iterations.