/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef UNIFORMGRIDGEOMETRY_H
#define UNIFORMGRIDGEOMETRY_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qgeometry.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qattribute.h>

class UniformGridGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(int resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)

public:
    explicit UniformGridGeometry(QNode *parent = 0);

    void updateVertices();
    void updateIndices();
    void init();
    int resolution() const;

public Q_SLOTS:
    void setResolution(int resolution);

Q_SIGNALS:
    void resolutionChanged(int resolution);

protected:

private:
    Qt3DRender::QAttribute *m_positionAttribute = nullptr;
    Qt3DRender::QAttribute *m_indexAttribute = nullptr;
    Qt3DRender::QBuffer *m_vertexBuffer = nullptr;
    Qt3DRender::QBuffer *m_indexBuffer = nullptr;
    class UniformGridVertexDataFunctor *m_dataFunctor = nullptr;
    int m_resolution = 32;
    int vertexCount() { return m_resolution*m_resolution*m_resolution; }
};

#endif // UNIFORMGRIDGEOMETRY_H
