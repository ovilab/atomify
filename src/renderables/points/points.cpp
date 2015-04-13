#include "points.h"

Points::Points()
{

}

Points::~Points()
{

}

void Points::setPositions(QVector<QVector3D> &positions)
{
    m_vertices = positions;
}

QVector<QVector3D> &Points::positions()
{
    return m_vertices;
}

RenderableRenderer *Points::createRenderer()
{
    return new PointsRenderer();
}

float Points::pointSize() const
{
    return m_pointSize;
}

QColor Points::color() const
{
    return m_color;
}

void Points::setPointSize(float arg)
{
    if (m_pointSize == arg)
        return;

    m_pointSize = arg;
    emit pointSizeChanged(arg);
}

void Points::setColor(QColor arg)
{
    if (m_color == arg)
        return;

    m_color = arg;
    emit colorChanged(arg);
}

PointsRenderer::PointsRenderer()
{
    m_numberOfVBOs = 1;
}

void PointsRenderer::synchronize(Renderable *renderable)
{
    Points* points = static_cast<Points*>(renderable);
    if(!m_isInitialized) {
        generateVBOs();
        m_isInitialized = true;
    }

    uploadVBO(points);
    m_vertexCount = points->m_vertices.size();
    m_pointSize = points->m_pointSize;
    m_color = QVector4D(points->m_color.redF(), points->m_color.greenF(), points->m_color.blueF(), points->m_color.alphaF());
}

void PointsRenderer::render()
{
    if(m_vertexCount == 0) {
        return;
    }

    QMatrix4x4 modelViewProjectionMatrix = m_projectionMatrix*m_modelViewMatrix;
    program().setUniformValue("modelViewProjectionMatrix", modelViewProjectionMatrix);
    program().setUniformValue("pointSize", m_pointSize);

    // Tell OpenGL which VBOs to use
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program().attributeLocation("a_position");
    program().setUniformValue("color", m_color);
    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), 0);

    // Draw cube geometry using indices from VBO 1
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif
    glDrawArrays(GL_POINTS, 0, m_vertexCount);

    program().disableAttributeArray(vertexLocation);
}

void PointsRenderer::uploadVBO(Points *points)
{
    if(points->m_vertices.size() < 1) {
        return;
    }
    // Transfer vertex data to VBO 0
    m_vertexCount = points->m_vertices.size();
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(QVector3D), &points->m_vertices.front(), GL_STATIC_DRAW);
}

void PointsRenderer::beforeLinkProgram()
{
    setShaderFromSourceCode(QOpenGLShader::Vertex,
                            "uniform highp float pointSize;\n"
                            "attribute highp vec4 a_position;\n"
                            "void main() {\n"
                            "    gl_PointSize = 10.0;\n"
                            "    gl_Position = cp_modelViewProjectionMatrix*a_position;\n"
                            "}");

    setShaderFromSourceCode(QOpenGLShader::Fragment,
                            "uniform highp vec4 color;"
                            "void main() {\n"
                            "    gl_FragColor = color;\n"
                            "}");
}
