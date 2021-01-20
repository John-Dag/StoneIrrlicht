#ifndef PLAYER_H
#define PLAYER_H

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;
using namespace io;

class Player : public IAnimationEndCallBack
{
public:
    struct SParticleImpact
    {
        u32 when;
        vector3df pos;
        vector3df outVector;
    };

    struct TimeFire
    {
        u32 flags;
        u32 next;
        u32 delta;
    };

    Player(IrrlichtDevice*);
    void Drop(void);
    void Create(ITriangleSelector*);
    void FireWeapon(void);
    void Jump(void);
    void AddWeapon(const c8* meshName, const c8* texture, const c8* name, const c8* animation);
    virtual void SetAnimation(const c8*);
    virtual void OnAnimationEnd(IAnimatedMeshSceneNode*);

    ISceneNodeAnimatorCollisionResponse* collisionResponder;
    IAnimatedMeshMD2* weaponMesh;
    IAnimatedMeshSceneNode* weaponNode;
    ILightSceneNode* flashlight;
    ICameraSceneNode* camera;
    ISceneNodeAnimator* animator = 0;
    c8 animation[64];
    irr::core::array<SParticleImpact> impacts;
    TimeFire anim[4];
    ISceneNode* bulletParent;
    ITriangleSelector* _selector;

    IrrlichtDevice* device;
    IVideoDriver* driver;
    ISceneManager* smgr;
    IGUIEnvironment* guienv;

    vector3df playerPosition;
    vector3df playerRotation;
    s32 currentStartPos = 0;
    s32 startPosIndex = 0;
    bool isJumping = false;
    float jumpSpeed = 500.0f;
};

#endif