#pragma once

#include "Game/Item.hpp"
#include <vector>

class Inventory
{
public:
    std::vector<Item*> m_items;

public:
    Inventory();
	~Inventory();
};