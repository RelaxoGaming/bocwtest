#include "unlockAll.h"

void UnlockAll()
{
	char* out;
	char* out2;
	char* out3;
	std::vector<int>itemIds;
	auto FindXAssetHeader = reinterpret_cast<uintptr_t(*)(int type, uintptr_t givenHash, bool errorIfMissing, int waitTime)>(ctx::offset::FindXAssetHeader.cast());

	auto loot_table =reinterpret_cast<StringTable*>(FindXAssetHeader(63, 0x411DEAF0BB60BA86, false, -1));
	if (!loot_table)
		return;


	for (int row = 0; row < loot_table->rowCount; ++row)
	{
		GameFunction::StringTable_GetColumnValueForRow((uintptr_t)loot_table, &out, row, 0);

		GameFunction::StringTable_GetColumnValueForRow((uintptr_t)loot_table, &out2, row, 1);

		GameFunction::StringTable_GetColumnValueForRow((uintptr_t)loot_table, &out3, row, 3);

		log::NotePad("%s -- %s --- %s\n", out, out2, out3);

		//int cell = atoi(out);

		//itemIds.push_back(cell);

	}

	/*auto loot_base = mem::Scanner::Scanner(ctx::offset::Loot_GetitemQuantity + 0x20).resolve_lea();
	auto playerinventory = reinterpret_cast<LootItem*>(loot_base + 0x3b0);
	for (int row = 0; row < loot_table->rowCount; ++row)
	{
		if (!GameFunction::StringTable_GetColumnValueForRow((uintptr_t)loot_table, &out, row, 0))
			continue;

		int cell = atoi(out);
		
		itemIds.push_back(cell);

	}
	int time = GameFunction::Sys_GetUTC();
	int lootSize = itemIds.size();
	for (int y = 0; y < lootSize; ++y) {
		auto item = &playerinventory[y];
		item->itemId = itemIds[y];
		item->itemQuantity = 1;
		item->itemDate = time;
	}
	*(int*)(loot_base + 0x61E34) = lootSize;
	*(int*)(loot_base + 0x61E3C) = 1;*/

}
