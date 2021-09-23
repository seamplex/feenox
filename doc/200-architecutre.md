

The tool must be aimed at being executed unattended on remote cloud servers which are expected to have a mainstream^[as of 2021] architecture regarding operating system (GNU/Linux variants and other UNIX-like OSes) and hardware stack (a few intel-compatible CPUs per host, a few levels of memory caches, a few gigabyes of random-access memory, several gigabytes of disk storage, etc.). It should successfully run on bare-metal, virtual and/or containerized servers using standard compilers, dependencies and libraries already available in the repositories of most current operating systems distributions.

Preference should be given to open source compilers, dependencies and libraries. Small problems might be executed in a single host but large problems ought to be split through several server instances depending on the processing and memory requirements. The computational implementation should adhere to open and well-established standards.

Ability to run on local desktop personal computers and/laptops is not required but suggested as a mean of giving the opportunity to users to test and debug small coarse computational models before launching the large computation on a HPC cluster or on a set of scalable cloud instances. Support for non-GNU/Linux operating systems is not required but also suggested.

Mobile platforms such as tablets and phones are not suitable to run engineering simulations due to their lack of proper electronic cooling mechanisms. They are suggested to be used to control one (or more) instances of the tool running on the cloud, and even to pre and post process results through mobile and/or web interfaces.
