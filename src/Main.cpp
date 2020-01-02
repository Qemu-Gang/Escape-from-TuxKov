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

#include "sdk/tags/weap.h"
#include "sdk/tags/matg.h"

//#define USE_EAC_LAUNCHER

#ifdef USE_EAC_LAUNCHER
#define PROCNAME "EasyAntiCheat_"
#define MODNAME "EasyAntiCheat_launcher.exe"
#else
#define PROCNAME "MCC-Win64-Ship"
#define PROCFULLNAME "MCC-Win64-Shipping.exe"
#define MODNAME "haloreach.dll"
#endif

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

static bool ParseStringTable( const char *buffer, size_t bufferSize, std::vector<std::string> *out ) {
    char currString[256];
    int index = 0;
    for( int i = 0; i < bufferSize; i++ ){
        currString[index] = buffer[i];
        if( currString[index] == '\0' ){
            index = 0;
            out->emplace_back( currString );
            continue;
        }


        index++;
        if( index >= 255 ){
            Logger::Log("Error - string at buffer index(%d) is too long!\n", i);
            return false;
        }
    }

    return true;
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
    mtr_init("/tmp/halo-reach-around.json");
    MTR_META_PROCESS_NAME("Halo");
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
                process = &i;

                for (auto &o : i.modules) {
                    if (!strcasecmp(MODNAME, o.info.name)) {
                        haloReachBase = o.info.baseAddress;
                        Logger::Log("Found Module: (%s) - baseAddr(%p)\n", o.info.name, o.info.baseAddress);
                    } else if (!strcasecmp(PROCFULLNAME, o.info.name)) {
                        mccBase = o.info.baseAddress;
                        Logger::Log("Found Module: (%s) - baseAddr(%p)\n", o.info.name, o.info.baseAddress);
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

        Logger::Log("g_tag_addresses @ Win(%p)\n", g_tag_addresses);

        MTR_END("Initialization", "FindSignatures");

        MTR_BEGIN("Initialization", "Setup");


        currentMapAddr = process->Read<uintptr_t>( currentMapAddr + 8 );
        Logger::Log("CurrentmapAddr(%p)\n", currentMapAddr);
        HaloMapHeader currentMap = process->Read<HaloMapHeader>( currentMapAddr );

        if( currentMap.header != 0x68656164 ){ // "daeh" - 0x68656164 -- seems to be zero when not in-game
            Logger::Log("Map header is invalid, we are not in game or shit is broke.\n");
            goto quit;
        }
        Logger::Log("Valid Map Header found.\n");

        // Load Map StringIDs - they normally start with nil, "default", "reload_1", "reload_2", "chamber_1".
        // Ranges from ~30-55k strings per map
        std::vector<std::string> stringIDs;
        // Load Map filenameStrings - starts with nil, nil, nil, "i've got a lovely bunch of coconuts", "there they are standing in a row"
        // About 10-15k filenames per map.
        std::vector<std::string> filenameStrings;
        // Basically typenames that tags can use, uses StringID.
        std::vector<ClassTableEntry> classes;
        // Tags are the variables/properties of the map
        std::vector<TagTableEntry> tags;

        currentMapStringIDTable = process->Read<uintptr_t>( currentMapStringIDTable );
        currentMapStringIDTable += 0x38;
        currentMapStringIDTable = process->Read<uintptr_t>( currentMapStringIDTable );

        uint64_t value = process->Read<uint64_t>( currentMapStringIDTable + 0x8 );

        Logger::Log("Virtual base addr(%p)\n", currentMap.virtualBaseAddr);

        /// happens on boardwalk.. very hacky lol
        if( value ){
            Logger::Log("Warning: non-zero value found in stringblobs(%x)... Trying hack\n", value);
            currentMapStringIDTable += 0x10;
            currentMapStringIDTable = process->Read<uintptr_t>( currentMapStringIDTable );
        }
        currentMapStringIDTable = process->Read<uintptr_t>( currentMapStringIDTable );

        char *buffer = new char[ currentMap.stringDataTableSize + 1 ];

        process->Read( currentMapStringIDTable, buffer, currentMap.stringDataTableSize );
        if( !ParseStringTable( buffer, currentMap.stringDataTableSize, &stringIDs ) ){
            delete buffer;
            Logger::Log("Failed to parse StringIDs!\n");
            goto quit;
        }

        delete buffer;
        buffer = new char[currentMap.fileNameTableSize + 1 ];

        ssize_t readStatus = process->Read( currentMap.fileNameData, buffer, currentMap.fileNameTableSize );
        if( readStatus < 0 ){
            Logger::Log("Hmmm read failed.\n");
            goto quit;
        }
        if( !ParseStringTable( buffer, currentMap.fileNameTableSize, &filenameStrings ) ){
            delete buffer;
            Logger::Log("Failed to parse filenames in map!\n");
            goto quit;
        }

        delete buffer;


        // Check for valid starting values - these are the same for all maps.
        if( strcmp( stringIDs[1].c_str(), "default" )  != 0 ){
            Logger::Log("Error: String parsing failed(is invalid) :(\n");
            goto quit;
        }
        if( strcmp( filenameStrings[3].c_str(), "i've got a lovely bunch of coconuts" ) != 0 ){
            Logger::Log("Error: filename parsing failed(is invalid)\n");
            goto quit;
        }

        Logger::Log("Found %d StringIDs and %d filenameStrings\n", stringIDs.size(), filenameStrings.size());

        // This "indexHeader" area of memory contains pointers to several Arrays/Tables and their sizes
        IndexHeader indexHeader = process->Read<IndexHeader>( (uintptr_t)currentMap.indexHeader );
        buffer = new char[ indexHeader.numClasses.value * sizeof(ClassTableEntry) ];
        process->Read( (uintptr_t)indexHeader.classTable, buffer, indexHeader.numClasses.value * sizeof(ClassTableEntry) );

        for( int i = 0; i < (indexHeader.numClasses.value * sizeof(ClassTableEntry)); i+=sizeof(ClassTableEntry) ){
            ClassTableEntry *entry = (ClassTableEntry*)&buffer[i];
            classes.push_back( *entry );
        }

        delete buffer;

        buffer = new char[ indexHeader.numTags.value * sizeof(TagTableEntry) ];
        process->Read( (uintptr_t)indexHeader.tagTable, buffer, indexHeader.numTags.value * sizeof(TagTableEntry) );

        for( int i = 0; i < (indexHeader.numTags.value * sizeof(TagTableEntry)); i+=sizeof(TagTableEntry) ){
            TagTableEntry *entry = (TagTableEntry*)&buffer[i];
            tags.push_back( *entry );
        }


        delete buffer;

        Logger::Log("Found %d classes and %d tags\n", classes.size(), tags.size());


        /*
        for( int i = 0; i < stringIDs.size(); i++ ){
            Logger::Log("String [%d] - (%s)\n", i, stringIDs[i].c_str());
        }

        for( int i = 0 ; i < filenameStrings.size(); i++ ){
            Logger::Log("Filename [%d] - (%s)\n", i, filenameStrings[i].c_str());
        }

        for( int i = 0; i < classes.size(); i++ ){
            Logger::Log("class %d - magic(%d) - parent(%d) -gparent(%d) - stringid(%d)\n", i, classes[i].magic, classes[i].parentMagic, classes[i].grandparentMagic, classes[i].stringID);
        }
        */

        debugPointerConverter = new SectionPointerConverter( currentMap.sections[SECTION_DEBUG], currentMap.sectionMasks[SECTION_DEBUG] );
        resourcePointerConverter = new SectionPointerConverter( currentMap.sections[SECTION_RESOURCE], currentMap.sectionMasks[SECTION_RESOURCE] );
        tagBufferPointerConverter = new SectionPointerConverter( currentMap.sections[SECTION_TAG], currentMap.sectionMasks[SECTION_TAG] );
        localePointerConverter = new SectionPointerConverter( currentMap.sections[SECTION_LOCALIZATION], currentMap.sectionMasks[SECTION_LOCALIZATION] );

        int sidIndexTableOff = debugPointerConverter->PointerToOffset( currentMap.stringIndexTableOffset );
        int sidDataOff = debugPointerConverter->PointerToOffset( currentMap.stringDataTableOffset );

        int nameIndexTableOff = debugPointerConverter->PointerToOffset( currentMap.fileIndexTableOffset );
        int nameDataOff = debugPointerConverter->PointerToOffset( currentMap.fileNameTableOffset );

        int tagDataOffset = tagBufferPointerConverter->PointerToOffset( currentMap.tagBufferOffset );
        int tagDataSize = currentMap.tagBufferSize;

        matg globals;
        PlayerTraitDefaultsSubTag playerTraits;
        ShieldsAndHealthSubTag shieldsAndHealth;
        WeaponsAndDamageSubTag weaponsAndDamage;
        MovementSubTag movement;

        weap gun;
        MagazineSubTag gunMag;
        BarrelsSubTag gunBarrel;
        TriggerSubTag gunTrigger;

        Logger::Log("Tag base Addr(%p) - tag[0](%p)\n", currentMap.partitions[5].loadAddr, ExpandPointer( tags[0].memoryAddressShortPtr ) );

        for( int i = 0; i < indexHeader.numTags.value; i++ ){

            const char *className = nullptr;
            ClassTableEntry *tagClass = nullptr;
            const char *fileName = nullptr;

            if( tags[i].classIndex > 0 && tags[i].classIndex < classes.size() ){
                tagClass = &classes[tags[i].classIndex];
                className = AllocClassMagicToString(tagClass->magic);
                fileName = filenameStrings[i].c_str();

                if( className && !strcmp("matg", className) ){
                    Logger::Log("Tag %d - class(%s) -filename(%s)- datumValue(%x) - addr(%p) -actualAddr(%p)\n", i, className, fileName,
                                DatumIndexToValue(tags[i].datumIndexSalt, i), ExpandPointer( tags[i].memoryAddressShortPtr ),
                                currentMap.partitions[5].loadAddr + (ExpandPointer( tags[i].memoryAddressShortPtr ) - ExpandPointer(tags[0].memoryAddressShortPtr)));
                    globals = process->Read<matg>( currentMap.partitions[5].loadAddr + (ExpandPointer( tags[i].memoryAddressShortPtr ) - ExpandPointer(tags[0].memoryAddressShortPtr)) );

                    Logger::Log("PlayerTraitDefaults @%p\n", currentMap.partitions[5].loadAddr + ((uint64_t)globals.playerTraitDefaults.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    playerTraits = process->Read<PlayerTraitDefaultsSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)globals.playerTraitDefaults.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );


                    /* I dont think any of these work in online multiplayer lol */
                    shieldsAndHealth = process->Read<ShieldsAndHealthSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)playerTraits.shieldsAndHealth.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)));
                    shieldsAndHealth.deathImmunity = DeathImmunity_t::ENABLED;
                    shieldsAndHealth.headShotImmunity = HeadShotImmunity_t::ENABLED;
                    shieldsAndHealth.assassinationImmunity = AssassinationImmunity_t::ENABLED;
                    shieldsAndHealth.shieldRechargeRate = ShieldRechargeRate_t::PERCENT_200;
                    shieldsAndHealth.shieldMultiplier = ShieldMultiplier_t::OVERSHIELD_4X;
                    shieldsAndHealth.dmgResist = DamageResist_t::INVULNERABLE;
                    shieldsAndHealth.hpMultiplier = HealthMultiplier_t::PERCENT_400;
                    shieldsAndHealth.hpRechargeRate = HealthRechargeRate_t::PERCENT_200;
                    process->Write<ShieldsAndHealthSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)playerTraits.shieldsAndHealth.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)), shieldsAndHealth );

                    weaponsAndDamage = process->Read<WeaponsAndDamageSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)playerTraits.weaponsAndDamage.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    weaponsAndDamage.infiniteAmmo = InfiniteAmmo_t::BOTTOMLESS_CLIP;
                    weaponsAndDamage.infiniteEquipment = InfiniteEquipment_t::ENABLED;
                    weaponsAndDamage.dmgModifier = DamageModifier_t::INSTANT_KILL;
                    weaponsAndDamage.dmgMeleeModifier = DamageModifier_t::INSTANT_KILL;
                    process->Write<WeaponsAndDamageSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)playerTraits.weaponsAndDamage.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)), weaponsAndDamage );

                    movement = process->Read<MovementSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)playerTraits.movement.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    movement.doubleJump = DoubleJump_t::ENABLED;
                    movement.jumpHeight = 300;
                    movement.playerSpeed = PlayerSpeed_t::PERCENT_300;
                    process->Write<MovementSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)playerTraits.movement.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)), movement );
                }

                if( className && !strcmp("weap", className )){
                    Logger::Log("Tag %d - class(%s) -filename(%s)- datumValue(%x) - addr(%p) -actualAddr(%p)\n", i, className, fileName,
                                DatumIndexToValue(tags[i].datumIndexSalt, i), ExpandPointer( tags[i].memoryAddressShortPtr ),
                                currentMap.partitions[5].loadAddr + (ExpandPointer( tags[i].memoryAddressShortPtr ) - ExpandPointer(tags[0].memoryAddressShortPtr)));
                    gun = process->Read<weap>( currentMap.partitions[5].loadAddr + (ExpandPointer( tags[i].memoryAddressShortPtr ) - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    gunMag = process->Read<MagazineSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)gun.magazines.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    gunBarrel = process->Read<BarrelsSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)gun.barrels.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    gunTrigger = process->Read<TriggerSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)gun.newTriggers.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    Logger::Log("Addr of magazine(%p)\n", currentMap.partitions[5].loadAddr + ((uint64_t)gun.magazines.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    Logger::Log("Addr of barrels(%p)\n", currentMap.partitions[5].loadAddr + ((uint64_t)gun.barrels.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );
                    Logger::Log("Addr of trigger(%p)\n", currentMap.partitions[5].loadAddr + ((uint64_t)gun.newTriggers.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)) );

                    gunMag.magazineSize = 500;
                    gunMag.startingAmmo = 9999;
                    gunMag.maxReserveAmmo = 9999;
                    gunMag.maxRoundsTotal = 9999;
                    gunMag.roundsPerReload = 500;
                    gunMag.reloadTime = 0.05f;

                    process->Write<MagazineSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)gun.magazines.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)), gunMag  );

                    gunBarrel.roundsPerSecondMin = 20.0f;
                    gunBarrel.roundsPerSecondMax = 20.0f;
                    gunBarrel.fireRecoveryTime = 0.02f;
                    gunBarrel.bloomRateOfDecay = 99.0f;
                    gunBarrel.errorAngleMax = 0.0f;
                    gunBarrel.errorAngleMaxRotation = 0.0f;
                    gunBarrel.errorAngleMin = 0.0f;

                    process->Write<BarrelsSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)gun.barrels.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)), gunBarrel );

                    gun.autoaimAngle = 85.0f;
                    gun.autoaimRangeLong = 450.0f;
                    gun.autoaimRangeShort = 234.0f;
                    gun.magnetismRangeLong = 234.0f;
                    gun.magnetismRangeShort = 10.0f;
                    gun.magnetismAngle = 450.0f;
                    gun.weaponFlags |= (uint32_t)WeaponFlags_t::THIRD_PERSON_CAMERA;


                    process->Write<weap>( currentMap.partitions[5].loadAddr + (ExpandPointer( tags[i].memoryAddressShortPtr ) - ExpandPointer(tags[0].memoryAddressShortPtr)), gun );

                    gunTrigger.behavior = TriggerBehavior_t::SPEW;

                    process->Write<TriggerSubTag>( currentMap.partitions[5].loadAddr + ((uint64_t)gun.newTriggers.GetTagOffset() - ExpandPointer(tags[0].memoryAddressShortPtr)), gunTrigger );
                }

                delete className;
            }

        }

        for( int i = 0; i < 4; i++ ){
            Logger::Log("Section (%d) - Offset(%p) - Size(%x)\n", i, (currentMap.sectionMasks[i] + currentMap.sections[i].virtualAddress), currentMap.sections[i].size);
        }
        for( int i = 0; i < 6; i++ ){
            Logger::Log("Partition (%d) - @(%p)->(%p) - Size(%x)\n", i, currentMap.partitions[i].loadAddr,
                        (uintptr_t(currentMap.partitions[i].loadAddr) + currentMap.partitions[i].size), currentMap.partitions[i].size);
        }
        for( int i = 0; i < 4; i++ ){
            Logger::Log("MemoryBuffer (%d) - @(%p)\n", i, currentMap.memoryBuffers[i]);
        }

        Logger::Log("tagDataOffset -(%x) - tagDataSize(%x)\n", tagDataOffset, tagDataSize);
        Logger::Log("sidIndexTableOff(%x) - sidDataOff(%x) - nameIndexTableOff(%x) - nameDataOff(%x)\n",
                    sidIndexTableOff, sidDataOff, nameIndexTableOff, nameDataOff);


        MTR_END("Initialization", "Setup");



        auto t2 = Clock::now();
        printf("Initialization time: %lld ms\n", (long long) std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());


        Logger::Log("Starting Main Loop.(q to quit)\n");

        static bool update = false;

        while (running) {
            update = false;

            if (update) {

            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(1000));
            }
        }

        Logger::Log("Main Loop Ended.\n");
    } catch (VMException &e) {
        Logger::Log("Initialization error: %d\n", e.value);
    }

    quit:
    running = false;

    Threading::FinishQueue(true);
    Threading::EndThreads();

    delete debugPointerConverter;
    delete resourcePointerConverter;
    delete tagBufferPointerConverter;
    delete localePointerConverter;

#ifdef MTR_ENABLED
    mtr_flush();
    mtr_shutdown();
#endif

    Logger::Log("Main Ended.\n");

    return nullptr;
}

static void __attribute__((constructor)) Startup() {
    //inputSystemThread = Threading::StartThread(InputSystem::InputSystem, nullptr, false);
    mainThread = Threading::StartThread(MainThread, nullptr, false);
}

static void __attribute__((destructor)) Shutdown() {
    Logger::Log("Unloading...");

    running = false;

    //Threading::JoinThread(inputSystemThread, nullptr);
    Threading::JoinThread(mainThread, nullptr);

    Logger::Log("Done\n");
}
