#include <iostream>
#include "LevelLoader.h"

LevelLoader::LevelLoader(IrrlichtDevice* dev)
{
    device = dev;
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    gpu = driver->getGPUProgrammingServices();
    shadingLanguage = video::EGSL_DEFAULT;
    mc = new ShaderCallback(device);
    modelLoader = new ModelLoader(device);
}

void LevelLoader::DropBSPLevel()
{
    //Drop our current level 
    driver->removeAllHardwareBuffers();
    driver->removeAllTextures();

    smgr->getMeshCache()->clear();
    smgr->clear();

    device->getLogger()->log("Level dropped");
}

void LevelLoader::LoadBSPLevel(std::wstring filename)
{
    std::wstring file = filename;
    std::wstring dir = filename;
    std::wstring bsp = L".bsp";
    std::wstring pk3 = L".pk3";
    std::wstring folder = L"../Stone/media/";

    dir.insert(0, folder);
    dir.append(pk3);

    if (!device->getFileSystem()->addFileArchive(dir.c_str(), true))
        return;

    DropBSPLevel();
    isLevelLoading = true;

    file.append(bsp);

    scene::IQ3LevelMesh* mesh = (scene::IQ3LevelMesh*) smgr->getMesh(file.c_str());
    scene::ISceneNode* levelNode = 0;
    scene::ISceneNode* lightingNode = 0;

    if (!mesh)
        return;
    
    //Some of these are experimental load parameters
    scene::IMesh* geometry = mesh->getMesh(quake3::E_Q3_MESH_GEOMETRY);
    smgr->setAmbientLight(SColorf(0.5f, 0.5f, 0.5f, 1.0f));
    IMesh* tangentMesh = smgr->getMeshManipulator()->createMeshWithTangents(mesh->getMesh(0));
    levelNode = smgr->addOctreeSceneNode(mesh);
    levelNode->setMaterialFlag(EMF_LIGHTING, true);
    levelNode->setMaterialFlag(EMF_FOG_ENABLE, true);
    levelNode->setMaterialType(EMT_LIGHTMAP);
    ITexture* normalMap = driver->getTexture("../Stone/media/wall.bmp");
    driver->makeNormalMapTexture(normalMap, 20.0f);
    levelNode->setMaterialTexture(1, normalMap);
    levelNode->setMaterialType(EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA);
    tangentMesh->drop();
    levelNode->getMaterial(0).EmissiveColor.set(255, 34, 23, 59);

    // the additional mesh can be quite huge and is unoptimized
    scene::IMesh* additional_mesh = mesh->getMesh(quake3::E_Q3_MESH_ITEMS);
    additional_mesh->setMaterialFlag(EMF_LIGHTING, true);
    for (u32 i = 0; i != additional_mesh->getMeshBufferCount(); ++i)
    {
        const IMeshBuffer* meshBuffer = additional_mesh->getMeshBuffer(i);
        const video::SMaterial& material = meshBuffer->getMaterial();

        // The ShaderIndex is stored in the material parameter
        const s32 shaderIndex = (s32)material.MaterialTypeParam2;

        // the meshbuffer can be rendered without additional support, or it has no shader
        const quake3::IShader *shader = mesh->getShader(shaderIndex);
        if (0 == shader)
        {
            continue;
        }

        // we can dump the shader to the console in its
        // original but already parsed layout in a pretty
        // printers way.. commented out, because the console
        // would be full...
        // quake3::dumpShader ( Shader );

        levelNode = smgr->addQuake3SceneNode(meshBuffer, shader);
    }

    //LoadBSPEntities(mesh, levelNode, geometry);
    ITriangleSelector* selector = 0;

    if (levelNode) {
        levelNode->setPosition(core::vector3df(0, -42, 0));
        selector = smgr->createOctreeTriangleSelector(
            geometry, levelNode, 128);
        levelNode->setTriangleSelector(selector);
    }

    player = new Player(device);
    player->Create(selector);

    if (currentStartPos >= GetPlayerStartPos(mesh, currentStartPos++))
    {
        currentStartPos = 0;
    }

    //Load some test models
    vector3df temp = player->camera->getPosition();
    temp.Y -= 40;
    temp.X -= 100;
    modelLoader->LoadMD2("../Stone/media/sydney.md2", "../Stone/media/sydney.bmp", temp, EMAT_STAND);
    temp.X -= 100;
    modelLoader->LoadMD2("../Stone/media/sydney.md2", "../Stone/media/sydney.bmp", temp, EMAT_CROUCH_DEATH);
    temp.X -= 100;
    modelLoader->LoadMD2("../Stone/media/sydney.md2", "../Stone/media/sydney.bmp", temp, EMAT_BOOM);
    temp.X -= 100;
    modelLoader->LoadMD2("../Stone/media/sydney.md2", "../Stone/media/sydney.bmp", temp, EMAT_RUN);

    isLevelLoading = false;
}

s32 LevelLoader::GetPlayerStartPos(IQ3LevelMesh* mesh, s32 startPosIndex)
{
    if (0 == mesh)
        return 0;

    tQ3EntityList &entityList = mesh->getEntityList();

    IEntity search;
    search.name = "info_player_start";    // "info_player_deathmatch";

                                        // find all entities in the multi-list
    s32 lastIndex;
    s32 index = entityList.binary_search_multi(search, lastIndex);

    if (index < 0)
    {
        search.name = "info_player_deathmatch";
        index = entityList.binary_search_multi(search, lastIndex);
    }

    if (index < 0)
        return 0;

    index += core::clamp(startPosIndex, 0, lastIndex - index);

    u32 parsepos;

    const SVarGroup *group;
    group = entityList[index].getGroup(1);

    parsepos = 0;
    vector3df pos = getAsVector3df(group->get("origin"), parsepos);
    pos += player->collisionResponder->getEllipsoidTranslation();

    parsepos = 0;
    f32 angle = getAsFloat(group->get("angle"), parsepos);

    vector3df target(0.f, 0.f, 1.f);
    target.rotateXZBy(angle - 90.f, vector3df());

    if (player->camera)
    {
        player->camera->setPosition(pos);
        player->camera->setTarget(pos + target);
        //! New. FPSCamera and animators catches reset on animate 0
        player->camera->OnAnimate(0);
    }

    return lastIndex - index + 1;
}

void LevelLoader::LoadBSPEntities(IQ3LevelMesh* mesh, ISceneNode* levelNode, IMesh* geometry)
{
    quake3::tQ3EntityList &entityList = mesh->getEntityList();
    quake3::IEntity search;
    search.name = "info_player_deathmatch";

    s32 index = entityList.binary_search(search);
    if (index >= 0)
    {
        s32 notEndList;
        do
        {
            const quake3::SVarGroup *group = entityList[index].getGroup(1);

            u32 parsepos = 0;
            const core::vector3df pos =
                quake3::getAsVector3df(group->get("origin"), parsepos);

            parsepos = 0;
            const f32 angle = quake3::getAsFloat(group->get("angle"), parsepos);

            core::vector3df target(0.f, 0.f, 1.f);
            target.rotateXZBy(angle);

            smgr->getActiveCamera()->setPosition(pos);
            smgr->getActiveCamera()->setTarget(pos + target);

            ++index;
            notEndList = index == 2;
        } while (notEndList);
    }

    isLevelLoading = false;
}

//Testing new AddEntityWithShader function
void LevelLoader::AddTestShaders()
{
    if (driver->getDriverType() == EDT_DIRECT3D9)
    {
        psFileName = "../Stone/media/d3d9.hlsl";
        vsFileName = "../Stone/media/d3d9.hlsl";
    }

    if (driver->getDriverType() == EDT_OPENGL)
    {
        psFileName = "../Stone/media/opengl.frag";
        vsFileName = "../Stone/media/opengl.vert";
    }

    s32 newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
        vsFileName, "vertexMain", video::EVST_VS_1_1,
        psFileName, "pixelMain", video::EPST_PS_1_1,
        mc, video::EMT_SOLID, 0, shadingLanguage);

    s32 newMaterialType2 = gpu->addHighLevelShaderMaterialFromFiles(
        vsFileName, "vertexMain", video::EVST_VS_1_1,
        psFileName, "pixelMain", video::EPST_PS_1_1,
        mc, video::EMT_TRANSPARENT_ADD_COLOR, 0, shadingLanguage);

    scene::ISceneNode* node = smgr->addCubeSceneNode(50);
    node->setPosition(core::vector3df(1000, 300, 1000));
    node->setMaterialTexture(0, driver->getTexture("media/wall.bmp"));
    node->setMaterialFlag(video::EMF_LIGHTING, true);
    node->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);

    smgr->addTextSceneNode(guienv->getBuiltInFont(),
        L"PS & VS & EMT_SOLID",
        video::SColor(255, 255, 255, 255), node);

    scene::ISceneNodeAnimator* anim = smgr->createRotationAnimator(
        core::vector3df(0, 0.3f, 0));
    node->addAnimator(anim);
    anim->drop();

    node = smgr->addCubeSceneNode(50);
    node->setPosition(core::vector3df(1050, 290, 1000));
    node->setMaterialTexture(0, driver->getTexture("media/wall.bmp"));
    node->setMaterialFlag(video::EMF_LIGHTING, false);
    node->setMaterialFlag(video::EMF_BLEND_OPERATION, true);
    node->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType2);

    smgr->addTextSceneNode(guienv->getBuiltInFont(),
        L"PS & VS & EMT_TRANSPARENT",
        video::SColor(255, 255, 255, 255), node);

    anim = smgr->createRotationAnimator(core::vector3df(0, 0.3f, 0));
    node->addAnimator(anim);
    anim->drop();

    node = smgr->addCubeSceneNode(50);
    node->setPosition(core::vector3df(1100, 300, 1000));
    node->setMaterialTexture(0, driver->getTexture("../Stone/media/wall.bmp"));
    node->setMaterialFlag(video::EMF_LIGHTING, true);
    smgr->addTextSceneNode(guienv->getBuiltInFont(), L"NO SHADER",
        video::SColor(255, 255, 255, 255), node);

    anim = smgr->createRotationAnimator(core::vector3df(0, 0.3f, 0));
    node->addAnimator(anim);
    anim->drop();
}

// Adds a skydome to the scene
void LevelLoader::AddSky(u32 dome, const c8 *texture)
{
    c8 buf[128];

    bool oldMipMapState = driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
    driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

    if (0 == dome)
    {
        // irrlicht order
        //static const c8*p[] = { "ft", "lf", "bk", "rt", "up", "dn" };
        // quake3 order
        static const c8*p[] = { "ft", "rt", "bk", "lf", "up", "dn" };

        u32 i = 0;
        snprintf(buf, 64, "%s_%s.jpg", texture, p[i]);
        SkyNode = smgr->addSkyBoxSceneNode(driver->getTexture(buf), 0, 0, 0, 0, 0);

        if (SkyNode)
        {
            for (i = 0; i < 6; ++i)
            {
                snprintf(buf, 64, "%s_%s.jpg", texture, p[i]);
                SkyNode->getMaterial(i).setTexture(0, driver->getTexture(buf));
            }
        }
    }
    else
        if (1 == dome)
        {
            snprintf(buf, 64, "%s.jpg", texture);
            SkyNode = smgr->addSkyDomeSceneNode(
                driver->getTexture(buf), 32, 32,
                1.f, 1.f, 1000.f, 0, 11);
        }
        else
            if (2 == dome)
            {
                snprintf(buf, 64, "%s.jpg", texture);
                SkyNode = smgr->addSkyDomeSceneNode(
                    driver->getTexture(buf), 16, 8,
                    0.95f, 2.f, 1000.f, 0, 11);
            }

    if (SkyNode)
        SkyNode->setName("skydome2");
    //SkyNode->getMaterial(0).ZBuffer = video::EMDF_DEPTH_LESS_EQUAL;

    driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, oldMipMapState);
}

//Adds a few cubes with shaders. Function for testing. We won't be using this.
void LevelLoader::AddEntityWithShader(s32 materialType, ISceneNode* node, IMesh* sceneNode, 
                                      ISceneNodeAnimator* animator)
{

}


