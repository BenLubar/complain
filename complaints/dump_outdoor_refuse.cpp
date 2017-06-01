#include "../complain.h"

#include "modules/Items.h"
#include "modules/Maps.h"

#include "df/item.h"
#include "df/items_other_id.h"
#include "df/tile_designation.h"
#include "df/world.h"

REQUIRE_GLOBAL(standing_orders_gather_refuse_outside);
REQUIRE_GLOBAL(world);

static class complaint_dump_outdoor_refuse : public timed_complaint
{
public:
    complaint_dump_outdoor_refuse() :
        timed_complaint(28 * 1200)
    {
    }

protected:
    virtual void check(color_ostream & out)
    {
        if (*standing_orders_gather_refuse_outside)
        {
            return;
        }

        for (auto i : world->items.other[items_other_id::IN_PLAY])
        {
            if (i->flags.bits.dump && !Items::getSpecificRef(i, specific_ref_type::JOB))
            {
                df::tile_designation *des = Maps::getTileDesignation(Items::getPosition(i));
                if (des && des->bits.outside)
                {
                    complain(out, COLOR_YELLOW, "An item located outdoors is marked for dumping, but outdoor refuse hauling is disabled. [oro]", Items::getPosition(i));
                    return;
                }
            }
        }
    }
} _;
