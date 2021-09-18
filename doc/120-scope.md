## Scope {#sec:scope}

The tool should allow users to define the problem to be solved programmatically. That is to say, the problem should be completely defined using one or more files either...
 
 a. specifically formatted for the tool to read such as JSON or a particular input format (historically called input decks in punched-card days), and/or 
 b. written in an high-level interpreted language such as Python or Julia.
  
It should be noted that an } graphical user interface is _not_ required. The tool may include one, but it should be able to run without needing any interactive user intervention rather than the preparation of a set of input files.
Nevertheless, the tool might _allow_ a GUI to be used. For example, for a basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool using a desktop, mobile and/or web-based interface in order to run the actual tool without needing to manually prepare the actual input files.

However, for general usage, users should be able to completely define the problem (or set of problems, i.e. a parametric study) they want to solve in one or more input files and to obtain one or more output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a detailed time and/or spatial distribution. If needed, a discretization of the domain may to be taken as a known input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be employed using a similar workflow as the one specified in this\ SRS.


The tool should define and document ([@sec:documentation]) the way the input files for a solving particular problem are to be prepared ([@sec:input]) and how the results are to be written ([@sec:output]).
Any GUI, pre-processor, post-processor or other related graphical tool used to provide a graphical interface for the user should integrate in the workflow described in the preceding paragraph: a pre-processor should create  the input files needed for the tool and a post-processor should read the output files created by the tool.
