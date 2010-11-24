// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "EC_Sound.h"
#include "IModule.h"
#include "Framework.h"
#include "Entity.h"
#include "EC_Placeable.h"
#include "SceneManager.h"
#include "ISoundService.h"

#include "LoggingFunctions.h"
DEFINE_POCO_LOGGING_FUNCTIONS("EC_Sound")

#include "MemoryLeakCheck.h"

EC_Sound::EC_Sound(IModule *module):
    IComponent(module->GetFramework()),
    sound_id_(0),
    soundId(this, "Sound ref"),
    soundInnerRadius(this, "Sound radius inner", 0.0f),
    soundOuterRadius(this, "Sound radius outer", 20.0f),
    loopSound(this, "Loop sound", false),
    triggerSound(this, "Trigger sound", false),
    soundGain(this, "Sound gain", 1.0f)
{
    static AttributeMetadata metaData("", "0", "1", "0.1");
    soundGain.SetMetadata(&metaData);

    connect(this, SIGNAL(ParentEntitySet()), SLOT(UpdateSignals()));
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)),
            SLOT(AttributeUpdated(IAttribute*)));
}

EC_Sound::~EC_Sound()
{
    StopSound();
}

void EC_Sound::AttributeUpdated(IAttribute *attribute)
{
    if(attribute->GetNameString() == soundId.GetNameString())
    {
        ISoundService *soundService = framework_->GetService<ISoundService>();
        if(soundService && soundService->GetSoundName(sound_id_) != soundId.Get())
            StopSound();
    }
    else if(attribute->GetNameString() == triggerSound.GetNameString())
    {
        // Play sound if sound asset id has been setted and if sound has been triggered or looped.
        if(triggerSound.Get() == true && (!soundId.Get().isNull() || loopSound.Get()))
            PlaySound();
    }
    UpdateSoundSettings();
}

void EC_Sound::RegisterActions()
{
    Scene::Entity *entity = GetParentEntity();
    assert(entity);
    if (entity)
    {
        entity->ConnectAction("PlaySound", this, SLOT(PlaySound()));
        entity->ConnectAction("StopSound", this, SLOT(StopSound()));
    }
}

void EC_Sound::PositionChange(const QVector3D &pos)
{
    EC_Placeable *placeable = qobject_cast<EC_Placeable*>(sender());
    ISoundService *soundService = framework_->Audio();
    if(soundService && placeable && sound_id_)
    {
        Vector3df position(pos.x(), pos.y(), pos.z());
        soundService->SetPosition(sound_id_, position);
    }
}

void EC_Sound::PlaySound()
{
    triggerSound.Set(false, AttributeChange::LocalOnly);
    ComponentChanged(AttributeChange::LocalOnly);

    ISoundService *soundService = framework_->Audio();
    if(!soundService)
    {
        LogWarning("Failed to get sound service from the framework.");
        return;
    }

    // If previous sound is still playing stop it before we apply a new sound.
    if(sound_id_)
        StopSound();

    EC_Placeable *placeable = dynamic_cast<EC_Placeable *>(FindPlaceable().get());
    if(placeable)
    {
        sound_id_ = soundService->PlaySound3D(soundId.Get(), ISoundService::Triggered, false, placeable->GetPosition());
        soundService->SetGain(sound_id_, soundGain.Get());
        soundService->SetLooped(sound_id_, loopSound.Get());
        soundService->SetRange(sound_id_, soundInnerRadius.Get(), soundOuterRadius.Get(), 2.0f);
    }
    else // If entity isn't holding placeable component treat sound as ambient sound.
    {
        sound_id_ = soundService->PlaySound(soundId.Get(), ISoundService::Ambient);
        soundService->SetGain(sound_id_, soundGain.Get());
    }
}

void EC_Sound::StopSound()
{
    ISoundService *soundService = framework_->GetService<ISoundService>();
    if (soundService)
        soundService->StopSound(sound_id_);

    sound_id_ = 0;
}

void EC_Sound::UpdateSoundSettings()
{
    ISoundService *soundService = framework_->Audio();
    if(!soundService || !sound_id_)
    {
        //LogWarning("Cannot update the sound settings cause sound service is not intialized or a sound wasn't on active state.");
        return;
    }

    soundService->SetGain(sound_id_, soundGain.Get());
    soundService->SetLooped(sound_id_, loopSound.Get());
    soundService->SetRange(sound_id_, soundInnerRadius.Get(), soundOuterRadius.Get(), 2.0f);
}

void EC_Sound::UpdateSignals()
{
    if (!GetParentEntity())
    {
        LogError("Couldn't update singals cause component dont have parent entity setted.");
        return;
    }
    Scene::SceneManager *scene = GetParentEntity()->GetScene();
    if(!scene)
    {
        LogError("Fail to update signals cause parent entity's scene is null.");
        return;
    }

    RegisterActions();
}

ComponentPtr EC_Sound::FindPlaceable() const
{
    assert(framework_);
    ComponentPtr comp;
    if(!GetParentEntity())
    {
        LogError("Fail to find a placeable component cause parent entity is null.");
        return comp;
    }
    comp = GetParentEntity()->GetComponent<EC_Placeable>();
    //We need to update sound source position when placeable component has changed it's transformation.
    connect(comp.get(), SIGNAL(PositionChanged(const QVector3D &)),
            SLOT(PositionChange(const QVector3D &)), Qt::UniqueConnection);
    return comp;
}