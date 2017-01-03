//#ifndef SHADERBUILDERMATERIALBINDING_H
//#define SHADERBUILDERMATERIALBINDING_H

//#include <QObject>
//#include <Qt3DCore/QNode>
//#include <Qt3DRender/QMaterial>
//#include "shaderbuilder.h"

//class ShaderBuilderMaterialBinding : public Qt3DCore::QNode
//{
//    Q_OBJECT
//    Q_PROPERTY(QMaterial* material READ material WRITE setMaterial NOTIFY materialChanged)
//    Q_PROPERTY(ShaderBuilder* shaderBuilder READ shaderBuilder WRITE setShaderBuilder NOTIFY shaderBuilderChanged)

//public:
//    explicit ShaderBuilderMaterialBinding(Qt3DCore::QNode *parent = 0);

//    QMaterial* material() const;
//    ShaderBuilder* shaderBuilder() const;

//signals:

//    void materialChanged(QMaterial* material);

//    void shaderBuilderChanged(ShaderBuilder* shaderBuilder);

//public slots:


//    void setMaterial(QMaterial* material);
//    void setShaderBuilder(ShaderBuilder* shaderBuilder);

//private:
//    void clear();
//    void apply();

//    QMaterial* m_material = nullptr;
//    ShaderBuilder* m_shaderBuilder = nullptr;
//};

//#endif // SHADERBUILDERMATERIALBINDING_H
