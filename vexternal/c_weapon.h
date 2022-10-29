#pragma once

enum weapon_ids
{
	MELEE = 18,

	SENTINEL = 1,
	CHARGE_RIFLE = 86,
	LONGBOW = 88,

	RE_45 = 85,
	P2020 = 101,
	R301 = 0,
	R99 = 100,
	ALTERNATOR = 84,

	CAR_MP = 107,
	RAMPAGE = 14,
	_30_30 = 106,
	WINGMAN = 104,
	PROWLER = 98,
	HEMLOK = 93,
	FLATLINE = 91,

	TRIPLE_TAKE = 103,
	DEVOTION = 87,
	HAVOC = 89,
	L_STAR = 95,

	MOSAMBIK = 97,
	PEACEKEEPER = 99,
	EVA_8_AUTO = 90,
	MASTIFF = 96,

	BOCEK = 2,

	KRABER = 94,
	G7_SCOUT = 92,
	VOLT = 105,
	SPITFIRE = 102
};

enum item_rarity { HEIRLOOM, LEGENDARY, EPIC, RARE, COMMON };

struct c_props {
	std::string item_name;
	item_rarity rarity;
};

std::map<uint32_t, c_props> item_map = {
	{ 1, { "Kraber", item_rarity::HEIRLOOM }},
	{ 2, { "Mastiff", item_rarity::COMMON }},
	{ 6, { "Mastiff Gold", item_rarity::LEGENDARY }},
	{ 7, { "LStar", item_rarity::COMMON }},
	{ 11, { "LStar Gold", item_rarity::LEGENDARY }},
	{ 12, { "Havoc", item_rarity::COMMON }},
	{ 16, { "Havoc Gold", item_rarity::LEGENDARY }},
	{ 17, { "Devotion", item_rarity::COMMON }},
	{ 21, { "Devotion Gold", item_rarity::LEGENDARY }},
	{ 22, { "Triple Take", item_rarity::COMMON }},
	{ 26, { "Triple Take Gold", item_rarity::LEGENDARY }},
	{ 27, { "Flatline", item_rarity::COMMON }},
	{ 31, { "Flatline Gold", item_rarity::LEGENDARY }},
	{ 32, { "Hemlock", item_rarity::COMMON }},
	{ 36, { "Hemlock Gold", item_rarity::LEGENDARY }},
	{ 37, { "G7 Scout", item_rarity::HEIRLOOM }},
	{ 41, { "G7 Scout Gold", item_rarity::LEGENDARY }},
	{ 42, { "Alternator", item_rarity::COMMON }},
	{ 46, { "Alternator", item_rarity::COMMON }},
	{ 47, { "R99", item_rarity::COMMON }},
	{ 51, { "R99 Gold", item_rarity::LEGENDARY }},
	{ 52, { "Prowler", item_rarity::COMMON }},
	{ 56, { "Prowler Gold", item_rarity::LEGENDARY }},
	{ 57, { "Volt", item_rarity::LEGENDARY }},
	{ 58, { "Longbow", item_rarity::COMMON }},
	{ 62, { "Longbow Gold", item_rarity::LEGENDARY }},
	{ 63, { "Charge Rifle", item_rarity::COMMON }},
	{ 67, { "Charge Rifle Gold", item_rarity::LEGENDARY }},
	{ 68, { "Spitfire", item_rarity::HEIRLOOM }},
	{ 73, { "R301", item_rarity::COMMON }},
	{ 69, { "R301 Gold", item_rarity::LEGENDARY }},
	{ 78, { "Eva-8 Auto", item_rarity::COMMON }},
	{ 74, { "Eva-8 Auto Gold", item_rarity::LEGENDARY }},
	{ 83, { "Peacekeeper", item_rarity::COMMON }},
	{ 79, { "Peacekeeper Gold", item_rarity::LEGENDARY }},
	{ 88, { "Mozambique", item_rarity::COMMON }},
	{ 84, { "Mozambique Gold", item_rarity::LEGENDARY }},
	{ 89, { "Wingman", item_rarity::COMMON }},
	{ 83, { "Wingman Gold", item_rarity::LEGENDARY }},
	{ 98, { "P2020", item_rarity::COMMON }},
	{ 94, { "P2020 Gold", item_rarity::LEGENDARY }},
	{ 103, { "RE45", item_rarity::COMMON }},
	{ 99, { "RE45 Gold", item_rarity::LEGENDARY }},
	{ 108, { "Sentinel", item_rarity::COMMON }},
	{ 104, { "Sentinel Gold", item_rarity::LEGENDARY }},
	{ 113, { "Bocek Bow", item_rarity::COMMON }},
	{ 109, { "Bocek Bow Gold", item_rarity::LEGENDARY }},
	{ 118, { "Repeater", item_rarity::COMMON }},
	{ 114, { "Repeater Gold", item_rarity::LEGENDARY }},
	{ 124, { "Light Ammo", item_rarity::COMMON }},
	{ 125, { "Energy Ammo", item_rarity::COMMON }},
	{ 126, { "Shotgun Ammo", item_rarity::COMMON }},
	{ 127, { "Heavy Ammo", item_rarity::COMMON }},
	{ 128, { "Sniper Ammo", item_rarity::COMMON }},
	{ 129, { "Arrow", item_rarity::COMMON }},
	{ 130, { "Rampage", item_rarity::COMMON }},
	{ 131, { "Rampage Gold", item_rarity::LEGENDARY }},
	{ 132, { "Car SMG", item_rarity::COMMON }},
	{ 133, { "Car SMG Gold", item_rarity::LEGENDARY }},
	{ 164, { "Ultimate Accelerant", item_rarity::RARE }},
	{ 165, { "Pheonix Kit", item_rarity::EPIC }},
	{ 166, { "Med Kit", item_rarity::RARE }},
	{ 167, { "Syringe", item_rarity::COMMON }},
	{ 168, { "Shield Battery", item_rarity::RARE }},
	{ 169, { "Shield Cell", item_rarity::COMMON }},
	{ 170, { "Helmet[L1]", item_rarity::COMMON }},
	{ 171, { "Helmet[L2]", item_rarity::RARE }},
	{ 172, { "Helmet[L3]", item_rarity::EPIC }},
	{ 173, { "Helmet[L4]", item_rarity::LEGENDARY }},
	{ 174, { "Body Armor[L1]", item_rarity::COMMON }},
	{ 175, { "Body Armor[L2]", item_rarity::RARE }},
	{ 176, { "Body Armor[L3]", item_rarity::EPIC }},
	{ 177, { "Body Armor[L4]", item_rarity::LEGENDARY }},
	{ 178, { "Body Armor[L5]", item_rarity::HEIRLOOM }},
	{ 179, { "Evo Body Armor[L1]", item_rarity::COMMON }},
	{ 180, { "Evo Body Armor[L2]", item_rarity::RARE }},
	{ 181, { "Evo Body Armor[L3]", item_rarity::EPIC }},
	{ 182, { "Evo Body Armor[L5]", item_rarity::HEIRLOOM }},
	{ 237, { "Heat Shield", item_rarity::HEIRLOOM }},
	{ 184, { "Knockdown Shield[L1]", item_rarity::COMMON }},
	{ 185, { "Knockdown Shield[L2]", item_rarity::RARE }},
	{ 186, { "Knockdown Shield[L3]", item_rarity::EPIC }},
	{ 187, { "Knockdown Shield[L4]", item_rarity::LEGENDARY }},
	{ 188, { "Backpack[L1]", item_rarity::COMMON }},
	{ 189, { "Backpack[L2]", item_rarity::RARE }},
	{ 190, { "Backpack[L3]", item_rarity::EPIC }},
	{ 191, { "Backpack[L4]", item_rarity::LEGENDARY }},
	{ 192, { "Thermite Grenade", item_rarity::COMMON }},
	{ 193, { "Frag Grenade", item_rarity::COMMON }},
	{ 194, { "Arc Star", item_rarity::COMMON }},
	{ 195, { "1x HCOG 'Classic'", item_rarity::COMMON }},
	{ 196, { "2x HCOG 'Bruiser'", item_rarity::RARE }},
	{ 197, { "1x Holo", item_rarity::COMMON }},
	{ 198, { "1x-2x Variable Holo", item_rarity::RARE }},
	{ 199, { "1x Digital Threat", item_rarity::LEGENDARY }},
	{ 200, { "3x HCOG 'Ranger'", item_rarity::EPIC }},
	{ 201, { "2x-4x Variable AOG", item_rarity::EPIC }},
	{ 202, { "6x Sniper", item_rarity::RARE }},
	{ 203, { "4x-8x Variable Sniper", item_rarity::EPIC }},
	{ 204, { "4x-10x Digital Sniper Threat", item_rarity::LEGENDARY }},
	{ 205, { "Barrel Stabilizer", item_rarity::COMMON }},
	{ 206, { "Barrel Stabilizer", item_rarity::RARE }},
	{ 207, { "Barrel Stabilizer", item_rarity::EPIC }},
	{ 209, { "Light Magazine[L1]", item_rarity::COMMON }},
	{ 210, { "Light Magazine[L2]", item_rarity::RARE }},
	{ 211, { "Light Magazine[L3]", item_rarity::EPIC }},
	{ 212, { "Light Magazine[L4]", item_rarity::LEGENDARY }},
	{ 213, { "Heavy Magazine[L1]", item_rarity::COMMON }},
	{ 214, { "Heavy Magazine[L2]", item_rarity::RARE }},
	{ 215, { "Heavy Magazine[L3]", item_rarity::EPIC }},
	{ 216, { "Heavy Magazine[L4]", item_rarity::LEGENDARY }},
	{ 217, { "Energy Magazine[L1]", item_rarity::COMMON }},
	{ 218, { "Energy Magazine[L2]", item_rarity::RARE }},
	{ 219, { "Energy Magazine[L3]", item_rarity::EPIC }},
	{ 220, { "Energy Magazine[L4]", item_rarity::LEGENDARY }},
	{ 221, { "Sniper Magazine[L1]", item_rarity::COMMON }},
	{ 222, { "Sniper Magazine[L2]", item_rarity::RARE }},
	{ 223, { "Sniper Magazine[L3]", item_rarity::EPIC }},
	{ 224, { "Sniper Magazine[L4]", item_rarity::LEGENDARY }},
	{ 225, { "Shotgun Bolt[L1]", item_rarity::COMMON }},
	{ 226, { "Shotgun Bolt[L2]", item_rarity::RARE }},
	{ 227, { "Shotgun Bolt[L3]", item_rarity::EPIC }},
	{ 228, { "Standard Stock[L1]", item_rarity::COMMON }},
	{ 229, { "Standard Stock[L2]", item_rarity::RARE }},
	{ 230, { "Standard Stock[L3]", item_rarity::EPIC }},
	{ 231, { "Sniper Stock[L1]", item_rarity::COMMON }},
	{ 232, { "Sniper Stock[L2]", item_rarity::RARE }},
	{ 233, { "Sniper Stock[L3]", item_rarity::EPIC }},
	{ 234, { "Turbocharger", item_rarity::LEGENDARY }},
	{ 238, { "Hammer Point", item_rarity::EPIC }},
	{ 237, { "Skull Piercer", item_rarity::LEGENDARY } },
	{ 245, { "Deadeye's Tempo", item_rarity::EPIC } },
	{ 246, { "Quickdraw Holster", item_rarity::EPIC } },
	{ 247, { "Shatter Caps", item_rarity::EPIC }},
	{ 248, { "Kinetic Feeder", item_rarity::EPIC } },
	{ 249, { "Boosted Loader", item_rarity::LEGENDARY }},
	{ 245, { "Mobile Respawn Beacon", item_rarity::RARE }},
	{ 246, { "Vault Key", item_rarity::HEIRLOOM }},
	{ 248, { "Treasure Pack", item_rarity::HEIRLOOM }},
	{ 249, { "Survival Doubler", item_rarity::HEIRLOOM }}, // TODO: Find this rarity
	{ 250, { "Survival Quadrupler", item_rarity::HEIRLOOM }}, // TODO: Find this rarity
	{ 251, { "Heat Shield", item_rarity::RARE }},
	{ 252, { "Mobile Respawn Beacon 2", item_rarity::RARE }},
	{ 253, { "Treasure Pack", item_rarity::HEIRLOOM }},
	{ 254, { "Vault Key", item_rarity::HEIRLOOM }}
};

std::map<uint32_t, std::string> weapon_map = {
	{ 1, { "kraber", item_rarity::HEIRLOOM }},
};

class c_weapon {
public:
	c_weapon() = default;
	c_weapon(uintptr_t pointer) {
		this->address = pointer;
	}

	inline bool valid() {
		return this->address != 0;
	}

	inline int weapon_id() {
		if (!this->address) return -1;
		return read<int>(this->address + OFFSET_WEAPON_NAME_INDEX);
	}

	inline int ammo_in_clip() {
		if (!this->address) return -1;
		return read<int>(this->address + OFFSET_AMMO_IN_CLIP);
	}

	inline float bullet_speed() {
		if (!this->address) return 0.0f;
		return read<float>(this->address + OFFSET_BULLET_SPEED);
	}

	inline float bullet_gravity() {
		if (!this->address) return 0.0f;
		return read<float>(this->address + OFFSET_BULLET_GRAVITY);
	}

	inline void change_skin(int skin_id) {
		if (!this->address) return;
		write<int>(this->address + OFFSET_NSKIN, skin_id);
		write<int>(this->address + OFFSET_SKIN_MODEL, skin_id);
	}

	inline std::string get_name() {
		if (!this->address) return _("none");
		for (const auto& [key, value] : weapon_map) {
			if (this->weapon_id() == key) {
				return value;
			}
		}
	}
private:
	uintptr_t address;
};