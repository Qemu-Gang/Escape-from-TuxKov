#include "utils.h"
#include "utils/Wrappers.h"
#include "features/Aimbot.h"
#include "features/Glow.h"


//#define SWAGGIN

#ifdef SWAGGIN
#define PROCNAME "EasyAntiCheat_"
#define MODNAME "EasyAntiCheat_launcher.exe"
#else
#define PROCNAME "r5apex.exe"
#define MODNAME "r5apex.exe"
#endif


FILE *out; // TODO: put in utils.h


static std::atomic<bool> running = true;

struct sigaction sa;
struct sigaction oldSa;


/* We need to stop threads before unloading.
 * The unload script will send a signal that we intercept here */
static void SignalHandler(int sigNum, siginfo_t *si, void *uContext) {
    running = false;
}


void MainThread() {
    out = fopen("/tmp/apex.log", "w"); // create new log
    setbuf(out, nullptr); // Turn off buffered I/O, decreases performance but if crash occurs, no unflushed buffer.

    fprintf(out, "--Start of log--\n");
    pid_t pid = getpid();

    try {
        //input.Init(kb);
//        input.Init(ms);

        WinContext ctx(pid);
        ctx.processList.Refresh();
        fprintf(out, "started search loop with pid %i\n", pid);
        for (auto &i : ctx.processList) {
            if (strcasecmp("inputsystem.ex", i.proc.name)) {
                //fprintf(out, "%s\n", i.proc.name);
                continue;
            }
            fprintf(out, "FOUND INPUTSYSTEM PROCESS %p\n", (void *) i.proc.process);
            inputSystem = &i;

            for (auto &o : i.modules) {
                fprintf(out, "%s\n", o.info.name);
                //fprintf(out, "\t%.8lx\t%.8lx\t%lx\t%s\n", o.info.baseAddress, o.info.entryPoint, o.info.sizeOfModule, o.info.name);
                if (!strcasecmp("inputsystem.exe", o.info.name)) {
                    inputSystemBase = o.info.baseAddress;
                    fprintf(out, "found input add\n");
                }
            }
            break;
        }
        for (auto &i : ctx.processList) {
            if (strcasecmp(PROCNAME, i.proc.name)) {

                continue;
            }
            fprintf(out, "\nFound process %s(PID:%ld)", i.proc.name, i.proc.pid);
            PEB peb = i.GetPeb();
            short magic = i.Read<short>(peb.ImageBaseAddress);
            uintptr_t translatedBase = VTranslate(&i.ctx->process, i.proc.dirBase, peb.ImageBaseAddress);
            fprintf(out, "\tWinBase:\t%p\tBase:\t%p\tQemuBase:\t%p\tMagic:\t%hx (valid: %hhx)\n", (void *) peb.ImageBaseAddress, (void *) i.proc.process, (void *) translatedBase,
                    magic, (char) (magic == IMAGE_DOS_SIGNATURE));
            fprintf(out, "Inputsystem base: %p\n", (void *) inputSystem->proc.process);
            fprintf(out, "Final Read Add: %p\n", (void *) (inputSystem->proc.process + 0x1D138));

            for (auto &o : i.modules) {
                //fprintf(out, "\t%.8lx\t%.8lx\t%lx\t%s\n", o.info.baseAddress, o.info.entryPoint, o.info.sizeOfModule, o.info.name);
                if (!strcasecmp(MODNAME, o.info.name)) {
                    apexLegendsBase = o.info.baseAddress;
                    for (auto &u : o.exports)
                        fprintf(out, "\t\t%lx\t%s\n", u.address, u.name);
                }
            }

            //fclose(out);
            // Infinite loop
            process = &i;
            static float col[3] = {125.0f, 0.0f, 0.0f};
            fprintf(out, "Initialized!\n");
            while (apexLegendsBase && running && inputSystemBase) {
                /*if (!localPlayer) {
                    localPlayer = GetLocalPlayer(&i);
                    continue;
                }

                static bool drawn = false;
                if (localPlayer && !drawn) {
                    localPlayer = GetLocalPlayer(&i);
                }
                int health = i.Read<int>(localPlayer + 0x3D4);
                if (health < 1) {
                    drawn = false;
                    continue;
                }*/
                entities.clear();
                for (int ent = 1; ent < 100; ent++) {
                    //fprintf( out, "Entity @%p\n", (void*)GetEntityById(ent, &i) );
                    uintptr_t entity = GetEntityById(ent);

                    entities.push_back(entity);
                }
                localPlayer = GetLocalPlayer();

                Glow::Glow(col, out);
                Aimbot::Aimbot(out);

                std::this_thread::sleep_for(std::chrono::microseconds(1000));
            }
        }
        fprintf(out, "meme error!");

    } catch (VMException &e) {
        fprintf(out, "Initialization error: %d\n", e.value);
        fclose(out);
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
    sigaction(SIGXCPU, &oldSa, NULL);
}