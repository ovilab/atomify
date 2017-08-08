
Several moltemplate examples make use of pair styles or fixes
which have not yet been submitted to the LAMMPS code base, like
    pair_style lj/charmm/coul/charmm/inter
When you run the moltemplate examples, LAMMPS may complain
about invalid pair_styles or other invalid styles.
If so, then copy the code in this directory into the "src" 
subdirectory of your LAMMPS installation directory and recompile LAMMPS.

Explanation:

The new  pair_styles contained here are usually slightly modified versions 
of existing LAMMPS pair_styles which have some additional features.
(I added mixing rules for purely repulsive Lennard-Jones particles. This makes
 it easier to create systems with a large number of different particle types.
 Moltemplate helps users create complex systems by making it easier for them to
 mix together a potentially very large number of atom types & molecule types.
 Unfortunately, as of 2014/7, ordinary LAMMPS does not handle purely 
 repulsive particles very well when many particle types are present.)

(In the past I sometimes included some additional code may appear here 
 containing bug-fixes for existing LAMMPS features which the LAMMPS developers
 have not had time to review and add to the main code base.
 When that code does get added to the main LAMMPS distribution, I remove 
 it from this directory.  Be sure to download a recent version of LAMMPS.)
