# Frequently asked questions about Feenox

## Why nothing happens when I double click on `feenox.exe`?



## Why is FeenoX written in C and not in...

### C++?

<https://www.quora.com/Why-is-C-still-in-use-even-though-we-have-C++-Is-there-anything-that-C-can-do-but-C++-cant-or-maybe-something-that-is-easier-to-do-in-C-rather-than-C++>

> **Why is PETSc programmed in C, instead of Fortran or C++**
>
> C enables us to build data structures for storing sparse matrices, solver information, etc. in ways that Fortran simply does not allow. ANSI C is a complete standard that all modern C compilers support. The language is identical on all machines. C++ is still evolving and compilers on different machines are not identical. Using C function pointers to provide data encapsulation and polymorphism allows us to get many of the advantages of C++ without using such a large and more complicated language. It would be natural and reasonable to have coded PETSc in C++; we opted to use C instead. 
>
> <https://www.mcs.anl.gov/petsc/documentation/faq.html#why-c>



> **Why Git is written in C and not in C++, by Linus Torvalds**
>
> C++ is a horrible language. It's made more horrible by the fact that a lot 
> of substandard programmers use it, to the point where it's much much 
> easier to generate total and utter crap with it. Quite frankly, even if 
> the choice of C were to do *nothing* but keep the C++ programmers out, 
> that in itself would be a huge reason to use C.
>
> <http://harmful.cat-v.org/software/c++/linus>


### Fortran?

Because I am not insane (yet).

### Python?

If you actually asked this then FeenoX is not for you.

### Go, Python or Julia?

I don't know either of them nor if any of these languages would be a good fit for FeenoX.
Keep in mind that it took me a while to know why not Fortran nor C++ even though there are people that would choose them over C.
Maybe something of the sort happens with these new ideas (or not, I don't know).

