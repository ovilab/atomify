#ifndef SHADERPARAMETER_H
#define SHADERPARAMETER_H

#include <Qt3DCore/QNode>

#include "shadernode.h"

class ShaderParameter : public ShaderNode
{
    Q_OBJECT
public:
    explicit ShaderParameter(ShaderNode *parent = 0);

    virtual QString generateHeader() const override;
    virtual QString generateBody() const override;
    virtual ShaderNodeSetupResult setup(ShaderBuilder *shaderBuilder, QString tempIdentifier) override;

private:
};

#endif // SHADERPARAMETER_H
