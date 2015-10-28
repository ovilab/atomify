/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "highlighter.h"

// List here: https://github.com/lammps/lammps/blob/master/tools/vim/lammps.vim
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;


    QTextCharFormat format;
    QStringList patterns;

    auto appendRule = [&](const QStringList &patterns, const QTextCharFormat &format) {
        foreach (const QString &pattern, patterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = format;
            highlightingRules.append(rule);
        }
    };

    auto createKeywordRules = [&](QStringList &keywords) {
        for(QString &pattern : keywords) {
            pattern = "\\b"+pattern+"\\b";
        }
    };

    // Individual commands
    patterns.clear();
    patterns << "angle_coeff" << "angle_style" << "atom_modify" << "atom_style" << "balance" << "bond_coeff" << "bond_style" << "boundary" << "box" << "change_box" << "clear" << "comm_modify" << "comm_style" << "compute" << "compute_modify" << "create_atoms" << "create_bonds" << "create_box" << "delete_atoms" << "delete_bonds" << "dielectric" << "dihedral_coeff" << "dihedral_style" << "dimension" << "displace_atoms" << "dump" << "dump image" << "dump_modify" << "dump movie" << "echo" << "fix" << "fix_modify" << "group" << "if" << "info" << "improper_coeff" << "improper_style" << "include" << "jump" << "kspace_modify" << "kspace_style" << "label" << "lattice" << "log" << "mass" << "minimize" << "min_modify" << "min_style" << "molecule" << "neb" << "neigh_modify" << "neighbor" << "newton" << "next" << "package" << "pair_coeff" << "pair_modify" << "pair_style" << "pair_write" << "partition" << "prd" << "print" << "processors" << "python" << "quit" << "read_data" << "read_dump" << "read_restart" << "region" << "replicate" << "rerun" << "reset_timestep" << "restart" << "run" << "run_style" << "set" << "shell" << "special_bonds" << "suffix" << "tad" << "temper" << "thermo" << "thermo_modify" << "thermo_style" << "timer" << "timestep" << "uncompute" << "undump" << "unfix" << "units" << "variable" << "velocity" << "write_data" << "write_dump" << "write_restart";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Fix styles
    patterns.clear();
    patterns << "adapt" << "addforce (c)" << "append/atoms" << "atom/swap" << "aveforce (c)" << "ave/atom" << "ave/chunk" << "ave/correlate" << "ave/histo" << "ave/histo/weight" << "ave/spatial" << "ave/time" << "balance" << "bond/break" << "bond/create" << "bond/swap" << "box/relax" << "deform (k)" << "deposit" << "drag" << "dt/reset" << "efield" << "enforce2d (c)" << "evaporate" << "external" << "freeze (c)" << "gcmc" << "gld" << "gravity (co)" << "heat" << "indent" << "langevin (k)" << "lineforce" << "momentum" << "move" << "msst" << "neb" << "nph (ko)" << "nphug (o)" << "nph/asphere (o)" << "nph/sphere (o)" << "npt (cko)" << "npt/asphere (o)" << "npt/sphere (o)" << "nve (cko)" << "nve/asphere" << "nve/asphere/noforce" << "nve/body" << "nve/limit" << "nve/line" << "nve/noforce" << "nve/sphere (o)" << "nve/tri" << "nvt (cko)" << "nvt/asphere (o)" << "nvt/sllod (o)" << "nvt/sphere (o)" << "oneway" << "orient/fcc" << "planeforce" << "poems" << "pour" << "press/berendsen" << "print" << "property/atom" << "qeq/comb (o)" << "qeq/dynamic" << "qeq/point" << "qeq/shielded" << "qeq/slater" << "reax/bonds" << "recenter" << "restrain" << "rigid (o)" << "rigid/nph (o)" << "rigid/npt (o)" << "rigid/nve (o)" << "rigid/nvt (o)" << "rigid/small (o)" << "rigid/small/nph" << "rigid/small/npt" << "rigid/small/nve" << "rigid/small/nvt" << "setforce (c)" << "shake (c)" << "spring" << "spring/rg" << "spring/self" << "srd" << "store/force" << "store/state" << "temp/berendsen (c)" << "temp/csld" << "temp/csvr" << "temp/rescale (c)" << "tfmc" << "thermal/conductivity" << "tmd" << "ttm" << "tune/kspace" << "vector" << "viscosity" << "viscous (c)" << "wall/colloid" << "wall/gran" << "wall/harmonic" << "wall/lj1043" << "wall/lj126" << "wall/lj93" << "wall/piston" << "wall/reflect (k)" << "wall/region" << "wall/srd";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Additional fix styles
    patterns.clear();
    patterns << "adapt/fep" << "addtorque" << "atc" << "ave/spatial/sphere" << "drude" << "drude/transform/direct" << "drude/transform/reverse" << "colvars" << "gle" << "imd" << "ipi" << "langevin/drude" << "langevin/eff" << "lb/fluid" << "lb/momentum" << "lb/pc" << "lb/rigid/pc/sphere" << "lb/viscous" << "meso" << "meso/stationary" << "nph/eff" << "npt/eff" << "nve/eff" << "nvt/eff" << "nvt/sllod/eff" << "phonon" << "pimd" << "qbmsst" << "qeq/reax" << "qmmm" << "qtb" << "reax/c/bonds" << "reax/c/species" << "saed/vtk" << "smd" << "smd/adjust/dt" << "smd/integrate/tlsph" << "smd/integrate/ulsph" << "smd/move/triangulated/surface" << "smd/setvel" << "smd/tlsph/reference/configuration" << "smd/wall/surface" << "temp/rescale/eff" << "ti/rs" << "ti/spring" << "ttm/mod";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Compute styles
    patterns.clear();
    patterns << "angle/local" << "angmom/chunk" << "body/local" << "bond/local" << "centro/atom" << "chunk/atom" << "cluster/atom" << "cna/atom" << "com" << "com/chunk" << "contact/atom" << "coord/atom" << "damage/atom" << "dihedral/local" << "dilatation/atom" << "displace/atom" << "erotate/asphere" << "erotate/rigid" << "erotate/sphere" << "erotate/sphere/atom" << "event/displace" << "group/group" << "gyration" << "gyration/chunk" << "heat/flux" << "improper/local" << "inertia/chunk" << "ke" << "ke/atom" << "ke/rigid" << "msd" << "msd/chunk" << "msd/nongauss" << "omega/chunk" << "pair" << "pair/local" << "pe (c)" << "pe/atom" << "plasticity/atom" << "pressure (c)" << "property/atom" << "property/local" << "property/chunk" << "rdf" << "reduce" << "reduce/region" << "slice" << "sna/atom" << "snad/atom" << "snav/atom" << "stress/atom" << "temp (ck)" << "temp/asphere" << "temp/com" << "temp/chunk" << "temp/deform" << "temp/partial (c)" << "temp/profile" << "temp/ramp" << "temp/region" << "temp/sphere" << "ti" << "torque/chunk" << "vacf" << "vcm/chunk" << "voronoi/atom";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Additional compute styles
    patterns.clear();
    patterns << "ackland/atom" << "basal/atom" << "fep" << "force/tally" << "heat/flux/tally" << "ke/eff" << "ke/atom/eff" << "meso_e/atom" << "meso_rho/atom" << "meso_t/atom" << "pe/tally" << "saed" << "smd/contact/radius" << "smd/damage" << "smd/hourglass/error" << "smd/internal/energy" << "smd/plastic/strain" << "smd/plastic/strain/rate" << "smd/rho" << "smd/tlsph/defgrad" << "smd/tlsph/dt" << "smd/tlsph/num/neighs" << "smd/tlsph/shape" << "smd/tlsph/strain" << "smd/tlsph/strain/rate" << "smd/tlsph/stress" << "smd/triangle/mesh/vertices" << "smd/ulsph/num/neighs" << "smd/ulsph/strain" << "smd/ulsph/strain/rate" << "smd/ulsph/stress" << "smd/vol" << "stress/tally" << "temp/drude" << "temp/eff" << "temp/deform/eff" << "temp/region/eff" << "temp/rotate" << "xrd";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Pair_style potentials
    patterns.clear();
    patterns << "none" << "hybrid" << "hybrid/overlay" << "adp" << "airebo" << "beck" << "body" << "bop" << "born" << "born/coul/long" << "born/coul/long/cs" << "born/coul/msm" << "born/coul/wolf" << "brownian" << "brownian/poly" << "buck" << "buck/coul/cut" << "buck/coul/long" << "buck/coul/long/cs" << "buck/coul/msm" << "buck/long/coul/long" << "colloid" << "comb" << "comb3" << "coul/cut" << "coul/debye" << "coul/dsf" << "coul/long" << "coul/long/cs" << "coul/msm" << "coul/streitz" << "coul/wolf" << "dpd" << "dpd/tstat" << "dsmc" << "eam" << "eam/alloy" << "eam/fs" << "eim" << "gauss" << "gayberne" << "gran/hertz/history" << "gran/hooke" << "gran/hooke/history" << "hbond/dreiding/lj" << "hbond/dreiding/morse" << "kim" << "lcbop" << "line/lj" << "lj/charmm/coul/charmm" << "lj/charmm/coul/charmm/implicit" << "lj/charmm/coul/long" << "lj/charmm/coul/msm" << "lj/class2" << "lj/class2/coul/cut" << "lj/class2/coul/long" << "lj/cubic" << "lj/cut" << "lj/cut/coul/cut" << "lj/cut/coul/debye" << "lj/cut/coul/dsf" << "lj/cut/coul/long" << "lj/cut/coul/long/cs" << "lj/cut/coul/msm" << "lj/cut/dipole/cut" << "lj/cut/dipole/long" << "lj/cut/tip4p/cut" << "lj/cut/tip4p/long" << "lj/expand" << "lj/gromacs" << "lj/gromacs/coul/gromacs" << "lj/long/coul/long" << "lj/long/dipole/long" << "lj/long/tip4p/long" << "lj/smooth" << "lj/smooth/linear" << "lj96/cut" << "lubricate" << "lubricate/poly" << "lubricateU" << "lubricateU/poly" << "meam" << "mie/cut" << "morse" << "nb3b/harmonic" << "nm/cut" << "nm/cut/coul/cut" << "nm/cut/coul/long" << "peri/eps" << "peri/lps" << "peri/pmb" << "peri/ves" << "polymorphic" << "reax" << "rebo" << "resquared" << "snap" << "soft" << "sw" << "table" << "tersoff" << "tersoff/mod" << "tersoff/zbl" << "tip4p/cut" << "tip4p/long" << "tri/lj" << "yukawa" << "yukawa/colloid" << "zbl";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Additional pair_style potentials
    patterns.clear();
    patterns << "awpmd/cut" << "coul/cut/soft" << "coul/diel" << "coul/long/soft" << "eam/cd" << "edip" << "eff/cut" << "gauss/cut" << "list" << "lj/charmm/coul/long/soft" << "lj/cut/coul/cut/soft" << "lj/cut/coul/long/soft" << "lj/cut/dipole/sf" << "lj/cut/soft" << "lj/cut/tip4p/long/soft" << "lj/sdk" << "lj/sdk/coul/long" << "lj/sdk/coul/msm" << "lj/sf" << "meam/spline" << "meam/sw/spline" << "quip" << "reax/c" << "smd/hertz" << "smd/tlsph" << "smd/triangulated/surface" << "smd/ulsph" << "sph/heatconduction" << "sph/idealgas" << "sph/lj" << "sph/rhosum" << "sph/taitwater" << "sph/taitwater/morris" << "srp" << "tersoff/table" << "thole" << "tip4p/long/soft";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Bond-style potentials
    patterns.clear();
    patterns << "none" << "hybrid" << "class2" << "fene" << "fene/expand" << "harmonic" << "morse" << "nonlinear" << "quartic" << "table";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Angle_style potentials
    patterns.clear();
    patterns << "none" << "hybrid" << "charmm" << "class2" << "cosine" << "cosine/delta" << "cosine/periodic" << "cosine/squared" << "harmonic" << "table";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Additional angle_style potentials
    patterns.clear();
    patterns << "cosine/shift" << "cosine/shift/exp" << "dipole" << "fourier" << "fourier/simple" << "quartic" << "sdk";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Dihedral_style potentials
    patterns.clear();
    patterns << "none" << "hybrid" << "charmm" << "class2" << "harmonic" << "helix" << "multi/harmonic" << "opls" << "cosine/shift/exp" << "fourier" << "nharmonic" << "quadratic" << "table";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // Improper_style potentials
    patterns.clear();
    patterns << "none" << "hybrid" << "class2" << "cvff" << "harmonic" << "umbrella" << "cossq" << "fourier" << "ring";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    //  Kspace solvers
    patterns.clear();
    patterns << "ewald" << "ewald/disp" << "msm" << "msm/cg" << "pppm" << "pppm/cg" << "pppm/disp" << "pppm/disp/tip4p" << "pppm/tip4p";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsRepeat
    patterns.clear();
    patterns << "^\\bjump\\b" << "^\\bnext\\b" << "^\\bloop\\b";
    // createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsOperator
    patterns.clear();
    patterns << "equal" << "add" << "sub" << "mult" << "div";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsConditional
    patterns.clear();
    patterns << "if" << "then" << "elif" << "else";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsSpecial
    patterns.clear();
    patterns << "EDGE" << "NULL" << "&";
    createKeywordRules(patterns);
    format.setForeground(Qt::green);
    format.setFontWeight(QFont::Normal);
    appendRule(patterns, format);

    // lammpsComment
    patterns.clear();
    patterns << "#\(.*&\s*\n\)*.*$";
    format.setForeground(Qt::red);
    format.setFontWeight(QFont::Normal);
    appendRule(patterns,format);

    // lammpsNumber
    patterns.clear();
    patterns << "\<[0-9]\+[ij]\=\>" << "\<[0-9]\+\.[0-9]*\([edED][-+]\=[0-9]\+\)\=[ij]\=\>" << "\.[0-9]\+\([edED][-+]\=[0-9]\+\)\=[ij]\=\>" << "\<[0-9]\+[edED][-+]\=[0-9]\+[ij]\=\>";
    format.setForeground(Qt::green);
    format.setFontWeight(QFont::Normal);
    appendRule(patterns,format);
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}

void HighlighterWrapper::setTextDocument(QQuickTextDocument *textDocument)
{
    m_highlighter = new Highlighter(textDocument->textDocument());
}
