#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include <vector>
#include <string>
using namespace std;





enum ItemType {
    ITEM_KEYCARD,       
    ITEM_FLASHLIGHT,    
    ITEM_DOCUMENT,      
    ITEM_MEDKIT,        
    ITEM_BATTERY,       
    NUM_ITEM_TYPES
};


struct WorldItem {
    ItemType    type;
    float       x, y, z;       
    bool        collected;      
    float       bobTimer;       
    float       rotAngle;       
    int         floor;          
};


struct ItemInfo {
    ItemType    type;
    string name;
    string description;
    bool        isRequired;     
};



void initItems();


void updateItems(float dt);


void checkItemPickup();


void drawItems();


int getCollectedRequiredCount();


int getTotalRequiredCount();


bool allRequiredItemsCollected();


void resetItems();

void drawItemHUD();

#endif
