# Feenox Dockerfiles

This directory contains dockerfiles used to build Feenox docker images.
For now, the only Dockerfile available is based on the official Ubuntu24.04 image for x86 architectures.

To build it, ensure Docker is available on your system, then navigate to the `dockerfiles/ubuntu24.04` directory and run:

```
docker build --tag feenox .
```

To run Feenox using docker, navigate to your project directory and run:

```
docker run -v .:/work feenox input.fee
```

Remember to change the input file name accordingly.
