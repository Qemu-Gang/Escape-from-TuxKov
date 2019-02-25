#include "vmread/hlapi/hlapi.h"
#include <unistd.h> //getpid
#include <thread>
#include <atomic>
#include <csignal>

//#define SWAGGIN

#ifdef SWAGGIN
    #define PROCNAME "EasyAntiCheat_"
    #define MODNAME "EasyAntiCheat_launcher.exe"
#else
    #define PROCNAME "r5apex.exe"
    #define MODNAME "r5apex.exe"
#endif


FILE* out;
uintptr_t base;
pthread_t mainThread;
static std::atomic<bool> running = true;

struct sigaction sa;
struct sigaction oldSa;

/* We need to stop threads before unloading.
 * The unload script will send a signal that we intercept here */
static void SignalHandler( int sigNum, siginfo_t *si, void * uContext )
{
    running = false;
}

uintptr_t GetEntityById(int ent, const WinProcess *process) {
    uintptr_t entList = base + 0x1F6CAB8;
    uintptr_t baseEntity = process->Read<uintptr_t>(entList);
    if( !baseEntity ){
        return NULL;
    }

    return process->Read<uintptr_t>(entList + (ent << 5));
}
void WriteGlow( uintptr_t entity, float r, float g, float b, WinProcess *process ) {
    process->Write<bool>(entity + 0x380, true); // Enabling the Glow
    process->Write<int>(entity + 0x2F0, 1); // Enabling the Glow
    process->Write<float>(entity + 0x1B0, r); // Setting a value for the Color Red between 0 and 255
    process->Write<float>(entity + 0x1B4, g); // Setting a value for the Color Green between 0 and 255
    process->Write<float>(entity + 0x1B8, b); // Setting a value for the Color Blue between 0 and 255

    for (int offset = 0x2B0; offset <= 0x2C8; offset += 0x4) // Setting the of the Glow at all necessary spots
        process->Write<float>(entity + offset, __FLT_MAX__); // Setting the time of the Glow to be the Max Float value so it never runs out

    process->Write<float>(entity + 0x2DC, __FLT_MAX__); //Set the Distance of the Glow to Max float value so we can see a long Distance
}

void MainThread() {
    out = fopen("/tmp/apex.log", "w"); // create new log
    setbuf( out, nullptr ); // Turn off buffered I/O, decreases performance but if crash occurs, no unflushed buffer.

    fprintf(out, "--Start of log--\n");
    pid_t pid = getpid();

    try {
        WinContext ctx(pid);
        ctx.processList.Refresh();

        for (WinProcess& i : ctx.processList) {
            if (strcasecmp(PROCNAME, i.proc.name))
                continue;
            fprintf(out, "\nFound process %s(PID:%ld)", i.proc.name, i.proc.pid);
            PEB peb = i.GetPeb();
            short magic = i.Read<short>(peb.ImageBaseAddress);
            uintptr_t translatedBase = VTranslate(&i.ctx->process, i.proc.dirBase, peb.ImageBaseAddress);
            fprintf(out, "\tWinBase:\t%p\tBase:\t%p\tQemuBase:\t%p\tMagic:\t%hx (valid: %hhx)\n", (void*)peb.ImageBaseAddress, (void*)i.proc.process, (void*)translatedBase, magic, (char)(magic == IMAGE_DOS_SIGNATURE));

            for (auto& o : i.modules) {
                //fprintf(out, "\t%.8lx\t%.8lx\t%lx\t%s\n", o.info.baseAddress, o.info.entryPoint, o.info.sizeOfModule, o.info.name);
                if (!strcasecmp(MODNAME, o.info.name)){
                    base = o.info.baseAddress;
                    for (auto& u : o.exports)
                        fprintf(out, "\t\t%lx\t%s\n", u.address, u.name);
                }
            }

            fclose(out);
            // Infinite loop
            while( base && running ){
                for( int ent = 1; ent < 100; ent++ ){
                    //fprintf( out, "Entity @%p\n", (void*)GetEntityById(ent, &i) );
                    uintptr_t entity = GetEntityById(ent, &i);
                    if( !entity )
                        continue;
                    WriteGlow( entity, 120.0f, 0.0f, 0.0f, &i );
                }
            }
        }

    } catch (VMException& e) {
        //fprintf(out, "Initialization error: %d\n", e.value);
        //fclose(out);
        return;
    }
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
    sigaction( SIGXCPU, &oldSa, NULL );
}