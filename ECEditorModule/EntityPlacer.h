// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_ECEditorModule_EntityPlacer_h
#define incl_ECEditorModule_EntityPlacer_h

#include "ForwardDefines.h"
#include "CoreTypes.h"
#include "InputContext.h"
#include "Vector3D.h"
#include "Quaternion.h"

namespace OgreRenderer
{
    class EC_OgrePlaceable;
    class Renderer;
    typedef boost::weak_ptr<Renderer> RendererWeakPtr;
}

namespace Ogre
{
    class SceneNode;
    class Entity;
}

namespace ECEditor
{
    class EntityPlacer: public QObject
    {
        Q_OBJECT
    public:
        //! If entity that we want to place in our world wont hold any mesh component. We use default mesh to
        //! tell the user where hes placing his mesh (e.g. placing sound source on the scene).
        //static const std::string CustomMeshName;

        EntityPlacer(Foundation::Framework *framework, entity_id_t entityId = 0, QObject *parent = 0);
        virtual ~EntityPlacer();

    public slots:
        void OnMouseEvent(MouseEvent *mouse);
        void MouseMove(MouseEvent *mouse);

    signals:
        void Finnished(Vector3df location, Quaternion orientation);
        void LocationChanged(Vector3df location);
        void OrientationChanged(Quaternion orientation);

    private:
         bool DoRayCast(int x, int y, Vector3df &result);

    private:
        Vector3df location_;
        Quaternion orientation_;
        InputContextPtr input_;
        OgreRenderer::EC_OgrePlaceable *placeable_;
        Foundation::Framework *framework_;
        
        Scene::EntityWeakPtr entity_;
        Ogre::Entity *meshEntity_;
        OgreRenderer::RendererWeakPtr renderer_;
        int previousScrollValue_;

        bool finnished_;
        bool useCustomMesh_;
    };
}

#endif