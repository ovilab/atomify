#include "spheres.h"

#include <QFileInfo>
#include <QColor>
#include <QRgb>
#include <cmath>
#include <QThread>
#include <QPainter>

#include "../../core/camera.h"

Spheres::Spheres(QQuickItem *parent)
    : Renderable(parent)
{

}

Spheres::~Spheres()
{

}

QVector3D Spheres::vectorFromColor(const QColor &color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}
QVector<float> &Spheres::scales()
{
    return m_scales;
}

void Spheres::setScales(const QVector<float> &scales)
{
    m_scales = scales;
}

QVector<QColor> &Spheres::colors()
{
    return m_colors;
}

void Spheres::setColors(const QVector<QColor> &colors)
{
    m_colors = colors;
}


QVector<QVector3D> &Spheres::positions()
{
    return m_positions;
}

float Spheres::scale() const
{
    return m_scale;
}

void Spheres::setScale(float scale)
{
    m_scale = scale;
}

QColor Spheres::color() const
{
    return m_color;
}

void Spheres::setColor(const QColor &color)
{
    m_color = color;
}

SpheresRenderer *Spheres::createRenderer()
{
    return new SpheresRenderer();
}

SpheresRenderer::SpheresRenderer()
{
    m_numberOfVBOs = 2;
}

void SpheresRenderer::synchronize(Renderable* renderer)
{
    Spheres* spheres= static_cast<Spheres*>(renderer);

    m_upVector = spheres->camera()->upVector().normalized();
    m_viewVector = spheres->camera()->viewVector().normalized();
    m_rightVector = QVector3D::crossProduct(m_viewVector, m_upVector);

    if(!m_isInitialized) {
        generateVBOs();
        m_isInitialized = true;
    }

    uploadVBOs(spheres);

    m_vertexCount = spheres->m_vertices.size();
    m_indexCount = spheres->m_indices.size();
}

void SpheresRenderer::uploadVBOs(Spheres* spheres)
{
    if(spheres->m_positions.size() < 1) {
        return;
    }

    double scale = spheres->scale();
    QVector<QVector3D>& positions = spheres->m_positions;
    QVector<QColor>& colors = spheres->m_colors;
    QVector<float>& scales = spheres->m_scales;
    QVector<SphereVBOData>& vertices = spheres->m_vertices;
    QVector<GLuint>& indices = spheres->m_indices;
    QVector3D color = spheres->vectorFromColor(spheres->color());

    QVector3D right = m_rightVector;
    QVector3D up = m_upVector;

    QVector3D ul = (0.5*up - 0.5*right)*scale;
    QVector3D ur = (0.5*up + 0.5*right)*scale;
    QVector3D dl = (-0.5*up - 0.5*right)*scale;
    QVector3D dr = (-0.5*up + 0.5*right)*scale;

    int numberOfVertices = positions.size()*4;
    vertices.resize(numberOfVertices);
    indices.resize(6*positions.size());
    bool individualColors = colors.size() == positions.size();
    bool individualScales = scales.size() == positions.size();
    for(auto i=0; i<positions.size(); i++) {
        QVector3D &position = positions[i];
        float additionalScale = 1.0;
        if(individualScales) {
            additionalScale = scales[i];
        }

        vertices[4*i + 0].sphereId = i;
        vertices[4*i + 0].position = position;
        vertices[4*i + 0].position[0] += dl[0]*additionalScale;
        vertices[4*i + 0].position[1] += dl[1]*additionalScale;
        vertices[4*i + 0].position[2] += dl[2]*additionalScale;
        vertices[4*i + 0].textureCoord= QVector2D(0,1);

        vertices[4*i + 1].sphereId = i;
        vertices[4*i + 1].position = position;
        vertices[4*i + 1].position[0] += dr[0]*additionalScale;
        vertices[4*i + 1].position[1] += dr[1]*additionalScale;
        vertices[4*i + 1].position[2] += dr[2]*additionalScale;
        vertices[4*i + 1].textureCoord= QVector2D(1,1);

        vertices[4*i + 2].sphereId = i;
        vertices[4*i + 2].position = position;
        vertices[4*i + 2].position[0] += ur[0]*additionalScale;
        vertices[4*i + 2].position[1] += ur[1]*additionalScale;
        vertices[4*i + 2].position[2] += ur[2]*additionalScale;
        vertices[4*i + 2].textureCoord= QVector2D(1,0);

        vertices[4*i + 3].sphereId = i;
        vertices[4*i + 3].position = position;
        vertices[4*i + 3].position[0] += ul[0]*additionalScale;
        vertices[4*i + 3].position[1] += ul[1]*additionalScale;
        vertices[4*i + 3].position[2] += ul[2]*additionalScale;
        vertices[4*i + 3].textureCoord= QVector2D(0,0);
        if(individualColors) {
            color = QVector3D(colors[i].redF(), colors[i].greenF(), colors[i].blueF());
        }
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
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SphereVBOData), &vertices.front(), GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glFunctions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
}

void Spheres::setPositions(QVector<QVector3D> &positions)
{
    m_positions = positions;
}

void SpheresRenderer::beforeLinkProgram() {
    setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/spheres/spheres.vsh");
    setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/spheres/spheres.fsh");
}

void SpheresRenderer::render()
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
    glFunctions()->glVertexAttribPointer(sphereIdLocation, 1, GL_FLOAT, GL_FALSE, sizeof(SphereVBOData), (const void *)offset);

    offset += sizeof(GLfloat);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program().attributeLocation("a_position");
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = program().attributeLocation("a_color");
    program().enableAttributeArray(colorLocation);
    glFunctions()->glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program().attributeLocation("a_texcoord");
    program().enableAttributeArray(texcoordLocation);
    glFunctions()->glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(SphereVBOData), (const void *)offset);
    // Draw cube geometry using indices from VBO 1

    glFunctions()->glEnable(GL_DEPTH_TEST);
    glFunctions()->glDepthMask(GL_TRUE);
    glFunctions()->glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glFunctions()->glDisable(GL_DEPTH_TEST);

    program().disableAttributeArray(vertexLocation);
    program().disableAttributeArray(colorLocation);
    program().disableAttributeArray(texcoordLocation);
}
