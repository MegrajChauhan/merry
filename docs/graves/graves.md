# Overview:

A detailed explaination will have to wait.

Graves only recognizes the interface and not the core. Graves uses
MerryCoreBase and MerryGravesCoreRepr to interface with any core.
Graves divides all cores into groups that have unique group IDs.
Once a core terminates, it will be ordered to clean itself and
Graves will help the core in the process. The core is responsible
for the initialization of its MerryCoreBase which will be used
by Graves to communicate with the core. 
