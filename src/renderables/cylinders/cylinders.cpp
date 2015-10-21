#include "cylinders.h"

Cylinders::Cylinders(QQuickItem *parent)
    : Renderable(parent)
{

}

Cylinders::~Cylinders()
{

}

CylindersRenderer *Cylinders::createRenderer()
{
    return new CylindersRenderer();
}

bool Cylinders::dirty() const
{
    return m_dirty;
}

void Cylinders::setDirty(bool dirty)
{
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
}

CylindersRenderer::CylindersRenderer()
{
    m_numberOfVBOs = 1;
}

void CylindersRenderer::synchronize(Renderable* renderer)
{
    Cylinders* cylinders= static_cast<Cylinders*>(renderer);
    if(!m_isInitialized) {
        generateVBOs();
        m_isInitialized = true;
    }
    uploadVBOs(cylinders);
    m_radius = cylinders->radius();
    m_vertexCount = cylinders->m_vertices.size();
}

void CylindersRenderer::uploadVBOs(Cylinders* cylinders)
{
    if(!cylinders->dirty()) return;
    QVector<CylinderVBOData>& vertices = cylinders->m_vertices;
    if(vertices.size() < 1) {
        return;
    }
    int numberOfVertices = vertices.size();
    cylinders->setDirty(false);

    // Transfer vertex data to VBO 0
    m_vao->bind();
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, numberOfVertices * sizeof(CylinderVBOData), &vertices.front(), GL_STATIC_DRAW);
    m_vertexCount = vertices.size();
}

void CylindersRenderer::beforeLinkProgram() {
    setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/cylinders/cylinders.vsh");
    setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/cylinders/cylinders.fsh");
    setShaderFromSourceFile(QOpenGLShader::Geometry, ":/org.compphys.SimVis/renderables/cylinders/cylinders.gsh");
}

void CylindersRenderer::render()
{
    QOpenGLFunctions funcs(QOpenGLContext::currentContext());
    m_vao->bind();

    int vertex1Location = 0;
    int vertex2Location = 1;
    int radius1Location = 2;
    int radius2Location = 3;
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]); // Tell OpenGL which VBOs to use
    program().setUniformValue("radius", m_radius);
    quintptr offset = 0;

    program().enableAttributeArray(vertex1Location);
    glFunctions()->glVertexAttribPointer(vertex1Location, 3, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);
    offset += sizeof(QVector3D);

    program().enableAttributeArray(vertex2Location);
    glFunctions()->glVertexAttribPointer(vertex2Location, 3, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);
    offset += sizeof(QVector3D);

    program().enableAttributeArray(radius1Location);
    glFunctions()->glVertexAttribPointer(radius1Location, 1, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);
    offset += sizeof(float);

    program().enableAttributeArray(radius2Location);
    glFunctions()->glVertexAttribPointer(radius2Location, 1, GL_FLOAT, GL_FALSE, sizeof(CylinderVBOData), (const void *)offset);

    funcs.glDisable(GL_CULL_FACE);
    glDrawArrays(GL_LINES, 0, m_vertexCount);

    program().disableAttributeArray(vertex1Location);
    program().disableAttributeArray(vertex2Location);
    program().disableAttributeArray(radius1Location);
    program().disableAttributeArray(radius2Location);

}
