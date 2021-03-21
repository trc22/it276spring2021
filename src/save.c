#include "simple_json.h"
#include "simple_logger.h"

#include "item.h"

#include "save.h"

void new_save()
{
	int i;
	SJson *json, *items, *quantities;
	slog("creating new save");
	json = sj_array_new();
	items = sj_array_new();
	quantities = sj_array_new();

	for (i = 0; i < 6; i++)
	{
		if (get_current_item(i)->itemID == NULL || get_current_item(i) == NULL)
		{
			sj_array_append(items, sj_new_str("NULL"));
			continue;
		}

		sj_array_append(items, sj_new_int(get_current_item(i)->itemID));
	}

	for (i = 0; i < 6; i++)
	{
		if (get_current_item(i)->itemID == NULL || get_current_item(i) == NULL)
		{
			sj_array_append(quantities, sj_new_str("NULL"));
			continue;
		}

		sj_array_append(quantities, sj_new_int(get_current_item(i)->quantity));
	}

	sj_array_append(json, items);
	sj_array_append(json, quantities);

	sj_save(json, "saves/save.json");

}

void load_save(const char *filename);
