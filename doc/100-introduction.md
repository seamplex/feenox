
A computational tool (herein after referred to as _the tool_) specifically designed to be executed in arbitrarily-scalable remote servers (i.e. in the cloud) is required in order to solve engineering problems following the current state-of-the-art methods and technologies impacting the high-performance computing world.
This (imaginary but plausible) Software Requirements Specification document describes the mandatory features this tool ought to have and lists some features which would be nice the tool had.
Also it contains requirements and guidelines about architecture, execution and interfaces in order to fulfill the needs of cognizant engineers as of the 2020s. In particular, it should be designed and implemented in such a way to explicitly allow and encourage LLMs to be able to provide assistance with the problem set up, model debugging and analysis of results.

On the one hand, the tool should allow to solve industrial problems under stringent efficiency (@sec:efficiency) and quality (@sec:qa) requirements. It is therefore mandatory to be able to assess the source code for

 * independent verification, and/or
 * performance profiling, and/or
 * quality control

by qualified third parties from all around the world.
Hence, it has to be _open source_ according to the definition of the Open Source Initiative.

On the other hand, the initial version of the tool is expected to provide a basic functionality which might be extended (@sec:objective and @sec:extensibility) by academic researchers and/or professional programmers. It thus should also be _free_---in the sense of freedom, not in the sense of price---as defined by the Free Software Foundation. There is no requirement on the pricing scheme, which is up to the vendor to define in the offer along with the detailed licensing terms. These should allow users to solve their problems the way they need and, eventually, to modify and improve the tool to suit their needs. If they cannot program themselves, they should have the _freedom_ to hire somebody to do it for them.
