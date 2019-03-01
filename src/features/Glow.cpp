#include "Glow.h"

void Glow::Glow(float *colors, FILE *out) {
    if (entities.empty()) {
        return;
    }
    for (int ent = 0; ent < entities.size(); ent++) {

        uintptr_t entity = entities.at(ent);
        if (!entity || entity == localPlayer) {
            continue;
        }
        process->Write<bool>(entity + 0x380, true); // Enabling the Glow
        process->Write<int>(entity + 0x2F0, 1); // Enabling the Glow
        process->Write<float>(entity + 0x1B0, colors[0]); // Setting a value for the Color Red between 0 and 255
        process->Write<float>(entity + 0x1B4, colors[1]); // Setting a value for the Color Green between 0 and 255
        process->Write<float>(entity + 0x1B8, colors[2]); // Setting a value for the Color Blue between 0 and 255

        for (int offset = 0x2B0; offset <= 0x2C8; offset += 0x4) // Setting the of the Glow at all necessary spots
            process->Write<float>(entity + offset, __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out

        process->Write<float>(entity + 0x2DC, __FLT_MAX__); //Set the Distance of the Glow to Max float value so we can see a long Distance
    }
}

void Glow::GlowPlayer(uintptr_t entity, float *colors) {
    process->Write<bool>(entity + 0x380, true); // Enabling the Glow
    process->Write<int>(entity + 0x2F0, 1); // Enabling the Glow
    process->Write<float>(entity + 0x1B0, colors[0]); // Setting a value for the Color Red between 0 and 255
    process->Write<float>(entity + 0x1B4, colors[1]); // Setting a value for the Color Green between 0 and 255
    process->Write<float>(entity + 0x1B8, colors[2]); // Setting a value for the Color Blue between 0 and 255

    for (int offset = 0x2B0; offset <= 0x2C8; offset += 0x4) // Setting the of the Glow at all necessary spots
        process->Write<float>(entity + offset, __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out

    process->Write<float>(entity + 0x2DC, __FLT_MAX__);
}
