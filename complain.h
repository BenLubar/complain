#pragma once

#include "Core.h"
#include <Console.h>
#include <Export.h>
#include <PluginManager.h>

#include "DataDefs.h"

#include "modules/Gui.h"

using namespace DFHack;
using namespace df::enums;

DFhackDataExport extern std::vector<std::string> *plugin_globals;

DFhackCExport command_result plugin_enable(color_ostream &, bool);
DFhackCExport command_result plugin_onstatechange(color_ostream &, state_change_event);
DFhackCExport command_result plugin_onupdate(color_ostream &);

class complaint
{
public:
    complaint(bool fortress = true, bool adventure = false);
    virtual ~complaint();

protected:
    void complain(color_ostream & out, color_value color, const std::string & message, df::coord position = df::coord());
    virtual void check(color_ostream & out) = 0;
    virtual bool check_update() = 0;
    virtual bool check_state_change(state_change_event) = 0;
    virtual void reset() {}

private:
    bool check_mode();
    bool fortress;
    bool adventure;

    friend command_result plugin_enable(color_ostream &, bool);
    friend command_result plugin_onstatechange(color_ostream &, state_change_event);
    friend command_result plugin_onupdate(color_ostream &);
};

class timed_complaint : public complaint
{
public:
    timed_complaint(int32_t ticks, bool fortress = true, bool adventure = false);

protected:
    virtual bool check_update();
    virtual bool check_state_change(state_change_event);
    virtual void reset();

private:
    int32_t ticks;
    int32_t last_year;
    int32_t last_tick;
};

template<typename V, typename = typename std::enable_if<std::is_base_of<df::viewscreen, V>::value>::type>
class viewscreen_complaint : public complaint
{
public:
    viewscreen_complaint(bool fortress = true, bool adventure = false) :
        complaint(fortress, adventure)
    {
    }

protected:
    virtual bool check_update()
    {
        return false;
    }

    virtual bool check_state_change(state_change_event event)
    {
        return event == SC_VIEWSCREEN_CHANGED && strict_virtual_cast<V>(Gui::getCurViewscreen()) != nullptr;
    }
};
