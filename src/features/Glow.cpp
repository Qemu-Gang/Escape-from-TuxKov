#include "Glow.h"

#include "../utils/Wrappers.h"
#include "../sdk/CBaseEntity.h"

static void WriteGlow( uintptr_t entity, float *colors )
{
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

void Glow::Glow(float *colors) {
    int teamTeamNumber = process->Read<int>(localPlayer + 0x3E4);

    for (size_t ent = 0; ent < entities.size(); ent++) {

        uintptr_t entity = entities.at(ent);
        if (entity == localPlayer) {
            continue;
        }

        int tmp = process->Read<int>(entity + 0x3E4);
        if (tmp == teamTeamNumber)
        {colors[0] = 0.0f; colors[1] = 125.0f; colors[3] = 0.0f;}

        WriteGlow( entity, colors );
    }
}

void Glow::GlowPlayer(uintptr_t entity, float *colors) {
    WriteGlow( entity, colors );
}
