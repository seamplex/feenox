FeenoX is licensed under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0) version\ 3 or, at the user convenience, any later version. This means that users get the four essential freedoms:^[
There are some examples of pieces of computational software which are described as “open source” in which even the first of the four freedoms is denied. The most iconic case is that of Android, whose sources are readily available online but there is no straightforward way of updating one’s mobile phone firmware with a customized version, not to mention vendor and hardware lock ins and the possibility of bricking devices if something unexpected happens. In the nuclear industry, it is the case of a Monte Carlo particle-transport program that requests users to sign an agreement about the objective of its usage before allowing its execution. The software itself might be open source because the source code is provided after signing the agreement, but it is not free (as in freedom) at all.]

 0. The freedom to _run_ the program as they wish, for _any_ purpose.
 1. The freedom to _study_ how the program works, and _change_ it so it does their computing as they wish.
 2. The freedom to _redistribute_ copies so they can help others.
 3. The freedom to _distribute_ copies of their _modified_ versions to others.

 
So a free program has to be open source, but it also has to explicitly provide the four freedoms above both through the written license and through appropriate mechanisms to get, modify, compile, run and document these modifications using well-established and/or reasonable straightforward procedures. 
That is why licensing FeenoX as GPLv3+ also implies that the source code and all the scripts and makefiles needed to compile and run it are available for anyone that requires it (i.e. it is compiled with `./configure && make`).
Anyone wanting to modify the program either to fix bugs, improve it or add new features is free to do so. And if they do not know how to program, the have the freedom to hire a programmer to do it without needing to ask permission to the original authors.
Even more, [the documentation](https://seamplex.com/feenox/doc/) is released under the terms of the [GNU Free Documentation License](https://www.gnu.org/licenses/fdl-1.3.html) so these new (or modified) features can be properly documented as well.

Nevertheless, since these original authors are the copyright holders, they still can use it to either enforce or prevent further actions from the users that receive FeenoX under the GPLv3+.
In particular, the license allows re-distribution of modified versions only if

 a. they are clearly marked as different from the original, and
 b. they are distributed under the same terms of the GPLv3+.
 
There are also some other subtle technicalities that need not be discussed here such as

 * what constitutes a modified version (which cannot be redistributed under a different license)
 * what is an aggregate (in which each part be distributed under different licenses)
 * usage over a network and the possibility of using [AGPL](https://en.wikipedia.org/wiki/GNU_Affero_General_Public_License) instead of GPL to further enforce freedom 
 
These issues are already taken into account in the FeenoX licensing scheme.

It should be noted that not only is FeenoX free and open source, but also all of the libraries it depends on (and their dependencies) also are.
It can also be compiled using free and open source build tool chains running over free and open source operating systems.
