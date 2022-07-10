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
#include "sdk/mono.h"
#include "peeper/client/peeper.h"
#include "hacks/esp.h"
#include "hacks/norecoil.h"
#include "hacks/aimbot.h"

#define PROCNAME "EscapeFromTarkov.exe"
#define MODNAME "UnityPlayer.dll"
#define MODNAME_MONO "mono-2.0-bdwgc.dll"

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

static void UpdateData()
{
    if( localPlayerAddr )
    {
        Player localPlayer = process->Read<Player>( localPlayerAddr );
        ProceduralWeaponAnimation localWeaponAnim = process->Read<ProceduralWeaponAnimation>( (uintptr_t)localPlayer.m_pProceduralWeaponAnimation );
        BreathEffector breathEffector = process->Read<BreathEffector>( (uintptr_t)localWeaponAnim.m_pBreath );
        localPlayerIsAiming = breathEffector.IsAiming;

        //OpticSight sight = process->Read<OpticSight>( (uintptr_t) localWeaponAnim.m_pOpticSightArray );
        //localPlayerIsOpticCamera = (bool)sight.m_pCamera;
    }
    if( cameraAddr )
    {
        cameraEntity = process->Read<CameraEntity>( cameraAddr );
    }
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
                    }
                    else if ( strstr( o.info.name, MODNAME_MONO ) ){
                        Logger::Log("Found Module: (%s) - baseAddr(%p)\n", o.info.name, o.info.baseAddress);
                        monoBase = o.info.baseAddress;
                        //Logger::Log("\t%.8lx\t%.8lx\t%lx\t%s\n", o.info.baseAddress, o.info.entryPoint, o.info.sizeOfModule, o.info.name);
                        for (auto& u : o.exports)
                        {
                            //Logger::Log("\t\t%lx\t%s\n", u.address, u.name);
                            if (!strcmp(u.name, "mono_get_root_domain"))
                            {
                                // 48 8B 05 21 CD 46 00    mov rax, cs:qword_12321213
                                // C3                      retn
                                monoRootDomain = GetAbsoluteAddressVm(*process, u.address, 3, 7);
                                Logger::Log("Mono Root domain(%p)\n", monoRootDomain);
                            }
                        }
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

        gameWorldAddr = Unity::GetWorldPtr();
        Logger::Log("Gameworld(%p)\n", gameWorldAddr);


        //Logger::Log("-----Tagged Objects-----\n");
        //Unity::PrintGOMObjects( true );
        //Logger::Log("------------------------\n");
        //Logger::Log("-----Active Objects-----\n");
        //Unity::PrintGOMObjects( false );
        //Logger::Log("------------------------\n");

        Unity::PrintItemStats();
        Unity::PrintPlayerList();

        MTR_END("Initialization", "Setup");

        //mono::init_functions( );
//
        //static auto eft_hard_settings = mono::find_class( "Assembly-CSharp", "EFTHardSettings" )->get_vtable( mono::get_root_domain( ) )->get_static_field_data( );
        //printf( "[EFTHardSettings] -> %p\n", eft_hard_settings );

        auto t2 = Clock::now();
        Logger::Log("Initialization time: %lld ms\n", (long long) std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

        Logger::Log("Starting Main Loop.(q to quit)\n");

        mainLoopDone = false;
        while (running) {
            static uint64_t callCount = 0;
            static std::time_t t = 0;
            std::time_t currentTime = std::time(0);
            callCount++;
            if( (currentTime - t) >= 1 ){
                Logger::Log("Hz in last second: %lld\n", callCount);
                callCount = 0;
                t = currentTime;
            }

            UpdateData();
            ESP::DrawPlayers();
            ESP::DrawItems();
            NoRecoil::ApplyNoRecoil();
            //Aimbot::Aim();

            //sleep:
            //std::this_thread::sleep_for(std::chrono::microseconds(1000));
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
    inputSystemThread = Threading::StartThread(InputSystem::InputSystem, nullptr, false);
    mainThread = Threading::StartThread(MainThread, nullptr, false);
}

static void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");

    running = false;

    while( !mainLoopDone ){
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    }

    Peeper::ClearDraws();
    Peeper::Close();

    if( inputSystemThread )
        Threading::JoinThread(inputSystemThread, nullptr);
    if( mainThread )
        Threading::JoinThread(mainThread, nullptr);

    Logger::Log("Done\n");
}
