#include "complain.h"

#include "DFHackVersion.h"
#include "complain-git-describe.h"

#include <set>
#include <vector>

DFHACK_PLUGIN("complain");
DFHACK_PLUGIN_IS_ENABLED(enabled);

REQUIRE_GLOBAL(cur_year);
REQUIRE_GLOBAL(cur_year_tick);
REQUIRE_GLOBAL(cur_year_tick_advmode);
REQUIRE_GLOBAL(gamemode);

static std::set<complaint *> complaints;

DFhackCExport command_result plugin_init(color_ostream & out, std::vector<PluginCommand> & commands)
{
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown(color_ostream & out)
{
    return CR_OK;
}

DFhackCExport command_result plugin_onstatechange(color_ostream & out, state_change_event event)
{
    for (auto c : complaints)
    {
        if (c->check_mode() && c->check_state_change(event))
        {
            c->check(out);
        }
    }
    return CR_OK;
}

DFhackCExport command_result plugin_onupdate(color_ostream & out)
{
    for (auto c : complaints)
    {
        if (c->check_mode() && c->check_update())
        {
            c->check(out);
        }
    }
    return CR_OK;
}

complaint::complaint(bool fortress, bool adventure) :
    fortress(fortress),
    adventure(adventure)
{
    complaints.insert(this);
}

complaint::~complaint()
{
    complaints.erase(this);
}

void complaint::complain(color_ostream & out, color_value color, const std::string & message, df::coord position)
{
    df::announcement_flags flags;
    flags.whole = 0;
    flags.bits.D_DISPLAY = 1;
    flags.bits.A_DISPLAY = 1;
    Gui::makeAnnouncement(df::announcement_type(0), flags, position, message, color & 7, (color >> 3) != 0);

    color_value old_color = out.color();
    out.color(color);
    out.print(message.c_str());
    out.print("\n");
    out.color(old_color);
}

bool complaint::check_mode()
{
    switch (*gamemode)
    {
    case game_mode::DWARF:
        return fortress;
    case game_mode::ADVENTURE:
        return adventure;
    case game_mode::num:
    case game_mode::NONE:
        return false;
    }
    return false;
}

timed_complaint::timed_complaint(int32_t ticks, bool fortress, bool adventure) :
    complaint(fortress, adventure),
    ticks(ticks),
    last_year(-1),
    last_tick(-1)
{
}

bool timed_complaint::check_update()
{
    if (!Core::getInstance().isWorldLoaded())
    {
        return false;
    }

    if (last_year == -1 || last_year != *cur_year)
    {
        last_year = *cur_year;
        last_tick = -1;
    }

    int32_t cur_tick = *gamemode == game_mode::ADVENTURE ? *cur_year_tick_advmode : *cur_year_tick;
    if (last_tick == -1 || last_tick + ticks >= cur_tick)
    {
        last_tick = cur_tick;
        return true;
    }

    return false;
}

bool timed_complaint::check_state_change(state_change_event state_change)
{
    switch (state_change)
    {
    case SC_WORLD_LOADED:
        last_year = -1;
        last_tick = -1;
        break;

    default:
        break;
    }

    return false;
}
