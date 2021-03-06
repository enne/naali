# Define target name and output directory
init_target (RexLogicModule OUTPUT modules/core)

GetEntityComponents()

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
file (GLOB XML_FILES *.xml)
file (GLOB UI_FILES ui/*.ui)
file (GLOB MOC_FILES RexLogicModule.h EventHandlers/LoginHandler.h RexMovementInput.h
    EventHandlers/MainPanelHandler.h EntityComponent/EC_*.h Environment/Primitive.h Communications/*.h
    Communications/InWorldChat/*.h Camera/ObjectCameraController.h Camera/CameraControl.h NotificationWidget.h)

# SubFolders to project with filtering
AddSourceFolder (Avatar)
AddSourceFolder (EntityComponent)
AddSourceFolder (EventHandlers)
AddSourceFolder (Environment)
AddSourceFolder (Communications)
AddSourceFolder (Communications/InWorldChat)
AddSourceFolder (Camera)

# Qt4 Moc files to subgroup "CMake Moc"
MocFolder ()

set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

# Qt4 Wrap
QT4_WRAP_CPP(MOC_SRCS ${MOC_FILES})
QT4_WRAP_UI(UI_SRCS ${UI_FILES})

set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

add_definitions (-DREXLOGIC_MODULE_EXPORTS)

if (USE_UIMODULE)
    add_definitions (-DUSE_UIMODULE)
endif ()

use_package (OPENJPEG)

use_modules (RexLogicModule Core Foundation Interfaces Scene OgreRenderingModule AssetModule AvatarModule RexCommon
    HttpUtilities RpcUtilities ProtocolUtilities ProtocolModuleOpenSim ProtocolModuleTaiga EnvironmentModule
    Input Asset Audio Console)

if (USE_UIMODULE)
    use_modules (UiModule)
endif ()

build_library (${TARGET_NAME} SHARED ${SOURCE_FILES} ${MOC_SRCS} ${UI_SRCS})

LinkEntityComponent(EntityComponents/EC_HoveringText EC_HoveringText)
LinkEntityComponent(EntityComponents/EC_Clone EC_Clone)
LinkEntityComponent(EntityComponents/EC_OpenSimPresence EC_OpenSimPresence)
LinkEntityComponent(EntityComponents/EC_OpenSimPrim EC_OpenSimPrim)
LinkEntityComponent(EntityComponents/EC_ChatBubble EC_ChatBubble)
LinkEntityComponent(EntityComponents/EC_Ruler EC_Ruler)
LinkEntityComponent(EntityComponents/EC_SoundRuler EC_SoundRuler)
LinkEntityComponent(EntityComponents/EC_Gizmo EC_Gizmo)
LinkEntityComponent(EntityComponents/EC_Selected EC_Selected)
LinkEntityComponent(EntityComponents/EC_SoundListener EC_SoundListener)
LinkEntityComponent(EntityComponents/EC_HoveringWidget EC_HoveringWidget)
LinkEntityComponent(EntityComponents/EC_NetworkPosition EC_NetworkPosition)

LinkEntityComponent(EntityComponents/EC_Highlight EC_Highlight)
LinkEntityComponent(EntityComponents/EC_InputMapper EC_InputMapper)
LinkEntityComponent(EntityComponents/EC_Sound EC_Sound)
LinkEntityComponent(EntityComponents/EC_ParticleSystem EC_ParticleSystem)
LinkEntityComponent(EntityComponents/EC_3DCanvas EC_3DCanvas)
LinkEntityComponent(EntityComponents/EC_3DCanvasSource EC_3DCanvasSource)
LinkEntityComponent(EntityComponents/EC_Touchable EC_Touchable)
LinkEntityComponent(EntityComponents/EC_Light EC_Light)
LinkEntityComponent(EntityComponents/EC_VideoSource EC_VideoSource)
LinkEntityComponent(EntityComponents/EC_PlanarMirror EC_PlanarMirror)
LinkEntityComponent(EntityComponents/EC_ProximityTrigger EC_ProximityTrigger)
LinkEntityComponent(EntityComponents/EC_WebView EC_WebView)
LinkEntityComponent(EntityComponents/EC_LaserPointer EC_LaserPointer)

link_modules (Core Foundation Interfaces Scene OgreRenderingModule AssetModule AvatarModule RexCommon
    HttpUtilities RpcUtilities ProtocolUtilities ProtocolModuleOpenSim ProtocolModuleTaiga EnvironmentModule
    Asset Audio Console)

if (USE_UIMODULE)
    link_modules (UiModule)
endif ()

link_ogre()
link_package (OPENJPEG)

SetupCompileFlagsWithPCH()
CopyModuleXMLFile()

final_target ()
