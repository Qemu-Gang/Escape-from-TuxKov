#include "vmread/hlapi/hlapi.h"
#include "utils/Logger.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "utils/Memutils.h"
#include "features/Aimbot.h"
#include "features/Glow.h"
#include "features/DumbExploits.h"
#include "sdk/CBaseEntity.h"
#include "sdk/CGlobalVars.h"
#include "utils/Memutils.h"
#include "globals.h"
#include "utils/Wrappers.h"
#include "utils/minitrace.h"

#include "m0dular/utils/threading.h"
#include "m0dular/utils/pattern_scan.h"

#include <unistd.h> //getpid
#include <thread>
#include <atomic>
#include <csignal>
#include <numeric>
#include <thread>
#include <chrono>

#define USE_EAC_LAUNCHER

#ifdef USE_EAC_LAUNCHER
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
        char c = (char)getchar();

        if (c == 'Q')
            break;
    }

    return 0;
}
#endif

typedef std::chrono::high_resolution_clock Clock;

static bool sigscanFailed = false;

static void* ThreadSignature(const Signature* sig)
{
    MTR_SCOPED_TRACE("Initialization", "ThreadedSignature");

    *sig->result = PatternScan::FindPattern(sig->pattern, sig->module);

    if (!*sig->result) {
        Logger::Log("Failed to find pattern {%s}\n", sig->pattern);
        sigscanFailed = true;
    }

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

#ifdef MTR_ENABLED
    Logger::Log("Initialize performance tracing...\n");
    mtr_init("/tmp/ape-ex-trace.json");
    MTR_META_PROCESS_NAME("Ape-ex");
#endif

    Threading::InitThreads();

    try {
        MTR_BEGIN("Initialization", "InitCTX");
        WinContext ctx(pid);
        MTR_END("Initialization", "InitCTX");
        MTR_BEGIN("Initialization", "FindProcesses");
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
        MTR_END("Initialization", "FindProcesses");

        if (!process || !inputSystem) {
            Logger::Log("Could not Find Apex/InputSystem Process/Base. Exiting...\n");
            goto quit;
        }

        auto t1 = Clock::now();

        MTR_BEGIN("Initialization", "FindOffsets");
        Threading::QueueJobRef(Interfaces::FindInterfaces, MODNAME);
        //Netvars::FindNetvars( *process, MODNAME );

        for (const Signature& sig : signatures)
            Threading::QueueJobRef(ThreadSignature, &sig);

        Threading::FinishQueue(true);
        MTR_END("Initialization", "FindOffsets");

        if (sigscanFailed) {
            Logger::Log("One of the sigs failed. Stopping.\n");
            goto quit;
        }
        Logger::Log("Localplayer: %p\n", (void *) GetLocalPlayer());
        Logger::Log("Entlist: %p\n", (void *) entList);
        Logger::Log("GlobalVars: %p\n", (void *) globalVarsAddr);
        Logger::Log("input: %p\n", (void *)inputAddr);
        Logger::Log("clientstate: %p\n", (void*)clientStateAddr);

        auto t2 = Clock::now();
        printf("Initialization time: %lld ms\n", (long long)std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

        Logger::Log("Starting Main Loop.\n");

        static int lastFrame = 0;
        static int lastTick = 0;
        static bool updateWrites = false;

        userCmdArr = process->Read<uintptr_t>( inputAddr + OFFSET_OF(&CInput::m_commands) );
        verifiedUserCmdArr = process->Read<uintptr_t>( inputAddr + OFFSET_OF(&CInput::m_verifiedCommands) );

        while (running) {
            globalVars = process->Read<CGlobalVars>(globalVarsAddr);

            /* Per Tick Operations */
            updateWrites = (globalVars.tickCount > lastTick || globalVars.framecount != lastFrame);

            if (globalVars.tickCount > lastTick) {
                MTR_SCOPED_TRACE("MainLoop", "Tick");

                lastTick = globalVars.tickCount;

                sortedEntities.clear();

                for (int ent = 1; ent < 100; ent++) {
                    uintptr_t entity = GetEntityById(ent);
                    if (!entity) continue;
                    sortedEntities.push_back(ent);
                    entities[ent].Update(entity);
                }
                localPlayer.Update(GetLocalPlayer());

                //Aimbot::Aimbot();
            }

            /* Per Frame Operations */
            if (globalVars.framecount != lastFrame) {
                MTR_SCOPED_TRACE("MainLoop", "Frame");

                pressedKeys = inputSystem->Read<int>(inputBase + 0x4388);

                // read first 0x344 bytes of clienstate (next member we want after 0x344 is over 100k bytes away)
                VMemRead(&process->ctx->process, process->proc.dirBase, (uint64_t)&clientState, clientStateAddr, 0x344); 
                netChan = process->Read<CNetChan>((uint64_t)clientState.m_netChan);

                Exploits::ServerCrasher();

/*
                if (clientState.m_signonState == SIGNONSTATE_INGAMEAPEX && pressedKeys & KEY_MOUSE4) {
                    if (netChan.m_chokedCommands < 1) {
                        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), std::numeric_limits<double>::max());
                    }
                    else {
                        int32_t commandNr= process->Read<int32_t>(clientStateAddr + OFFSET_OF(&CClientState::m_lastOutGoingCommand));
                        int32_t targetCommand = (commandNr - 1) % 300;

                        CUserCmd userCmd = process->Read<CUserCmd>(userCmdArr + targetCommand * sizeof(CUserCmd));

                        userCmd.m_buttons |= IN_ATTACK;
                        userCmd.m_viewangles->y += 90.f;

                        process->Write<CUserCmd>(userCmdArr + targetCommand * sizeof(CUserCmd), userCmd);
                        process->Write<CUserCmd>(verifiedUserCmdArr + targetCommand * sizeof(CVerifiedUserCmd), userCmd);

                        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
                    }

                    Glow::Glow();
                }
                else
                    process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
*/

                lastFrame = globalVars.framecount;
            }
            if (updateWrites) {
                MTR_SCOPED_TRACE("MainLoop", "WriteBack");
                WriteList writeList(process);

                for (size_t i : sortedEntities)
                    entities[i].WriteBack(writeList);

                localPlayer.WriteBack(writeList);

                writeList.Commit();
            } 

            std::this_thread::sleep_for(std::chrono::microseconds(2000));
        }

        process->Write<double>(clientStateAddr + OFFSET_OF(&CClientState::m_nextCmdTime), 0.0);
        process->Write<float>( timescale, 1.0f ); // reset speedhack // reset speedhack

        Logger::Log("Main Loop Ended.\n");
    } catch (VMException &e) {
        Logger::Log("Initialization error: %d\n", e.value);
    }

  quit:
    running = false;

    Threading::FinishQueue(true);
    Threading::EndThreads();

#ifdef MTR_ENABLED
    mtr_flush();
    mtr_shutdown();
#endif

    Logger::Log("Main Ended.\n");

    return nullptr;
}

static void __attribute__((constructor)) Startup() {
    mainThread = Threading::StartThread(MainThread, nullptr, false);
}

static void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");

    running = false;

    Threading::JoinThread(mainThread, nullptr);

    Logger::Log("Done\n");
}
