//#include "shaderbuildermaterialbinding.h"

//#include <Qt3DRender/QParameter>

//ShaderBuilderMaterialBinding::ShaderBuilderMaterialBinding(Qt3DCore::QNode *parent) : Qt3DCore::QNode(parent)
//{

//}

//QMaterial *ShaderBuilderMaterialBinding::material() const
//{
//    return m_material;
//}

//ShaderBuilder *ShaderBuilderMaterialBinding::shaderBuilder() const
//{
//    return m_shaderBuilder;
//}

//void ShaderBuilderMaterialBinding::setMaterial(QMaterial *material)
//{
//    if (m_material == material)
//        return;
//    if(m_material) {
//        clear();
//    }
//    m_material = material;
//    if(m_material) {
//        apply();
//    }
//    emit materialChanged(material);
//}

//void ShaderBuilderMaterialBinding::setShaderBuilder(ShaderBuilder *shaderBuilder)
//{
//    if (m_shaderBuilder == shaderBuilder)
//        return;
//    if(m_shaderBuilder) {
//        clear();
//        disconnect(shaderBuilder, &ShaderBuilder::clearBegin, this, &ShaderBuilderMaterialBinding::clear);
//        disconnect(shaderBuilder, &ShaderBuilder::buildFinished, this, &ShaderBuilderMaterialBinding::apply);
//    }
//    m_shaderBuilder = shaderBuilder;
//    if(m_shaderBuilder) {
//        connect(shaderBuilder, &ShaderBuilder::clearBegin, this, &ShaderBuilderMaterialBinding::clear);
//        connect(shaderBuilder, &ShaderBuilder::buildFinished, this, &ShaderBuilderMaterialBinding::apply);
//        apply();
//    }
//    emit shaderBuilderChanged(shaderBuilder);
//}

//void ShaderBuilderMaterialBinding::clear()
//{
//    if(m_material && m_shaderBuilder) {
//        for(UniformValue &uniformValue : m_shaderBuilder->m_uniforms) {
//            m_material->removeParameter(uniformValue.parameter);
//        }
//    }
//}

//void ShaderBuilderMaterialBinding::apply()
//{
//    if(m_material && m_shaderBuilder) {
//        for(UniformValue &uniformValue : m_shaderBuilder->m_uniforms) {
//            m_material->addParameter(uniformValue.parameter);
//        }
//    }
//}
