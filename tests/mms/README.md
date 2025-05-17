# Verification with the Method of Manufactured Solution


Verification of problems in FeenoX:

 * [Heat conduction `thermal`](thermal)
   - [2D](thermal/2d)
   - [3D](thermal/3d)
 * [Elasticity `mechanical`](mecahnical)
   - [3D small deformation](mechanical/sdef)
   - [3D large deformation](mechanical/ldef)

> [!NOTE]
> Check out this video for an explanation about MMS: <https://youtu.be/mGNwL8TGijg>

## Summary of the MMS

 * **Traditional verification**: choose a problem with known solution and check if your solver matches it.

 * **Method of manufactured solutions**: choose a solution, work out which source terms and BCs you need so your problem has the solution you chose as the solution and check it your solver matches it.

> [!TIP]
> **Bonus point**: show that the order of convergence matches the theoretical order $O(n)$, i.e. a log-log plot of the $L_2$ norm vs. the element size $h$ should be a straight line with a slope equal to the order $n$.


