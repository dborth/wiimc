// Translated by:  Daniel Beňa, benad (at) centrum.cz
// last sync on 2006-04-28 with 1.249
// but not compleated

// ========================= MPlayer help ===========================

// Preklad do sloven�?iny

static const char help_text[]=
"Použitie:   mplayer [prepína�?e] [url|cesta/]menosúboru\n"
"\n"
"Základné prepína�?e: (Kompletný zoznam nájdete v man stránke)\n"
" -vo <drv[:dev]> výber výstup. video ovláda�?a&zariadenia (-vo help pre zoznam)\n"
" -ao <drv[:dev]> výber výstup. audio ovláda�?a&zariadenia (-ao help pre zoznam)\n"
#ifdef CONFIG_VCD
" vcd://<trackno>  prehrať VCD (video cd) stopu zo zariadenia namiesto zo súboru\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>  prehrať DVD titul/stopu zo zariadenia (mechaniky) namiesto súboru\n"
" -alang/-slang   vybrať jazyk DVD zvuku/titulkov(pomocou 2-miest. kódu krajiny)\n"
#endif
" -ss <timepos>   posun na pozíciu (sekundy alebo hh:mm:ss)\n"
" -nosound        prehrávať bez zvuku\n"
" -fs             voľby pre celú obrazovku (alebo -vm -zoom, detaily vi�?. man stránku)\n"
" -x <x> -y <y>   zvä�?šenie obrazu na rozmer <x>*<y> (pokiaľ to vie -vo ovláda�?!)\n"
" -sub <file>     voľba súboru s titulkami (vi�? tiež -subfps, -subdelay)\n"
" -playlist <file> ur�?enie súboru so zoznamom prehrávaných súborov\n"
" -vid x -aid y   výber �?ísla video (x) a audio (y) prúdu pre prehrávanie\n"
" -fps x -srate y voľba pre zmenu video (x fps) a audio (y Hz) frekvencie\n"
" -pp <quality>   aktivácia postprocesing filtra (0-4 pre DivX, 0-63 pre mpegy)\n"
" -framedrop      povoliť zahadzovanie snímkov (pre pomalé stroje)\n"
"\n"
"Zákl. klávesy:   (pre kompl. pozrite aj man stránku a input.conf)\n"
" <-  alebo  ->   posun vzad/vpred o 10 sekund\n"
" hore / dole     posun vzad/vpred o  1 minútu\n"
" pgup alebo pgdown  posun vzad/vpred o 10 minút\n"
" < alebo >       posun vzad/vpred v zozname prehrávaných súborov\n"
" p al. medzerník pauza (pokra�?ovanie stla�?ením klávesy)\n"
" q alebo ESC     koniec prehrávania a ukon�?enie programu\n"
" + alebo -       upraviť spozdenie zvuku v krokoch +/- 0.1 sekundy\n"
" o               cyklická zmena režimu OSD:  ni�? / pozícia / pozícia+�?as\n"
" * alebo /       pridať alebo ubrať hlasitosť (stla�?ením 'm' výber master/pcm)\n"
" z alebo x       upraviť spozdenie titulkov v krokoch +/- 0.1 sekundy\n"
" r alebo t       upraviť pozíciu titulkov hore/dole, pozrite tiež -vf!\n"
"\n"
" * * * * PREČ�?TAJTE SI MAN STR�?NKU PRE DETAILY (ĎALŠIE VOĽBY A KL�?VESY)! * * * *\n"
"\n";

#define MSGTR_SamplesWanted "Potrebujeme vzorky tohto formátu, aby sme zlepšili podporu. Prosím kontaktujte vývojárov.\n"

// ========================= MPlayer messages ===========================
// mplayer.c:

#define MSGTR_Exiting "\nKon�?ím...\n"
#define MSGTR_ExitingHow "\nKon�?ím... (%s)\n"
#define MSGTR_Exit_quit "Koniec"
#define MSGTR_Exit_eof "Koniec súboru"
#define MSGTR_Exit_error "Závažná chyba"
#define MSGTR_IntBySignal "\nMPlayer prerušený signálom %d v module: %s \n"
#define MSGTR_NoHomeDir "Nemôžem najsť domáci (HOME) adresár\n"
#define MSGTR_GetpathProblem "get_path(\"config\") problém\n"
#define MSGTR_CreatingCfgFile "Vytváram konfigura�?ný súbor: %s\n"
#define MSGTR_CantLoadFont "Nemôžem na�?ítať font: %s\n"
#define MSGTR_CantLoadSub "Nemôžem na�?ítať titulky: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: požadovaný prúd chýba!\n"
#define MSGTR_CantOpenDumpfile "Nejde otvoriť súbor pre dump!!!\n"
#define MSGTR_CoreDumped "jadro vypísané :)\n"
#define MSGTR_FPSnotspecified "V hlavi�?ke súboru nie je udané (alebo je zlé) FPS! Použite voľbu -fps!\n"
#define MSGTR_TryForceAudioFmtStr "Pokúšam sa vynútiť rodinu audiokodeku %s...\n"
#define MSGTR_CantFindAudioCodec "Nemôžem nájsť kodek pre audio formát 0x%X!\n"
#define MSGTR_TryForceVideoFmtStr "Pokúšam se vnútiť rodinu videokodeku %s...\n"
#define MSGTR_CantFindVideoCodec "Nemôžem najsť kodek pre video formát 0x%X!\n"
#define MSGTR_CannotInitVO "FATAL: Nemôžem inicializovať video driver!\n"
#define MSGTR_CannotInitAO "nemôžem otvoriť/inicializovať audio driver -> TICHO\n"
#define MSGTR_StartPlaying "Za�?ínam prehrávať...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"         ***********************************************************\n"\
"         ****  Na prehratie tohoto je váš systém príliš POMAL�?!  ****\n"\
"         ***********************************************************\n"\
"!!! Možné prí�?iny, problémy a riešenia:\n"\
"- Nej�?astejšie: nesprávny/chybný _zvukový_ ovláda�?.\n"\
"  - Skúste -ao sdl alebo použite OSS emuláciu ALSA.\n"\
"  - Experimentujte s rôznymi hodnotami -autosync, 30 je dobrý za�?iatok.\n"\
"- Pomalý video výstup\n"\
"  - Skúste iný -vo ovláda�? (pre zoznam: -vo help) alebo skúste -framedrop!\n"\
"- Pomalý CPU\n"\
"  - Neskúšajte prehrávať veľké dvd/divx na pomalom cpu! Skúste lavdopts,\n"\
"    napr. -vfm ffmpeg -lavdopts lowres=1:fast:skiploopfilter=all.\n"\
"- Poškodený súbor\n"\
"  - Skúste rôzne kombinácie týchto volieb -nobps -ni -forceidx -mc 0.\n"\
"- Pomalé médium (NFS/SMB, DVD, VCD...)\n"\
"  - Skúste -cache 8192.\n"\
"- Používate -cache na prehrávanie non-interleaved súboru?\n"\
"  - Skúste -nocache.\n"\
"Pre�?ítajte si DOCS/HTML/en/video.html sú tam tipy na vyladenie/zrýchlenie.\n"\
"Ak ni�? z tohto nepomohlo, pre�?ítajte si DOCS/HTML/en/bugreports.html.\n\n"

#define MSGTR_NoGui "MPlayer bol zostavený BEZ podpory GUI!\n"
#define MSGTR_GuiNeedsX "MPlayer GUI vyžaduje X11!\n"
#define MSGTR_Playing "Prehrávam %s\n"
#define MSGTR_NoSound "Audio: bez zvuku!!!\n"
#define MSGTR_FPSforced "FPS vnútené na hodnotu %5.3f  (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "Dostupné video výstupné ovláda�?e:\n"
#define MSGTR_AvailableAudioOutputDrivers "Dostupné audio výstupné ovláda�?e:\n"
#define MSGTR_AvailableAudioCodecs "Dostupné audio kodeky:\n"
#define MSGTR_AvailableVideoCodecs "Dostupné video kodeky:\n"
#define MSGTR_AvailableAudioFm "Dostupné (vkompilované) audio rodiny kodekov/ovláda�?e:\n"
#define MSGTR_AvailableVideoFm "Dostupné (vkompilované) video rodiny kodekov/ovláda�?e:\n"
#define MSGTR_AvailableFsType "Dostupné zmeny plnoobrazovkových módov:\n"
#define MSGTR_CannotReadVideoProperties "Video: nemôžem �?ítať vlastnosti\n"
#define MSGTR_NoStreamFound "Nenájdený prúd\n"
#define MSGTR_ErrorInitializingVODevice "Chyba pri otváraní/inicializácii vybraných video_out (-vo) zariadení!\n"
#define MSGTR_ForcedVideoCodec "Vnútený video kodek: %s\n"
#define MSGTR_ForcedAudioCodec "Vnútený video kodek: %s\n"
#define MSGTR_Video_NoVideo "Video: žiadne video!!!\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: Nemôžem inicializovať video filtre (-vf) alebo video výstup (-vo)!\n"
#define MSGTR_Paused "  =====  PAUZA  ====="
#define MSGTR_PlaylistLoadUnable "\nNemôžem na�?ítať playlist %s\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer zhavaroval na 'Illegal Instruction'.\n"\
"  Môže to byť chyba v našom novom kóde na detekciu procesora...\n"\
"  Prosím pre�?ítajte si DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer zhavaroval na 'Illegal Instruction'.\n"\
"  Oby�?ajne sa to stáva, ke�? ho používate na inom procesore ako pre ktorý bol\n"\
"  skompilovaný/optimalizovaný.\n"\
"  Skontrolujte si to!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer zhavaroval nesprávnym použitím CPU/FPU/RAM.\n"\
"  Prekompilujte MPlayer s --enable-debug a urobte 'gdb' backtrace a\n"\
"  disassemblujte. Pre detaily, pozrite DOCS/HTML/en/bugreports_what.html#bugreports_crash.b.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer zhavaroval. To sa nemalo stať.\n"\
"  Môže to byť chyba v MPlayer kóde _alebo_ vo Vaších ovláda�?och _alebo_ gcc\n"\
"  verzii. Ak si myslíte, že je to chyba MPlayeru, prosím pre�?ítajte si DOCS/HTML/en/bugreports.html\n"\
"  a postupujte podľa inštrukcii. Nemôžeme Vám pomôcť, pokiaľ neposkytnete\n"\
"  tieto informácie pri ohlasovaní možnej chyby.\n"
#define MSGTR_LoadingConfig "Čítam konfiguráciu '%s'\n"
#define MSGTR_AddedSubtitleFile "SUB: pridaný súbor titulkov (%d): %s\n"
#define MSGTR_RemovedSubtitleFile "SUB: odobratý súbor titulkov (%d): %s\n"
#define MSGTR_ErrorOpeningOutputFile "Chyba pri otváraní súboru [%s] pre zápis!\n"
#define MSGTR_RTCDeviceNotOpenable "Nepodarilo sa otvoriť %s: %s (malo by to byť �?itateľné pre používateľa.)\n"
#define MSGTR_LinuxRTCInitErrorIrqpSet "Chyba pri inicializácii Linuxových RTC v ioctl (rtc_irqp_set %lu): %s\n"
#define MSGTR_IncreaseRTCMaxUserFreq "Skúste pridať \"echo %lu > /proc/sys/dev/rtc/max-user-freq\" do štartovacích skriptov vášho systému.\n"
#define MSGTR_LinuxRTCInitErrorPieOn "Chyba pri inicializácii Linuxových RTC v ioctl (rtc_pie_on): %s\n"
#define MSGTR_UsingTimingType "Používam %s �?asovanie.\n"
#define MSGTR_Getch2InitializedTwice "VAROVANIE: getch2_init je volaná dvakrát!\n"
#define MSGTR_DumpstreamFdUnavailable "Nemôžem uložiť (dump) tento prúd - nie je dostupný žiaden deskriptor súboru.\n"
#define MSGTR_CantOpenLibmenuFilterWithThisRootMenu "Nemôžem otvoriť video filter libmenu s koreňovým menu %s.\n"
#define MSGTR_AudioFilterChainPreinitError "Chyba pri predinicializácii reťazca audio filtrov!\n"
#define MSGTR_LinuxRTCReadError "Chyba pri �?ítaní z Linuxových RTC: %s\n"
#define MSGTR_SoftsleepUnderflow "Pozor! Podte�?enie softsleep!\n"
#define MSGTR_DvdnavNullEvent "DVDNAV udalosť NULL?!\n"
#define MSGTR_DvdnavHighlightEventBroken "DVDNAV udalosť: Vadné zvýraznenie udalostí\n"
#define MSGTR_DvdnavEvent "DVDNAV udalosť: %s\n"
#define MSGTR_DvdnavHighlightHide "DVDNAV udalosť: skryť zvýraznenie\n"
#define MSGTR_DvdnavStillFrame "######################################## DVDNAV udalosť: Stojací snímok: %d sec(s)\n"
#define MSGTR_DvdnavNavStop "DVDNAV udalosť: Nav Stop\n"
#define MSGTR_DvdnavNavNOP "DVDNAV udalosť: Nav NOP\n"
#define MSGTR_DvdnavNavSpuStreamChangeVerbose "DVDNAV udalosť: Zmena Nav SPU prúdu: fyz: %d/%d/%d logicky: %d\n"
#define MSGTR_DvdnavNavSpuStreamChange "DVDNAV udalosť: Zmena Nav SPU prúdu: fyz: %d logicky: %d\n"
#define MSGTR_DvdnavNavAudioStreamChange "DVDNAV udalosť: Zmena Nav Audio prúdu: fyz: %d logicky: %d\n"
#define MSGTR_DvdnavNavVTSChange "DVDNAV udalosť: Zmena Nav VTS\n"
#define MSGTR_DvdnavNavCellChange "DVDNAV udalosť: Zmena Nav bunky \n"
#define MSGTR_DvdnavNavSpuClutChange "DVDNAV udalosť: Zmena Nav SPU CLUT\n"
#define MSGTR_DvdnavNavSeekDone "DVDNAV udalosť: Prevíjanie Nav dokon�?ené\n"
#define MSGTR_MenuCall "Volanie menu\n"

#define MSGTR_EdlOutOfMem "Nedá sa alokovať dostatok pamäte pre EDL dáta.\n"
#define MSGTR_EdlRecordsNo "Čítam %d EDL akcie.\n"
#define MSGTR_EdlQueueEmpty "Všetky EDL akcie boly už vykonané.\n"
#define MSGTR_EdlCantOpenForWrite "Nedá sa otvoriť EDL súbor [%s] pre zápis.\n"
#define MSGTR_EdlCantOpenForRead "Nedá sa otvoriť EDL súbor [%s] na �?ítanie.\n"
#define MSGTR_EdlNOsh_video "EDL sa nedá použiť bez videa, vypínam.\n"
#define MSGTR_EdlNOValidLine "Chyba EDL na riadku: %s\n"
#define MSGTR_EdlBadlyFormattedLine "Zle formátovaný EDL riadok [%d] Zahadzujem.\n"
#define MSGTR_EdlBadLineOverlap "Posledná stop zna�?ka bola [%f]; �?alší štart je "\
"[%f]. Záznamy musia byť chronologicky, a nesmú sa prekrývať. Zahadzujem.\n"
#define MSGTR_EdlBadLineBadStop "Časová zna�?ka stop má byť za zna�?kou start.\n"

// mplayer.c OSD

#define MSGTR_OSDenabled "zapnuté"
#define MSGTR_OSDdisabled "vypnuté"
#define MSGTR_OSDChannel "Kanál: %s"
#define MSGTR_OSDSubDelay "Zpozdenie tit: %d ms"
#define MSGTR_OSDSpeed "Rýchlosť: x %6.2f"
#define MSGTR_OSDosd "OSD: %s"

// property values
#define MSGTR_Enabled "zapnuté"
#define MSGTR_EnabledEdl "zapnuté (edl)"
#define MSGTR_Disabled "vypnuté"
#define MSGTR_HardFrameDrop "hard"
#define MSGTR_Unknown "neznáme"
#define MSGTR_Bottom "dole"
#define MSGTR_Center "stred"
#define MSGTR_Top "hore"

// osd bar names
#define MSGTR_Volume "Hlasitosť"
#define MSGTR_Panscan "Panscan"
#define MSGTR_Gamma "Gama"
#define MSGTR_Brightness "Jas"
#define MSGTR_Contrast "Kontrast"
#define MSGTR_Saturation "Sýtosť"
#define MSGTR_Hue "Tón"

// property state
#define MSGTR_MuteStatus "Utlmenie zvuku: %s"
#define MSGTR_AVDelayStatus "A-V odchylka: %s"
#define MSGTR_OnTopStatus "Vždy navrchu: %s"
#define MSGTR_RootwinStatus "Hlavné okno: %s"
#define MSGTR_BorderStatus "Ohrani�?enie: %s"
#define MSGTR_FramedroppingStatus "Zahadzovanie snímkov: %s"
#define MSGTR_VSyncStatus "VSync: %s"
#define MSGTR_SubSelectStatus "Titulky: %s"
#define MSGTR_SubPosStatus "Pozícia tit.: %s/100"
#define MSGTR_SubAlignStatus "Zarovnanie tit.: %s"
#define MSGTR_SubDelayStatus "Spozdenie tit.: %s"
#define MSGTR_SubVisibleStatus "Zobr. titulkov: %s"
#define MSGTR_SubForcedOnlyStatus "Iba vynútené tit.: %s"

// mencoder.c:

#define MSGTR_UsingPass3ControlFile "Používam pass3 ovládací súbor: %s\n"
#define MSGTR_MissingFilename "\nChýbajúce meno súboru.\n\n"
#define MSGTR_CannotOpenFile_Device "Nemôžem otvoriť súbor/zariadenie\n"
#define MSGTR_CannotOpenDemuxer "Nemôžem otvoriť demuxer\n"
#define MSGTR_NoAudioEncoderSelected "\nNevybraný encoder (-oac)! Vyberte jeden alebo -nosound. Použitie -oac help!\n"
#define MSGTR_NoVideoEncoderSelected "\nNevybraný encoder (-ovc)! Vyberte jeden, použitie -ovc help!\n"
#define MSGTR_CannotOpenOutputFile "Nemôžem otvoriť súbor '%s'\n"
#define MSGTR_EncoderOpenFailed "Zlyhalo spustenie enkóderu\n"
#define MSGTR_MencoderWrongFormatAVI "\nVAROVANIE: FORM�?T V�?STUPNÉHO SÚBORU JE _AVI_. viz -of help.\n"
#define MSGTR_MencoderWrongFormatMPG "\nVAROVANIE: FORM�?T V�?STUPNÉHO SÚBORU JE _MPEG_. viz -of help.\n"
#define MSGTR_MissingOutputFilename "Nebol nastavený výstupný súbor, preštudujte si volbu -o"
#define MSGTR_ForcingOutputFourcc "Vnucujem výstupný formát (fourcc) na %x [%.4s]\n"
#define MSGTR_ForcingOutputAudiofmtTag "Vynucujem zna�?ku výstupného zvukového formátu 0x%x\n"
#define MSGTR_DuplicateFrames "\nduplikujem %d snímkov!!!    \n"
#define MSGTR_SkipFrame "\npresko�?iť snímok!!!    \n"
#define MSGTR_ResolutionDoesntMatch "\nNový video súbor má iné rozlišení alebo farebný priestor ako jeho predchodca.\n"
#define MSGTR_FrameCopyFileMismatch "\nVšetky video soubory musí mít shodné fps, rozlišení a kodek pro -ovc copy.\n"
#define MSGTR_AudioCopyFileMismatch "\nVšetky súbory musí používať identický audio kódek a formát pro -oac copy.\n"
#define MSGTR_NoAudioFileMismatch "\nNemôžete mixovať iba video s audio a video súbormi. Skúste -nosound.\n"
#define MSGTR_NoSpeedWithFrameCopy "VAROVANIE: -speed nemá zaru�?enú funk�?nosť s -oac copy!\n"\
"Výsledny súbor môže byť vadný!\n"
#define MSGTR_ErrorWritingFile "%s: chyba pri zápise súboru.\n"
#define MSGTR_RecommendedVideoBitrate "Odporú�?aný dátový tok videa pre CD %s: %d\n"
#define MSGTR_VideoStreamResult "\nVideo prúd: %8.3f kbit/s  (%d B/s)  velkosť: %"PRIu64" bytov  %5.3f sekund  %d snímkov\n"
#define MSGTR_AudioStreamResult "\nAudio prúd: %8.3f kbit/s  (%d B/s)  velkosť: %"PRIu64" bytov  %5.3f sekund\n"
#define MSGTR_OpenedStream "úspech: formát: %d  dáta: 0x%X - 0x%x\n"
#define MSGTR_VCodecFramecopy "videokódek: framecopy (%dx%d %dbpp fourcc=%x)\n"
#define MSGTR_ACodecFramecopy "audiokódek: framecopy (formát=%x kanálov=%d frekvencia=%d bitov=%d B/s=%d vzorka-%d)\n"
#define MSGTR_CBRPCMAudioSelected "zvolený CBR PCM zvuk\n"
#define MSGTR_MP3AudioSelected "zvolený MP3 zvuk\n"
#define MSGTR_CannotAllocateBytes "Nedá sa alokovať %d bajtov\n"
#define MSGTR_SettingAudioDelay "Nastavujem spozdenie zvuku na %5.3f\n"
#define MSGTR_SettingVideoDelay "Nastavujem spozděnie videa na %5.3fs\n"
#define MSGTR_LimitingAudioPreload "Obmedzujem predna�?ítanie zvuku na 0.4s\n"
#define MSGTR_IncreasingAudioDensity "Zvyšujem hustotu audia na 4\n"
#define MSGTR_ZeroingAudioPreloadAndMaxPtsCorrection "Vnucujem predna�?ítanie zvuku na 0, max korekciu pts na 0\n"
#define MSGTR_LameVersion "LAME verzia %s (%s)\n\n"
#define MSGTR_InvalidBitrateForLamePreset "Chyba: Špecifikovaný dátový tok je mimo rozsah pre tento preset.\n"\
"\n"\
"Pokiaľ používate tento režim, musíte zadat hodnotu od \"8\" do \"320\".\n"\
"\n"\
"Dalšie informácie viz: \"-lameopts preset=help\"\n"
#define MSGTR_InvalidLamePresetOptions "Chyba: Nezadali ste platný profil a/alebo voľby s presetom.\n"\
"\n"\
"Dostupné profily sú:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (ABR Mode) - Implikuje režim ABR. Pre jeho použitie,\n"\
"                      jednoduche zadejte dátový tok. Napríklad:\n"\
"                      \"preset=185\" aktivuje tento preset\n"\
"                      a použije priemerný dátový tok 185 kbps.\n"\
"\n"\
"    Niekolko príkladov:\n"\
"\n"\
"    \"-lameopts fast:preset=standard  \"\n"\
" or \"-lameopts  cbr:preset=192       \"\n"\
" or \"-lameopts      preset=172       \"\n"\
" or \"-lameopts      preset=extreme   \"\n"\
"\n"\
"Dalšie informácie viz: \"-lameopts preset=help\"\n"
#define MSGTR_LamePresetsLongInfo "\n"\
"Preset prepína�?e sú navrhnuté tak, aby poskytovaly �?o najvyššiu možnú kvalitu.\n"\
"\n"\
"Vä�?šina z nich bola testovaná a vyladená pomocou dôkladných zdvojených slepých\n"\
"posluchových testov, za ú�?elom dosiahnutia a overenia tohto ciela.\n"\
"\n"\
"Nastavenia sú neustále aktualizované v súlade s najnovším vývojom\n"\
"a mali by poskytovať prakticky najvyššiu možnú kvalitu, aká je v sú�?asnosti \n"\
"s kódekom LAME dosažiteľná.\n"\
"\n"\
"Aktivácia presetov:\n"\
"\n"\
"   Pre režimy VBR (vo všeobecnosti najvyššia kvalita):\n"\
"\n"\
"     \"preset=standard\" Tento preset by mal býť jasnou voľbou\n"\
"                             pre vä�?šinu ludí a hudobných žánrov a má\n"\
"                             už vysokú kvalitu.\n"\
"\n"\
"     \"preset=extreme\" Pokiaľ máte výnimo�?ne dobrý sluch a zodpovedajúce\n"\
"                             vybavenie, tento preset vo všeob. poskytuje\n"\
"                             mierně vyšší kvalitu ako režim \"standard\".\n"\
"\n"\
"   Pre CBR 320kbps (najvyššia možná kvalita ze všetkých presetov):\n"\
"\n"\
"     \"preset=insane\"  Tento preset je pre vä�?šinu ludí a situácii\n"\
"                             predimenzovaný, ale pokiaľ vyžadujete\n"\
"                             absolutne najvyššiu kvalitu bez ohľadu na\n"\
"                             velkosť súboru, je toto vaša voľba.\n"\
"\n"\
"   Pre režimy ABR (vysoká kvalita pri danom dátovém toku, ale nie tak ako VBR):\n"\
"\n"\
"     \"preset=<kbps>\"  Použitím tohoto presetu obvykle dosiahnete dobrú\n"\
"                             kvalitu pri danom dátovém toku. V závislosti\n"\
"                             na zadanom toku tento preset odvodí optimálne\n"\
"                             nastavenie pre danú situáciu.\n"\
"                             Hoci tento prístup funguje, nie je ani z�?aleka\n"\
"                             tak flexibilný ako VBR, a obvykle nedosahuje\n"\
"                             úrovne kvality ako VBR na vyšších dátových tokoch.\n"\
"\n"\
"Pre zodpovedajúce profily sú k dispozícii tiež nasledujúce voľby:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (ABR režim) - Implikuje režim ABR. Pre jeho použitie\n"\
"                      jednoducho zadajte dátový tok. Napríklad:\n"\
"                      \"preset=185\" aktivuje tento preset\n"\
"                      a použije priemerný dátový tok 185 kbps.\n"\
"\n"\
"   \"fast\" - V danom profile aktivuje novú rýchlu VBR kompresiu.\n"\
"            Nevýhodou je obvykle mierne vyšší dátový tok ako v normálnom\n"\
"            režime a tiež môže dôjsť k miernemu poklesu kvality.\n"\
"   Varovanie:v aktuálnej verzi môže nastavenie \"fast\" viesť k príliš\n"\
"            vysokému dátovému toku v porovnaní s normálnym nastavením.\n"\
"\n"\
"   \"cbr\"  - Pokiaľ použijete režim ABR (viz vyššie) s významným\n"\
"            dátovým tokom, napr. 80, 96, 112, 128, 160, 192, 224, 256, 320,\n"\
"            môžete použíť voľbu \"cbr\" k vnúteniu kódovánia v režime CBR\n"\
"            (konštantný tok) namiesto štandardního ABR režimu. ABR poskytuje\n"\
"            lepšiu kvalitu, ale CBR môže byť užito�?ný v situáciach ako je\n"\
"            vysielanie mp3 prúdu po internete.\n"\
"\n"\
"    Napríklad:\n"\
"\n"\
"      \"-lameopts fast:preset=standard  \"\n"\
" alebo \"-lameopts  cbr:preset=192       \"\n"\
" alebo \"-lameopts      preset=172       \"\n"\
" alebo \"-lameopts      preset=extreme   \"\n"\
"\n"\
"\n"\
"Pre ABR režim je k dispozícii niekolko skratiek:\n"\
"phone => 16kbps/mono        phon+/lw/mw-eu/sw => 24kbps/mono\n"\
"mw-us => 40kbps/mono        voice => 56kbps/mono\n"\
"fm/radio/tape => 112kbps    hifi => 160kbps\n"\
"cd => 192kbps               studio => 256kbps"
#define MSGTR_LameCantInit "Nedá sa nastaviť voľba pre LAME, overte dátový_tok/vzorkovaciu_frekv.,"\
"niektoré veľmi nízke dátové toky (<32) vyžadujú nižšiu vzorkovaciu frekv. (napr. -srate 8000)."\
"Pokud všetko ostané zlyhá, zkúste prednastavenia (presets)."
#define MSGTR_ConfigFileError "chyba konfigura�?ného súboru"
#define MSGTR_ErrorParsingCommandLine "chyba pri spracovávaní príkazového riadku"
#define MSGTR_VideoStreamRequired "Video prúd je povinný!\n"
#define MSGTR_ForcingInputFPS "vstupné fps bude interpretované ako %5.3f\n"
#define MSGTR_RawvideoDoesNotSupportAudio "Výstupný formát súboru RAWVIDEO nepodporuje zvuk - vypínam ho\n"
#define MSGTR_DemuxerDoesntSupportNosound "Tento demuxer zatiaľ nepodporuje -nosound.\n"
#define MSGTR_MemAllocFailed "Alokácia pamäte zlyhala\n"
#define MSGTR_NoMatchingFilter "Nemožem nájsť zodpovedajúci filter/ao formát!\n"
#define MSGTR_MP3WaveFormatSizeNot30 "sizeof(MPEGLAYER3WAVEFORMAT)==%d!=30, možno je vadný preklada�? C?\n"
#define MSGTR_NoLavcAudioCodecName "Audio LAVC, chýba meno kódeku!\n"
#define MSGTR_LavcAudioCodecNotFound "Audio LAVC, nemôžem nájsť enkóder pre kódek %s\n"
#define MSGTR_CouldntAllocateLavcContext "Audio LAVC, nemôžem alokovať kontext!\n"
#define MSGTR_CouldntOpenCodec "Nedá sa otvoriť kódek %s, br=%d\n"
#define MSGTR_CantCopyAudioFormat "Audio formát 0x%x je nekompatibilný s '-oac copy', skúste prosím '-oac pcm',\n alebo použite '-fafmttag' pre jeho prepísanie.\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     metóda variabilnej bit. rýchlosti \n"\
"                0: cbr (konštantná bit.rýchlosť)\n"\
"                1: mt (Mark Taylor VBR alg.)\n"\
"                2: rh(Robert Hegemann VBR alg. - default)\n"\
"                3: abr (priemerná bit.rýchlosť)\n"\
"                4: mtrh (Mark Taylor Robert Hegemann VBR alg.)\n"\
"\n"\
" abr           priemerná bit. rýchlosť\n"\
"\n"\
" cbr           konštantná bit. rýchlosť\n"\
"               Vnúti tiež CBR mód na podsekvenciách ABR módov\n"\
"\n"\
" br=<0-1024>   špecifikovať bit. rýchlosť v kBit (platí iba pre CBR a ABR)\n"\
"\n"\
" q=<0-9>       kvalita (0-najvyššia, 9-najnižšia) (iba pre VBR)\n"\
"\n"\
" aq=<0-9>      algoritmická kvalita (0-najlep./najpomalšia, 9-najhoršia/najrýchl.)\n"\
"\n"\
" ratio=<1-100> kompresný pomer\n"\
"\n"\
" vol=<0-10>    nastavenie audio zosilnenia\n"\
"\n"\
" mode=<0-3>    (default: auto)\n"\
"                0: stereo\n"\
"                1: joint-stereo\n"\
"                2: dualchannel\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: no\n"\
"                1: all\n"\
"                2: adjust\n"\
"\n"\
" fast          prepnúť na rýchlejšie kódovanie na podsekvenciách VBR módov,\n"\
"               mierne nižšia kvalita and vyššia bit. rýchlosť.\n"\
"\n"\
" preset=<value> umožňuje najvyššie možné nastavenie kvality.\n"\
"                 medium: VBR  kódovanie,  dobrá kvalita\n"\
"                 (150-180 kbps rozpätie bit. rýchlosti)\n"\
"                 standard:  VBR kódovanie, vysoká kvalita\n"\
"                 (170-210 kbps rozpätie bit. rýchlosti)\n"\
"                 extreme: VBR kódovanie, velmi vysoká kvalita\n"\
"                 (200-240 kbps rozpätie bit. rýchlosti)\n"\
"                 insane:  CBR  kódovanie, najvyššie nastavenie kvality\n"\
"                 (320 kbps bit. rýchlosť)\n"\
"                 <8-320>: ABR kódovanie na zadanej kbps bit. rýchlosti.\n\n"

//codec-cfg.c:
#define MSGTR_DuplicateFourcc "zdvojené FourCC"
#define MSGTR_TooManyFourccs "príliš vela FourCCs/formátov..."
#define MSGTR_ParseError "chyba spracovania (parse)"
#define MSGTR_ParseErrorFIDNotNumber "chyba spracovania (parse) (ID formátu nie je �?íslo?)"
#define MSGTR_ParseErrorFIDAliasNotNumber "chyba spracovania (parse) (alias ID formátu nie je �?íslo?)"
#define MSGTR_DuplicateFID "duplikátne format ID"
#define MSGTR_TooManyOut "príliš mnoho výstupu..."
#define MSGTR_InvalidCodecName "\nmeno kódeku(%s) nie je platné!\n"
#define MSGTR_CodecLacksFourcc "\nmeno kódeku(%s) nemá FourCC/formát!\n"
#define MSGTR_CodecLacksDriver "\nmeno kódeku(%s) nemá ovláda�?!\n"
#define MSGTR_CodecNeedsDLL "\nkódek(%s) vyžaduje 'dll'!\n"
#define MSGTR_CodecNeedsOutfmt "\nkódek(%s) vyžaduje 'outfmt'!\n"
#define MSGTR_CantAllocateComment "Nedá sa alokovať pamäť pre poznámku. "
#define MSGTR_GetTokenMaxNotLessThanMAX_NR_TOKEN "get_token(): max >= MAX_MR_TOKEN!"
#define MSGTR_CantGetMemoryForLine "Nejde získať pamäť pre 'line': %s\n"
#define MSGTR_CantReallocCodecsp "Nedá sa realokovať '*codecsp': %s\n"
#define MSGTR_CodecNameNotUnique " Meno kódeku '%s' nie je jedine�?né."
#define MSGTR_CantStrdupName "Nedá sa spraviť strdup -> 'name': %s\n"
#define MSGTR_CantStrdupInfo "Nedá sa spraviť strdup -> 'info': %s\n"
#define MSGTR_CantStrdupDriver "Nedá sa spraviť strdup -> 'driver': %s\n"
#define MSGTR_CantStrdupDLL "Nedá sa spraviť strdup -> 'dll': %s"
#define MSGTR_AudioVideoCodecTotals "%d audio & %d video codecs\n"
#define MSGTR_CodecDefinitionIncorrect "Kódek nie je definovaný korektne."
#define MSGTR_OutdatedCodecsConf "Súbor codecs.conf je príliš starý a nekompatibilný s touto verziou MPlayer-u!"

// fifo.c
#define MSGTR_CannotMakePipe "Nedá sa vytvoriť PIPE!\n"

// m_config.c
#define MSGTR_SaveSlotTooOld "Príliš starý save slot nájdený z lvl %d: %d !!!\n"
#define MSGTR_InvalidCfgfileOption "Voľba %s sa nedá použiť v konfigura�?nom súbore.\n"
#define MSGTR_InvalidCmdlineOption "Voľba %s sa nedá použiť z príkazového riadku.\n"
#define MSGTR_InvalidSuboption "Chyba: voľba '%s' nemá žiadnu podvoľbu '%s'.\n"
#define MSGTR_MissingSuboptionParameter "Chyba: podvoľba '%s' voľby '%s' musí mať parameter!\n"
#define MSGTR_MissingOptionParameter "Chyba: voľba '%s' musí mať parameter!\n"
#define MSGTR_OptionListHeader "\n Názov                Typ             Min        Max      Globál  CL    Konfig\n\n"
#define MSGTR_TotalOptions "\nCelkovo: %d volieb\n"
#define MSGTR_ProfileInclusionTooDeep "VAROVANIE: Príliš hlboké vnorovanie profilov.\n"
#define MSGTR_NoProfileDefined "Žiadny profil nebol definovaný.\n"
#define MSGTR_AvailableProfiles "Dostupné profily:\n"
#define MSGTR_UnknownProfile "Neznámy profil '%s'.\n"
#define MSGTR_Profile "Profil %s: %s\n"

// m_property.c
#define MSGTR_PropertyListHeader "\n Meno                 Typ             Min        Max\n\n"
#define MSGTR_TotalProperties "\nCelkovo: %d vlastností\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM zariadenie '%s' nenájdené!\n"
#define MSGTR_ErrTrackSelect "Chyba pri výbere VCD stopy!"
#define MSGTR_ReadSTDIN "Čítam z stdin...\n"
#define MSGTR_UnableOpenURL "Nejde otvoriť URL: %s\n"
#define MSGTR_ConnToServer "Pripojený k servru: %s\n"
#define MSGTR_FileNotFound "Súbor nenájdený: '%s'\n"

#define MSGTR_SMBInitError "Nemôžem inicializovať knižnicu libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "Nemôžem otvoriť z LAN: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer mebol skompilovaný s podporou �?ítania z SMB\n"

#define MSGTR_CantOpenDVD "Nejde otvoriť DVD zariadenie: %s (%s)\n"
#define MSGTR_NoDVDSupport "MPlayer bol skompilovaný bez podpory DVD, koniec\n"
#define MSGTR_DVDnumTitles "Na tomto DVD je %d titulov.\n"
#define MSGTR_DVDinvalidTitle "Neplatné �?íslo DVD titulu: %d\n"
#define MSGTR_DVDnumChapters "Na tomto DVD je %d kapitol.\n"
#define MSGTR_DVDinvalidChapter "Neplatné �?íslo kapitoly DVD: %d\n"
#define MSGTR_DVDinvalidChapterRange "Nesprávně nastavený rozsah kapitol %s\n"
#define MSGTR_DVDinvalidLastChapter "Neplatné �?íslo poslednej DVD kapitoly: %d\n"
#define MSGTR_DVDnumAngles "Na tomto DVD je %d uhlov pohľadov.\n"
#define MSGTR_DVDinvalidAngle "Neplatné �?íslo uhlu pohľadu DVD: %d\n"
#define MSGTR_DVDnoIFO "Nemôžem otvoriť súbor IFO pre DVD titul %d.\n"
#define MSGTR_DVDnoVMG "Nedá sa otvoriť VMG info!\n"
#define MSGTR_DVDnoVOBs "Nemôžem otvoriť VOB súbor (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDnoMatchingAudio "DVD zvuk v požadovanom jazyku nebyl nájdený!\n"
#define MSGTR_DVDaudioChannel "Zvolený DVD zvukový kanál: %d jazyk: %c%c\n"
#define MSGTR_DVDnoMatchingSubtitle "DVD titulky v požadovanom jazyku neboli nájdené!\n"
#define MSGTR_DVDsubtitleChannel "Zvolený DVD titulkový kanál: %d jazyk: %c%c\n"

// muxer.c, muxer_*.c:
#define MSGTR_TooManyStreams "Príliš veľa prúdov!"
#define MSGTR_RawMuxerOnlyOneStream "Rawaudio muxer podporuje iba jeden audio prúd!\n"
#define MSGTR_IgnoringVideoStream "Ignorujem video prúd!\n"
#define MSGTR_UnknownStreamType "Varovanie! neznámy typ prúdu: %d\n"
#define MSGTR_WarningLenIsntDivisible "Varovanie! dĺžka nie je deliteľná velkosťou vzorky!\n"
#define MSGTR_MuxbufMallocErr "Nedá sa alokovať pamäť pre frame buffer muxeru!\n"
#define MSGTR_MuxbufReallocErr "Nedá sa realokovať pamäť pre frame buffer muxeru!\n"
#define MSGTR_WritingHeader "Zapisujem header...\n"
#define MSGTR_WritingTrailer "Zapisujem index...\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "Upozornenie! Hlavi�?ka audio prúdu %d predefinovaná!\n"
#define MSGTR_VideoStreamRedefined "Upozornenie! Hlavi�?ka video prúdu %d predefinovaná!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: Príliš mnoho (%d v %d bajtoch) audio paketov v bufferi!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: Príliš mnoho (%d v %d bajtoch) video paketov v bufferi!\n"
#define MSGTR_MaybeNI "(možno prehrávate neprekladaný prúd/súbor alebo kodek zlyhal)\n" \
                      "Pre .AVI súbory skúste vynútiť neprekladaný mód voľbou -ni\n"
#define MSGTR_SwitchToNi "\nDetekovaný zle prekladaný .AVI - prepnite -ni mód!\n"
#define MSGTR_Detected_XXX_FileFormat "Detekovaný %s formát súboru!\n"
#define MSGTR_DetectedAudiofile "Detekovaný audio súbor!\n"
#define MSGTR_InvalidMPEGES "Neplatný MPEG-ES prúd??? kontaktujte autora, možno je to chyba (bug) :(\n"
#define MSGTR_FormatNotRecognized "========== Žiaľ, tento formát súboru nie je rozpoznaný/podporovaný =======\n"\
                                  "==== Pokiaľ je tento súbor AVI, ASF alebo MPEG prúd, kontaktujte autora! ====\n"
#define MSGTR_MissingVideoStream "Žiadny video prúd nenájdený!\n"
#define MSGTR_MissingAudioStream "Žiadny audio prúd nenájdený...  -> bez zvuku\n"
#define MSGTR_MissingVideoStreamBug "Chýbajúci video prúd!? Kontaktujte autora, možno to je chyba (bug) :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: súbor neobsahuje vybraný audio alebo video prúd\n"

#define MSGTR_NI_Forced "Vnútený"
#define MSGTR_NI_Detected "Detekovaný"
#define MSGTR_NI_Message "%s NEPREKLADAN�? formát súboru AVI!\n"

#define MSGTR_UsingNINI "Používam NEPREKLADAN�? poškodený formát súboru AVI!\n"
#define MSGTR_CouldntDetFNo "Nemôžem ur�?iť po�?et snímkov (pre absolútny posun)  \n"
#define MSGTR_CantSeekRawAVI "Nemôžem sa posúvať v surových (raw) .AVI prúdoch! (Potrebujem index, zkuste použíť voľbu -idx!)  \n"
#define MSGTR_CantSeekFile "Nemôžem sa posúvať v tomto súbore!  \n"

#define MSGTR_MOVcomprhdr "MOV: Komprimované hlavi�?ky nie sú (ešte) podporované!\n"
#define MSGTR_MOVvariableFourCC "MOV: Upozornenie! premenná FOURCC detekovaná!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Upozornenie! Príliš veľa stôp!"
#define MSGTR_DetectedTV "TV detekovaný! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Nemôžem otvoriť ogg demuxer\n"
#define MSGTR_CannotOpenAudioStream "Nemôžem otvoriť audio prúd: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Nemôžem otvoriť prúd titulkov: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Nemôžem otvoriť audio demuxer: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Nemôžem otvoriť demuxer titulkov: %s\n"
#define MSGTR_TVInputNotSeekable "v TV vstupe nie je možné sa pohybovať! (možno posun bude na zmenu kanálov ;)\n"
#define MSGTR_ClipInfo "Informácie o klipe: \n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: detekovaný 30000/1001 fps NTSC, prepínam frekvenciu snímkov.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: detekovaný 24000/1001 fps progresívny NTSC, prepínam frekvenciu snímkov.\n"

#define MSGTR_CacheFill "\rNaplnenie cache: %5.2f%% (%"PRId64" bajtov)   "
#define MSGTR_NoBindFound "Tla�?idlo '%s' nemá priradenú žiadnu funkciu.\n"
#define MSGTR_FailedToOpen "Zlyhalo otvorenie %s\n"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "nemôžem otvoriť kodek\n"
#define MSGTR_CantCloseCodec "nemôžem uzavieť kodek\n"

#define MSGTR_MissingDLLcodec "CHYBA: Nemôžem otvoriť potrebný DirectShow kodek: %s\n"
#define MSGTR_ACMiniterror "Nemôžem na�?ítať/inicializovať Win32/ACM AUDIO kodek (chýbajúci súbor DLL?)\n"
#define MSGTR_MissingLAVCcodec "Nemôžem najsť kodek '%s' v libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF - koniec súboru v priebehu vyhľadávania hlavi�?ky sekvencie\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: Nemôžem pre�?ítať hlavi�?ku sekvencie!\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: Nemôžem pre�?ítať rozšírenie hlavi�?ky sekvencie!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Zlá hlavi�?ka sekvencie!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Zlé rozšírenie hlavi�?ky sekvencie!\n"

#define MSGTR_ShMemAllocFail "Nemôžem alokovať zdieľanú pamäť\n"
#define MSGTR_CantAllocAudioBuf "Nemôžem alokovať pamäť pre výstupný audio buffer\n"

#define MSGTR_UnknownAudio "Neznámy/chýbajúci audio formát -> bez zvuku\n"

#define MSGTR_UsingExternalPP "[PP] Používam externý postprocessing filter, max q = %d\n"
#define MSGTR_UsingCodecPP "[PP] Požívam postprocessing z kodeku, max q = %d\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Požadovaná rodina video kodekov [%s] (vfm=%s) nie je dostupná (zapnite ju pri kompilácii!)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Požadovaná rodina audio kodekov [%s] (afm=%s) nie je dostupná (zapnite ju pri kompilácii!)\n"
#define MSGTR_OpeningVideoDecoder "Otváram video dekóder: [%s] %s\n"
#define MSGTR_SelectedVideoCodec "Zvolený video kódek: [%s] vfm: %s (%s)\n"
#define MSGTR_OpeningAudioDecoder "Otváram audio dekóder: [%s] %s\n"
#define MSGTR_SelectedAudioCodec "Zvolený audio kódek: [%s] afm: %s (%s)\n"
#define MSGTR_VDecoderInitFailed "VDecoder init zlyhal :(\n"
#define MSGTR_ADecoderInitFailed "ADecoder init zlyhal :(\n"
#define MSGTR_ADecoderPreinitFailed "ADecoder preinit zlyhal :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Zlyhal pokus o otvorenie podpory LIRC!\n"
#define MSGTR_LIRCcfgerr "Zlyhalo �?ítanie konfigura�?ného súboru LIRC %s!\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Nemôžem nájsť video filter '%s'\n"
#define MSGTR_CouldNotOpenVideoFilter "Nemôžem otvoriť video filter '%s'\n"
#define MSGTR_OpeningVideoFilter "Otváram video filter: "
#define MSGTR_CannotFindColorspace "Nemôžem nájsť bežný priestor farieb, ani vložením 'scale' :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: kodek nenastavil sh->disp_w a sh->disp_h, skúšam to obísť!\n"
#define MSGTR_CouldNotFindColorspace "Nemôžem nájsť zhodný priestor farieb - skúšam znova s -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Movie-Aspect je %.2f:1 - mením rozmery na správne.\n"
#define MSGTR_MovieAspectUndefined "Movie-Aspect je nedefinovný - nemenia sa rozmery.\n"

// vd_dshow.c, vd_dmo.c
#define MSGTR_DownloadCodecPackage "Potrebujete aktualizovať alebo nainštalovať binárne kódeky.\nChodte na http://www.mplayerhq.hu/dload.html\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: Nemôžem poslať udalosť EWMH fullscreen!\n"
#define MSGTR_CouldNotFindXScreenSaver "xscreensaver_disable: Nedá sa nájsť XScreenSaveru.\n"
#define MSGTR_SelectedVideoMode "XF86VM: Zvolený videorežim %dx%d pre obraz velkosti %dx%d.\n"

#define MSGTR_InsertingAfVolume "[Mixer] Hardvérový mixér nie je k dispozicí, vkladám filter pre hlasitosť.\n"
#define MSGTR_NoVolume "[Mixer] Ovládanie hlasitosti nie je dostupné.\n"

// ====================== GUI messages/buttons ========================

#ifdef CONFIG_GUI

// --- labels ---
#define MSGTR_About "O aplikácii"
#define MSGTR_FileSelect "Vybrať súbor..."
#define MSGTR_SubtitleSelect "Vybrať titulky..."
#define MSGTR_OtherSelect "Vybrať..."
#define MSGTR_AudioFileSelect "Vybrať externý audio kanál..."
#define MSGTR_FontSelect "Vybrať font..."
// Note: If you change MSGTR_PlayList please see if it still fits MSGTR_MENU_PlayList
#define MSGTR_PlayList "PlayList"
#define MSGTR_Equalizer "Equalizer"
#define MSGTR_ConfigureEqualizer "Konfigurovať Equalizer"
#define MSGTR_SkinBrowser "Prehliada�? tém"
#define MSGTR_Network "Sieťové prehrávanie (streaming)..."
// Note: If you change MSGTR_Preferences please see if it still fits MSGTR_MENU_Preferences
#define MSGTR_Preferences "Preferencie"
#define MSGTR_AudioPreferences "Konfiguracia ovlada�?a zvuku"
#define MSGTR_NoMediaOpened "Ni�? nie je otvorené"
#define MSGTR_NoChapter "Žiadna kapitola"
#define MSGTR_Chapter "Kapitola %d"
#define MSGTR_NoFileLoaded "Nenahraný žiaden súbor"

// --- buttons ---
#define MSGTR_Ok "Ok"
#define MSGTR_Cancel "Zrušiť"
#define MSGTR_Add "Pridať"
#define MSGTR_Remove "Odobrať"
#define MSGTR_Clear "Vy�?istiť"
#define MSGTR_Config "Konfigurácia"
#define MSGTR_ConfigDriver "Konfigurovať ovláda�?"
#define MSGTR_Browse "Prehliadať"

// --- error messages ---
#define MSGTR_NEMDB "Žiaľ, nedostatok pamäte pre buffer na kreslenie."
#define MSGTR_NEMFMR "Žiaľ, nedostatok pamäte pre vytváranie menu."
#define MSGTR_IDFGCVD "Žiaľ, nemôžem nájsť gui kompatibilný ovláda�? video výstupu."
#define MSGTR_NEEDLAVC "Žiaľ, nemôžete prehrávať nie mpeg súbory s DXR3/H+ zariadením bez prekódovania.\nProsím zapnite lavc v DXR3/H+ konfig. okne."

// --- skin loader error messages
#define MSGTR_SKIN_ERRORMESSAGE "[témy] chyba v konfig. súbore tém %d: %s"
#define MSGTR_SKIN_SkinFileNotFound "[skin] súbor ( %s ) nenájdený.\n"
#define MSGTR_SKIN_SkinFileNotReadable "[skin] súbor ( %s ) sa nedá pre�?ítať.\n"
#define MSGTR_SKIN_BITMAP_16bit  "bitmapa s hĺbkou 16 bit a menej je nepodporovaná (%s).\n"
#define MSGTR_SKIN_BITMAP_FileNotFound  "súbor nenájdený (%s)\n"
#define MSGTR_SKIN_BITMAP_PNGReadError "chyba �?ítania PNG (%s)\n"
#define MSGTR_SKIN_BITMAP_ConversionError "chyba konverzie z 24 bit do 32 bit (%s)\n"
#define MSGTR_SKIN_UnknownMessage "neznáma správa: %s\n"
#define MSGTR_SKIN_NotEnoughMemory "nedostatok pamäte\n"
#define MSGTR_SKIN_FONT_TooManyFontsDeclared "príliš mnoho fontov deklarovaných\n"
#define MSGTR_SKIN_FONT_FontFileNotFound "súbor fontov nenájdený\n"
#define MSGTR_SKIN_FONT_FontImageNotFound "súbor obrazov fontu nenájdený\n"
#define MSGTR_SKIN_FONT_NonExistentFont "neexistujúci identifikátor fontu (%s)\n"
#define MSGTR_SKIN_UnknownParameter "neznámy parameter (%s)\n"
#define MSGTR_SKIN_SKINCFG_SkinNotFound "Téma nenájdená (%s).\n"
#define MSGTR_SKIN_SKINCFG_SelectedSkinNotFound "Vybraná téma ( %s ) nenájdená, skúšam 'prednastavenú'...\n"
#define MSGTR_SKIN_LABEL "Témy:"

// --- gtk menus
#define MSGTR_MENU_AboutMPlayer "O aplikácii MPlayer"
#define MSGTR_MENU_Open "Otvoriť..."
#define MSGTR_MENU_PlayFile "Prehrať súbor..."
#define MSGTR_MENU_PlayVCD "Prehrať VCD..."
#define MSGTR_MENU_PlayDVD "Prehrať DVD..."
#define MSGTR_MENU_PlayURL "Prehrať URL..."
#define MSGTR_MENU_LoadSubtitle "Na�?ítať titulky..."
#define MSGTR_MENU_DropSubtitle "Zahodiť titulky..."
#define MSGTR_MENU_LoadExternAudioFile "Na�?ítať externý audio súbor..."
#define MSGTR_MENU_Playing "Prehrávam"
#define MSGTR_MENU_Play "Prehrať"
#define MSGTR_MENU_Pause "Pauza"
#define MSGTR_MENU_Stop "Zastaviť"
#define MSGTR_MENU_NextStream "Ďalší prúd"
#define MSGTR_MENU_PrevStream "Predchádzajúci prúd"
#define MSGTR_MENU_Size "Veľkosť"
#define MSGTR_MENU_HalfSize   "Polovi�?ná velikosť"
#define MSGTR_MENU_NormalSize "Normálna veľkosť"
#define MSGTR_MENU_DoubleSize "Dvojnásobná veľkosť"
#define MSGTR_MENU_FullScreen "Celá obrazovka"
#define MSGTR_MENU_DVD "DVD"
#define MSGTR_MENU_VCD "VCD"
#define MSGTR_MENU_PlayDisc "Prehrať disk..."
#define MSGTR_MENU_ShowDVDMenu "Zobraziť DVD menu"
#define MSGTR_MENU_Titles "Tituly"
#define MSGTR_MENU_Title "Titul %2d"
#define MSGTR_MENU_None "(ni�?)"
#define MSGTR_MENU_Chapters "Kapitoly"
#define MSGTR_MENU_Chapter "Kapitola %2d"
#define MSGTR_MENU_AudioLanguages "Jazyk zvuku"
#define MSGTR_MENU_SubtitleLanguages "Jazyk titulkov"
#define MSGTR_MENU_PlayList "Playlist"
#define MSGTR_MENU_SkinBrowser "Prehliada�? tém"
#define MSGTR_MENU_Preferences MSGTR_Preferences
#define MSGTR_MENU_Exit "Koniec"
#define MSGTR_MENU_Mute "Stlmiť zvuk"
#define MSGTR_MENU_Original "Originál"
#define MSGTR_MENU_AspectRatio "Pomer strán obrazu"
#define MSGTR_MENU_AudioTrack "Audio stopa"
#define MSGTR_MENU_Track "Stopa %d"
#define MSGTR_MENU_VideoTrack "Video stopa"
#define MSGTR_MENU_Subtitles "Titulky"

// --- equalizer
// Note: If you change MSGTR_EQU_Audio please see if it still fits MSGTR_PREFERENCES_Audio
#define MSGTR_EQU_Audio "Audio"
// Note: If you change MSGTR_EQU_Video please see if it still fits MSGTR_PREFERENCES_Video
#define MSGTR_EQU_Video "Video"
#define MSGTR_EQU_Contrast "Kontrast: "
#define MSGTR_EQU_Brightness "Jas: "
#define MSGTR_EQU_Hue "Odtieň: "
#define MSGTR_EQU_Saturation "Nasýtenie: "
#define MSGTR_EQU_Front_Left "Predný Ľavý"
#define MSGTR_EQU_Front_Right "Predný Pravý"
#define MSGTR_EQU_Back_Left "Zadný Ľavý"
#define MSGTR_EQU_Back_Right "Zadný Pravý"
#define MSGTR_EQU_Center "Stredný"
#define MSGTR_EQU_Bass "Basový"
#define MSGTR_EQU_All "Všetko"
#define MSGTR_EQU_Channel1 "Kanál 1:"
#define MSGTR_EQU_Channel2 "Kanál 2:"
#define MSGTR_EQU_Channel3 "Kanál 3:"
#define MSGTR_EQU_Channel4 "Kanál 4:"
#define MSGTR_EQU_Channel5 "Kanál 5:"
#define MSGTR_EQU_Channel6 "Kanál 6:"

// --- playlist
#define MSGTR_PLAYLIST_Path "Cesta"
#define MSGTR_PLAYLIST_Selected "Vybrané súbory"
#define MSGTR_PLAYLIST_Files "Súbory"
#define MSGTR_PLAYLIST_DirectoryTree "Adresárový strom"

// --- preferences
#define MSGTR_PREFERENCES_Audio MSGTR_EQU_Audio
#define MSGTR_PREFERENCES_Video MSGTR_EQU_Video
#define MSGTR_PREFERENCES_SubtitleOSD "Titulky a OSD"
#define MSGTR_PREFERENCES_Codecs "Kódeky a demuxer"
// Poznámka: Pokiaľ zmeníte MSGTR_PREFERENCES_Misc, uistite sa prosím, že vyhovuje aj pre MSGTR_PREFERENCES_FRAME_Misc
#define MSGTR_PREFERENCES_Misc "Rôzne"

#define MSGTR_PREFERENCES_None "Ni�?"
#define MSGTR_PREFERENCES_DriverDefault "východzie nastavenie"
#define MSGTR_PREFERENCES_AvailableDrivers "Dostupné ovláda�?e:"
#define MSGTR_PREFERENCES_DoNotPlaySound "Nehrať zvuk"
#define MSGTR_PREFERENCES_NormalizeSound "Normalizovať zvuk"
#define MSGTR_PREFERENCES_EnableEqualizer "Zapnúť equalizer"
#define MSGTR_PREFERENCES_SoftwareMixer "Aktivovať softvérový mixér"
#define MSGTR_PREFERENCES_ExtraStereo "Zapnúť extra stereo"
#define MSGTR_PREFERENCES_Coefficient "Koeficient:"
#define MSGTR_PREFERENCES_AudioDelay "Audio oneskorenie"
#define MSGTR_PREFERENCES_DoubleBuffer "Zapnúť dvojtý buffering"
#define MSGTR_PREFERENCES_DirectRender "Zapnúť direct rendering"
#define MSGTR_PREFERENCES_FrameDrop "Povoliť zahadzovanie rámcov"
#define MSGTR_PREFERENCES_HFrameDrop "Povoliť TVRDÉ zahadzovanie rámcov (nebezpe�?né)"
#define MSGTR_PREFERENCES_Flip "prehodiť obraz horná strana-dole"
#define MSGTR_PREFERENCES_Panscan "Panscan: "
#define MSGTR_PREFERENCES_Subtitle "Titulky:"
#define MSGTR_PREFERENCES_SUB_Delay "Oneskorenie: "
#define MSGTR_PREFERENCES_SUB_FPS "FPS:"
#define MSGTR_PREFERENCES_SUB_POS "Pozícia: "
#define MSGTR_PREFERENCES_SUB_AutoLoad "Zakázať automatické nahrávanie titulkov"
#define MSGTR_PREFERENCES_SUB_Unicode "Titulky v Unicode"
#define MSGTR_PREFERENCES_SUB_MPSUB "Konvertovať dané titulky do MPlayer formátu"
#define MSGTR_PREFERENCES_SUB_SRT "Konvertovať dané titulky do �?asovo-ur�?eného SubViewer (SRT) formátu"
#define MSGTR_PREFERENCES_SUB_Overlap "Zapnúť prekrývanie titulkov"
#define MSGTR_PREFERENCES_Font "Font:"
#define MSGTR_PREFERENCES_FontFactor "Font faktor:"
#define MSGTR_PREFERENCES_PostProcess "Zapnúť postprocess"
#define MSGTR_PREFERENCES_AutoQuality "Automatická qualita: "
#define MSGTR_PREFERENCES_NI "Použiť neprekladaný AVI parser"
#define MSGTR_PREFERENCES_IDX "Obnoviť index tabulku, ak je potrebné"
#define MSGTR_PREFERENCES_VideoCodecFamily "Rodina video kodekov:"
#define MSGTR_PREFERENCES_AudioCodecFamily "Rodina audeo kodekov:"
#define MSGTR_PREFERENCES_FRAME_OSD_Level "OSD úroveň"
#define MSGTR_PREFERENCES_FRAME_Subtitle "Titulky"
#define MSGTR_PREFERENCES_FRAME_Font "Font"
#define MSGTR_PREFERENCES_FRAME_PostProcess "Postprocess"
#define MSGTR_PREFERENCES_FRAME_CodecDemuxer "Kódek & demuxer"
#define MSGTR_PREFERENCES_FRAME_Cache "Vyrovnávacia pamäť"
#define MSGTR_PREFERENCES_FRAME_Misc MSGTR_PREFERENCES_Misc
#define MSGTR_PREFERENCES_Audio_Device "Zariadenie:"
#define MSGTR_PREFERENCES_Audio_Mixer "Mixér:"
#define MSGTR_PREFERENCES_Audio_MixerChannel "Kanál mixéru:"
#define MSGTR_PREFERENCES_Message "Prosím pamätajte, nietoré voľby potrebujú reštart prehrávania!"
#define MSGTR_PREFERENCES_DXR3_VENC "Video kóder:"
#define MSGTR_PREFERENCES_DXR3_LAVC "Použiť LAVC (FFmpeg)"
#define MSGTR_PREFERENCES_FontEncoding1 "Unicode"
#define MSGTR_PREFERENCES_FontEncoding2 "Western European Languages (ISO-8859-1)"
#define MSGTR_PREFERENCES_FontEncoding3 "Western European Languages with Euro (ISO-8859-15)"
#define MSGTR_PREFERENCES_FontEncoding4 "Slavic/Central European Languages (ISO-8859-2)"
#define MSGTR_PREFERENCES_FontEncoding5 "Esperanto, Galician, Maltese, Turkish (ISO-8859-3)"
#define MSGTR_PREFERENCES_FontEncoding6 "Old Baltic charset (ISO-8859-4)"
#define MSGTR_PREFERENCES_FontEncoding7 "Cyrillic (ISO-8859-5)"
#define MSGTR_PREFERENCES_FontEncoding8 "Arabic (ISO-8859-6)"
#define MSGTR_PREFERENCES_FontEncoding9 "Modern Greek (ISO-8859-7)"
#define MSGTR_PREFERENCES_FontEncoding10 "Turkish (ISO-8859-9)"
#define MSGTR_PREFERENCES_FontEncoding11 "Baltic (ISO-8859-13)"
#define MSGTR_PREFERENCES_FontEncoding12 "Celtic (ISO-8859-14)"
#define MSGTR_PREFERENCES_FontEncoding13 "Hebrew charsets (ISO-8859-8)"
#define MSGTR_PREFERENCES_FontEncoding14 "Russian (KOI8-R)"
#define MSGTR_PREFERENCES_FontEncoding15 "Ukrainian, Belarusian (KOI8-U/RU)"
#define MSGTR_PREFERENCES_FontEncoding16 "Simplified Chinese charset (CP936)"
#define MSGTR_PREFERENCES_FontEncoding17 "Traditional Chinese charset (BIG5)"
#define MSGTR_PREFERENCES_FontEncoding18 "Japanese charsets (SHIFT-JIS)"
#define MSGTR_PREFERENCES_FontEncoding19 "Korean charset (CP949)"
#define MSGTR_PREFERENCES_FontEncoding20 "Thai charset (CP874)"
#define MSGTR_PREFERENCES_FontEncoding21 "Cyrillic Windows (CP1251)"
#define MSGTR_PREFERENCES_FontEncoding22 "Slavic/Central European Windows (CP1250)"
#define MSGTR_PREFERENCES_FontNoAutoScale "Nemeniť rozmery"
#define MSGTR_PREFERENCES_FontPropWidth "Proporcionálne k šírke obrazu"
#define MSGTR_PREFERENCES_FontPropHeight "Proporcionálne k výške obrazu"
#define MSGTR_PREFERENCES_FontPropDiagonal "Proporcionálne k diagonále obrazu"
#define MSGTR_PREFERENCES_FontEncoding "Kódovanie:"
#define MSGTR_PREFERENCES_FontBlur "Rozmazanie:"
#define MSGTR_PREFERENCES_FontOutLine "Obrys:"
#define MSGTR_PREFERENCES_FontTextScale "Mierka textu:"
#define MSGTR_PREFERENCES_FontOSDScale "OSD mierka:"
#define MSGTR_PREFERENCES_Cache "Vyrovnávacia pamäť zap./vyp."
#define MSGTR_PREFERENCES_CacheSize "Veľkosť vyr. pamäte: "
#define MSGTR_PREFERENCES_LoadFullscreen "Naštartovať v režime celej obrazovky"
#define MSGTR_PREFERENCES_SaveWinPos "Uložiť pozíciu okna"
#define MSGTR_PREFERENCES_XSCREENSAVER "Zastaviť XScreenSaver"
#define MSGTR_PREFERENCES_PlayBar "Zapnúť playbar"
#define MSGTR_PREFERENCES_AutoSync "Automatická synchronizácia zap./vyp."
#define MSGTR_PREFERENCES_AutoSyncValue "Automatická synchronizácia: "
#define MSGTR_PREFERENCES_CDROMDevice "CD-ROM zariadenie:"
#define MSGTR_PREFERENCES_DVDDevice "DVD zariadenie:"
#define MSGTR_PREFERENCES_FPS "Snímková rýchlosť (FPS):"
#define MSGTR_PREFERENCES_ShowVideoWindow "Ukázať video okno pri neaktivite"
#define MSGTR_PREFERENCES_ArtsBroken "Novšie verze aRts sú nekompatibilné "\
           "s GTK 1.x a zhavarujú GMPlayer!"

#define MSGTR_ABOUT_UHU "vývoj GUI sponzoroval UHU Linux\n"
#define MSGTR_ABOUT_Contributors "Přispievatelia kódu a dokumentacie\n"
#define MSGTR_ABOUT_Codecs_libs_contributions "Kódeky a knižnice tretích strán\n"
#define MSGTR_ABOUT_Translations "Preklady\n"
#define MSGTR_ABOUT_Skins "Témy\n"

// --- messagebox
#define MSGTR_MSGBOX_LABEL_FatalError "Fatálna chyba!"
#define MSGTR_MSGBOX_LABEL_Error "Chyba!"
#define MSGTR_MSGBOX_LABEL_Warning "Upozornenie!"

#endif
