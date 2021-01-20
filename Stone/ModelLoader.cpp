#include "ModelLoader.h"

ModelLoader::ModelLoader(IrrlichtDevice* dev)
{
    device = dev;
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
}

//Loads an MD2 style character mesh
void ModelLoader::LoadMD2(c8* name, c8* texture, vector3df startPos, EMD2_ANIMATION_TYPE startingAnim)
{
    SMaterial material;

    node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(name), 0);
    node->setMD2Animation(startingAnim);
    node->setAnimationSpeed(20.0f);
    node->setPosition(startPos);
    material.setTexture(0, driver->getTexture(texture));
    material.Lighting = true;
    material.NormalizeNormals = true;
    node->getMaterial(0) = material;
    node->addShadowVolumeSceneNode();
    node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
}