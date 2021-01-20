#include "Player.h"

Player::Player(IrrlichtDevice* dev)
{
    device = dev;
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
}

void Player::Drop()
{
    if (!device)
        return;

    smgr->getActiveCamera()->drop();
    device = 0;
}

void Player::Create(ITriangleSelector* selector)
{
    if (!device)
        return;

    _selector = selector;

    // create the keyMap
    SKeyMap keyMap[10];
    keyMap[0].Action = EKA_MOVE_FORWARD;
    keyMap[0].KeyCode = KEY_UP;
    keyMap[1].Action = EKA_MOVE_FORWARD;
    keyMap[1].KeyCode = KEY_KEY_W;

    keyMap[2].Action = EKA_MOVE_BACKWARD;
    keyMap[2].KeyCode = KEY_DOWN;
    keyMap[3].Action = EKA_MOVE_BACKWARD;
    keyMap[3].KeyCode = KEY_KEY_S;

    keyMap[4].Action = EKA_STRAFE_LEFT;
    keyMap[4].KeyCode = KEY_LEFT;
    keyMap[5].Action = EKA_STRAFE_LEFT;
    keyMap[5].KeyCode = KEY_KEY_A;

    keyMap[6].Action = EKA_STRAFE_RIGHT;
    keyMap[6].KeyCode = KEY_RIGHT;
    keyMap[7].Action = EKA_STRAFE_RIGHT;
    keyMap[7].KeyCode = KEY_KEY_D;

    keyMap[8].Action = EKA_JUMP_UP;
    keyMap[8].KeyCode = KEY_SPACE;
    keyMap[9].Action = EKA_CROUCH;
    keyMap[9].KeyCode = KEY_SHIFT;

    //Set up FPS camera. We don't need to make this a class member since we can access the current
    //camera with irrlicht
    camera = smgr->addCameraSceneNodeFPS(
        0, // parent scene node index
        100.0f, // rotation speed
        0.5f, // movement speed
        -1, // id
        keyMap, // keyMapArray
        10, // keyMapSize
        true, // noVerticalMovement
        500.0f, // jumpSpeed
        false, // invertMouse
        true // makeActive
    );

    camera->setPosition(core::vector3df(50, 50, -60));
    camera->setTarget(core::vector3df(-70, 30, -60));

    AddWeapon("media/gun.md2", "media/gun.jpg", "test gun", "idle");

    // set up the collisionResponseAnimator
    if (selector) {
        collisionResponder = smgr->createCollisionResponseAnimator(
            _selector, // ITriangleSelector* world
            camera,  // camera (sceneNode)
            core::vector3df(30, 45, 30), // ellipsoid radius
            core::vector3df(0, -1000, 0),  // gravity
            core::vector3df(0, 40, 0)); // ellipsoid translation
        selector->drop();
        camera->addAnimator(collisionResponder);
        collisionResponder->drop();
    }

    bulletParent = smgr->addEmptySceneNode();
    if (bulletParent)
        bulletParent->setName("Bullet Container");

    device->getLogger()->log("Player loaded");
}

void Player::SetAnimation(const c8* name)
{
    if (!name)
        return;

    snprintf(animation, 64, "%s", name);

    if (weaponNode)
    {
        weaponNode->setAnimationEndCallback(this);
        weaponNode->setMD2Animation(name);
    }
    else
    {
        animation[0] = 0;
        weaponNode->setAnimationEndCallback(0);
    }
}

void Player::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{
    SetAnimation(0);
}

//Adds a player weapon
void Player::AddWeapon(const c8* meshName, const c8* texture, const c8* name, const c8* animation)
{
    weaponMesh = (IAnimatedMeshMD2*)smgr->getMesh(meshName);
    if (weaponMesh == 0)
        return;

    if (weaponMesh->getMeshType() == EAMT_MD2)
    {
        s32 count = weaponMesh->getAnimationCount();
        for (s32 i = 0; i != count; ++i)
        {
            //device->getLogger()->log(buf, ELL_INFORMATION);
        }
    }

    weaponNode = smgr->addAnimatedMeshSceneNode(weaponMesh, smgr->getActiveCamera(), 10,
                                                vector3df(0, 0, 0), vector3df(90, -90, -90));
    weaponNode->setMaterialFlag(EMF_LIGHTING, false);
    weaponNode->setMaterialTexture(0, driver->getTexture(texture));
    weaponNode->setLoopMode(false);
    weaponNode->setName(name);
    SetAnimation(animation);
    
    //NOTE: Most of this is for testing new features
    flashlight = smgr->addLightSceneNode();
    SLight flashlightData;
    flashlightData.Direction = camera->getTarget();
    //flashlightData.OuterCone = 50;
    flashlightData.CastShadows = true;
    flashlightData.DiffuseColor = SColor(255, 50, 70, 125);
    flashlightData.AmbientColor = SColor(255, 50, 70, 125);
    flashlightData.Position = camera->getPosition();
    //flashlightData.Falloff = 100;
    flashlightData.Type = ELT_POINT;
    flashlight->setLightData(flashlightData);
    flashlight->setRadius(100);
    flashlight->setParent(camera);
    
    flashlight->setDebugDataVisible(EDS_FULL);
}

void Player::Jump()
{
    const ISceneNodeAnimatorList& animators = smgr->getActiveCamera()->getAnimators();
    ISceneNodeAnimatorList::ConstIterator it = animators.begin();

    //Allows the player to jump if grounded
    while (it != animators.end())
    {
        if (ESNAT_COLLISION_RESPONSE == (*it)->getType())
        {
            ISceneNodeAnimatorCollisionResponse * collisionResponse =
                static_cast<ISceneNodeAnimatorCollisionResponse *>(*it);

            if (!collisionResponse->isFalling())
                collisionResponse->jump(jumpSpeed);
        }

        it++;
    }

    isJumping = false;
}

//Fires a projectile with a texture and some lighting effects applied. Most of this is for testing.
void Player::FireWeapon()
{
    SParticleImpact impact;
    impact.when = 0;
    vector3df start = camera->getPosition();
    scene::ILightSceneNode* light1;

    if (weaponNode)
    {
        start.X += 0.f;
        start.Y += 0.f;
        start.Z += 0.f;
    }

    vector3df end = (camera->getTarget() - start);
    end.normalize();
    start += end * 20.0f;

    end = start + (end * camera->getFarValue());

    triangle3df triangle;
    line3d<f32> line(start, end);
    
    //Calculate the point of impact of the projectile
    ISceneNode* hitNode;
    if (smgr->getSceneCollisionManager()->getCollisionPoint(line, _selector, end, triangle, hitNode))
    {
        vector3df out = triangle.getNormal();
        out.setLength(0.03f);

        impact.when = 1;
        impact.outVector = out;
        impact.pos = end;

        SetAnimation("pow");
        anim[1].next += anim[1].delta;
    }
    else
    {
        vector3df start = camera->getPosition();

        if (weaponNode)
        {
            //NOTE: Finish
        }

        vector3df end = (camera->getTarget() - start);
        end.normalize();
        start += end * 20.0f;
        end = start + (end * camera->getFarValue());
    }

    ISceneNode* node = 0;
    node = smgr->addBillboardSceneNode(bulletParent, dimension2d<f32>(10, 10), start);

    node->setMaterialFlag(EMF_LIGHTING, false);
    node->setMaterialTexture(0, device->getVideoDriver()->getTexture("media/particlegreen.jpg"));
    node->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
    node->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

    light1 =
    smgr->addLightSceneNode(bulletParent, core::vector3df(0, 0, 0),
    video::SColorf(0.5f, 1.0f, 0.5f, 0.0f), 100.0f);
    light1->setPosition(bulletParent->getPosition());

    f32 length = (f32)(end - start).getLength();
    const f32 speed = 0.5f;
    u32 time = (u32)(length / speed);

    ISceneNodeAnimator* anim = 0;

    animator = smgr->createFlyStraightAnimator(start, end, time);
    node->addAnimator(animator);
    light1->addAnimator(animator);
    animator->drop();

    //node->setName(buf);

    animator = smgr->createDeleteAnimator(time);
    node->addAnimator(animator);
    animator->drop();
    
    if (impact.when)
    {
        impact.when = device->getTimer()->getTime() +
            (time + (s32)((1.f + quake3::Noiser::get()) * 250.f));
        impacts.push_back(impact);
    }
}