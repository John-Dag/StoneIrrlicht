#include "ShaderCallback.h"
#include <iostream>

ShaderCallback::ShaderCallback(IrrlichtDevice* dev)
{
    device = dev;
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
}

//Helper function for loading q3 map shader assets. Experimental directx and opengl shaders.
void ShaderCallback::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
    if (driver->getDriverType() == EDT_DIRECT3D9)
    {
        psFileName = "media/d3d9.hlsl";
        vsFileName = "media/d3d9.hlsl";
    }

    if (driver->getDriverType() == EDT_OPENGL)
    {
        psFileName = "media/opengl.frag";
        vsFileName = "media/opengl.vert";
    }

    core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
    invWorld.makeInverse();
    services->setPixelShaderConstant("mInvWorld", reinterpret_cast<f32*>(1), 0);

    core::matrix4 Proj;
    core::matrix4 View;
    core::matrix4 World;

    Proj = driver->getTransform(video::ETS_PROJECTION);
    View = driver->getTransform(video::ETS_VIEW);
    World = driver->getTransform(video::ETS_WORLD);

    services->setVertexShaderConstant("World", World.pointer(), 16);
    core::vector3df pos = device->getSceneManager()->getActiveCamera()->getAbsolutePosition();
    services->setVertexShaderConstant("LightDirection", reinterpret_cast<f32*>(&pos), 3);
    video::SColorf col(0.0f, 1.0f, 1.0f, 0.0f);
    services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(&col), 4);
    core::matrix4 world = driver->getTransform(video::ETS_WORLD);
    world = world.getTransposed();

    services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

    // set texture, for textures you can use both an int and a float setPixelShaderConstant interfaces (You need it only for an OpenGL driver).
    s32 TextureLayerID = 0;
    services->setPixelShaderConstant("myTexture", &TextureLayerID, 1);
}