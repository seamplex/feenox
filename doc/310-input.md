

The problem should be completely defined by one or more input files.
These input files might be

 a. particularly formatted files to be read by the tool in an _ad-hoc_ way, and/or
 b. source files for interpreted languages which can call the tool through and API or equivalent method, and/or
 c. any other method that can fulfill the requirements described so far.

Preferably, these input files should be plain ASCII files in order to allow to manage changes using distributed version control systems such as Git. If the tool provides an API for an interpreted language such as Python, then the Python source used to solve a particular problem should be Git-friendly. It is recommended not to track revisions of mesh data files but of the source input files, i.e. to track the mesher’s input and not the mesher’s output. Therefore, it is recommended not to mix the problem definition with the problem mesh data.

It is not mandatory to include a GUI in the main distribution, but the input/output scheme should be such that graphical pre and post-processing tools can create the input files and read the output files so as to allow third parties to develop interfaces.  It is recommended to design the workflow as to make it possible for the interfaces to be accessible from mobile devices and web browsers.

It is expected that 80% of the problems need 20% of the functionality.
It is acceptable if only basic usage can be achieved through the usage of graphical interfaces to ease basic usage at first. Complex problems involving non-trivial material properties and boundary conditions not be treated by a GUI and only available by needing access to the input files.
