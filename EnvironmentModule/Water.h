// For conditions of distribution and use, see copyright notice in license.txt

//! @file Water.h
//! @brief Manages Water-related environment module.

#ifndef incl_Water_h
#define incl_Water_h

#include "EnvironmentModuleApi.h"
#include "ForwardDefines.h"
#include <AttributeChangeType.h>
#include <QObject>

namespace Environment
{
    class EnvironmentModule;
    class EC_WaterPlane;

    /**
     * Water class is used to control default water plane, this is depricated way to control environment water. 
     * This class handles all environment editor changes, and deals RexDrawWater-message information from server. 
     * Note! This class uses a idea of Environment - entity. IF there exist a entity in world which has component @p EC_NAME
     * which name attribute is WaterEnvironment, then it will be used as enviroment control entity (and all syncronization of ocean etc.) is done through
     * that entity. IF there does not exist a entity EnviromenModule will (and this class) create LocalEnviroment entity, which owns ocean etc. this will be not syncronizable.
     *
     */

    class ENVIRONMENT_MODULE_API Water : public QObject
    {
        Q_OBJECT 

        public:
            //! Constructor.
            //! @param owner EnvironmentModule pointer.
            Water(EnvironmentModule *owner);

            //! Destructor.
            virtual ~Water();

            void Update();

            /**
             * Exist there waterplane?
             * @returns true if there is false if not. 
             */

            bool IsWaterPlane() const;
            
            /**
             * Creates water geometry, uses a given value to water height. 
             * @param height is a water height for newly created water plane.
             *
             **/
            
            void CreateWaterGeometry(float height = 20.f, AttributeChange::Type change = AttributeChange::LocalOnly);

            /** 
             * Removes water geometry totally. 
             */
            void RemoveWaterGeometry();

            //! @return The scene water height.
            //! @note If error occuers returns 0.0
            float GetWaterHeight();
        
             /** 
              * Sets a water fog for default water plane.
              * @param fogStart distance in world unit at which linear fog start ot encroach. 
              * @param fogEnd distance in world units at which linear fog becomes completely opaque.
              * @param color the colour of the fog. 
              **/
            
            void SetWaterFog(float fogStart, float fogEnd, const QVector<float>& color);
 
             /**
              * Set new water fog color.
              * @param new color value.
              **/
            
            void SetWaterFogColor(const QVector<float>& color);

             /**
              * Set new underwater fog distance.
              * @param fogStart start distance from the viewpoint.
              * @param fogEnd end distance from the viewpoint.
              **/
            
            void SetWaterFogDistance(float fogStart, float fogEnd);

             /**
              * @return underwater fog start distance. 
              */
            float GetWaterFogStartDistance();

            /**
             * @return underwater fog end distance. 
             */
            float GetWaterFogEndDistance();

             /** 
              * Returns current fog water color.
              **/
            QVector<float> GetFogWaterColor();


        public slots:

            //! Sets a new water height to scene water.
            //! height New water height.
            void SetWaterHeight(float height, AttributeChange::Type type );

        signals:
            //! Emited when water height has been changed.
            //! height New water height.
            void HeightChanged(double height);

            //! Emitted when water fog is adjusted.
            void WaterFogAdjusted(float fogStart, float fogEnd, const QVector<float>& color);

            //! Emitted in each update loop.
            void ExistWater(bool on);
          

        private:

            
            //! EnvironmentModule pointer.
            EnvironmentModule *owner_;

            EC_WaterPlane* GetEnvironmentWaterComponent();

           
            
    };
}

#endif
