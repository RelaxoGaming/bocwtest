#pragma once
#include"includes.h"

struct LootItem {
    char pad[4];
    int itemId;
    int itemQuantity;
    int itemDate;
    char pad2[4];
};

struct StringTable {
    uintptr_t hash;
    int columnCount;
    int rowCount;
    char pad[40];
};

void UnlockAll();
