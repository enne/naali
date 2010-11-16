// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "ISoundService.h"
#include "IModule.h"
#include "ServiceManager.h"
#include "EC_AttachedSound.h"

namespace RexLogic
{
    EC_AttachedSound::EC_AttachedSound(IModule *module) :
        IComponent(module->GetFramework())
    {
        InitSoundVector();
    }

    EC_AttachedSound::~EC_AttachedSound()
    {
        RemoveAllSounds();
    }

    void EC_AttachedSound::Update(f64 frametime)
    {
        ISoundService *soundsystem = framework_->GetService<ISoundService>();
        if (!soundsystem)
            return;

        // Check if any of the sounds have stopped, remove from list in that case
        for(uint i = 0; i < sounds_.size(); ++i)
            if (sounds_[i])
                if (soundsystem->GetSoundState(sounds_[i]) == ISoundService::Stopped)
                    sounds_[i] = 0;
    }

    void EC_AttachedSound::SetPosition(const Vector3df &position)
    {
        ISoundService *soundsystem = framework_->GetService<ISoundService>();
        if (!soundsystem)
            return;

        for (uint i = 0; i < sounds_.size(); ++i)
            if (sounds_[i])
                soundsystem->SetPosition(sounds_[i], position);
    }

    void EC_AttachedSound::RemoveSound(sound_id_t sound)
    {
        if (sound == 0)
            return;

        ISoundService *soundsystem = framework_->GetService<ISoundService>();

        for (uint i = 0; i < sounds_.size(); ++i)
            if (sounds_[i] == sound)
            {
                if (soundsystem)
                    soundsystem->StopSound(sounds_[i]);
                sounds_[i] = 0;
            }
    }

    void EC_AttachedSound::RemoveSound(SoundSlot slot)
    {
        ISoundService *soundsystem = framework_->GetService<ISoundService>();

        uint i = (uint)slot;
        if (i < (uint)Other)
            if (sounds_[i])
            {
                if (soundsystem)
                    soundsystem->StopSound(sounds_[i]);
                sounds_[i] = 0;
            }
    }

    void EC_AttachedSound::RemoveAllSounds()
    {
        ISoundService *soundsystem = framework_->GetService<ISoundService>();
        if (soundsystem)
            for (uint i = 0; i < sounds_.size(); ++i)
                if (sounds_[i])
                    soundsystem->StopSound(sounds_[i]);

        InitSoundVector();
    }

    void EC_AttachedSound::InitSoundVector()
    {
        // Have always room for the opensim & rex-style ambient sounds 
        sounds_.clear();
        sounds_.resize((uint)Other);
        sounds_[OpenSimAttachedSound] = 0;
        sounds_[RexAmbientSound] = 0;
    }

    void EC_AttachedSound::SetSound(const QString& name, QVector3D& pos, float soundRadius, float soundVolume)
    {
        ISoundService *soundsystem = framework_->GetService<ISoundService>();
        if (!soundsystem)
            return;

        sound_id_t sound_id = soundsystem->PlaySound3D(name, ISoundService::Ambient, false, Vector3df(pos.x(), pos.y(), pos.z()), 0);

        this->AddSound(sound_id, EC_AttachedSound::RexAmbientSound); 

        // The ambient sounds will always loop
        soundsystem->SetLooped(sound_id, true);

        // Set sound radius
        if(soundRadius > 0.0)
        {
            soundsystem->SetPositional(sound_id, true);
            soundsystem->SetRange(sound_id, 0.0f, soundRadius, 2.0f);
        }
        else
        {
            soundsystem->SetPositional(sound_id, false);
        }
        // set volume
        soundsystem->SetGain(sound_id, soundVolume);
    }

    void EC_AttachedSound::AddSound(sound_id_t sound, SoundSlot slot)
    {
        if (sound == 0)
            return;

        if (slot != Other)
        {
            // Remove previous sound from slot first
            RemoveSound(slot);
            sounds_[slot] = sound;
        }
        else
        {
            for (uint i = (uint)Other; i < sounds_.size(); ++i)
                if (sounds_[i] == 0)
                {
                    sounds_[i] = sound;
                    return;
                }

            sounds_.push_back(sound);
        }
    }
}
