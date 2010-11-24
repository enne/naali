/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   JavascriptModule.h
 *  @brief  Enables Javascript execution and scripting in Naali.
 */

#ifndef incl_JavascriptModule_JavascriptModule_h
#define incl_JavascriptModule_JavascriptModule_h

#include "IModule.h"
#include "ModuleLoggingFunctions.h"
#include "AttributeChangeType.h"
#include "ScriptServiceInterface.h"

#include <QObject>

class QScriptEngine;
class QScriptContext;
class QScriptEngine;
class QScriptValue;
class JavascriptEngine;

/// Enables Javascript execution and scripting in Naali.
/**
*/
class JavascriptModule : public QObject, public IModule, public Foundation::ScriptServiceInterface
{
    Q_OBJECT

public:
    /// Default constructor.
    JavascriptModule();

    /// Destructor.
    ~JavascriptModule();

    /// IModule override.
    void Load();

    /// IModule override.
    void PreInitialize();

    /// IModule override.
    void Initialize();

    /// IModule override.
    void PostInitialize();

    /// IModule override.
    void Uninitialize();

    /// IModule override.
    void Update(f64 frametime);

    /// IModule override.
    bool HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data);

    MODULE_LOGGING_FUNCTIONS

    /// Returns name of this module. Needed for logging.
    static const std::string &NameStatic() { return type_name_static_; }

    /// Returns the currently initialized JavascriptModule.
    static JavascriptModule *GetInstance();

    void RunScript(const QString &scriptname);
    void RunString(const QString &codestr, const QVariantMap &context = QVariantMap());

    Console::CommandResult ConsoleRunString(const StringVector &params);
    Console::CommandResult ConsoleRunFile(const StringVector &params);
    Console::CommandResult ConsoleReloadScripts(const StringVector &params);
    
public slots:
    //! New scene has been added to foundation.
    void SceneAdded(const QString &name);

    //! EC_Script script source ref has changed.
    void ScriptChanged(const QString &scriptRef);

    //! New component has been added to scene.
    void ComponentAdded(Scene::Entity* entity, IComponent* comp, AttributeChange::Type change);

    //! Component has been removed from scene.
    void ComponentRemoved(Scene::Entity* entity, IComponent* comp, AttributeChange::Type change);

private:
    //! Load & execute startup scripts
    /*! Destroys old scripts if they exist
     */
    void LoadStartupScripts();
    
    //! Stop & delete startup scripts
    void UnloadStartupScripts();
    
    //! Prepare a script engine by registering all needed services to it
    void PrepareScriptEngine(JavascriptEngine* engine);
    
    /// Type name of the module.
    static std::string type_name_static_;
    typedef QMap<QString, QObject*> ServiceMap;
    ServiceMap services_;

    /// Default engine for console & commandline script execution
    QScriptEngine *engine;
    
    /// Engines for executing startup (possibly persistent) scripts
    std::vector<JavascriptEngine*> startupScripts_;
};

//api stuff
QScriptValue Print(QScriptContext *context, QScriptEngine *engine);
QScriptValue ScriptRunFile(QScriptContext *context, QScriptEngine *engine);

#endif
