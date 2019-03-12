#include "vmread/hlapi/hlapi.h"
#include "utils/Logger.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "features/Aimbot.h"
#include "features/Glow.h"
#include "sdk/CBaseEntity.h"
#include "sdk/CGlobalVars.h"
#include "utils/Memutils.h"
#include "globals.h"
#include "utils/Wrappers.h"

#include "m0dular/utils/threading.h"
#include "m0dular/utils/pattern_scan.h"

#include <unistd.h> //getpid
#include <thread>
#include <atomic>
#include <csignal>
#include <numeric>
#include <thread>
#include <chrono>

//#define SWAGGIN

#ifdef SWAGGIN
#define PROCNAME "EasyAntiCheat_"
#define MODNAME "EasyAntiCheat_launcher.exe"
#else
#define PROCNAME "r5apex.exe"
#define MODNAME "R5Apex.exe"
#endif

#include "Signatures.h"

static bool running = true;
static thread_t mainThread;

#if (LMODE() == MODE_EXTERNAL())
int main() {
    while (running){
        usleep(1000000);
    }

    return 0;
}
#endif

typedef std::chrono::high_resolution_clock Clock;

static void* ThreadSignature(const Signature* sig)
{
    *sig->result = PatternScan::FindPattern(sig->pattern, sig->module);

    if (!*sig->result)
        Logger::Log("Failed to find pattern {%s}\n", sig->pattern);

    return nullptr;
}

static void* MainThread(void*) {
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
        WinContext ctx(pid);
        ctx.processList.Refresh();
        for (auto &i : ctx.processList) {
            if (!inputSystem && !strcasecmp("inputsystem.ex", i.proc.name)) {
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

        if (!process || !apexBase || !inputSystem || !inputBase) {
            Logger::Log("Could not Find Apex/InputSystem Process/Base. Exiting...\n");
            running = false;
            return nullptr;
        }

        auto t1 = Clock::now();

        Threading::QueueJobRef(Interfaces::FindInterfaces, MODNAME);
        //Netvars::FindNetvars( *process, MODNAME );

        for (const Signature& sig : signatures)
            Threading::QueueJobRef(ThreadSignature, &sig);

        Threading::FinishQueue(true);

        if (!entList || !globalVars || !netTime || !nextCmdTime || !signonState || !netChannel) {
            Logger::Log("One of the sigs failed. Stopping.\n");
            running = false;
            return nullptr;
        }
        Logger::Log("Localplayer: %p\n", (void *) GetLocalPlayer());
        Logger::Log("Entlist: %p\n", (void *) entList);
        Logger::Log("GlobalVars: %p\n", (void *) globalVars);
        Logger::Log("nextCmdTime: %p\n", (void *) nextCmdTime);
        Logger::Log("netTime: %p\n", (void *) netTime);
        Logger::Log("SignonState: %p\n", (void *) signonState);
        Logger::Log("netChannel: %p\n", (void *) netChannel);

        auto t2 = Clock::now();
        printf("Initialization time: %lld ms\n", (long long)std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

        Logger::Log("Starting Main Loop.\n");

        static int lastFrame = 0;
        static int lastTick = 0;
        static bool updateWrites = false;

        while (running) {
            CGlobalVars globalvars = process->Read<CGlobalVars>(globalVars);
            sendpacket = (process->Read<int>(netChannel + 0x10) > 12);
            /* Per Tick Operations */
            updateWrites = (globalvars.tickCount > lastTick || globalvars.framecount != lastFrame);

            if (globalvars.tickCount > lastTick) {
                //if (globalvars.tickCount != lastTick + 1) {
                //Logger::Log("Missed a Tick!: [%d->%d]\n", lastTick, globalvars.tickCount);
                //}
                lastTick = globalvars.tickCount;

                sortedEntities.clear();

                for (int ent = 1; ent < 100; ent++) {
                    uintptr_t entity = GetEntityById(ent);
                    if (!entity) continue;
                    sortedEntities.push_back(ent);
                    entities[ent].Update(entity);
                }
                localPlayer.Update(GetLocalPlayer());
                Aimbot::Aimbot();
            }
            /* Per Frame Operations */
            if (globalvars.framecount != lastFrame) {
                Glow::Glow();
                lastFrame = globalvars.framecount;
            }
            if (updateWrites) {
                WriteList writeList(process);

                for (size_t i : sortedEntities)
                    entities[i].WriteBack(writeList);

                localPlayer.WriteBack(writeList);

                writeList.Commit();

            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(2000));
            }
            process->Write<double>(nextCmdTime, sendpacket ? 0.0 : std::numeric_limits<double>::max());
        }

        process->Write<double>(nextCmdTime, 0.0); // reset sendpacket
        Logger::Log("Main Loop Ended.\n");
    } catch (VMException &e) {
        Logger::Log("Initialization error: %d\n", e.value);
        running = false;
        return nullptr;
    }

    Logger::Log("Main Ended.\n");

    return nullptr;
}

static void __attribute__((constructor)) Startup() {
    Threading::InitThreads();
    mainThread = Threading::StartThread(MainThread, nullptr);
}

static void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");

    running = false;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Threading::JoinThread(mainThread, nullptr);
    Threading::EndThreads();

    Logger::Log("Done\n");
}
