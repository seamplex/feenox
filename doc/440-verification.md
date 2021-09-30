

Verification, defined as

>  The process of determining that a model implementation accurately represents the developer’s conceptual description of the model and the solution to the model.

i.e. checking if the tool is solving right the equations, should be performed before applying the code to solve any industrial problem. Depending on the nature and regulation of the industry, the verification guidelines and requirements may vary. Since it is expected that code verification tasks could be performed by arbitrary individuals or organizations not necessarily affiliated with the tool vendor, the source code should be available to independent third parties. In this regard, changes in the source code should be controllable, traceable and well documented.

Even though the verification requirements may vary among problem types, industries and particular applications, a common method to verify the code is to compare solutions obtained with the tool with known exact solutions or benchmarks. It is thus mandatory to be able to compare the results with analytical solutions, either internally in the tool or through external libraries or tools. This approach is called the Method of Exact Solutions and it is the most widespread scheme for verifying computational software, although it does not provide a comprehensive method to verify the whole spectrum of features. In any case, the tool's output should be susceptible of being post-processed and analysed in such a way to be able to determine the order of convergence of the numerical solution as compared to the exact one.

Another possibility is to follow the Method of Manufactured Solutions, which does address all the shortcomings of MES. It is highly encouraged that the tool allows the application of MMS for software verification. Indeed, this method needs a full explanation of the equations solved by the tool, up to the point that [@sandia-mms] says that

> Difficulties in determination of the governing equations arises frequently with
> commercial software, where some information is regarded as proprietary.
> If the governing equations cannot be determined, we would question the validity of using the code.

To enforce the availability of the governing equations, the tool has to be open source as required in @sec:introduction and well documented as required in @sec:documentation.


A report following either the MES and/or MMS procedures has to be prepared for each type of equation that the tool can solve. The report should show how the numerical results converge to the exact or manufactured results with respect to the mesh size or number of degrees of freedom. This rate should then be compared to the theoretical expected order.

Whenever a verification task is performed and documented, at least one of the cases should be added to the test suite. Even though the verification report must contain a parametric mesh study, a single-mesh case is enough to be added to the test suite. The objective of the tests defined in [@sec:testing] is to be able to detect regressions which might have been inadvertently introduced in the code and not to do any actual verification.
Therefore a single-mesh case is enough for the test suites.

