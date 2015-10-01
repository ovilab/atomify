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

    // lammpsOutput
    patterns << "log" << "write_restart" << "restart" << "dump" << "undump" << "thermo" << "thermo_modify" << "thermo_style" << "print";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsRead
    patterns.clear();
    patterns << "include" << "read" << "read_restart" << "read_data";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // package stuff
    patterns.clear();
    patterns << "package";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsLattice
    patterns.clear();
    patterns << "boundary" << "units" << "atom_style" << "lattice" << "region" << "create_box" << "create_atoms" << "dielectric";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsLattice
    patterns.clear();
    patterns << "delete_atoms" << "change_box" << "dimension" << "replicate";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsParticle
    patterns.clear();
    patterns << "pair_coeff" << "pair_style" << "pair_modify" << "mass" << "velocity" << "angle_coeff" << "angle_style";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsParticle
    patterns.clear();
    patterns << "atom_modify" << "atom_style" << "bond_coeff" << "bond_style" << "delete_bonds" << "kspace_style";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsParticle
    patterns.clear();
    patterns << "kspace_modify" << "dihedral_style" << "dihedral_coeff" << "improper_style" << "improper_coeff";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsSetup
    patterns.clear();
    patterns << "min_style" << "fix_modify" << "run_style" << "timestep" << "neighbor" << "neigh_modify" << "fix" << "unfix";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsSetup
    patterns.clear();
    patterns << "communicate" << "newton" << "nthreads" << "processors" << "reset_timestep";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsRun
    patterns.clear();
    patterns << "minimize" << "run";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkBlue);
    format.setFontWeight(QFont::Bold);
    appendRule(patterns, format);

    // lammpsDefine
    patterns.clear();
    patterns << "variable" << "group" << "compute";
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
    format.setFontWeight(QFont::Normal);
    appendRule(patterns, format);

    // lammpsConditional
    patterns.clear();
    patterns << "if" << "then" << "elif" << "else";
    createKeywordRules(patterns);
    format.setForeground(Qt::darkRed);
    format.setFontWeight(QFont::Normal);
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
