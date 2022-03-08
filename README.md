# bmomp
Bare Metal OpenMP:

At the moment this is a very rough development project, with a lot of experimentation going on.

THe objecetive is to get OpenMP working on a bare metal microprocessor. The initial experimentation
is being done on a Cortex-M7. OpenMP may not make a lot of sense on a single-core CPU, but with an
underlying thread switching mechanism, it is a useful platform for initial development. Later efforts
will use a NXP quad-core Cortex-A9.
