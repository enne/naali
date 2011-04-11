/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_PlanarMirror.cpp
 *  @brief  EC_PlanarMirror enables one to create planar mirrors.
 *  @note   The entity should have EC_Placeable available in advance.
 */

#include "StableHeaders.h"
#include "EC_PlanarMirror.h"

#include "Renderer.h"
#include "Entity.h"
#include "EC_Placeable.h"
#include "LoggingFunctions.h"
#include "FrameAPI.h"
#include "EC_OgreCamera.h"
#include "EC_RttTarget.h"
#include "UiAPI.h"
#include "NaaliMainWindow.h"

DEFINE_POCO_LOGGING_FUNCTIONS("EC_PlanarMirror")

int EC_PlanarMirror::mirror_cam_num_ = 0;

EC_PlanarMirror::EC_PlanarMirror(IModule *module)
    :IComponent(module->GetFramework()),
    reflectionPlaneVisible(this, "Show reflection plane", true),
    mirror_texture_(0),
    tex_unit_state_(0),
    mat_(0),
    mirror_plane_entity_(0),
    mirror_plane_(0)
{
    connect(this, SIGNAL(ParentEntitySet()), this, SLOT(Initialize()));
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)),
            SLOT(AttributeUpdated(IAttribute*)));
}

EC_PlanarMirror::~EC_PlanarMirror()
{
    if(renderer_.expired())
        return;
    Ogre::SceneManager *mngr = renderer_.lock()->GetSceneManager();
    tex_unit_state_->setProjectiveTexturing(false);
    mngr->destroyEntity(mirror_plane_entity_);
    SAFE_DELETE(mirror_plane_);
}

void EC_PlanarMirror::AttributeUpdated(IAttribute* attr)
{
    if (!ViewEnabled())
        return;

    if(attr->GetNameString()=="Show Reflection Plane")
        mirror_plane_entity_->setVisible(getreflectionPlaneVisible());
}

void EC_PlanarMirror::Update(float val)
{
    if (!ViewEnabled())
        return;

    if(renderer_.expired())
        return;

    const Ogre::Camera* cam = renderer_.lock()->GetCurrentCamera();

    if(mirror_cam_)
    {
        mirror_cam_->setOrientation(cam->getRealOrientation());
        mirror_cam_->setPosition(cam->getRealPosition());
        //mirror_cam_->getViewport()->update();
    }
}

void EC_PlanarMirror::Initialize()
{
    if (!ViewEnabled())
        return;

    renderer_ = framework_->GetServiceManager()->GetService<OgreRenderer::Renderer>(Service::ST_Renderer);
    assert(!renderer_.expired());
    
    Scene::Entity *entity = GetParentEntity();
    assert(entity);
    if (!entity)
        return;

    EC_OgreCamera *cam = entity->GetComponent<EC_OgreCamera>().get();
    assert(cam);
    if (!cam)
    {
        LogError("No EC_OgreCamera on this entity. EC_PlanarMirror can't function.");
        return;
    }
    EC_Placeable *placeable = entity->GetComponent<EC_Placeable>().get();
    assert(placeable);
    if (!placeable)
    {
        LogError("No EC_Placeable on this entity. EC_PlanarMirror can't function.");
        return;
    }

    EC_RttTarget *target = entity->GetComponent<EC_RttTarget>().get();
    assert(target);
    if (!target)
    {
        LogError("No EC_RttTarget on this entity. EC_PlanarMirror can't function.");
        return;
    }

    const Ogre::Camera* v_cam = renderer_.lock()->GetCurrentCamera();
    const Ogre::Viewport* vp = renderer_.lock()->GetViewport();

    mirror_cam_ = cam->GetCamera();
    mirror_cam_->setFarClipDistance(v_cam->getFarClipDistance());
    mirror_cam_->setNearClipDistance(v_cam->getNearClipDistance());
    mirror_cam_->setAutoAspectRatio(false);
    mirror_cam_->setAspectRatio(Ogre::Real(vp->getActualWidth())/Ogre::Real(vp->getActualHeight()));
    mirror_cam_->setFOVy(v_cam->getFOVy());

    QString texname = target->gettargettexture();
    Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName(Ogre::String(texname.toStdString()));
    
    if (!tex.get())
    {
        LogError("Texture called " + texname.toStdString() +  " not found. EC_PlanarMirror can't function. (Propably something wrong in the EC_RttTarget).");
        return;
    }
    mirror_texture_ = tex.get();

    CreatePlane();
    placeable->GetSceneNode()->attachObject(mirror_plane_entity_);
    placeable->GetSceneNode()->attachObject(mirror_plane_);

    mirror_cam_->enableCustomNearClipPlane(mirror_plane_);
    mirror_cam_->enableReflection(mirror_plane_);

    mirror_cam_num_++;
    disconnect(this, SIGNAL(ParentEntitySet()), this, SLOT(Initialize()));

    mirror_cam_->getViewport()->setOverlaysEnabled(false);

    connect(framework_->Frame(), SIGNAL(Updated(float)), this, SLOT(Update(float)));
    connect(framework_->Ui()->MainWindow(), SIGNAL(WindowResizeEvent(int, int)), this, SLOT(WindowResized(int,int)));
}

void EC_PlanarMirror::WindowResized(int w,int h)
{
    if (!ViewEnabled())
        return;

    if(!renderer_.expired())
    {
        const Ogre::Viewport* vp = renderer_.lock()->GetViewport();
        mirror_cam_->setAspectRatio(Ogre::Real(vp->getActualWidth())/Ogre::Real(vp->getActualHeight()));
    }
}

void EC_PlanarMirror::CreatePlane()
{
    if (!ViewEnabled())
        return;

    mirror_plane_ = new Ogre::MovablePlane("mirror_plane" + Ogre::StringConverter::toString(mirror_cam_num_));
    mirror_plane_->d = 0;
    mirror_plane_->normal = Ogre::Vector3::UNIT_Y;
    Ogre::MeshManager::getSingleton().createPlane("mirror_plane_mesh_" + Ogre::StringConverter::toString(mirror_cam_num_),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *mirror_plane_, 80, 80, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
    mirror_plane_entity_ = renderer_.lock()->GetSceneManager()->createEntity("mirror_plane_entity" + Ogre::StringConverter::toString(mirror_cam_num_),
        "mirror_plane_mesh_" + Ogre::StringConverter::toString(mirror_cam_num_));

    ///for now
    mat_ = dynamic_cast<Ogre::Material*>(Ogre::MaterialManager::getSingleton().create(
        "MirrorMat" + Ogre::StringConverter::toString(mirror_cam_num_), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).get());
    tex_unit_state_ = mat_->getTechnique(0)->getPass(0)->createTextureUnitState(mirror_texture_->getName());
    tex_unit_state_->setProjectiveTexturing(true, mirror_cam_);
    tex_unit_state_->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    Ogre::MaterialPtr mat(mat_);
    mirror_plane_entity_->setMaterial(mat);
}

Ogre::Texture* EC_PlanarMirror::GetMirrorTexture() const
{
    return mirror_texture_;
}


