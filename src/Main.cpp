#include "vmread/hlapi/hlapi.h"
#include "utils/Logger.h"
#include "utils/Memutils.h"
#include "globals.h"
#include "utils/minitrace.h"
#include "utils/InputSystem.h"

#include "m0dular/utils/threading.h"
#include "m0dular/utils/pattern_scan.h"

#include <unistd.h> //getpid
#include <thread>
#include <atomic>
#include <csignal>
#include <numeric>
#include <thread>
#include <chrono>
#include <iostream>

#include "sdk/unity.h"
#include "peeper/client/peeper.h"
#include "hacks/esp.h"
#include "hacks/norecoil.h"

#define PROCNAME "EscapeFromTark"
#define MODNAME "UnityPlayer.dll"

#include "Signatures.h"

static thread_t mainThread;
static thread_t inputSystemThread;

#if (LMODE() == MODE_EXTERNAL())

int main() {
    while (running) {
        char c = (char) getchar();

        if (c == 'Q' || c =='q')
            break;
    }

    return 0;
}
#endif

typedef std::chrono::high_resolution_clock Clock;

static bool sigscanFailed = false;

static void *ThreadSignature(const Signature *sig) {
    MTR_SCOPED_TRACE("Initialization", "ThreadedSignature");

    *sig->result = PatternScan::FindPattern(sig->pattern, sig->module);

    if (!*sig->result) {
        Logger::Log("Failed to find pattern {%s}\n", sig->pattern);
        sigscanFailed = true;
    }

    return nullptr;
}

static void *MainThread(void *) {
    Logger::Log("Main Loaded.\n");
    pid_t pid;

#if (LMODE() == MODE_EXTERNAL())
    FILE *pipe = popen("pidof qemu-system-x86_64", "r");
    fscanf(pipe, "%d", &pid);
    pclose(pipe);
#else
    pid = getpid();
#endif

#ifdef MTR_ENABLED
    Logger::Log("Initialize performance tracing...\n");
    mtr_init("/tmp/tuxkov.json");
    MTR_META_PROCESS_NAME("TuxKov");
#endif

    Threading::InitThreads();
    auto t1 = Clock::now();

    try {
        MTR_BEGIN("Initialization", "InitCTX");
        WinContext ctx(pid);
        MTR_END("Initialization", "InitCTX");

        MTR_BEGIN("Initialization", "FindProcesses");
        ctx.processList.Refresh();
        for (auto &i : ctx.processList) {
            //Logger::Log("\nFound Process %s(PID:%ld)", i.proc.name, i.proc.pid);
            if (!strcasecmp(PROCNAME, i.proc.name)) {
                Logger::Log("\nFound Process %s(PID:%ld)", i.proc.name, i.proc.pid);
                PEB peb = i.GetPeb();
                short magic = i.Read<short>(peb.ImageBaseAddress);
                uintptr_t translatedBase = VTranslate(&i.ctx->process, i.proc.dirBase, peb.ImageBaseAddress);
                Logger::Log("\tWinBase:\t%p\tBase:\t%p\tQemuBase:\t%p\tMagic:\t%hx (valid: %hhx)\n", (void *) peb.ImageBaseAddress, (void *) i.proc.process,
                            (void *) translatedBase,
                            magic, (char) (magic == IMAGE_DOS_SIGNATURE));
                for (auto &o : i.modules) {
                    if( strstr( o.info.name, MODNAME ) ){
                        Logger::Log("Found Module: (%s) - baseAddr(%p)\n", o.info.name, o.info.baseAddress);
                        process = &i;
                        unityplayerBase = o.info.baseAddress;
                        break;
                    }
                }
            }
        }
        MTR_END("Initialization", "FindProcesses");

        if (!process) {
            Logger::Log("Could not Find Process/Base. Exiting...\n");
            goto quit;
        }

        MTR_BEGIN("Initialization", "FindSignatures");

        for (const Signature &sig : signatures)
            Threading::QueueJobRef(ThreadSignature, &sig);

        Threading::FinishQueue(true);

        if (sigscanFailed) {
            Logger::Log("One of the sigs failed. Stopping. (Make sure you are in-game)\n");
            goto quit;
        }

        MTR_END("Initialization", "FindSignatures");

        MTR_BEGIN("Initialization", "Setup");
        Logger::Log("gom(%p)\n", gomAddr);
        gom = process->Read<GlobalObjectManager>( gomAddr );

        cameraAddr = Unity::GetObjectPtrByName( "FPS Camera", true );
        Logger::Log("Camera(%p)\n", cameraAddr );

        uintptr_t gameWorldWrapperAddr = Unity::GetObjectPtrByName("GameWorld", false );
        if( gameWorldWrapperAddr ){
            GameWorldWrapper gameWorldWrapper = process->Read<GameWorldWrapper>( gameWorldWrapperAddr );
            gameWorldAddr = (uintptr_t)gameWorldWrapper.localgameworld;
        }
        Logger::Log("Gameworld(%p)\n", gameWorldAddr);


        //Logger::Log("-----Tagged Objects-----\n");
        //Unity::PrintGOMObjects( true );
        //Logger::Log("------------------------\n");
        //Logger::Log("-----Active Objects-----\n");
        //Unity::PrintGOMObjects( false );
        //Logger::Log("------------------------\n");

        Unity::PrintPlayerList();

        MTR_END("Initialization", "Setup");


        auto t2 = Clock::now();
        printf("Initialization time: %lld ms\n", (long long) std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

        Logger::Log("Starting Main Loop.(q to quit)\n");

        mainLoopDone = false;
        while (running) {
            cameraAddr = Unity::GetObjectPtrByName( "FPS Camera", true );

            // Not in-game
            if( !cameraAddr ){
                Peeper::StopDraws();
                goto sleep;
            }

            ESP::DrawPlayers();
            NoRecoil::ApplyNoRecoil();

            sleep:
            std::this_thread::sleep_for(std::chrono::microseconds(1500));
        }
        mainLoopDone = true;
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
    int peeperCode = Peeper::Open();
    if( peeperCode ){
        Logger::Log("Couldn't start peeper! Code: %d \n", peeperCode);
        return;
    }
    //inputSystemThread = Threading::StartThread(InputSystem::InputSystem, nullptr, false);
    mainThread = Threading::StartThread(MainThread, nullptr, false);
}

static void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");

    running = false;

    while( !mainLoopDone ){
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    }

    Peeper::StopDraws();
    Peeper::Close();

    //Threading::JoinThread(inputSystemThread, nullptr);
    if( mainThread )
        Threading::JoinThread(mainThread, nullptr);

    Logger::Log("Done\n");
}
