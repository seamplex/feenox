---
title: Input files and expressions
subtitle: FeenoX Tutorial #1
...

# Hello world!

To solve the canonical Hello World! problem with wasora, just prepare a text file like the one shown below and call it for example hello.was. Then run wasora with hello.was as its only argument as also shown below, and you are done. If this is the first wasora example you read, please do try to run it by yourself before proceeding with the other ones.

```
# this example greets the world
PRINT TEXT "hello world!"
```

$ wasora hello.was
hello world!    
$ 

The keyword TEXT is not strictly needed (unless given the IMPLICIT NONE keyword), as the construction hello world! does not resolve to any known symbol. However, it is a good practice to explicitly precede any text with the TEXT keyword.

# The Peano axiom

This example shows how the sum $1+1$ can be computed and printed to the standard output---i.e. the screen---in a variety of ways.

The PRINT keyword may take algebraic expressions, which are rendered to ASCII expressions using a scanf-type format specifier. The default format is %e (see the C library reference for scanf for details). The last format specifier given is used to render each expression. New PRINT keywords reset the format specifier to the default. Expressions may be mixed with text, as shown in the last line.


```
PRINT 1+1      # with default format
PRINT %g 1+1   # with numeric format

# by means of an intermediate variable
a = 1+1
PRINT %g a 

# with an informative text
PRINT "Peano says that one plus one equals" %.0f a
```

$ wasora peano.was
2
2
2
Peano says that one plus one equals 2
$ 

# Mathematical expressions

These examples show how algebraic expressions work in wasora, which is basically as they should. The basic operators are composed by the four basic operations +, -, * and / plus the exponentation operator ^. Parenthesis also work as expected, and can be nested up to any level. Other operators such as comparisons and logicals are introduced in other examples.


Expressions works as expected. If you find one case where they do not, please report it to the author of wasora.

```
# algebraic expressions do whatever one expects them to do
# thus a should evaluate to 7
a = 1 + 2*3
# and b should evaluate to 9
b = (1+2) * 3

# expressions can contain variables and functions, whose
# arguments may be expressions themselves, and so on
c = (1 + sin(pi/4)^(5/2))/(1 - log(abs(a-b)))

PRINT %g a b c

# when an expression appears as an argument of a keyword and
# contains spaces it should be written within double quotes
PRINT %g 1 -1 "1 -1"
```

Mind spaces! In an assignment after the `=` sign, one can go wild with spaces.
But when an expression is read as an argument to a keyword, the expression ought to constitute a single "token," that is to say, it should either

 * not contain any space, i.e. `1+1`
 * be quoted inside double qutoes `"1 + 1"`


$ wasora expr.was
7   9   4.62909
1   -1  0
$ 

By default PRINT introduces a tab between its arguments. This behavior can be changed with the SEPARATOR keyword.


When PRINT finds a new argument, it tries to evaluate it. If it does not make any sense, then wasora treats it as a text string. However, if a certain argument that constitutes a valid algebraic expression is to be treated as a text, it should be prefixed with the TEXT keyword, as the following example illustrates.

```
# actually arguments of the PRINT keyword may be either text
# strings, scalar expressions, vectors or matrices (more about
# these latter two issues in more complex examples)

# the example of Kernigham & Ritchie HOC code
PRINT TEXT "1+2*sin(0.7)" "=" %.6f 1+2*sin(0.7)
```

$ wasora hoc.was
1+2*sin(0.7)    =   2.288435
$ 

Note that the double quotes are not needed, as there are no spaces in 1+2*sin(0.7). However, it is a good practice to leave them for text strings, as one might want to add extra spaces in the future.


