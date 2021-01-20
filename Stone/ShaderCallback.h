#ifndef SHADERCALLBACK_H
#define SHADERCALLBACK_H

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class ShaderCallback : public IShaderConstantSetCallBack
{
public:
    ShaderCallback::ShaderCallback(IrrlichtDevice*);
    virtual void OnSetConstants(IMaterialRendererServices*, s32);

    IrrlichtDevice* device;
    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;

    path vsFileName;
    path psFileName;
};

#endif