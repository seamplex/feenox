## Scope {#sec:scope}

The tool should allow advanced users to define the problem to be solved programmatically, using one or more files either...
 
 a. specifically formatted for the tool to read such as JSON or a particular input format (historically called input decks in punched-card days), and/or 
 b. written in an high-level interpreted language such as Python or Julia.
  
For a basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool using a desktop, mobile and/or web GUI.

For general usage, users should be able to completely define the problem (or set of problems, i.e. a parametric study) they want to solve in one or more input files and to obtain one or more output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a time and/or spatial distribution. If needed, a discretization of the domain may to be taken as a know input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be employed using a similar workflow specified in this\ SRS.


The tool should define and document ([@sec:documentation]) the way the input files for a solving particular problem are to be prepared ([@sec:input]) and how the results are to be written ([@sec:output]).
Any GUI, pre-processor, post-processor or other related graphical tool used to provide a graphical interface for the user should integrate in the workflow described in the preceeding paragraph: a pre-processor should create  the input files needed for the tool and a post-processor should read the output files created by the tool.
