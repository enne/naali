# Define target name
init_target (Interfaces)

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
file (GLOB MOC_FILES CommunicationsService.h UiSettingsServiceInterface.h
    MediaPlayerService.h WorldBuildingServiceInterface.h RenderServiceInterface.h
    LoginServiceInterface.h WorldLogicInterface.h InputServiceInterface.h IOpenSimSceneService.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

MocFolder ()

set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

# Qt4 Wrap
QT4_WRAP_CPP(MOC_SRCS ${MOC_FILES})

add_definitions (-DMODULE_EXPORTS)

use_modules (Core Foundation Scene Interfaces Asset)

build_library (${TARGET_NAME} STATIC ${SOURCE_FILES} ${MOC_SRCS})

link_package_knet()
link_modules (Core Foundation Scene Interfaces Asset)

SetupCompileFlagsWithPCH()

# Add Kristalli needed for the binary serializer/deserializer
target_link_libraries (${TARGET_NAME})

final_target ()
