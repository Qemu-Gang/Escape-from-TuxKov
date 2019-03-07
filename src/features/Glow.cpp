#include "Glow.h"

#include "../utils/Wrappers.h"
#include "../sdk/CBaseEntity.h"


static float teamColors[][3] = {
        { 0, 150, 0 }, // green
        { 0, 75, 0 }, // dark green
        { 75, 0, 0 }, // dark red
        { 150, 0, 0 }, // red
        { 0, 0, 75 }, // dark blue
        { 0, 0, 150 }, // blue
        { 75, 0, 75 }, // purpleish
        { 0, 0, 0 }, // black
        { 255, 255, 255 }, // white
        { 0, 200, 200 }, // cyan
        { 255, 128, 0 }, // pink
        { 255, 255, 0 }, // yellow
        { 100, 50, 0 }, // brown
        { 96, 96, 96 }, // grey
        { 0, 0, 255 }, // bright blue
        { 255, 0, 0 }, // bright red
        { 0, 255, 0 }, // bright green
        { 50, 100, 0 }, // dark slime
        { 75, 75, 0 }, // dank yellow
        { 0, 150, 150 }, // dark teal
        { 255, 0, 255 }, // hot pink - 20
};


static void WriteGlow(uintptr_t entity, float *colors) {
    process->Write<bool>(entity + OFFSET_OF(&CBaseEntity::bGlowEnable), true); // Enabling the Glow
    process->Write<int>(entity + OFFSET_OF(&CBaseEntity::iGlowEnable), 1); // Enabling the Glow

    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowR), colors[0]); // Setting a value for the Color Red between 0 and 255
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowG), colors[1]); // Setting a value for the Color Green between 0 and 255
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowB), colors[2]); // Setting a value for the Color Blue between 0 and 255

    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes1), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes2), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes3), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes4), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes5), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes6), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out

    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowDistance), __FLT_MAX__); //Set the Distance of the Glow to Max float value so we can see a long Distance
}

void Glow::Glow() {
    int localTeam = process->Read<int>(localPlayer + 0x3E4);

    for (size_t ent = 0; ent < entities.size(); ent++) {

        uintptr_t entity = entities.at(ent);
        if (entity == localPlayer) {
            continue;
        }

        int team = process->Read<int>(entity + 0x3E4);
        if (team != localTeam) {
            WriteGlow(entity, teamColors[std::min(team, 20)]);
        }
    }
}

void Glow::GlowPlayer(uintptr_t entity, float *colors) {
    WriteGlow(entity, colors);
}
