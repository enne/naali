// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_TtsChatModule_Provider_h
#define incl_TtsChatModule_Provider_h

#include <QObject>
#include "TtsProviderInterface.h"
#include "TtsSessionInterface.h"
#include "TtsService.h"
#include "NetworkEvents.h"
#include "Framework.h"
#include "EventManager.h"

namespace Foundation
{
    class Framework;
    class EventDataInterface;
}

namespace TTS
{
		class TtsChatProvider : public TtsProviderInterface
        {
            Q_OBJECT

        public:
            TtsChatProvider(Foundation::Framework* framework);
            virtual ~TtsChatProvider();
            virtual TtsSessionInterface* Session();
            virtual QString& Description();
            virtual bool HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface* data);
			virtual void RegisterToTtsService(); 
			virtual void Update(f64 frametime);
			virtual Type getType();
			virtual void CloseSession();

		private:
            Foundation::Framework* framework_;
            QString description_;
			TtsSessionInterface* session_;
            event_category_id_t networkstate_event_category_;
			Type type_;
        };

} // TTS

#endif // incl_TtsChatModule_Provider_h
