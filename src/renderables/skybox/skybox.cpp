#include "skybox.h"
#include <QOpenGLTexture>
#include <QImage>
#include <QFileInfo>
#include <QOpenGLFunctions>

SkyBoxRenderer *SkyBox::createRenderer()
{
    return new SkyBoxRenderer();
}

void SkyBoxRenderer::uploadTexture()
{
    if(m_texture) {
        m_texture->destroy();
        m_texture = 0;
    }
    QStringList fileAppendNames;
    fileAppendNames << "_positiveX" << "_negativeX"
                    << "_positiveY" << "_negativeY"
                    << "_positiveZ" << "_negativeZ";

    QList<QOpenGLTexture::CubeMapFace> faces;
    faces << QOpenGLTexture::CubeMapPositiveX << QOpenGLTexture::CubeMapNegativeX
          << QOpenGLTexture::CubeMapPositiveY << QOpenGLTexture::CubeMapNegativeY
          << QOpenGLTexture::CubeMapPositiveZ << QOpenGLTexture::CubeMapNegativeZ;

    QFileInfo f(m_textureFileBase);
    QString baseName = f.absolutePath()+f.baseName();
    QString suffix = f.suffix();
    m_texture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
#if defined(Q_OS_IOS)
        m_texture->setFormat(QOpenGLTexture::RGBAFormat);
#else
        m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
#endif

    for(int i=0; i<6; i++) {
        QString imageFileName = QString("%1%2.%3").arg(baseName, fileAppendNames[i], suffix);

        QImage image(imageFileName);
        if(image.isNull()) {
            qDebug() << "Error, file " << imageFileName << " does not exist, aborting!";
            exit(1);
        }
        image = image.convertToFormat(QImage::Format_RGBA8888);
        if(i==0) {
            m_texture->setSize(image.width(), image.height());
            m_texture->allocateStorage();
            m_texture->bind();
        }

        m_texture->setData(0,0,QOpenGLTexture::CubeMapFace(QOpenGLTexture::CubeMapPositiveX + i),
                           QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.bits());

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_texture->release();

    fileAppendNames.clear();
    faces.clear();
}

void SkyBoxRenderer::uploadVBO()
{
    QVector<QVector2D> vertices;
    vertices.push_back(QVector2D(-1, -1));
    vertices.push_back(QVector2D(1, -1));
    vertices.push_back(QVector2D(-1, 1));
    vertices.push_back(QVector2D(1, -1));
    vertices.push_back(QVector2D(1, 1));
    vertices.push_back(QVector2D(-1, 1));

    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glFunctions()->glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(QVector2D), &vertices.front(), GL_STATIC_DRAW);
    vertices.clear();
}

QString SkyBox::texture() const
{
    return m_texture;
}

QOpenGLTexture *SkyBox::texturePointer()
{
    return m_texturePointer;
}

void SkyBox::setTexture(QString arg)
{
    if (m_texture == arg)
        return;

    m_texture = arg;
    emit textureChanged(arg);
}

SkyBoxRenderer::SkyBoxRenderer()
{
    m_numberOfVBOs = 1;
}

void SkyBoxRenderer::beforeLinkProgram()
{
    setShaderFromSourceFile(QOpenGLShader::Fragment, ":/org.compphys.SimVis/renderables/skybox/skybox.fsh");
    setShaderFromSourceFile(QOpenGLShader::Vertex, ":/org.compphys.SimVis/renderables/skybox/skybox.vsh");
}

void SkyBoxRenderer::synchronize(Renderable *renderable)
{
    SkyBox *skybox = qobject_cast<SkyBox*>(renderable);
//    if(m_textureFileBase != skybox->texture()) {
//        m_texture->destroy();
//        m_texture = 0;
//    }
    m_textureFileBase = skybox->texture();

    if(!m_isInitialized) {
        generateVBOs();
        uploadVBO();
        m_isInitialized = true;
    }

    if(!m_texture) {
        uploadTexture();
        skybox->m_texturePointer = m_texture;
    }
}

void SkyBoxRenderer::render()
{
    QMatrix4x4 modelViewMatrixPrimeInverted;
    modelViewMatrixPrimeInverted.setToIdentity();
    modelViewMatrixPrimeInverted.lookAt(-m_viewCenter, -m_cameraPosition, m_upVector);
    modelViewMatrixPrimeInverted = modelViewMatrixPrimeInverted.inverted();
    program().setUniformValue("modelViewMatrixPrimeInverted", modelViewMatrixPrimeInverted);

    // Tell OpenGL which VBOs to use
    glFunctions()->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program().attributeLocation("a_position");

    program().enableAttributeArray(vertexLocation);
    glFunctions()->glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    m_texture->bind();

    glDisable(GL_DEPTH_TEST); // Important so it will be behind everything else drawn later
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_texture->release();
    program().disableAttributeArray(vertexLocation);
}
