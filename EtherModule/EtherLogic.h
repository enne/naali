// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_EtherModule_EtherLogic_h
#define incl_EtherModule_EtherLogic_h

#include "View/EtherScene.h"
#include "EtherSceneController.h"

#include <QObject>
#include <QUuid>
#include <QPair>
#include <QRectF>

namespace Foundation
{
    class Framework;
}

namespace Ether
{
    enum ConnectionState
    {
        Connected,
        Disconnected,
        Failed,
        Kicked
    };

    namespace Logic
    {
        class EtherLogic : public QObject
        {
            Q_OBJECT

        public:
            EtherLogic(Foundation::Framework *framework);
            virtual ~EtherLogic();

        public slots:
            void Start();

            void PrintAvatarMap();
            void PrintWorldMap();

            void SetConnectionState(Ether::ConnectionState connection_state, const QString &message = "");

            View::EtherScene *GetScene() const { return scene_; }
            EtherSceneController *GetSceneController() const { return scene_controller_; }
            QObject *GetQObjSceneController() { return (QObject*)scene_controller_; }

            EtherLoginNotifier *GetLoginNotifier() const { return login_notifier_; }
            QPair<QString, QString> GetLastLoginScreenshotData(std::string conf_path);
            void UpdateUiPixmaps();
			void LoginSuccesful();

        private slots:
            void SetVisibleItems();
            void StoreDataToFilesIfEmpty();
            void GenerateAvatarInfoCards();
            void GenerateWorldInfoCards();

            void ParseInfoFromCards(QPair<View::InfoCard*, View::InfoCard*> ui_cards);

            void TitleUpdate(QUuid card_uuid, QString new_title);
            void RemoveObjectFromData(QUuid uuid);
            void AvatarCreated(Data::AvatarInfo *avatar_data);
            void WorldCreated(Data::WorldInfo *world_data);

			//! Handles Tundra login (send an event, which TundraLogicModule will handle)
			void ProcessTundraLogin(const QMap<QString, QString> &data);

        private:
            Foundation::Framework *framework_;

            Data::DataManager *data_manager_;
            View::EtherScene *scene_;
            EtherSceneController *scene_controller_;
            EtherLoginNotifier *login_notifier_;

            QGraphicsScene *previous_scene_;

            QMap<QUuid, Data::AvatarInfo*> avatar_map_;
            QMap<QUuid, Data::WorldInfo*> world_map_;
            QMap<QUuid, View::InfoCard*> avatar_card_map_;
            QMap<QUuid, View::InfoCard*> world_card_map_;

            QPair<Data::AvatarInfo*, Data::WorldInfo*> last_login_cards_;

            QRectF card_size_;

            int top_menu_visible_items_;
            int bottom_menu_visible_items_;
			//To know if it has been disconnected or kicked out
			bool is_user_connecting_;
        };
    }
}

#endif // ETHERLOGIC_H
