// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_OpenALAudio_EC_AttachedSound_h
#define incl_OpenALAudio_EC_AttachedSound_h

#include "IComponent.h"
#include "RexLogicModuleApi.h"
#include "Vector3D.h"
#include "Declare_EC.h"

#include <QVector3D>

namespace RexLogic
{
/// Component which holds references to attached spatial sounds.
/**
<table class="header">
<tr>
<td>
<h2>AttachedSound</h2>
Component which holds references to attached spatial sounds.
Their positions will be updated automatically to match the position of the entity
To use, first setup the sound usually using the sound service, then use AddSound()
To be useful, the entity also needs the EC_Placeable component.

Registered by RexLogic::RexLogicModule.

<b>No Attributes</b>

<b>Exposes the following scriptable functions:</b>
<ul>
<li>"SetSound": Set a new sound and properties for it
        @param name The asset_id of the sound to add
        @param pos The position for the sound to play at
        @param soundRadius The radius for the sound to be heard at
        @param soundVolume The volume for the sound to play at
<li>"AddSound": Add sound to entity. 
        @param sound Channel id from ISoundService
        @param slot Sound slot definition
<li>"RemoveSound": Stop and remove sound from entity by channel id or by sound slot
        @param sound Channel id from ISoundService
        or
        @param slot Sound slot definition, only OpenSimAttachedSound or RexAmbientSound will have effect
<li>"RemoveAllSounds": Stop and remove all sounds from entity.
        Also called automatically upon destruction of this EntityComponent.
<li>"SetPosition":Syncs position. Called from RexLogicModule.
</ul>

<b>Reacts on the following actions:</b>
<ul>
<li>...
</ul>
</td>
</tr>

Does not emit any actions.

<b>Doesn't depend on other components.</b>
</table>
*/
    class REXLOGIC_MODULE_API EC_AttachedSound : public IComponent
    {
        Q_OBJECT
        Q_ENUMS(SoundSlot)

        DECLARE_EC(EC_AttachedSound);
    public:
        //! Sound slot definitions. 
        /*! When a new sound is added to either OpenSimAttachedSound or RexAmbientSound slot, the previous will be killed.
            "Other" slot on the other hand allows as many sounds to be added as desired
         */
        enum SoundSlot
        {
            OpenSimAttachedSound = 0,
            RexAmbientSound,
            Other
        };

        virtual ~EC_AttachedSound();

    public slots:
        //! Set a new sound and properties for it
        /*! \param name The asset_id of the sound to add
            \param pos The position for the sound to play at
            \param soundRadius The radius for the sound to be heard at
            \param soundVolume The volume for the sound to play at
        */
        void SetSound(const QString& name, QVector3D& pos, float soundRadius, float soundVolume);

        //! Add sound to entity. 
        /*! \param sound Channel id from ISoundService
            \param slot Sound slot definition
         */
        void AddSound(sound_id_t sound, SoundSlot slot = Other);

        //! Stop and remove sound from entity by channel id
        /*! \param sound Channel id from ISoundService
         */
        void RemoveSound(sound_id_t sound);

        //! Stop and remove sound from entity by sound slot
        /*! \param slot Sound slot definition, only OpenSimAttachedSound or RexAmbientSound will have effect
         */
        void RemoveSound(SoundSlot slot);

        //! Stop and remove all sounds from entity.
        /*! Also called automatically upon destruction of this EntityComponent.
         */
        void RemoveAllSounds();

        //! Syncs position. Called from RexLogicModule.
        void SetPosition(const Vector3df &position);

    public:
        //! Get channel ids of currently playing sounds. Note that some may be zero (stopped)
        const std::vector<sound_id_t>& GetSounds() const { return sounds_; }

        //! Performs per-frame update, if necessary. Called from RexLogicModule.
        void Update(f64 frametime);

    private:
        EC_AttachedSound(IModule *module);

        //! Init sound slot vector
        void InitSoundVector();

        //! Vector of current attached sounds
        std::vector<sound_id_t> sounds_;
    };
}

#endif
