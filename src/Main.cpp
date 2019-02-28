#include "vmread/hlapi/hlapi.h"
#include "utils/Logger.h"
#include "utils/Scanner.h"
#include "utils/Memutils.h"
#include "Interfaces.h"
#include "sdk/CBaseEntity.h"

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

uintptr_t base;
static std::atomic<bool> running = true;

struct sigaction sa;
struct sigaction oldSa;

/* We need to stop threads before unloading.
 * The unload script will send a signal that we intercept here */
static void SignalHandler( int sigNum, siginfo_t *si, void * uContext )
{
    running = false;
}

uintptr_t GetEntityById(int ent, WinProcess &process) {
    uintptr_t baseEntity = process.Read<uintptr_t>(entList);
    if( !baseEntity ){
        return (uintptr_t)NULL;
    }

    return process.Read<uintptr_t>(entList + (ent << 5));
}

void WriteGlow( uintptr_t entity, float r, float g, float b, WinProcess *process ) {
    process->Write<bool>(entity + OFFSET_OF(&CBaseEntity::bGlowEnable), true); // Enabling the Glow
    process->Write<int>(entity + OFFSET_OF(&CBaseEntity::iGlowEnable), 1); // Enabling the Glow
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowR), r); // Setting a value for the Color Red between 0 and 255
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowG), g); // Setting a value for the Color Green between 0 and 255
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowB), b); // Setting a value for the Color Blue between 0 and 255

    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes1), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes2), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes3), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes4), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes5), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out
    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowTimes6), __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out

    process->Write<float>(entity + OFFSET_OF(&CBaseEntity::glowDistance), __FLT_MAX__); //Set the Distance of the Glow to Max float value so we can see a long Distance
}

void MainThread() {

    Logger::Log("Main Loaded.\n");
    pid_t pid = getpid();

    try {
        WinContext ctx(pid);
        ctx.processList.Refresh();

        for (WinProcess& i : ctx.processList) {
            //Logger::Log("Process name: %s[%d]\n", i.proc.name, i.proc.pid);
            if (strcasecmp(PROCNAME, i.proc.name))
                continue;
            Logger::Log("\nFound process %s(PID:%ld)", i.proc.name, i.proc.pid);
            PEB peb = i.GetPeb();
            short magic = i.Read<short>(peb.ImageBaseAddress);
            uintptr_t translatedBase = VTranslate(&i.ctx->process, i.proc.dirBase, peb.ImageBaseAddress);
            Logger::Log("\tWinBase:\t%p\tBase:\t%p\tQemuBase:\t%p\tMagic:\t%hx (valid: %hhx)\n", (void*)peb.ImageBaseAddress, (void*)i.proc.process, (void*)translatedBase, magic, (char)(magic == IMAGE_DOS_SIGNATURE));


            for (auto& o : i.modules) {
                //Logger::Log("\t%.8lx\t%.8lx\t%lx\t%s\n", o.info.baseAddress, o.info.entryPoint, o.info.sizeOfModule, o.info.name);
                if (!strcasecmp(MODNAME, o.info.name)){
                    base = o.info.baseAddress;
                    for (auto& u : o.exports) {
                        Logger::Log( "\t\t%lx\t%s\n", u.address, u.name );
                    }
                    Logger::Log("Base: %p\n", (void*)base);
                }
            }
            Interfaces::FindInterfaces( i, MODNAME );

            entList = GetAbsoluteAddressVm( i, Scanner::FindPatternInModule( "48 8D 05 ?? ?? ?? ?? 48 C1 E1 05 48 03 C8 0F B7 05 ?? ?? ?? ?? 39 41 08 75 51", MODNAME, i ), 3, 7 );

            // Infinite loop
            Logger::Log("Starting Main Loop.\n");
            while( base && running ){
                for( int ent = 1; ent < 100; ent++ ){
                    uintptr_t entity = GetEntityById(ent, i);
                    if( !entity )
                        continue;
                    WriteGlow( entity, 120.0f, 0.0f, 0.0f, &i );
                }
            }
            Logger::Log("Main Loop Ended.\n");
        }

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