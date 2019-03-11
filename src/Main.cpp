#include "vmread/hlapi/hlapi.h"
#include "utils/Logger.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "features/Aimbot.h"
#include "features/Glow.h"
#include "sdk/CBaseEntity.h"
#include "sdk/CGlobalVars.h"
#include "utils/Scanner.h"
#include "utils/Memutils.h"
#include "globals.h"
#include "utils/Wrappers.h"

#include <unistd.h> //getpid
#include <thread>
#include <atomic>
#include <csignal>

#define SWAGGIN

#ifdef SWAGGIN
#define PROCNAME "EasyAntiCheat_"
#define MODNAME "EasyAntiCheat_launcher.exe"
#else
#define PROCNAME "r5apex.exe"
#define MODNAME "r5apex.exe"
#endif

static bool running = true;

#if (LMODE() == MODE_EXTERNAL())
int main() {
    while (running)
        usleep(1000000);
    return 0;
}
#endif

static void MainThread() {
    Logger::Log("Main Loaded.\n");
    pid_t pid;
#if (LMODE() == MODE_EXTERNAL())
    FILE* pipe = popen("pidof qemu-system-x86_64", "r");
    fscanf(pipe, "%d", &pid);
    pclose(pipe);
#else
    pid = getpid();
#endif
    try {
        //input.Init(kb);
        //input.Init(ms);

        WinContext ctx(pid);
        ctx.processList.Refresh();
        for (auto &i : ctx.processList) {
            if (!strcasecmp("inputsystem.ex", i.proc.name)) {
                inputSystem = &i;

                for (auto &o : i.modules) {
                    //Logger::Log("%s\n", o.info.name);
                    if (!strcasecmp("inputsystem.exe", o.info.name)) {
                        Logger::Log("Found InputSystem Base: %p\n", (void *) o.info.baseAddress);
                        inputBase = o.info.baseAddress;
                    }
                }
            }

            if (!strcasecmp(PROCNAME, i.proc.name)) {
                Logger::Log("\nFound Apex Process %s(PID:%ld)", i.proc.name, i.proc.pid);
                PEB peb = i.GetPeb();
                short magic = i.Read<short>(peb.ImageBaseAddress);
                uintptr_t translatedBase = VTranslate(&i.ctx->process, i.proc.dirBase, peb.ImageBaseAddress);
                Logger::Log("\tWinBase:\t%p\tBase:\t%p\tQemuBase:\t%p\tMagic:\t%hx (valid: %hhx)\n", (void *) peb.ImageBaseAddress, (void *) i.proc.process,
                            (void *) translatedBase,
                            magic, (char) (magic == IMAGE_DOS_SIGNATURE));
                process = &i;

                for (auto &o : i.modules) {
                    if (!strcasecmp(MODNAME, o.info.name)) {
                        apexBase = o.info.baseAddress;
                        for (auto &u : o.exports)
                            Logger::Log("\t\t%lx\t%s\n", u.address, u.name);
                    }
                }

            }
        }

        if (!process || !apexBase) {
            Logger::Log("Could not Find Apex Process/Base. Exiting...\n");
            running = false;
            return;
        }
        if (!inputSystem || !inputBase) {
            Logger::Log("Could not Find Input Process/Base. Exiting...\n");
            running = false;
            return;
        }

        Interfaces::FindInterfaces(*process, MODNAME);
        //Netvars::FindNetvars( *process, MODNAME );

        entList = GetAbsoluteAddressVm(*process, Scanner::FindPatternInModule("48 8D 05 ?? ?? ?? ?? 48 C1 E1 05 48 03 C8 0F B7 05 ?? ?? ?? ?? 39 41 08 75 51", MODNAME, *process),
                                       3, 7);
        sendpacket = Scanner::FindPatternInModule("41 B7 01 44 0F 29", MODNAME, *process) + 2;
        globalVars = process->Read<uintptr_t>(GetAbsoluteAddressVm(*process, Scanner::FindPatternInModule("4C 8B 15 ?? ?? ?? ?? 88", MODNAME, *process), 3, 7));
        netTime = GetAbsoluteAddressVm(*process, Scanner::FindPatternInModule("F2 0F 58 0D ?? ?? ?? ?? 66 0F 2F C1 77", MODNAME, *process), 4, 8);
        nextCmdTime = GetAbsoluteAddressVm(*process, Scanner::FindPatternInModule("F2 0F 10 05 ?? ?? ?? ?? F2 0F 58 0D", MODNAME, *process), 4, 8);

        Logger::Log("Entlist: %p\n", (void *) entList);
        Logger::Log("Localplayer: %p\n", (void *) GetLocalPlayer());
        Logger::Log("GlobalVars: %p\n", (void *) globalVars);
        Logger::Log("nextCmdTime: %p\n", (void *)nextCmdTime);
        Logger::Log("netTime: %p\n", (void *)netTime);

        Logger::Log("Starting Main Loop.\n");
        while (running) {
            static int oldTickCount = 0;
            CGlobalVars globalvars = process->Read<CGlobalVars>(globalVars);

            if (globalvars.tickCount == oldTickCount) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            } else if (globalvars.tickCount - oldTickCount > 1);
            //Logger::Log("tick: %i\n", globalvars.tickCount);

            oldTickCount = globalvars.tickCount;
            sortedEntities.clear();

            for (int ent = 1; ent < 100; ent++) {
                uintptr_t entity = GetEntityById(ent);
                if (!entity) continue;
                sortedEntities.push_back(ent);
                entities[ent].Update(entity);
            }
            localPlayer = GetLocalPlayer();

            Glow::Glow();
            Aimbot::Aimbot();

            WriteList writeList(process);

            for (size_t i : sortedEntities)
                entities[i].WriteBack(writeList);

            writeList.Commit();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        Logger::Log("Main Loop Ended.\n");

    } catch (VMException &e) {
        Logger::Log("Initialization error: %d\n", e.value);
        running = false;
        return;
    }
    Logger::Log("Main Ended.\n");
}

static void __attribute__((constructor)) Startup() {

    std::thread mainthread(MainThread);
    mainthread.detach();
}

static void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");

    running = false;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Logger::Log("Done\n");
}
