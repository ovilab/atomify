#include "Cylinders.h"

#include <QFileInfo>
#include <QColor>
#include <QRgb>
#include <cmath>
#include <QThread>
#include <QPainter>

#include "../../core/camera.h"

Cylinders::Cylinders(QQuickItem *parent)
    : Renderable(parent)
{

}

Cylinders::~Cylinders()
{

}

QVector3D Cylinders::vectorFromColor(const QColor &color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}

QVector<QVector3D> &Cylinders::positions()
{
    return m_positions;
}

float Cylinders::scale() const
{
    return m_scale;
}

void Cylinders::setScale(float scale)
{
    m_scale = scale;
}
QVector3D Cylinders::color() const
{
    return m_color;
}

void Cylinders::setColor(const QColor &color)
{
    m_color = vectorFromColor(color);
}

CylindersRenderer *Cylinders::createRenderer()
{
    return new CylindersRenderer();
}

CylindersRenderer::CylindersRenderer()
{
    m_numberOfVBOs = 2;
}

void CylindersRenderer::synchronize(Renderable* renderer)
{
    Cylinders* cylinders= static_cast<Cylinders*>(renderer);

    m_upVector = cylinders->camera()->upVector().normalized();
    m_viewVector = cylinders->camera()->viewVector().normalized();
    m_rightVector = QVector3D::crossProduct(m_viewVector, m_upVector);

    if(!m_isInitialized) {
        generateVBOs();
        m_isInitialized = true;
    }
    uploadVBOs(cylinders);

    m_vertexCount = Cylinders->m_vertices.size();
    m_indexCount = Cylinders->m_indices.size();
}

void CylindersRenderer::uploadVBOs(Cylinders* cylinders)
{
    if(Cylinders->m_positions.size() < 1) {
        return;
    }

    double scale = cylinders->scale();
    QVector<QVector3D>& positions = cylinders->m_positions;
    QVector<CylinderVBOData>& vertices = cylinders->m_vertices;
    QVector<GLuint>& indices = cylinders->m_indices;
    QVector3D& color = cylinders->m_color;

    QVector3D right = m_rightVector;
    QVector3D up = m_upVector;

    QVector3D ul = (0.5*up - 0.5*right)*scale;
    QVector3D ur = (0.5*up + 0.5*right)*scale;
    QVector3D dl = (-0.5*up - 0.5*right)*scale;
    QVector3D dr = (-0.5*up + 0.5*right)*scale;

    int numberOfVertices = positions.size()*4;
    vertices.resize(numberOfVertices);
    indices.resize(6*positions.size());

    for(auto i=0; i<positions.size(); i++) {
        QVector3D &position = positions[i];

        vertices[4*i + 0].sphereId = i;
        vertices[4*i + 0].position = position;
        vertices[4*i + 0].position[0] += dl[0];
        vertices[4*i + 0].position[1] += dl[1];
        vertices[4*i + 0].position[2] += dl[2];
        vertices[4*i + 0].textureCoord= QVector2D(0,1);

        vertices[4*i + 1].sphereId = i;
        vertices[4*i + 1].position = position;
        vertices[4*i + 1].position[0] += dr[0];
        vertices[4*i + 1].position[1] += dr[1];
        vertices[4*i + 1].position[2] += dr[2];
        vertices[4*i + 1].textureCoord= QVector2D(1,1);

        vertices[4*i + 2].sphereId = i;
        vertices[4*i + 2].position = position;
        vertices[4*i + 2].position[0] += ur[0];
        vertices[4*i + 2].position[1] += ur[1];
        vertices[4*i + 2].position[2] += ur[2];
        vertices[4*i + 2].textureCoord= QVector2D(1,0);

        vertices[4*i + 3].sphereId = i;
        vertices[4*i + 3].position = position;
        vertices[4*i + 3].position[0] += ul[0];
        vertices[4*i + 3].position[1] += ul[1];
        vertices[4*i + 3].position[2] += ul[2];
        vertices[4*i + 3].textureCoord= QVector2D(0,0);

        vertices[4*i + 0].color = color;
        vertices[4*i + 1].color = color;
        vertices[4*i + 2].color = color;
        vertices[4*i + 3].color = color;

        indices [6*i + 0] = 4*i+0;
        indices [6*i + 1] = 4*i+1;
        indices [6*i + 2] = 4*i+2;

        indices [6*i + 3] = 4*i+2;
        indices [6*i + 4] = 4*i+3;
        indices [6*i + 5] = 4*i+0;
    }

    // Transfer vertex data to VBO 0
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CylinderVBOData), &vertices.front(), GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glFunctions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
}

void Cylinders::setPositions(QVector<QVector3D> &positions)
{
    m_positions = positions;
}

void CylindersRenderer::beforeLinkProgram() {
    setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/Cylinders/Cylinders.vsh");
    setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/Cylinders/Cylinders.fsh");
}

void CylindersRenderer::render()
{
    if(m_vertexCount == 0) {
        return;
    }

    // Tell OpenGL which VBOs to use
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int sphereIdLocation = program().attributeLocation("a_sphereId");
    program().enableAttributeArray(sphereIdLocation);
    glFunctions()->glVertexAttribPointer(sphereIdLocation, 1, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);

    offset += sizeof(GLfloat);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program().attributeLocation("a_position");
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = program().attributeLocation("a_color");
    program().enableAttributeArray(colorLocation);
    glFunctions()->glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program().attributeLocation("a_texcoord");
    program().enableAttributeArray(texcoordLocation);
    glFunctions()->glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

    program().disableAttributeArray(vertexLocation);
    program().disableAttributeArray(colorLocation);
    program().disableAttributeArray(texcoordLocation);
}
