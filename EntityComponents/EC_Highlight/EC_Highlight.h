/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Highlight.h
 *  @brief  EC_Highlight enables visual highlighting effect for of scene entity.
 *  @note   The entity must have EC_OgrePlaceable and EC_Mesh (if mesh) or
 *          EC_OgreCustomObject (if prim) components available in advance.
 */

#ifndef incl_EC_Highlight_EC_Highlight_h
#define incl_EC_Highlight_EC_Highlight_h

#include "IComponent.h"
#include "Declare_EC.h"

namespace OgreRenderer
{
    class Renderer;
}

namespace Ogre
{
    class SceneNode;
    class Entity;
}
/**
<table class="header">
<tr>
<td>
<h2>HighLight</h2>
Enables visual highlighting effect for of scene entity.

Registered by RexLogic::RexLogicModule.

<b>No Attributes.</b>

<b>Exposes the following scriptable functions:</b>
<ul>
<li>"hide": Disables the highlighting effect.
<li>"show": Shows the highlighting effect.
<li>"IsVisible": Returns if the highlight component is visible or not.
  @true If the highlight component is visible, false if it's hidden or not initialized properly.
</ul>

<b>Reacts on the following actions:</b>
<ul>
<li>...
</ul>
</td>
</tr>

Does not emit any actions.

<b>Depends on components OgrePlaceable, OgreMesh and OgreCustomObject</b>. 
</table>
*/


class EC_Highlight : public IComponent
{
    Q_OBJECT
    DECLARE_EC(EC_Highlight);

public:
    /// Destructor.
    ~EC_Highlight();

public slots:
    /// Shows the highlighting effect.
    void Show();

    /// Hides the highlighting effect.
    void Hide();

    /// Returns if the highlight component is visible or not.
    /// @true If the highlight component is visible, false if it's hidden or not initialized properly.
    bool IsVisible() const;

private:
    /// Constuctor.
    /// @param module Owner module.
    explicit EC_Highlight(IModule *module);

    /// Creates the clone entity used for highlighting from the original.
    void Create();

    /// Renderer pointer.
    boost::weak_ptr<OgreRenderer::Renderer> renderer_;

    /// Ogre entity clone created for highlighting.
    Ogre::Entity *entityClone_;

    /// Ogre scene node where this EC is attached.
    Ogre::SceneNode *sceneNode_;

    /// Name of the cloned entity used for highlighting
    std::string cloneName_;
};

#endif
