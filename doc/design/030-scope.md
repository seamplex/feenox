## Scope {#sec:scope}

The tool should allow advanced users to define the problem to be solved programmatically, using one or more files either...
 
 a. specifically formatted for the tool to read such as JSON or a particular input format (historically called input decks in punched-card days), and/or 
 b. written in an high-level interpreted language such as Python or Julia.
  
It should be noted that a graphical user interface is not required. The tool may include one, but it should be able to run without needing any user intervention rather than the preparation of a set of input files.
Nevertheless, there tool might _allow_ a GUI to be used. For example, for a basic usage involving simple cases, a user interface engine should be able to create these problem-definition files in order to give access to less advanced users to the tool using a desktop, mobile and/or web-based interface in order to run the actual tool without further intervention.

However, for general usage, users should be able to completely define the problem (or set of problems, i.e. a parametric study) they want to solve in one or more input files and to obtain one or more output files containing the desired results, either a set of scalar outputs (such as maximum stresses or mean temperatures), and/or a time and/or spatial distribution. If needed, a discretization of the domain may to be taken as a know input, i.e. the tool is not required to create the mesh as long as a suitable mesher can be employed using a similar workflow specified in this\ SRS.


The tool should define and document ([@sec:documentation]) the way the input files for a solving particular problem are to be prepared ([@sec:input]) and how the results are to be written ([@sec:output]).
Any GUI, pre-processor, post-processor or other related graphical tool used to provide a graphical interface for the user should integrate in the workflow described in the preceding paragraph: a pre-processor should create  the input files needed for the tool and a post-processor should read the output files created by the tool.
