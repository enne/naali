/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   ForwardDefines.h
 *  @brief  Forward declarations and type defines for commonly used Naali framework classes.
 */

#ifndef incl_Framework_ForwardDefines_h
#define incl_Framework_ForwardDefines_h

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>

#include <vector>

class ISoundService;

namespace Scene
{
    class Entity;
    class SceneManager;

    typedef boost::shared_ptr<SceneManager> ScenePtr;
    typedef boost::weak_ptr<SceneManager> SceneWeakPtr;
    typedef boost::weak_ptr<Entity> EntityWeakPtr;
    typedef boost::shared_ptr<Entity> EntityPtr;
}

namespace Console
{
    struct Command;
    struct CommandResult;
}

class IComponentFactory;
typedef boost::shared_ptr<IComponentFactory> ComponentFactoryPtr;

class IComponent;
typedef boost::shared_ptr<IComponent> ComponentPtr;
typedef boost::weak_ptr<IComponent> ComponentWeakPtr;

namespace Foundation
{
    class ModuleManager;
    class ComponentManager;
    class ServiceManager;
    class ConfigurationManager;
    class EventManager;
    class Platform;
    class Application;
    class ConfigurationManager;
    class ThreadTaskManager;
    class Profiler;
    class Framework;
    class KeyBindings;
    class MainWindow;

    typedef boost::shared_ptr<ModuleManager> ModuleManagerPtr;
    typedef boost::shared_ptr<ComponentManager> ComponentManagerPtr;
    typedef boost::shared_ptr<ServiceManager> ServiceManagerPtr;
    typedef boost::shared_ptr<ConfigurationManager> ConfigurationManagerPtr;
    typedef boost::shared_ptr<EventManager> EventManagerPtr;
    typedef boost::shared_ptr<Platform> PlatformPtr;
    typedef boost::shared_ptr<Application> ApplicationPtr;
    typedef boost::shared_ptr<ThreadTaskManager> ThreadTaskManagerPtr;

    void RootLogFatal(const std::string &msg);
    void RootLogCritical(const std::string &msg);
    void RootLogError(const std::string &msg);
    void RootLogWarning(const std::string &msg);
    void RootLogNotice(const std::string &msg);
    void RootLogInfo(const std::string &msg);
    void RootLogTrace(const std::string &msg);
    void RootLogDebug(const std::string &msg);

    class RenderServiceInterface;
    typedef boost::shared_ptr<RenderServiceInterface> RendererPtr;
    typedef boost::weak_ptr<RenderServiceInterface> RendererWeakPtr;
    struct RaycastResult;
}

class IAttribute;
typedef std::vector<IAttribute*> AttributeVector;

class KeyEvent;
class MouseEvent;
class InputContext;
class InputServiceInterface;
typedef boost::shared_ptr<InputContext> InputContextPtr;

class IEventData;

class UiServiceInterface;
typedef boost::shared_ptr<UiServiceInterface> UiServicePtr;
typedef boost::weak_ptr<UiServiceInterface> UiServiceWeakPtr;

#endif
