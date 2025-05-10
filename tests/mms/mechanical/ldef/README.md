# MMS


 boundary condition | element type | algorithm | order of convergence
--------------------|--------------|-----------|:----------------------:
 dirichlet | tet4 | struct | 2.0
 dirichlet | tet4 | hxt | 2.4
 dirichlet | tet10 | struct | 3.0
 dirichlet | tet10 | hxt | 0.9
 dirichlet | hex8 | struct | 2.0
 dirichlet | hex20 | struct | 3.0
 dirichlet | hex27 | struct | 3.0
    
![$L-2$ error for the large-deformation non-linear mechanical problem in FeenoX](mechanical-ldefcube-results.svg)



## **Method of Manufactured Solutions (MMS) for Large Deformation Problems**

To verify your FEM code for large deformations using the **Method of Manufactured Solutions (MMS)**, follow these steps:

* * *

### **1\. Choose a Displacement Field**

Define smooth, analytically known displacement functions ux(X,Y,Z)u\_x(X,Y,Z)ux​(X,Y,Z), uy(X,Y,Z)u\_y(X,Y,Z)uy​(X,Y,Z), and uz(X,Y,Z)u\_z(X,Y,Z)uz​(X,Y,Z) in the **reference configuration**.  
**Example (3D):**

ux\=Asin⁡(πXL),uy\=Bcos⁡(πYL),uz\=CXYZu\_x = A \\sin\\left(\\frac{\\pi X}{L}\\right), \\quad u\_y = B \\cos\\left(\\frac{\\pi Y}{L}\\right), \\quad u\_z = C XYZux​\=Asin(LπX​),uy​\=Bcos(LπY​),uz​\=CXYZ

where A,B,CA, B, CA,B,C are constants, and LLL is a characteristic length.

* * *

### **2\. Compute Deformation Gradient F\\mathbf{F}F**

F\=I+∇u,where∇u\=\[∂ux∂X∂ux∂Y∂ux∂Z∂uy∂X∂uy∂Y∂uy∂Z∂uz∂X∂uz∂Y∂uz∂Z\].\\mathbf{F} = \\mathbf{I} + \\nabla \\mathbf{u}, \\quad \\text{where} \\quad \\nabla \\mathbf{u} = \\begin{bmatrix} \\frac{\\partial u\_x}{\\partial X} & \\frac{\\partial u\_x}{\\partial Y} & \\frac{\\partial u\_x}{\\partial Z} \\\\ \\frac{\\partial u\_y}{\\partial X} & \\frac{\\partial u\_y}{\\partial Y} & \\frac{\\partial u\_y}{\\partial Z} \\\\ \\frac{\\partial u\_z}{\\partial X} & \\frac{\\partial u\_z}{\\partial Y} & \\frac{\\partial u\_z}{\\partial Z} \\end{bmatrix}.F\=I+∇u,where∇u\=​∂X∂ux​​∂X∂uy​​∂X∂uz​​​∂Y∂ux​​∂Y∂uy​​∂Y∂uz​​​∂Z∂ux​​∂Z∂uy​​∂Z∂uz​​​​.

**Example term:**

F11\=1+∂ux∂X\=1+AπLcos⁡(πXL).F\_{11} = 1 + \\frac{\\partial u\_x}{\\partial X} = 1 + \\frac{A \\pi}{L} \\cos\\left(\\frac{\\pi X}{L}\\right).F11​\=1+∂X∂ux​​\=1+LAπ​cos(LπX​).

* * *

### **3\. Compute Green-Lagrange Strain E\\mathbf{E}E**

E\=12(FTF−I).\\mathbf{E} = \\frac{1}{2} (\\mathbf{F}^T \\mathbf{F} - \\mathbf{I}).E\=21​(FTF−I).

**Component EXXE\_{XX}EXX​:**

EXX\=∂ux∂X+12\[(∂ux∂X)2+(∂uy∂X)2+(∂uz∂X)2\].E\_{XX} = \\frac{\\partial u\_x}{\\partial X} + \\frac{1}{2} \\left\[ \\left(\\frac{\\partial u\_x}{\\partial X}\\right)^2 + \\left(\\frac{\\partial u\_y}{\\partial X}\\right)^2 + \\left(\\frac{\\partial u\_z}{\\partial X}\\right)^2 \\right\].EXX​\=∂X∂ux​​+21​\[(∂X∂ux​​)2+(∂X∂uy​​)2+(∂X∂uz​​)2\].

* * *

### **4\. Compute Second Piola-Kirchhoff Stress S\\mathbf{S}S**

For a **linear elastic material** (Lamé parameters λ,μ\\lambda, \\muλ,μ):

S\=λ tr(E)I+2μE.\\mathbf{S} = \\lambda \\, \\text{tr}(\\mathbf{E}) \\mathbf{I} + 2 \\mu \\mathbf{E}.S\=λtr(E)I+2μE.

**Voigt notation (for 3D):**

\[SXXSYYSZZSXYSYZSZX\]\=\[λ+2μλλ000λλ+2μλ000λλλ+2μ000000μ000000μ000000μ\]\[EXXEYYEZZ2EXY2EYZ2EZX\].\\begin{bmatrix} S\_{XX} \\\\ S\_{YY} \\\\ S\_{ZZ} \\\\ S\_{XY} \\\\ S\_{YZ} \\\\ S\_{ZX} \\end{bmatrix} = \\begin{bmatrix} \\lambda + 2\\mu & \\lambda & \\lambda & 0 & 0 & 0 \\\\ \\lambda & \\lambda + 2\\mu & \\lambda & 0 & 0 & 0 \\\\ \\lambda & \\lambda & \\lambda + 2\\mu & 0 & 0 & 0 \\\\ 0 & 0 & 0 & \\mu & 0 & 0 \\\\ 0 & 0 & 0 & 0 & \\mu & 0 \\\\ 0 & 0 & 0 & 0 & 0 & \\mu \\end{bmatrix} \\begin{bmatrix} E\_{XX} \\\\ E\_{YY} \\\\ E\_{ZZ} \\\\ 2E\_{XY} \\\\ 2E\_{YZ} \\\\ 2E\_{ZX} \\end{bmatrix}.​SXX​SYY​SZZ​SXY​SYZ​SZX​​​\=​λ+2μλλ000​λλ+2μλ000​λλλ+2μ000​000μ00​0000μ0​00000μ​​​EXX​EYY​EZZ​2EXY​2EYZ​2EZX​​​.

* * *

### **5\. Compute Divergence of PK2 Stress (∇⋅S\\nabla \\cdot \\mathbf{S}∇⋅S)**

The equilibrium equation in the **reference configuration** is:

∇X⋅(FS)+b0\=0,\\nabla\_X \\cdot (\\mathbf{F} \\mathbf{S}) + \\mathbf{b}\_0 = \\mathbf{0},∇X​⋅(FS)+b0​\=0,

where b0\\mathbf{b}\_0b0​ is the **manufactured body force**.  
Here, ∇X\\nabla\_X∇X​ denotes derivatives w.r.t. reference coordinates (X,Y,Z)(X,Y,Z)(X,Y,Z).

#### **Step-by-Step Divergence Calculation:**

1.  Compute P\=FS\\mathbf{P} = \\mathbf{F} \\mathbf{S}P\=FS (First Piola-Kirchhoff stress).
    
2.  Take the divergence of P\\mathbf{P}P:
    
    (∇X⋅P)i\=∂PiX∂X+∂PiY∂Y+∂PiZ∂Z,i\=x,y,z.(\\nabla\_X \\cdot \\mathbf{P})\_i = \\frac{\\partial P\_{iX}}{\\partial X} + \\frac{\\partial P\_{iY}}{\\partial Y} + \\frac{\\partial P\_{iZ}}{\\partial Z}, \\quad i = x,y,z.(∇X​⋅P)i​\=∂X∂PiX​​+∂Y∂PiY​​+∂Z∂PiZ​​,i\=x,y,z.
3.  The manufactured body force is then:
    
    b0\=−∇X⋅(FS).\\mathbf{b}\_0 = -\\nabla\_X \\cdot (\\mathbf{F} \\mathbf{S}).b0​\=−∇X​⋅(FS).

**Example (2D):**

b0x\=−(∂(FxxSXX+FxySYX)∂X+∂(FxxSXY+FxySYY)∂Y).b\_{0x} = -\\left( \\frac{\\partial (F\_{xx} S\_{XX} + F\_{xy} S\_{YX})}{\\partial X} + \\frac{\\partial (F\_{xx} S\_{XY} + F\_{xy} S\_{YY})}{\\partial Y} \\right).b0x​\=−(∂X∂(Fxx​SXX​+Fxy​SYX​)​+∂Y∂(Fxx​SXY​+Fxy​SYY​)​).

* * *

### **6\. Verification Steps**

1.  **Apply u\\mathbf{u}u to your FEM code** and solve for stresses.
    
2.  **Compare numerical stresses** against analytical S\\mathbf{S}S.
    
3.  **Apply b0\\mathbf{b}\_0b0​ as a body force** and check if the solution matches u\\mathbf{u}u.
    

* * *

### **7\. Example (2D MMS Walkthrough)**

#### **Manufactured Displacement:**

ux\=Asin⁡(πXL),uy\=Bcos⁡(πYL).u\_x = A \\sin\\left(\\frac{\\pi X}{L}\\right), \\quad u\_y = B \\cos\\left(\\frac{\\pi Y}{L}\\right).ux​\=Asin(LπX​),uy​\=Bcos(LπY​).

#### **Deformation Gradient F\\mathbf{F}F:**

F\=\[1+AπLcos⁡(πXL)001−BπLsin⁡(πYL)\].\\mathbf{F} = \\begin{bmatrix} 1 + \\frac{A \\pi}{L} \\cos\\left(\\frac{\\pi X}{L}\\right) & 0 \\\\ 0 & 1 - \\frac{B \\pi}{L} \\sin\\left(\\frac{\\pi Y}{L}\\right) \\end{bmatrix}.F\=\[1+LAπ​cos(LπX​)0​01−LBπ​sin(LπY​)​\].

#### **Green-Lagrange Strain E\\mathbf{E}E:**

EXX\=AπLcos⁡(πXL)+12(AπLcos⁡(πXL))2.E\_{XX} = \\frac{A \\pi}{L} \\cos\\left(\\frac{\\pi X}{L}\\right) + \\frac{1}{2} \\left( \\frac{A \\pi}{L} \\cos\\left(\\frac{\\pi X}{L}\\right) \\right)^2.EXX​\=LAπ​cos(LπX​)+21​(LAπ​cos(LπX​))2.

#### **PK2 Stress S\\mathbf{S}S:**

SXX\=(λ+2μ)EXX+λEYY,SYY\=λEXX+(λ+2μ)EYY.S\_{XX} = (\\lambda + 2\\mu) E\_{XX} + \\lambda E\_{YY}, \\quad S\_{YY} = \\lambda E\_{XX} + (\\lambda + 2\\mu) E\_{YY}.SXX​\=(λ+2μ)EXX​+λEYY​,SYY​\=λEXX​+(λ+2μ)EYY​.

#### **Body Force b0\\mathbf{b}\_0b0​:**

b0x\=−∂∂X(FxxSXX)−∂∂Y(FxxSXY).b\_{0x} = -\\frac{\\partial}{\\partial X} \\left( F\_{xx} S\_{XX} \\right) - \\frac{\\partial}{\\partial Y} \\left( F\_{xx} S\_{XY} \\right).b0x​\=−∂X∂​(Fxx​SXX​)−∂Y∂​(Fxx​SXY​).

* * *

### **8\. Key Points**

-   **Consistency**: Ensure all operations (divergence, FS\\mathbf{F} \\mathbf{S}FS) are in the **reference configuration**.
    
-   **Material Model**: Use the same constitutive law in MMS and your FEM code.
    
-   **Code Check**: Compare numerical u\\mathbf{u}u with the manufactured solution under b0\\mathbf{b}\_0b0​.



