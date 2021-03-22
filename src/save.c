#include "simple_json.h"
#include "simple_logger.h"

#include "item.h"

#include "save.h"

int a2i(const char *s);

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

	sj_free(json);

}

Bool load_save(const char *filename)
{
	SJson *json, *items, *quantities, *tempJS;
	int i, temp;
	char *string;

	if (!filename)
	{
		slog("filename is NULL, cannot load the save");
		return false;
	}
	slog("loading save");
	json = sj_load(filename);
	if (!json)return false;
	slog("save loaded");
	json = sj_object_get_value(json, "inventory");
	
	slog("array size: %i", sj_array_get_count(&json[0]));
	items = sj_array_get_nth(&json[0], 0);
	slog("item array size: %i", sj_array_get_count(items));
	//slog("%s", sj_get_string_value(tempJS));
	clear_inventory();
	for (i = 0; i < 6; i++)
	{
		tempJS = sj_array_get_nth(items, i);
		string = sj_get_string_value(tempJS, i);
		temp = a2i(string);
		if (temp == 34008)
			temp = NULL;
		inventory_insert(get_item_by_id(temp));
		//slog("Slot %i: %i", i, temp);
	}

	quantities = sj_array_get_nth(&json[0], 1);
	slog("quantity array size: %i", sj_array_get_count(quantities));

	sj_free(json);

	return true;
}

int a2i(const char *s) //Convert string to int from stack overflow: https://stackoverflow.com/questions/13145777/c-char-to-int-conversion
{
	int sign = 1;
	if (*s == '-'){
		sign = -1;
		s++;
	}
	int num = 0;
	while (*s){
		num = ((*s) - '0') + num * 10;
		s++;
	}
	return num*sign;
}