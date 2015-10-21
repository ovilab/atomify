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

bool Spheres::dirty() const
{
    return m_dirty;
}

void Spheres::setDirty(bool dirty)
{
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
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

}

void SpheresRenderer::synchronize(Renderable* renderer)
{
    Spheres* spheres= static_cast<Spheres*>(renderer);

    m_upVector = spheres->camera()->upVector().normalized();
    m_viewVector = spheres->camera()->viewVector().normalized();
    m_rightVector = QVector3D::crossProduct(m_viewVector, m_upVector);

    if(!m_isInitialized) {
        int major = QOpenGLContext::currentContext()->format().majorVersion();
        int minor = QOpenGLContext::currentContext()->format().minorVersion();
        m_geometryShaderSupported = major>3 || (major==3 && minor>=3);

        if(m_geometryShaderSupported) m_numberOfVBOs = 1;
        else m_numberOfVBOs = 2;
        generateVBOs();
        m_isInitialized = true;
    }

    uploadVBOs(spheres);
}

void SpheresRenderer::uploadVBONoGeometryShader(Spheres* spheres) {
    if(!spheres->dirty() || spheres->m_positions.size() == 0) {
        return;
    }
    float scale = spheres->scale();
    QVector<QVector3D>& positions = spheres->m_positions;
    QVector<QColor>& colors = spheres->m_colors;
    QVector<float>& scales = spheres->m_scales;
    QVector<SphereNoGeometryShaderVBOData>& vertices = spheres->m_verticesNoGeometryShader;
    QVector<GLuint>& indices = spheres->m_indices;
    QVector3D color = spheres->vectorFromColor(spheres->color());

    int numberOfVertices = positions.size()*4;
    vertices.resize(numberOfVertices);
    indices.resize(6*positions.size());
    bool individualColors = colors.size() == positions.size();
    bool individualScales = scales.size() == positions.size();
    for(auto i=0; i<positions.size(); i++) {
        QVector3D &position = positions[i];
        float additionalScale = scale;
        if(individualScales) {
            additionalScale *= scales[i];
        }

        vertices[4*i + 0].sphereId = i;
        vertices[4*i + 0].scale = additionalScale;
        vertices[4*i + 0].vertexId = 0;
        vertices[4*i + 0].position = position;
        vertices[4*i + 0].textureCoord= QVector2D(0,1);

        vertices[4*i + 1].sphereId = i;
        vertices[4*i + 1].scale = additionalScale;
        vertices[4*i + 1].vertexId = 1;
        vertices[4*i + 1].position = position;
        vertices[4*i + 1].textureCoord= QVector2D(1,1);

        vertices[4*i + 2].sphereId = i;
        vertices[4*i + 2].scale = additionalScale;
        vertices[4*i + 2].vertexId = 2;
        vertices[4*i + 2].position = position;
        vertices[4*i + 2].textureCoord= QVector2D(1,0);

        vertices[4*i + 3].sphereId = i;
        vertices[4*i + 3].scale = additionalScale;
        vertices[4*i + 3].vertexId = 3;
        vertices[4*i + 3].position = position;
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

    spheres->setDirty(false);

    // Transfer vertex data to VBO 0
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SphereNoGeometryShaderVBOData), &vertices.front(), GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glFunctions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);

    m_vertexCount = spheres->m_verticesNoGeometryShader.size();
    m_indexCount = spheres->m_indices.size();
}

void SpheresRenderer::uploadVBOGeometryShader(Spheres* spheres) {
    QVector<QVector3D>& positions = spheres->m_positions;
    QVector<QColor>& colors = spheres->m_colors;
    QVector<float>& scales = spheres->m_scales;
    QVector<SphereGeometryShaderVBOData>& vertices = spheres->m_verticesGeometryShader;
    if(!spheres->dirty()) {
        return;
    }
    if(colors.size() < 1 || scales.size() < 1 || positions.size() < 1) {
        return;
    }
    int numberOfVertices = positions.size();
    vertices.resize(numberOfVertices);

    for(auto i=0; i<numberOfVertices; i++) {
        vertices[i].position = positions[i];
        vertices[i].color[0] = colors[i].redF();
        vertices[i].color[1] = colors[i].greenF();
        vertices[i].color[2] = colors[i].blueF();
        vertices[i].scale = scales[i]*spheres->scale()*0.5;
    }
    spheres->setDirty(false);

    if(vertices.size() < 1) {
        return;
    }

    // Transfer vertex data to VBO 0
    m_vao->bind();
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, numberOfVertices * sizeof(SphereGeometryShaderVBOData), &vertices.front(), GL_STATIC_DRAW);
    m_vertexCount = spheres->m_verticesGeometryShader.size();
}

void SpheresRenderer::uploadVBOs(Spheres* spheres)
{
    if(m_geometryShaderSupported) uploadVBOGeometryShader(spheres);
    else uploadVBONoGeometryShader(spheres);
}

void Spheres::setPositions(QVector<QVector3D> &positions)
{
    m_positions = positions;
}

void SpheresRenderer::beforeLinkProgram() {
    if(m_geometryShaderSupported) {
        setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/spheres/spheresgs.vsh");
        setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/spheres/spheresgs.fsh");
        setShaderFromSourceFile(QOpenGLShader::Geometry, ":/org.compphys.SimVis/renderables/spheres/spheres.gsh");
    } else {
        setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/spheres/spheres.vsh");
        setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/spheres/spheres.fsh");
    }
}

void SpheresRenderer::renderNoGeometryShader() {
    // Tell OpenGL which VBOs to use
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    QVector3D upMinusRightHalf = (m_upVector - m_rightVector)*0.5;
    QVector3D upPlusRightHalf = (m_upVector + m_rightVector)*0.5;
    program().setUniformValue("cp_upMinusRightHalf", upMinusRightHalf);
    program().setUniformValue("cp_upPlusRightHalf", upPlusRightHalf);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int sphereIdLocation = program().attributeLocation("a_sphereId");
    program().enableAttributeArray(sphereIdLocation);
    glFunctions()->glVertexAttribPointer(sphereIdLocation, 1, GL_FLOAT, GL_FALSE, sizeof(SphereNoGeometryShaderVBOData), (const void *)offset);

    offset += sizeof(GLfloat);

    int scaleLocation = program().attributeLocation("a_scale");
    program().enableAttributeArray(scaleLocation);
    glFunctions()->glVertexAttribPointer(scaleLocation, 1, GL_FLOAT, GL_FALSE, sizeof(SphereNoGeometryShaderVBOData), (const void *)offset);

    offset += sizeof(GLfloat);

    int vertexIdLocation = program().attributeLocation("a_vertexId");
    program().enableAttributeArray(vertexIdLocation);
    glFunctions()->glVertexAttribPointer(vertexIdLocation, 1, GL_FLOAT, GL_FALSE, sizeof(SphereNoGeometryShaderVBOData), (const void *)offset);

    offset += sizeof(GLfloat);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program().attributeLocation("a_position");
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SphereNoGeometryShaderVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = program().attributeLocation("a_color");
    program().enableAttributeArray(colorLocation);
    glFunctions()->glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SphereNoGeometryShaderVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program().attributeLocation("a_texcoord");
    program().enableAttributeArray(texcoordLocation);
    glFunctions()->glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(SphereNoGeometryShaderVBOData), (const void *)offset);
    // Draw cube geometry using indices from VBO 1

    glFunctions()->glEnable(GL_DEPTH_TEST);
    glFunctions()->glDepthMask(GL_TRUE);
    glFunctions()->glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glFunctions()->glDisable(GL_DEPTH_TEST);

    program().disableAttributeArray(vertexLocation);
    program().disableAttributeArray(colorLocation);
    program().disableAttributeArray(texcoordLocation);
}

void SpheresRenderer::renderGeometryShader() {
    QOpenGLFunctions funcs(QOpenGLContext::currentContext());

    m_vao->bind();

    int positionLocation = 0;
    int colorLocation = 1;
    int scaleLocation = 2;

    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]); // Tell OpenGL which VBOs to use
    quintptr offset = 0;

    program().enableAttributeArray(positionLocation);
    glFunctions()->glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SphereGeometryShaderVBOData), (const void *)offset);
    offset += sizeof(QVector3D);

    program().enableAttributeArray(colorLocation);
    glFunctions()->glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SphereGeometryShaderVBOData), (const void *)offset);
    offset += sizeof(QVector3D);

    program().enableAttributeArray(scaleLocation);
    glFunctions()->glVertexAttribPointer(scaleLocation, 1, GL_FLOAT, GL_FALSE, sizeof(SphereGeometryShaderVBOData), (const void *)offset);
    funcs.glDisable(GL_CULL_FACE);
    glDrawArrays(GL_POINTS, 0, m_vertexCount);

    program().disableAttributeArray(positionLocation);
    program().disableAttributeArray(colorLocation);
    program().disableAttributeArray(scaleLocation);
}

void SpheresRenderer::render()
{
    if(m_vertexCount == 0) {
        return;
    }
    if(m_geometryShaderSupported) renderGeometryShader();
    else renderNoGeometryShader();

}
