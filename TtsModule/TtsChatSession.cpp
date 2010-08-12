#include "StableHeaders.h"

#include "TtsChatSession.h"

#include "Vector3D.h"
#include "EC_OgrePlaceable.h" // for avatar position
#include "EC_OpenSimPresence.h" // for avatar position
#include "ModuleManager.h"    // for avatar info
#include "WorldLogicInterface.h" // for avatar position
#include "Entity.h" // for avatar position
#include "SceneManager.h"

#include "TtsModule.h"

namespace TTS
{

		std::string TtsChatSession::type_name_static_ = "Tts";

        TtsChatSession::TtsChatSession(Foundation::Framework* framework) : 
            state_(STATE_CONNECTING),
            framework_(framework),
            description_("Esta es la sesión del tts")
        {
        }

        TtsChatSession::~TtsChatSession()
        {
        }

        void TtsChatSession::Close()
        {
            if (state_ != STATE_CLOSED && state_ != STATE_ERROR)
            {
                state_ = STATE_CLOSED;
            }
        }

        TtsSessionInterface::State TtsChatSession::GetState() const
        {
            return state_;
        }

        QString TtsChatSession::Description() const
        {
            return description_;
        }

        void TtsChatSession::Update(f64 frametime)
        {
        }
		void TtsChatSession::SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message)
		{
			//Aqui llamamos al festival según las opciones establecidas, por ahora solo muestra un mensaje
			LogError("Manda hablar al festival");
		}
        bool TtsChatSession::GetOwnAvatarPosition(Vector3df& position, Vector3df& direction)
        {
            using namespace Foundation;
            boost::shared_ptr<WorldLogicInterface> world_logic = framework_->GetServiceManager()->GetService<WorldLogicInterface>(Service::ST_WorldLogic).lock();
            if (!world_logic)
                return false;

            Scene::EntityPtr user_avatar = world_logic->GetUserAvatarEntity();
            if (!user_avatar)
                return false;

            boost::shared_ptr<OgreRenderer::EC_OgrePlaceable> ogre_placeable = user_avatar->GetComponent<OgreRenderer::EC_OgrePlaceable>();
            if (!ogre_placeable)
                return false;

            Quaternion q = ogre_placeable->GetOrientation();
            position = ogre_placeable->GetPosition(); 
            direction = q*Vector3df::UNIT_Z;

            return true;
        }

        QString TtsChatSession::OwnAvatarId()
        {
            using namespace Foundation;
            boost::shared_ptr<WorldLogicInterface> world_logic = framework_->GetServiceManager()->GetService<WorldLogicInterface>(Service::ST_WorldLogic).lock();
            if (!world_logic)
                return "";

            Scene::EntityPtr user_avatar = world_logic->GetUserAvatarEntity();
            if (!user_avatar)
                return "";

            boost::shared_ptr<EC_OpenSimPresence> opensim_presence = user_avatar->GetComponent<EC_OpenSimPresence>();
            if (!opensim_presence)
                return "";

            return opensim_presence->agentId.ToQString();
        }

        QString TtsChatSession::GetAvatarFullName(QString uuid) const
        {
            Scene::ScenePtr current_scene = framework_->GetDefaultWorldScene();
            if (current_scene.get())
            {
                for(Scene::SceneManager::iterator iter = current_scene->begin(); iter != current_scene->end(); ++iter)
                {
                    Scene::Entity &entity = **iter;
                    EC_OpenSimPresence *presence_component = entity.GetComponent<EC_OpenSimPresence>().get();
                    if (presence_component)
                        if (presence_component->agentId.ToQString() == uuid)
                        {
                            QString name = ""; 
                            name = QString(presence_component->GetFullName().c_str());
                            if (name.length() == 0)
                                name = QString(presence_component->GetFirstName().c_str());
                            return name;
                        }
                }
            }
            return "";
        }

} // TTS