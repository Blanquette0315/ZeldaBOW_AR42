#pragma once
#include "ResUI.h"

class GraphicsShaderUI :
    public ResUI
{
private:
    D3D10_PRIMITIVE_TOPOLOGY    m_eTopology;
    SHADER_DOMAIN               m_eDomainType;
    RS_TYPE                     m_eRSType;
    DS_TYPE                     m_eDSType;
    BS_TYPE                     m_eBSType;

    bool                        m_bVS;
    bool                        m_bHS;
    bool                        m_bDS;
    bool                        m_bGS;
    bool                        m_bPS;

    bool                        m_bUpdateOnce;

public:
    virtual void update() override;
    virtual void render_update() override;

public:

public:
    GraphicsShaderUI();
    ~GraphicsShaderUI();
};

