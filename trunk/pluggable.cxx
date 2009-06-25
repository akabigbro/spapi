#include "pluggable.h"

CreatePlugin * Pluggable::getCreator(void * handle) throw(int&)
{
    CreatePlugin * creator = (CreatePlugin *) dlsym(handle,"CreatePlugin");

    if (NULL == creator)
    {
        throw PLUGGABLE_EXCEPTION;
    }

    return creator;
}

DestroyPlugin * Pluggable::getDestroyer(void * handle) throw(int&)
{
    DestroyPlugin * destroyer = (DestroyPlugin *) dlsym(handle,"DestroyPlugin");

    if (NULL == destroyer)
    {
        throw PLUGGABLE_EXCEPTION;
    }

    return destroyer;
}

Plugin * Pluggable::loadPlugin(std::string name, int flag) throw(int&)
{
    Plugin * instance = NULL;
    struct PluginHandle * phdl = new PluginHandle;
    phdl->handle = dlopen(name.c_str(), flag);

    if (NULL == phdl->handle)
    {
        delete phdl;
        throw PLUGIN_LOAD_EXCEPTION;
    }

    try
    {
        phdl->creator = getCreator(phdl->handle);
        phdl->destroyer = getDestroyer(phdl->handle);
        instance = phdl->creator();
        plugins[instance] = phdl;
    }
    catch (int & ex)
    {
        throw PLUGIN_LOAD_EXCEPTION;
    }

    return instance;
}

void Pluggable::unloadPlugin(Plugin * plugin) throw(int&)
{
    std::map<Plugin *,PluginHandle *>::iterator iter = plugins.find(plugin);

    if (iter != plugins.end())
    {
        PluginHandle * phdl = iter->second;
        phdl->destroyer(plugin);
        dlclose(phdl->handle);
        plugins.erase(iter);
        delete phdl;
    }
    else
    {
        throw PLUGIN_UNLOAD_EXCEPTION;
    }
}

Pluggable::~Pluggable()
{
    for (std::map<Plugin *,PluginHandle *>::iterator iter = plugins.begin(); iter != plugins.end(); iter++)
    {
        unloadPlugin(iter->first);
    }
}
