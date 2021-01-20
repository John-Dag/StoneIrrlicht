#ifndef MODELLOADER_H
#define MODELLOADER_H
#define _CRT_SECURE_NO_WARNINGS

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class ModelLoader
{
public:
    ModelLoader(IrrlichtDevice*);
    void LoadMD2(c8*, c8*, vector3df, EMD2_ANIMATION_TYPE);

    IrrlichtDevice* device;
    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;

    IAnimatedMeshSceneNode* node = 0;
};

#endif