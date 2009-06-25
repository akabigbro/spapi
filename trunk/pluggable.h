#ifndef PLUGGABLE_H
#define PLUGGABLE_H

#include <dlfcn.h>
#include <map>
#include <string>
#include "plugin.h"

#define PLUGIN_EXCEPTION 2000
#define PLUGIN_LOAD_EXCEPTION 2001
#define PLUGIN_UNLOAD_EXCEPTION 2002
#define PLUGGABLE_EXCEPTION 2100

typedef Plugin * CreatePlugin();
typedef void DestroyPlugin(Plugin * plugin);

struct PluginHandle
{
    void * handle;
    CreatePlugin * creator;
    DestroyPlugin * destroyer;
};

class Pluggable : public Plugin
{
    private:
        std::map<Plugin *,PluginHandle *> plugins;
        CreatePlugin * getCreator(void * handle) throw(int&);
        DestroyPlugin * getDestroyer(void * handle) throw(int&);
    protected:
        Plugin * loadPlugin(std::string name, int flag = RTLD_LAZY) throw(int&);
        void unloadPlugin(Plugin * plugin) throw(int&);
    public:
        ~Pluggable();
};

#endif//PLUGGABLE_H
