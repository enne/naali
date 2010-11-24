// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_IOpenSimSceneService_h
#define incl_Interfaces_IOpenSimSceneService_h

#include "IService.h"
#include "Vector3D.h"

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QUrl>
#include <QDir>

namespace Scene
{
    class Entity;
}

/// Service to import and export Naali scene xml to a opensim server or a local file
class IOpenSimSceneService : public QObject, public IService 
{

Q_OBJECT

public:
    virtual ~IOpenSimSceneService() {};

public slots:
    /// Publish content from a web xml file
    /// @param QUrl - url to the xml description file
    /// @param bool - if set position will be adjusted to instantiate in front of avatar
    virtual void PublishToServer(QUrl xml_url, Vector3df drop_position = Vector3df::ZERO) = 0;

    /// Publish content of the file to the active opensim server
    /// @param QString - filename to read the scene data
    /// @param bool - if set position will be adjusted to instantiate in front of avatar
    virtual void PublishToServer(const QString &load_filename, bool adjust_pos_to_avatar, Vector3df drop_position = Vector3df::ZERO) = 0;

    /// Publish content of the file to the active opensim server
    /// @param QByteArray - scene data
    /// @param bool - if set position will be adjusted to instantiate in front of avatar
    virtual void PublishToServer(const QByteArray &content, bool adjust_pos_to_avatar, Vector3df drop_position = Vector3df::ZERO) = 0;

    /// Store entities as a xml scene to file
    /// @param QString - filename where xml scene data will be stored
    /// @param QList of Scene::Entity* - entities to be exported to file
    virtual void StoreEntities(const QString &save_filename, QList<Scene::Entity *> entities) = 0;
    
    /// Store whole scene with assets and make it http publisable
    /// @param QDir - directory where to export NOTE: the folder will be emptied!
    /// @param QString - base asset url for asset references
    virtual void StoreSceneAndAssets(QDir store_location, const QString &asset_base_url) = 0;

    /// Export xml data to QByteArray and return it
    /// @param QList of Scene::Entity* - entities to be exported to file
    /// @return QByteArray - xml content
    virtual QByteArray ExportEntities(QList<Scene::Entity *> entities) = 0;

    /// Gets a position in front of the avatar, used for drops when raycast does not hit any object
    virtual Vector3df GetPosFrontOfAvatar() = 0;
};

typedef boost::shared_ptr<IOpenSimSceneService> OpenSimSceneServicePtr;

#endif
