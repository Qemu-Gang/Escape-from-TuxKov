#include "Glow.h"

#include "../utils/Wrappers.h"
#include "../sdk/CBaseEntity.h"
#include "../utils/minitrace.h"


static Vector teamColors[] = {
        {150, 0,   0}, // red
        {0,   150, 0}, // green
        {0,   100, 0}, // dark green
        {130, 0,   0}, // dark red
        {0,   0,   125}, // dark blue
        {0,   0,   150}, // blue
        {120, 0,   120}, // purpleish
        {150, 150, 150}, // idk
        {255, 255, 255}, // white
        {0,   200, 200}, // cyan
        {255, 128, 0}, // pink
        {255, 255, 0}, // yellow
        {100, 50,  0}, // brown
        {96,  96,  96}, // grey
        {0,   0,   255}, // bright blue
        {255, 0,   0}, // bright red
        {0,   255, 0}, // bright green
        {50,  100, 0}, // dark slime
        {100, 100, 0}, // dank yellow
        {0,   150, 150}, // dark teal
        {255, 0,   255}, // hot pink - 20
        {255, 0,   255}, // hot pink - 20
};

Vector getHealthColor(CBaseEntity Entity) {
    Vector result;
    const float multiplier = 1.00; //number we multiply our health by to get our colors(multiply our health by 2.55 to give a number we then use for the color. since 255 is the max of any color for our esp ie. 100 full health * 2.55 = 255 or max color)

    int red, green, blue, alpha;    //colors we will solve for based on our entities health
    int health = Entity.health;    //get entities health

    red = 100 - (health * multiplier);    //find red value (no health = max red, full health = no red)
    green = health * multiplier;    //find green value (full health = max green, no health = no green)
    blue = 0;    //no blue on color scale red to green
    alpha = 255;    //max alpha

    Vector Color(red, green, blue);    //create color and fill it with values
    return Color;    //return color

}

static void WriteItemGlow(CBaseEntity &entity, Vector &colors) {
    // Write members of struct independently to avoid race conditions

    process->Write<bool>(entity.bGlowEnable.GetAddress(), true);

    process->Write<float>(entity.glowDistance.GetAddress(), __FLT_MAX__);

    process->Write<float>(entity.glowHighlight.GetAddress(), __FLT_MAX__);

    process->Write<Vector>(entity.glowCol.GetAddress(), colors);


    process->Write<int>(entity.glowOutline.GetAddress(), 0x4D407D7E);
}

static void WriteGlow(CBaseEntity &entity, Vector &colors, float distance) {
    entity.bGlowEnable = true; // Enabling the Glow
    entity.iGlowEnable = 1; // Enabling the Glow

    entity.glowCol = colors;

    entity.glowTimes1 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes2 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes3 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes4 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes5 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes6 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out

    entity.glowHighlight = __FLT_MAX__;
    entity.glowDistance = __FLT_MAX__; //Set the Distance of the Glow to Max float value so we can see a long Distance
    entity.glowOutline = 0x4D407D7E; // outline effect
}

void Glow::Glow() {
    for (size_t entID = 0; entID < validEntities.size(); entID++) {

        CBaseEntity &entity = entities[validEntities[entID]];
        if (entity.GetPlayerState()) {
            if (entity.GetBaseClass().address == localPlayer) {
                /*Vector color = Vector(0, 0, 60);

                WriteGlow(entity, color, 0.0f);*/
                continue;
            }

            if (entity.GetTeamNum() == localPlayer.GetTeamNum())
                continue;

            if (entity.GetBaseClass().address == aimbotEntity) {
                Vector color = Vector(0, 0, 100);

                WriteGlow(entity, color, 0.0f);
                continue;
            }

            if (entity.GetBleedoutState() != 0) {
                Vector color = Vector(48, 48, 48);

                WriteGlow(entity, color, 0.0f);
                continue;
            }
            Vector color = getHealthColor(entity);
            WriteGlow(entity, color, 0.0f);
        } else {
            Vector color = Vector(1, 1, 1);

            WriteItemGlow(entity, color);
            continue;
        }
    }
}

void Glow::GlowPlayer(CBaseEntity &entity, Vector &colors) {
    WriteGlow(entity, colors, 100);
}