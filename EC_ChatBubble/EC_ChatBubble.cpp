/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_ChatBubble.cpp
 *  @brief  EC_ChatBubble Chat bubble component wich shows billboard with chat bubble and text on entity.
 */

#include "StableHeaders.h"
#include "EC_ChatBubble.h"
#include "ModuleInterface.h"
#include "Renderer.h"
#include "EC_OgrePlaceable.h"
#include "Entity.h"
#include "OgreMaterialUtils.h"

#include <Ogre.h>
#include <OgreBillboardSet.h>
#include <OgreTextureManager.h>
#include <OgreResource.h>

#include <QFile>
#include <QPainter>
#include <QTimer>

EC_ChatBubble::EC_ChatBubble(Foundation::ModuleInterface *module) :
    Foundation::ComponentInterface(module->GetFramework()),
    font_(QFont("Arial", 72)),
    bubbleColor_(QColor(82, 134, 255, 210)),
    textColor_(Qt::white),
    billboardSet_(0),
    billboard_(0)
{
/*
    boost::shared_ptr<OgreRenderer::Renderer> renderer = framework_->GetServiceManager()->GetService
        <OgreRenderer::Renderer>(Foundation::Service::ST_Renderer).lock();
    if (!renderer)
        return;

    Ogre::SceneManager *scene = renderer->GetSceneManager();
    assert(scene);
    if (!scene)
        return;

    Scene::Entity *entity = GetParentEntity();
    assert(entity);
    if (!entity)
        return;

    OgreRenderer::EC_OgrePlaceable *node = entity->GetComponent<OgreRenderer::EC_OgrePlaceable>().get();
    if (!node)
        return;

    sceneNode_ = node->GetSceneNode();
    assert(sceneNode_);

    billboardSet_ = scene->createBillboardSet(renderer->GetUniqueObjectName(), 1);
    assert(billboardSet_);

    std::string newName = std::string("material") + renderer->GetUniqueObjectName(); 
    Ogre::MaterialPtr material = OgreRenderer::CloneMaterial("UnlitTexturedSoftAlpha", newName);
    //OgreRenderer::SetTextureUnitOnMaterial(material, imageName);
    billboardSet_->setMaterialName(newName);

    billboard_ = billboardSet_->createBillboard(Ogre::Vector3(0,0,1.5));
    assert(billboard_);
    billboard_->setDimensions(1, 1);

    sceneNode_->attachObject(billboardSet_);
*/
}

EC_ChatBubble::~EC_ChatBubble()
{
}

void EC_ChatBubble::SetPosition(const Vector3df& position)
{
    if (billboard_)
        billboard_->setPosition(Ogre::Vector3(position.x, position.y, position.z));
}

void EC_ChatBubble::ShowMessage(const QString &msg)
{
    boost::shared_ptr<OgreRenderer::Renderer> renderer = framework_->GetServiceManager()->GetService
        <OgreRenderer::Renderer>(Foundation::Service::ST_Renderer).lock();
    if (!renderer)
        return;

    Ogre::SceneManager *scene = renderer->GetSceneManager();
    assert(scene);
    if (!scene)
        return;

    Scene::Entity *entity = GetParentEntity();
    assert(entity);
    if (!entity)
        return;

    OgreRenderer::EC_OgrePlaceable *node = entity->GetComponent<OgreRenderer::EC_OgrePlaceable>().get();
    if (!node)
        return;

    sceneNode_ = node->GetSceneNode();
    assert(sceneNode_);

    // Create billboard if it doesn't exist.
    if (!billboardSet_ && !billboard_)
    {
        billboardSet_ = scene->createBillboardSet(renderer->GetUniqueObjectName(), 1);
        assert(billboardSet_);

        std::string newName = std::string("material") + renderer->GetUniqueObjectName(); 
        Ogre::MaterialPtr material = OgreRenderer::CloneMaterial("UnlitTexturedSoftAlpha", newName);
        //OgreRenderer::SetTextureUnitOnMaterial(material, imageName);
        billboardSet_->setMaterialName(newName);

        billboard_ = billboardSet_->createBillboard(Ogre::Vector3(0,0,1.5));
        assert(billboard_);
        billboard_->setDimensions(2, 1);

        sceneNode_->attachObject(billboardSet_);
    }

    if (msg.isNull() || msg.isEmpty())
        return;

    messages_.push_back(msg);

    // Set timer for removing the message
    int time = msg.length() * 400;
    QTimer::singleShot(time, this, SLOT(RemoveLastMessage()));

    Refresh();
}

void EC_ChatBubble::RemoveLastMessage()
{
    messages_.pop_front();
    Refresh();
}

void EC_ChatBubble::RemoveAllMessages()
{
    messages_.clear();
    Refresh();
}

void EC_ChatBubble::Refresh()
{
    boost::shared_ptr<OgreRenderer::Renderer> renderer = framework_->GetServiceManager()->GetService
        <OgreRenderer::Renderer>(Foundation::Service::ST_Renderer).lock();
    if (!renderer)
        return;

    if (!sceneNode_ || !billboardSet_ || !billboard_)
        return;

    if (messages_.size() == 0)
    {
        billboardSet_->setVisible(false);
        return;
    }
    else
        billboardSet_->setVisible(true);

    // Get pixmap with chat bubble and text rendered to it.
    QPixmap pixmap = GetChatBubblePixmap();
    if (pixmap.isNull())
        return;

    // Create texture
    QImage img = pixmap.toImage();
    Ogre::DataStreamPtr stream(new Ogre::MemoryDataStream((void*)img.bits(), img.byteCount()));
    std::string tex_name("ChatBubbleTexture" + renderer->GetUniqueObjectName());
    Ogre::TextureManager &manager = Ogre::TextureManager::getSingleton();
    Ogre::Texture *tex = dynamic_cast<Ogre::Texture *>(manager.create(tex_name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).get());
    assert(tex);

    tex->loadRawData(stream, img.width(), img.height(), Ogre::PF_A8R8G8B8);

    // Set new material with the new texture name in it.
    std::string newMatName = std::string("material") + renderer->GetUniqueObjectName(); 
    Ogre::MaterialPtr material = OgreRenderer::CloneMaterial("UnlitTexturedSoftAlpha", newMatName);
    OgreRenderer::SetTextureUnitOnMaterial(material, tex_name);
    billboardSet_->setMaterialName(newMatName);
}

QPixmap EC_ChatBubble::GetChatBubblePixmap()
{
    int max_width = 1500;
    int max_height = 800;
    QRect max_rect(0, 0, max_width, max_height);

    const QString &filename("./media/textures/Transparent.png");
    assert(QFile::exists(filename));
    if (!QFile::exists(filename))
        return 0;

    // Gather chat log.
    QStringListIterator it(messages_);
    QString fullChatLog;
    while(it.hasNext())
    {
        fullChatLog.append(it.next());
        fullChatLog.append('\n');
    }

    // Create pixmap and painter.
    QPixmap pixmap;
    pixmap.load(filename);
    pixmap = pixmap.scaled(max_rect.size());

    QPainter painter(&pixmap);
    painter.setFont(font_);

    // Calculate the bounding rect size.
    QRect rect = painter.boundingRect(max_rect, Qt::AlignCenter | Qt::TextWordWrap, fullChatLog);
    // Set padding for text.
//    rect.setX(rect.x() - 20);
//    rect.setY(rect.y() - 20);
    pixmap = pixmap.scaled(rect.size());

    // Draw rounded rect.
    QBrush brush(bubbleColor_, Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawRoundedRect(rect, 20.0, 20.0);

    // Draw text
    painter.setPen(textColor_);
    painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, fullChatLog);

    return pixmap;
}
