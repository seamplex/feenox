## Verification {#sec:verification}

Verification, a.k.a. checking if the tool is solving right the equations, should be performed before applying the code to solve any industrial problem. Depending on the nature and regulation of the industry, the verification guidelines and requirements may vary. Since it is expected that code verification tasks could be performed by arbitrary individuals or organizations, the tool’s source code should be available to independent third parties. In this regard, changes in the source code should be controllable, traceable and well documented.
Stable releases ought to be digitally signed by a responsible engineer.

Even though the verification requirements may vary among problem types, industries and particular applications, a common method to verify the code is to compare solutions obtained with the tool with known exact solutions or benchmarks. It is thus mandatory to be able to compare the results with analytical solutions, either internally in the tool or through external libraries or tools.  Another possibility is to follow the Method of Manufactured Solutions, which is highly encouraged to be supported by the tool.


Whenever a verification task is performed and documented, at least one test should be added to the test suite. These tests should check that the verified features are kept by future changes and no regressions that break the verification are introduced. Verifications that the tool fails when it is expected to fail are encouraged as much as positive verifications that results are the expected ones.

