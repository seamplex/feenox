---
title: FeenoX Software Design Specification
lang: en-US
abstract: This design specification addresses the (hypothetical) Software Requirement Specification for developing a piece of free and open source engineering software with certain specifications, defined in an imaginary tender.
number-sections: true

fontsize: 11pt
geometry:
- paper=a4paper
- left=2.5cm
- right=2cm
- bottom=3.5cm
- foot=2cm
- top=3.5cm
- head=2cm
colorlinks: true
mathspec: true
syntax-definition: feenox.xml
listings: true
toc: true
...

# Introduction {#sec:introduction}

> 
> A cloud-based computational tool (herein after referred to as _the tool_) is required in order to solve engineering problems following the current state-of-the-art methods and technologies impacting the high-performance computing world.
> This (imaginary but plausible) Software Requirements Specification document describes the mandatory features this tool ought to have and lists some features which would be nice the tool had.
> Also it contains requirements and guidelines about architecture, execution and interfaces in order to fulfill the needs of cognizant engineers as of 2021 (and the years to come) are defined. 
> 
> On the one hand, the tool should be applicable to solving industrial problems under stringent efficiency ([@sec:efficiency]) and quality ([@sec:qa]) requirements. It is therefore mandatory to be able to assess the source code for potential performance and verification revision by qualified third parties from all around the world, so it has to be _open source_. On the other hand, the initial version of the tool is expected to be a basic framework which might be extended ([@sec:objective] and [@sec:extensibility]) by academic researchers and programmers. It thus should also be _free_---in the sense of freedom, not in the sense of price. There is no requirement on the pricing scheme.
> The detailed licensing terms are left to the offer but it should allow users to solve their problems the way they need and, eventually, to modify and improve the tool to suit their needs. If they cannot program themselves, they should have the freedom to hire somebody to do it for them.



Besides noting that software being _free_ (regarding freedom, not price) does not imply the same as being _open source_, the requirement is clear in that the tool has to be both _free_ and _open source_, a combination which is usually called [FOSS](https://en.wikipedia.org/wiki/Free_and_open-source_software). This condition leaves all of the well-known non-free finite-element solvers in the market out of the tender.

FeenoX is licensed under the terms of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0) version\ 3 or, at the user convenience, any later version. This means that users get the four essential freedoms:

 0. The freedom to _run_ the program as they wish, for _any_ purpose.
 1. The freedom to _study_ how the program works, and _change_ it so it does their computing as they wish.
 2. The freedom to _redistribute_ copies so they can help others.
 3. The freedom to _distribute_ copies of their _modified_ versions to others.

There are some examples of pieces of computational software which are described as “open source” in which even the first of the four freedoms is denied. The most iconic case is that of Android, whose sources are readily available online but there is no straightforward way of updating one’s mobile phone firmware with a customized version, not to mention vendor and hardware lock ins and the possibility of bricking devices if something unexpected happens. In the nuclear industry, it is the case of a Monte Carlo particle-transport program that requests users to sign an agreement about the objective of its usage before allowing its execution. The software itself might be open source because the source code is provided after signing the agreement, but it is not free (as in freedom) at all.
 
So a free program has to be open source, but it also has to explicitly provide the four freedoms above both through the written license and through the mechanisms available to get, modify, compile, run and document these modifications. 
That is why licensing FeenoX as GPLv3+ also implies that the source code and all the scripts and makefiles needed to compile and run it are available for anyone that requires it. Anyone wanting to modify the program either to fix bugs, improve it or add new features is free to do so. And if they do not know how to program, the have the freedom to hire a programmer to do it without needing to ask permission to the original authors. 

Nevertheless, since these original authors are the copyright holders, they still can use it to either enforce or prevent further actions from the users that receive FeenoX under the GPLv3+. In particular, the license allows re-distribution of modified versions only if they are clearly marked as different from the original and only under the same terms of the GPLv3+. There are also some other subtle technicalities that need not be discussed here such as what constitutes a modified version (which cannot be redistributed under a different license) and what is an aggregate (in which each part be distributed under different licenses) and about usage over a network and the possibility of using [AGPL](https://en.wikipedia.org/wiki/GNU_Affero_General_Public_License) instead of GPL to further enforce freedom (TLDR; it does not matter for FeenoX), but which are already taken into account in FeenoX licensing scheme.


## Objective {#sec:objective}

> 
> The main objective of the tool is to be able to solve engineering problems which are usually casted as differential-algebraic equations (DAEs) or partial differential equations (PDEs), such as
> 
>  * heat conduction
>  * mechanical elasticity
>  * structural modal analysis
>  * frequency studies
>  * electromagnetism
>  * chemical diffusion
>  * process control dynamics
>  * computational fluid dynamics
>  * ...
> 
> \noindent
> on one or more manistream cloud servers, i.e. computers with an architecture (i.e. hardware and operating systems, futher discussed in [@sec:architecture]) that allows them to be available online and accessed remotely either interactively or automatically by other computers as well. Other architectures such as high-end desktop personal computers or even low-end laptops might be supported but they should not the main target. 
>  
> The initial version of the tool must be able to handle a subset of the above list of problem types.
> Afterward, the set of supported problem types, models, equations and features of the tool should grow to include other models as well, as required in\ [@sec:extensibility].


The choice of the initial supported features is based on the types of problem that the FeenoX' precursor codes (namely wasora, Fino and milonga) already have been supporting since more than ten years now. It is also a first choice so work can be bounded and a subsequent road map and release plans can be designed. FeenoX' first version includes a subset of the required functionality, namely

 * dynamical systems
 * Laplace/Poisson/Helmholtz equations
 * plant control dynamics
 * heat conduction
 * mechanical elasticity
 * structural modal analysis
 * multigroup neutron transport and diffusion
 
FeenoX is designed to be developed and executed under GNU/Linux, which is the architecture of more than 95% of the internet servers which we collectively call “the cloud.” It should be noted that GNU/Linux is a POSIX-compliant version of UNIX and that FeenoX follows the rules of UNIX philosophy ([@sec:unix]) regarding its computational implementation code. Besides POSIX, FeenoX also uses MPI which is a well-known industry standard for massive parallel executions of processes, both in multi-core hosts and multi-hosts environments. Finally, if performance and/or scalability are not important issues, FeenoX can be run in a (properly cooled) local PC or laptop.

The requirement to run in the cloud and scale up as needed rules out some of the FOSS solvers available online, such as CalculiX.


## Scope {#sec:scope}

> 
> The tool should allow advanced users to define the problem to be solved programmatically. That is to say, the problem should be completely defined using one or more files either...
>  
>  a. specifically formatted for the tool to read such as JSON or a particular input format (historically called input decks in punched-card days), and/or 
>  b. written in an high-level interpreted language such as Python or Julia.
>   
> It should be noted that a graphical user interface is not required. The tool may include one, but it should be able to run without needing any user intervention rather than the preparation of a set of input files.
> Nevertheless, there tool might _allow_ a GUI to be used. For example, for a basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool using a desktop, mobile and/or web-based interface in order to run the actual tool without further intervention.
> 
> However, for general usage, users should be able to completely define the problem (or set of problems, i.e. a parametric study) they want to solve in one or more input files and to obtain one or more output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a time and/or spatial distribution. If needed, a discretization of the domain may to be taken as a known input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be employed using a similar workflow specified in this\ SRS.
> 
> 
> The tool should define and document ([@sec:documentation]) the way the input files for a solving particular problem are to be prepared ([@sec:input]) and how the results are to be written ([@sec:output]).
> Any GUI, pre-processor, post-processor or other related graphical tool used to provide a graphical interface for the user should integrate in the workflow described in the preceding paragraph: a pre-processor should create  the input files needed for the tool and a post-processor should read the output files created by the tool.


Indeed, FeenoX is designed to work very much like a transfer function between two (or more) files---usually the FeenoX input file and the problem mesh file---and zero or more output files---usually the terminal output and a VTK post-processing file---as illustrated in [@fig:transfer].

![FeenoX working as a transfer function between input and output files](transfer.svg){#fig:transfer width=50%}

In some particular cases, FeenoX can also provide an API for high-level interpreted languages such as Python or Julia such that a problem can be completely defined in a script, increasing also flexibility.


As already stated, FeenoX is designed and implemented following the UNIX philosophy in general and Eric Raymond's 17 Unix Rules ([sec:unix]) in particular. One of the main ideas is the rule of _separation_ that essentially implies to separate mechanism from policy, that in the computational engineering world translates into separating the frontend from the backend. Even though most FEA programs eventually separate the interface from the solver up to some degree, there are cases in which they are still dependent such that changing the former needs updating the latter.

From the very beginning, FeenoX is designed as a pure backend which should nevertheless provide appropriate mechanisms for different frontends to be able to communicate and to provide a friendly interface for the final user. Yet, the separation is complete in the sense that the nature of the frontends can radically change (say from a desktop-based point-and-click program to a web-based immersive augmented-reality application) without needing the modify the backend. Not only far more flexibility is given by following this path, but also develop efficiency and quality is encouraged since programmers working on the lower-level of an engineering tool usually do not have the skills needed to write good user-experience interfaces, and conversely.

In the very same sense, FeenoX does not discretize continuous domains for PDE problems itself, but relies on separate tools for this end. Fortunately, there already exists one meshing tool which is FOSS (GPLv2) and shares most (if not all) of the design basis principles with FeenoX: the three-dimensional finite element mesh generator [Gmsh](http://gmsh.info/).
Strictly speaking, FeenoX does not need to be used along with Gmsh but with any other mesher able to write meshes in Gmsh's format 
