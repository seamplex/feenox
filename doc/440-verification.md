

Verification, defined as

>  The process of determining that a model implementation accurately represents the developer’s conceptual description of the model and the solution to the model.

a.k.a. checking if the tool is solving right the equations, should be performed before applying the code to solve any industrial problem. Depending on the nature and regulation of the industry, the verification guidelines and requirements may vary. Since it is expected that code verification tasks could be performed by arbitrary individuals or organizations, the tool’s source code should be available to independent third parties. In this regard, changes in the source code should be controllable, traceable and well documented.
Stable releases ought to be digitally signed by a responsible engineer.

Even though the verification requirements may vary among problem types, industries and particular applications, a common method to verify the code is to compare solutions obtained with the tool with known exact solutions or benchmarks. It is thus mandatory to be able to compare the results with analytical solutions, either internally in the tool or through external libraries or tools. This approach is called the Method of Exact solutions and it is the most widespread scheme for verifying computational software, although it does not provide a comprehensive method to verify the whole spectrum of features. Another possibility is to follow the Method of Manufactured Solutions, which does. It is highly encouraged that the tool allows the application of MMS for software verification. Indeed, this method needs a full explanation of the equations solved by the tool, up to the point that [@sandia-mms] says that

> Difficulties in determination of the governing equations arises frequently with
> commercial software, where some information is regarded as proprietary.
> If the governing equations cannot be determined, we would question the validity of using the code.

To enforce the availability of the governing equations, the tool has to be open source as required in @sec:introduction and well documented as required in @sec:documentation.



Whenever a verification task is performed and documented, at least one test should be added to the test suite. For MES or MMS, only one mesh refinement is enough, there is no need to add to whole test for different mesh sizes. The tests in the tests suite should check that the verified features are kept by future changes and no regressions that break the verification are introduced. Verifications that the tool fails when it is expected to fail are encouraged as much as positive verifications that results are the expected ones.

**TO BE COMPLETED**
