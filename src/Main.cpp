#include "vmread/hlapi/hlapi.h"
#include "utils/Logger.h"
#include "Interfaces.h"
#include "Netvars.h"
#include "features/Aimbot.h"
#include "features/Glow.h"
#include "sdk/CBaseEntity.h"
#include "utils/Scanner.h"
#include "utils/Memutils.h"

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

static std::atomic<bool> running = true;

struct sigaction sa;
struct sigaction oldSa;


/* We need to stop threads before unloading.
 * The unload script will send a signal that we intercept here */
static void SignalHandler(int sigNum, siginfo_t *si, void *uContext) {
    running = false;
}


void MainThread() {
    Logger::Log("Main Loaded.\n");
    pid_t pid = getpid();

    try {
        //input.Init(kb);
        //input.Init(ms);

        WinContext ctx(pid);
        ctx.processList.Refresh();
        for (auto &i : ctx.processList) {
            if (!strcasecmp("inputsystem.ex", i.proc.name)) {
                inputSystem = &i;

                for (auto &o : i.modules) {
                    Logger::Log("%s\n", o.info.name);
                    if (!strcasecmp("inputsystem.exe", o.info.name)) {
                        Logger::Log("Found InputSystem Base: %p\n", (void*)o.info.baseAddress);
                        inputBase = o.info.baseAddress;
                    }
                }
            }

            if (!strcasecmp(PROCNAME, i.proc.name)) {
                Logger::Log("\nFound Apex Process %s(PID:%ld)", i.proc.name, i.proc.pid);
                PEB peb = i.GetPeb();
                short magic = i.Read<short>(peb.ImageBaseAddress);
                uintptr_t translatedBase = VTranslate(&i.ctx->process, i.proc.dirBase, peb.ImageBaseAddress);
                Logger::Log("\tWinBase:\t%p\tBase:\t%p\tQemuBase:\t%p\tMagic:\t%hx (valid: %hhx)\n", (void *) peb.ImageBaseAddress, (void *) i.proc.process, (void *) translatedBase,
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

        if( !process || !apexBase ){
            Logger::Log("Could not Find Apex Process/Base. Exiting...\n");
            return;
        }
        if( !inputSystem || !inputBase ){
            Logger::Log("Could not Find Input Process/Base. Exiting...\n");
            return;
        }

        Interfaces::FindInterfaces( *process, MODNAME );
        Netvars::FindNetvars( *process, MODNAME );
        localPlayer = GetLocalPlayer();

        entList = GetAbsoluteAddressVm( *process, Scanner::FindPatternInModule( "48 8D 05 ?? ?? ?? ?? 48 C1 E1 05 48 03 C8 0F B7 05 ?? ?? ?? ?? 39 41 08 75 51", MODNAME, *process ), 3, 7 );

        static float col[3] = {125.0f, 0.0f, 0.0f};
        Logger::Log("Starting Main Loop.\n");
        while ( running ) {
            entities.clear();
            for (int ent = 1; ent < 100; ent++) {
                uintptr_t entity = GetEntityById(ent);
                if( !entity ) continue;
                entities.push_back(entity);
            }
            Glow::Glow(col);
            Aimbot::Aimbot();

            std::this_thread::sleep_for(std::chrono::microseconds(1000));
        }
        Logger::Log("Main Loop Ended.\n");

    } catch (VMException& e) {
        Logger::Log("Initialization error: %d\n", e.value);
        return;
    }
    Logger::Log("Main Ended.\n");
}

void __attribute__((constructor)) Startup() {
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = SignalHandler;
    sigaction(SIGXCPU, &sa, &oldSa);

    std::thread mainthread(MainThread);
    mainthread.detach();
}

void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");
    sigaction( SIGXCPU, &oldSa, NULL );
    Logger::Log("Done\n");
}