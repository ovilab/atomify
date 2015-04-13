#include "marchingcubes.h"
#include <iostream>
#include <QElapsedTimer>
#include <QFile>
using namespace std;

function<float (const QVector3D point)> MarchingCubes::scalarFieldEvaluator() const
{
    return m_scalarFieldEvaluator;
}

void MarchingCubes::setScalarFieldEvaluator(const function<float(const QVector3D pos)> &scalarFieldEvaluator)
{
    m_scalarFieldEvaluator = scalarFieldEvaluator;
    m_hasScalarField = true;
    setDirty(true);
}

QColor MarchingCubes::color() const
{
    return m_color;
}

MarchingCubes::Mode MarchingCubes::mode() const
{
    return m_mode;
}


float MarchingCubes::scale() const
{
    return m_scale;
}

function<QVector4D (const QVector3D point)> MarchingCubes::colorEvaluator() const
{
    return m_colorEvaluator;
}

void MarchingCubes::setColorEvaluator(const function<QVector3D (const QVector3D point)> &colorEvaluator)
{
    setDirty(true);
    m_colorEvaluator = colorEvaluator;
}

MarchingCubes::MarchingCubes()
{

}

MarchingCubes::~MarchingCubes()
{

}

RenderableRenderer *MarchingCubes::createRenderer()
{
    return new MarchingCubesRenderer();
}

float MarchingCubes::threshold() const
{
    return m_threshold;
}

QVector3D MarchingCubes::min() const
{
    return m_min;
}

QVector3D MarchingCubes::max() const
{
    return m_max;
}

bool MarchingCubes::dirty() const
{
    return m_dirty;
}

QVector3D MarchingCubes::numVoxels() const
{
    return m_numVoxels;
}

bool MarchingCubes::hasScalarField() const
{
    return m_hasScalarField;
}

void MarchingCubes::setThreshold(float arg)
{
    if (m_threshold == arg)
        return;

    m_threshold = arg;
    setDirty(true);
    emit thresholdChanged(arg);
}

void MarchingCubes::setMin(QVector3D arg)
{
    if (m_min == arg)
        return;

    m_min = arg;
    setDirty(true);
    emit minChanged(arg);
}

void MarchingCubes::setMax(QVector3D arg)
{
    if (m_max == arg)
        return;

    m_max = arg;
    setDirty(true);
    emit maxChanged(arg);
}

void MarchingCubes::setDirty(bool arg)
{
    if (m_dirty == arg)
        return;

    m_dirty = arg;
    emit dirtyChanged(arg);
}

void MarchingCubes::setNumVoxels(QVector3D arg)
{
    if (m_numVoxels == arg)
        return;

    m_numVoxels = arg;
    setDirty(true);
    emit numVoxelsChanged(arg);
}

void MarchingCubes::setColor(QColor arg)
{
    if (m_color == arg)
        return;

    setDirty(true);
    m_color = arg;
    emit colorChanged(arg);
}

void MarchingCubes::setMode(MarchingCubes::Mode arg)
{
    if (m_mode == arg)
        return;

    m_mode = arg;
    emit modeChanged(arg);
}

void MarchingCubes::setScale(float arg)
{
    if (m_scale == arg)
        return;

    m_scale = arg;
    emit scaleChanged(arg);
}

void MarchingCubes::setNumberOfTriangles(int arg)
{
    if (m_numberOfTriangles == arg)
        return;

    m_numberOfTriangles = arg;
    emit numberOfTrianglesChanged(arg);
}

void MarchingCubes::setNumberOfLines(int arg)
{
    if (m_numberOfLines == arg)
        return;

    m_numberOfLines = arg;
    emit numberOfLinesChanged(arg);
}

bool MarchingCubes::hasContinuousScalarField() const
{
    return m_hasContinuousScalarField;
}

void MarchingCubes::setHasContinuousScalarField(bool hasContinuousScalarField)
{
    m_hasContinuousScalarField = hasContinuousScalarField;
}

int MarchingCubes::numberOfTriangles() const
{
    return m_numberOfTriangles;
}

int MarchingCubes::numberOfLines() const
{
    return m_numberOfLines;
}

MarchingCubesRenderer::MarchingCubesRenderer()
{
    m_numberOfVBOs = 4;
}

void MarchingCubesRenderer::synchronize(Renderable *renderable)
{
    MarchingCubes* marchingCubes = static_cast<MarchingCubes*>(renderable);
    if(!m_isInitialized) {
        generateVBOs();
        m_isInitialized = true;
    }

    if(marchingCubes->dirty()) {
        if(marchingCubes->hasScalarField()) {
            if(marchingCubes->m_colorEvaluator != 0) {
                m_generator.m_colorEvaluator = marchingCubes->m_colorEvaluator;
                m_generator.m_hasColorEvaluator = true;
            } else {
                m_generator.setColor(marchingCubes->color());
                m_generator.m_hasColorEvaluator = false;
            }
            m_generator.m_hasContinuousField = marchingCubes->m_hasContinuousScalarField;
            m_generator.setScalarFieldEvaluator(marchingCubes->scalarFieldEvaluator());
            qDebug() << "Will generate surface with min: " << marchingCubes->min() << " and max: " << marchingCubes->max() << " and num voxels: " << marchingCubes->numVoxels() << " and threshold: " << marchingCubes->threshold();
            m_generator.generateSurface(marchingCubes->min(), marchingCubes->max(), marchingCubes->numVoxels(), marchingCubes->threshold());
            uploadVBOs();
        }

        marchingCubes->setDirty(false);
    }

    if(marchingCubes->m_shadersDirty) {
        m_shadersDirty = true;
        marchingCubes->m_shadersDirty = false;
    }
    marchingCubes->setNumberOfTriangles(2*m_generator.m_trianglesFront.size());
    marchingCubes->setNumberOfLines(6*m_generator.m_lines.size());
    m_color = QVector3D(marchingCubes->color().redF(), marchingCubes->color().greenF(), marchingCubes->color().blueF());
    m_mode = marchingCubes->mode();
    m_scale = marchingCubes->scale();

}

void MarchingCubesRenderer::render()
{
    if(m_triangleIndexCount == 0) {
        return;
    }

    program().setUniformValue("scale", m_scale);

    // Tell OpenGL which VBOs to use
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program().attributeLocation("a_position");
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MarchingCubesVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    vertexLocation = program().attributeLocation("a_normal");
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MarchingCubesVBOData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    vertexLocation = program().attributeLocation("a_color");
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(MarchingCubesVBOData), (const void *)offset);

    glFunctions()->glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    // Draw cube geometry using indices from VBO 1
    if(m_mode == MarchingCubes::FRONT_AND_BACK || m_mode == MarchingCubes::FRONT) {
        glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
        program().setUniformValue("normalVectorSign", 1.0f);
        glDrawElements(GL_TRIANGLES, m_triangleIndexCount, GL_UNSIGNED_INT, 0);
    }

    if(m_mode == MarchingCubes::FRONT_AND_BACK || m_mode == MarchingCubes::BACK) {
        glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[2]);
        program().setUniformValue("normalVectorSign", -1.0f);
        glDrawElements(GL_TRIANGLES, m_triangleIndexCount, GL_UNSIGNED_INT, 0);
    }

    if(m_mode == MarchingCubes::LINES) {
        glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[3]);
        program().setUniformValue("normalVectorSign", 1.0f);
        glDrawElements(GL_LINES, m_lineIndexCount, GL_UNSIGNED_INT, 0);
    }

    program().disableAttributeArray(vertexLocation);
}

void MarchingCubesRenderer::uploadVBOs()
{
    if(m_generator.m_data.size() == 0) return;
    // Transfer vertex data to VBO 0
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, m_generator.m_data.size() * sizeof(MarchingCubesVBOData), &m_generator.m_data[0], GL_STATIC_DRAW);

    // Transfer index data for front triangles to VBO 1
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glFunctions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_generator.m_trianglesFront.size() * sizeof(Triangle), &m_generator.m_trianglesFront[0], GL_STATIC_DRAW);

    // Transfer index data for back triangles to VBO 2
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[2]);
    glFunctions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_generator.m_trianglesBack.size() * sizeof(Triangle), &m_generator.m_trianglesBack[0], GL_STATIC_DRAW);

    // Transfer index data for back triangles to VBO 2
    glFunctions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[3]);
    glFunctions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_generator.m_lines.size() * sizeof(TriangleLines), &m_generator.m_lines[0], GL_STATIC_DRAW);

    m_triangleIndexCount = 3*m_generator.m_trianglesFront.size();
    m_lineIndexCount = 6*m_generator.m_lines.size();
}

void MarchingCubesRenderer::beforeLinkProgram()
{
    setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/marchingcubes/marchingcubes.fsh");
    setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/marchingcubes/marchingcubes.vsh");
}
