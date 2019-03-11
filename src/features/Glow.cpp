#include "Glow.h"

#include "../utils/Wrappers.h"
#include "../sdk/CBaseEntity.h"


static Vector teamColors[] = {
        {0,   150, 0}, // green
        {0,   75,  0}, // dark green
        {75,  0,   0}, // dark red
        {150, 0,   0}, // red
        {0,   0,   75}, // dark blue
        {0,   0,   150}, // blue
        {75,  0,   75}, // purpleish
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
        {75,  75,  0}, // dank yellow
        {0,   150, 150}, // dark teal
        {255, 0,   255}, // hot pink - 20
        {255, 0,   255}, // hot pink - 20
};


static void WriteGlow(CBaseEntity &entity, Vector &colors, float distance) {
    distance *= 0.01905f; // metric
    distance /= 1.7f;
    distance = 255.0f - distance;
    std::clamp(distance, 1.0f, 255.0f);

    entity.bGlowEnable = true; // Enabling the Glow
    entity.iGlowEnable = 1; // Enabling the Glow


    entity.glowCol = colors / distance; // Setting a value for the Color Red between 0 and 255

    //process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowG), colors[1] / 100.0f); // Setting a value for the Color Red between 0 and 255
    //process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowB), colors[2] / 100.0f); // Setting a value for the Color Red between 0 and 255

    entity.glowTimes1 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes2 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes3 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes4 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes5 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out
    entity.glowTimes6 = __FLT_MAX__; // Setting the time of the Glow to be the Max Float value so it never runs out

    entity.glowDistance = __FLT_MAX__; //Set the Distance of the Glow to Max float value so we can see a long Distance
}

void Glow::Glow() {
    int localTeam = process->Read<int>(localPlayer + 0x3E4);
    Vector localPos = process->Read<Vector>(localPlayer + 0x12C);

    WriteList writeList(process);

    for (size_t entID = 0; entID < entities.size(); entID++) {

        uintptr_t entity = entities.at(entID);
        if (entity == localPlayer) {
            continue;
        }

        CBaseEntity ent(entity);
        ent.Update();

        int team = process->Read<int>(entity + 0x3E4);
        if (team != localTeam) {
            WriteGlow(ent, teamColors[std::min(team, 20)], localPos.DistTo(process->Read<Vector>(entity + 0x12C)));
        }

        ent.WriteBack(writeList);
    }

    writeList.Commit();
}

void Glow::GlowPlayer(uintptr_t entity, Vector &colors) {
    WriteList writeList(process);
    CBaseEntity ent(entity);
    ent.Update();
    WriteGlow(ent, colors, 100);
    ent.WriteBack(writeList);
}
