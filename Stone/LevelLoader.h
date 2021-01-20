#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <irrlicht.h>
#include <string>
#include "Player.h"
#include "ShaderCallback.h"
#include "ModelLoader.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;
using namespace quake3;

class LevelLoader
{
public:
    LevelLoader(IrrlichtDevice*);
    void LoadBSPLevel(std::wstring);
    void DropBSPLevel(void);
    void AddSky(u32, const c8*);
    void AddTestShaders(void);
    void AddEntityWithShader(s32, ISceneNode*, IMesh*, ISceneNodeAnimator*);

    ISceneNode* SkyNode;
    IrrlichtDevice* device;
    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;

    Player* player;
    ModelLoader* modelLoader;
    ShaderCallback* mc;
    IGPUProgrammingServices* gpu;
    E_GPU_SHADING_LANGUAGE shadingLanguage;

    path psFileName;
    path vsFileName;
    vector3df playerPosition;
    vector3df playerRotation;
    s32 currentStartPos = 0;
    bool isLevelLoading = false;

private:
    void LoadBSPEntities(IQ3LevelMesh*, ISceneNode*, IMesh*);
    s32 GetPlayerStartPos(IQ3LevelMesh*, s32);
};

#endif
