// Translated to Japanese.
// Translated by smoker <http://smokerz.net/~smoker/>

// This translation is not complete yet.

// ========================= MPlayer help ===========================

static const char help_text[]=
"ä½¿ã?„æ–¹:   mplayer [ã‚ªãƒ—ã‚·ãƒ§ãƒ³] [url|ãƒ‘ã‚¹/]ãƒ•ã‚¡ã‚¤ãƒ«å??\n"
"\n"
"åŸºæœ¬çš„ã?ªã‚ªãƒ—ã‚·ãƒ§ãƒ³: (man page ã?«å…¨ã?¦ç¶²ç¾…ã?•ã‚Œã?¦ã?„ã?¾ã?™)\n"
" -vo <drv[:dev]>  æ˜ åƒ?å‡ºåŠ›ãƒ‰ãƒ©ã‚¤ãƒ?å?Šã?³ãƒ‡ãƒ?ã‚¤ã‚¹ã‚’é?¸æŠžã?—ã?¾ã?™ ('-vo help'ã?§ä¸€è¦§è¡¨ç¤ºã?•ã‚Œã?¾ã?™)\n"
" -ao <drv[:dev]>  éŸ³å£°å‡ºåŠ›ãƒ‰ãƒ©ã‚¤ãƒ?å?Šã?³ãƒ‡ãƒ?ã‚¤ã‚¹ã‚’é?¸æŠžã?—ã?¾ã?™ ('-ao help'ã?§ä¸€è¦§è¡¨ç¤ºã?•ã‚Œã?¾ã?™)\n"
#ifdef CONFIG_VCD
" vcd://<trackno>   play VCD (Video CD) track from device instead of plain file\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>   play DVD title from device instead of plain file\n"
" -alang/-slang    DVDã?® éŸ³å£°/ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ« è¨€èªžè¨­å®š (2æ–‡å­—ã?®ã‚«ãƒ³ãƒˆãƒªãƒ¼ã‚³ãƒ¼ãƒ‰ã?§æŒ‡å®š)\n"
#endif
" -ss <timepos>    timeposã?«ä¸Žã?ˆã‚‰ã‚Œã?Ÿå ´æ‰€ã?‹ã‚‰å†?ç”Ÿã?—ã?¾ã?™(seconds or hh:mm:ss)\n"
" -nosound         éŸ³å£°å‡ºåŠ›ã‚’æŠ‘æ­¢ã?—ã?¾ã?™\n"
" -fs              ãƒ•ãƒ«ã‚¹ã‚¯ãƒªãƒ¼ãƒ³è¡¨ç¤ºã?—ã?¾ã?™(ã‚‚ã?—ã??ã?¯ -vm, -zoom, è©³ç´°ã?¯manã?«ã?‚ã‚Šã?¾ã?™)\n"
" -x <x> -y <y>    è¡¨ç¤ºã‚µã‚¤ã‚ºã‚’æŒ‡å®šã?—ã?¾ã?™ (ä¸€ç·’ã?«æ¬¡ã?®ã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚’åˆ©ç”¨ä¸‹ã?•ã?„ -vm or -zoom)\n"
" -sub <file>      åˆ©ç”¨ã?™ã‚‹ subtitle ãƒ•ã‚¡ã‚¤ãƒ«ã‚’é?¸æŠžã?™ã‚‹(-subfps, -subdelay ã‚‚å¾¡è¦§ä¸‹ã?•ã?„)\n"
" -playlist <file> playlistãƒ•ã‚¡ã‚¤ãƒ«ã‚’é?¸æŠžã?™ã‚‹\n"
" -vid x -aid y    select video (x) and audio (y) stream to play\n"
" -fps x -srate y  change video (x fps) and audio (y Hz) rate\n"
" -pp <quality>    postprocessing filterã‚’æœ‰åŠ¹ã?«ã?™ã‚‹ (è©³ç´°ã?¯ man page ã?«ã?‚ã‚Šã?¾ã?™)\n"
" -framedrop       frame droppingã‚’æœ‰åŠ¹ã?«ã?™ã‚‹ (ä½Žé€Ÿã?ªãƒžã‚·ãƒ³å?‘ã??ã?§ã?™)\n"
"\n"
"åŸºæœ¬çš„ã?ªã‚³ãƒžãƒ³ãƒ‰: (man pageã?«å…¨ã?¦ç¶²ç¾…ã?•ã‚Œã?¦ã?„ã?¾ã?™ã€‚äº‹å‰?ã?«input.confã‚‚ç¢ºèª?ã?—ã?¦ä¸‹ã?•ã?„)\n"
" <-  or  ->       10ç§’å?˜ä½?ã?§å‰?å¾Œã?«ã‚·ãƒ¼ã‚¯ã?—ã?¾ã?™\n"
" up or down       1åˆ†å?˜ä½?ã?§å‰?å¾Œã?«ã‚·ãƒ¼ã‚¯ã?—ã?¾ã?™\n"
" pgup or pgdown   10åˆ†å?˜ä½?ã?§å‰?å¾Œã?«ã‚·ãƒ¼ã‚¯ã?—ã?¾ã?™\n"
" < or >           ãƒ—ãƒ¬ã‚¤ãƒªã‚¹ãƒˆã‚’å…ƒã?«å‰?å¾Œã?®ãƒ•ã‚¡ã‚¤ãƒ«ã?«é?·ç§»ã?—ã?¾ã?™\n"
" p or SPACE       å†?ç”Ÿã‚’é?™æ­¢ã?—ã?¾ã?™(ä½•ã?‹ãƒœã‚¿ãƒ³ã‚’æŠ¼ä¸‹ã?™ã‚‹ã?¨å†?ç”Ÿã‚’é–‹å§‹ã?—ã?¾ã?™)\n"
" q or ESC         å†?ç”Ÿã‚’é?™æ­¢ã?—ã€?ãƒ—ãƒ­ã‚°ãƒ©ãƒ ã‚’å?œæ­¢ã?—ã?¾ã?™\n"
" + or -           éŸ³å£°ã‚’ 0.1 ç§’å?˜ä½?ã?§æ—©ã‚?ã?Ÿã‚Šé?…ã‚Œã?•ã?›ã?Ÿã‚Šèª¿æ•´ã?™ã‚‹\n"
" o                cycle OSD mode:  none / seekbar / seekbar + timer\n"
" * or /           PCM éŸ³é‡?ã‚’ä¸Šã?’ã?Ÿã‚Šä¸‹ã?’ã?Ÿã‚Šã?™ã‚‹\n"
" z or x           subtitleã‚’ 0.1 ç§’å?˜ä½?ã?§æ—©ã‚?ã?Ÿã‚Šé?…ã‚Œã?•ã?›ã?Ÿã‚Šèª¿æ•´ã?™ã‚‹\n"
" r or t           subtitleã?®ä½?ç½®ã‚’ä¸Šã?’ã?Ÿã‚Šä¸‹ã?’ã?Ÿã‚Šèª¿æ•´ã?™ã‚‹, -vfã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚‚ç¢ºèª?ã?—ã?¦ä¸‹ã?•ã?„\n"
"\n"
" * * * man pageã?«è©³ç´°ã?Œã?‚ã‚Šã?¾ã?™ã?®ã?§ã€?ç¢ºèª?ã?—ã?¦ä¸‹ã?•ã?„ã€‚ã?•ã‚‰ã?«é«˜åº¦ã?§é€²ã‚“ã? ã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚„ã‚­ãƒ¼ã‚‚è¨˜è¼‰ã?—ã?¦ã?¾ã?™ * * *\n"
"\n";

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\nçµ‚äº†ã?—ã?¦ã?„ã?¾ã?™\n"
#define MSGTR_ExitingHow "\nçµ‚äº†ã?—ã?¦ã?„ã?¾ã?™... (%s)\n"
#define MSGTR_Exit_quit "çµ‚äº†"
#define MSGTR_Exit_eof "ãƒ•ã‚¡ã‚¤ãƒ«ã?®æœ«ç«¯ã?§ã?™"
#define MSGTR_Exit_error "è‡´å‘½çš„ã‚¨ãƒ©ãƒ¼"
#define MSGTR_IntBySignal "\nMPlayer ã?¯ã‚·ã‚°ãƒŠãƒ« %d ã?«ã‚ˆã?£ã?¦ä¸­æ–­ã?—ã?¾ã?—ã?Ÿï¼Žæ¬¡ã?®ãƒ¢ã‚¸ãƒ¥ãƒ¼ãƒ«ã?‹ã‚‰ã?§ã?™: %s\n"
#define MSGTR_NoHomeDir "ãƒ›ãƒ¼ãƒ ãƒ‡ã‚£ãƒ¬ã‚¯ãƒˆãƒªã‚’è¦‹ä»˜ã?‘ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ã?§ã?—ã?Ÿ.\n"
#define MSGTR_GetpathProblem "get_path(\"config\") ã?§å•?é¡Œã?Œèµ·ã??ã?¾ã?—ã?Ÿ\n"
#define MSGTR_CreatingCfgFile "config fileã‚’ä½œæˆ?ã?—ã?¾ã?—ã?Ÿ: %s\n"
#define MSGTR_CantLoadFont "ãƒ•ã‚©ãƒ³ãƒˆã‚’ãƒ­ãƒ¼ãƒ‰å‡ºæ?¥ã?¾ã?›ã‚“: %s\n"
#define MSGTR_CantLoadSub "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«ã‚’ãƒ­ãƒ¼ãƒ‰å‡ºæ?¥ã?¾ã?›ã‚“: %s\n"
#define MSGTR_CantOpenDumpfile "dump fileã‚’é–‹ã?‘ã?¾ã?›ã‚“\n"
#define MSGTR_CoreDumped "ã‚³ã‚¢ãƒ€ãƒ³ãƒ— ;)\n"
#define MSGTR_FPSnotspecified "FPS ã?Œãƒ˜ãƒƒãƒ€ã?«æŒ‡å®šã?•ã‚Œã?¦ã?„ã?ªã?„ã?‹ä¸?æ­£ã?§ã?™. -fps ã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚’åˆ©ç”¨ã?—ã?¦ä¸‹ã?•ã?„.\n"
#define MSGTR_CantFindAudioCodec "audio format 0x%X å?‘ã?‘ã?®ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã‚’è¦‹ä»˜ã?‘ã‚‹äº‹ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ã?§ã?—ã?Ÿ.\n"
#define MSGTR_VOincompCodec "é?¸æŠžã?•ã‚Œã?Ÿæ˜ åƒ?å‡ºåŠ›ãƒ‡ãƒ?ã‚¤ã‚¹ã?¯ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã?¨äº’æ?›æ€§ã?Œã?‚ã‚Šã?¾ã?›ã‚“\n"
#define MSGTR_CannotInitVO "FATAL: æ˜ åƒ?å‡ºåŠ›ãƒ‰ãƒ©ã‚¤ãƒ?ã?®åˆ?æœŸåŒ–ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ.\n"
#define MSGTR_CannotInitAO "éŸ³å£°ãƒ‡ãƒ?ã‚¤ã‚¹ã?®åˆ?æœŸåŒ–ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ -> ç„¡éŸ³å£°ã?«ã?ªã‚Šã?¾ã?™.\n"
#define MSGTR_StartPlaying "å†?ç”Ÿé–‹å§‹...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"           ************************************************\n"\
"           ***  è²´æ–¹ã?®ã‚·ã‚¹ãƒ†ãƒ ã?¯ã?“ã‚Œã‚’å†?ç”Ÿã?™ã‚‹ã?«ã?¯é?…ã?„  ***\n"\
"           ************************************************\n\n"\
"äºˆæƒ³ã?•ã‚Œã‚‹å•?é¡Œã‚„ç’°å¢ƒã?¯:\n"\
"- Most common: å£Šã‚Œã?¦ã‚‹ã?‹ã€?ãƒ?ã‚®ãƒ¼ã?ª éŸ³å£°ãƒ‰ãƒ©ã‚¤ãƒ?ãƒ¼\n"\
"  - -ao ã‚’ä½¿ã?„ sdl ã?‹ ALSA 0.5 ã‚‚ã?—ã??ã?¯ALSA 0.9ã?§OSSã‚¨ãƒŸãƒ¥ãƒ¬ãƒ¼ã‚·ãƒ§ãƒ³ã‚’è©¦ã?¿ã‚‹.\n"\
"  - Experiment with different values for -autosync, 30 is a good start.\n"\
"- æ˜ åƒ?å‡ºåŠ›ã?Œé?…ã?„\n"\
"  - é?•ã?† -vo ãƒ‰ãƒ©ã‚¤ãƒ?ã‚’åˆ©ç”¨ã?™ã‚‹ã?‹(-vo helpã?§ãƒªã‚¹ãƒˆã?Œå¾—ã‚‰ã‚Œã?¾ã?™) -framedropã‚’è©¦ã?¿ã‚‹\n"\
"- CPUã?Œé?…ã?„\n"\
"  - å·¨å¤§ã?ªDVDã‚„DivXã‚’é?…ã?„CPUã?§å†?ç”Ÿã?—ã‚ˆã?†ã?¨è©¦ã?¿ã?ªã?„ ;-) -hardframedropã‚’è©¦ã?¿ã‚‹\n"\
"- ãƒ•ã‚¡ã‚¤ãƒ«ã?Œå£Šã‚Œã?¦ã‚‹\n"\
"  - æ¬¡ã?®ã‚ªãƒ—ã‚·ãƒ§ãƒ³ã?®æ§˜ã€…ã?ªçµ„å?ˆã?›ã‚’è©¦ã?¿ã?¦ä¸‹ã?•ã?„: -nobps -ni -forceidx -mc 0.\n"\
"- é?…ã?„ãƒ¡ãƒ‡ã‚£ã‚¢(NFS/SMB ã? ã?£ã?Ÿã‚Š, DVD, VCD ã?ªã?©ã?®ãƒ‰ãƒ©ã‚¤ãƒ–ã?Œé?…ã?‹ã?£ã?Ÿã‚Š)\n"\
"  -æ¬¡ã?®ã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚’è©¦ã?¿ã‚‹ -cache 8192.\n"\
"- non-interleaved AVI ãƒ•ã‚¡ã‚¤ãƒ«ã?« -cacheã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚’ä½¿ã?£ã?¦ã?¾ã?›ã‚“ã?‹?\n"\
"  - æ¬¡ã?®ã‚ªãƒ—ã‚·ãƒ§ãƒ³ã‚’è©¦ã?¿ã‚‹ -nocache.\n"\
"ãƒ?ãƒ¥ãƒ¼ãƒ‹ãƒ³ã‚°ã€?ã‚¹ãƒ”ãƒ¼ãƒ‰ã‚¢ãƒƒãƒ—ã?®ç‚ºã?« DOCS/HTML/en/video.html ã‚’å¾¡è¦§ä¸‹ã?•ã?„.\n"\
"ã‚‚ã?—ã€?ã?“ã‚Œã‚‰ã‚’è©¦ã?—ã?¦ã‚‚ä½•ã‚‚ã?“ã?†åŒ–ã?Œå¾—ã‚‰ã‚Œã?ªã?„å ´å?ˆã?¯ã€?DOCS/HTML/en/bugreports.html ã‚’å¾¡è¦§ä¸‹ã?•ã?„.\n\n"

#define MSGTR_NoGui "MPlayer ã?¯GUIã‚µãƒ?ãƒ¼ãƒˆã‚’ç„¡åŠ¹ã?«ã?—ã?¦ã‚³ãƒ³ãƒ‘ã‚¤ãƒ«ã?•ã‚Œã?¾ã?—ã?Ÿ.\n"
#define MSGTR_GuiNeedsX "MPlayerã?®GUIã?¯X11ã‚’å¿…è¦?ã?¨ã?—ã?¾ã?™.\n"
#define MSGTR_Playing "%s ã‚’å†?ç”Ÿä¸­\n"
#define MSGTR_NoSound "éŸ³å£°: ç„¡ã?—\n"
#define MSGTR_FPSforced "FPS forced to be %5.3f  (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "æœ‰åŠ¹ã?ªæ˜ åƒ?å‡ºåŠ›ãƒ‰ãƒ©ã‚¤ãƒ?:\n"
#define MSGTR_AvailableAudioOutputDrivers "æœ‰åŠ¹ã?ªéŸ³å£°å‡ºåŠ›ãƒ‰ãƒ©ã‚¤ãƒ?:\n"
#define MSGTR_AvailableAudioCodecs "æœ‰åŠ¹ã?ªéŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯:\n"
#define MSGTR_AvailableVideoCodecs "æœ‰åŠ¹ã?ªæ˜ åƒ?ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯:\n"
#define MSGTR_AvailableAudioFm "\næœ‰åŠ¹ã?ª(çµ„ã?¿è¾¼ã?¾ã‚Œã?Ÿ)éŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ families/drivers:\n"
#define MSGTR_AvailableVideoFm "\næœ‰åŠ¹ã?ª(çµ„ã?¿è¾¼ã?¾ã‚Œã?Ÿ)æ˜ åƒ?ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ families/drivers:\n"
#define MSGTR_AvailableFsType "å…¨ç”»é?¢è¡¨ç¤ºãƒ¢ãƒ¼ãƒ‰ã?¸ã?®åˆ‡æ›¿ã?ˆã?¯å?¯èƒ½ã?§ã?™:\n"
#define MSGTR_CannotReadVideoProperties "Video: ãƒ—ãƒ­ãƒ‘ãƒ†ã‚£ãƒ¼ã‚’èª­ã?¿å?–ã‚Œã?¾ã?›ã‚“.\n"
#define MSGTR_NoStreamFound "ã‚¹ãƒˆãƒªãƒ¼ãƒ ã‚’è¦‹ä»˜ã?‘ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ã?§ã?—ã?Ÿ.\n"
#define MSGTR_ErrorInitializingVODevice "é?¸æŠžã?•ã‚Œã?Ÿæ˜ åƒ?å‡ºåŠ›(-vo)ãƒ‡ãƒ?ã‚¤ã‚¹ã‚’é–‹ã??äº‹ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ã?§ã?—ã?Ÿ.\n"
#define MSGTR_ForcedVideoCodec "æŒ‡å®šã?•ã‚Œã?Ÿæ˜ åƒ?ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯: %s\n"
#define MSGTR_ForcedAudioCodec "æŒ‡å®šã?•ã‚Œã?ŸéŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯: %s\n"
#define MSGTR_Video_NoVideo "Video: æ˜ åƒ?ã?Œã?‚ã‚Šã?¾ã?›ã‚“\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: ç”»åƒ?ãƒ•ã‚£ãƒ«ã‚¿ãƒ¼(-vf)ã?‹ç”»åƒ?å‡ºåŠ›(-vo)ã?®åˆ?æœŸåŒ–ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ.\n"
#define MSGTR_Paused "  =====  å?œæ­¢  =====" // no more than 23 characters (status line for audio files)
#define MSGTR_PlaylistLoadUnable "\nãƒ—ãƒ¬ã‚¤ãƒªã‚¹ãƒˆã?®èª­ã?¿è¾¼ã?¿ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ %s.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayerã?¯ä¸?æ­£ã?ªå‘½ä»¤(Illegal Instruction)ã?«ã‚ˆã‚Šã‚¯ãƒ©ãƒƒã‚·ãƒ¥ã?—ã?¾ã?—ã?Ÿ\n"\
"  æ??ã‚‰ã??ã?“ã‚Œã?¯ ã?‚ã?Ÿã‚‰ã?—ã?„CPU-Detection codeã?«ãƒ?ã‚°ã?Œã?‚ã‚Šã?¾ã?™\n"\
"  DOCS/HTML/en/bugreports.html ã‚’ã?Šèª­ã?¿ä¸‹ã?•ã?„.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayerã?¯ä¸?æ­£ã?ªå‘½ä»¤(Illegal Instruction)ã?«ã‚ˆã‚Šã‚¯ãƒ©ãƒƒã‚·ãƒ¥ã?—ã?¾ã?—ã?Ÿ\n"\
"  It usually happens when you run it on a CPU different than the one it was\n"\
"  compiled/optimized for.\n"\
"  Verify this!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayerã?¯ä¸?è‰¯ã?ª CPU/FPU/RAM ã?«ã‚ˆã?£ã?¦ã‚¯ãƒ©ãƒƒã‚·ãƒ¥ã?—ã?¾ã?—ã?Ÿ.\n"\
"  Recompile MPlayer with --enable-debug and make a 'gdb' backtrace and\n"\
"  --enable-debugã‚’ã?¤ã?‘ã?¦MPlyaerã‚’ã‚³ãƒ³ãƒ‘ã‚¤ãƒ«ã?—ã?ªã?Šã?—ã€?gdbã?§èª¿æŸ»ã?—ã?¾ã?—ã‚‡ã?†\n"\
"  è©³ç´°ã?¯ DOCS/HTML/en/bugreports.html#bugreports_crash ã?«ã?‚ã‚Šã?¾ã?™\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer ã?¯æƒ³å®šã?•ã‚Œã?¦ã?„ã?ªã?„ã‚¯ãƒ©ãƒƒã‚·ãƒ¥ã‚’èµ·ã?“ã?—ã?¾ã?—ã?Ÿ.\n"\
"  It can be a bug in the MPlayer code _or_ in your drivers _or_ in your\n"\
"  gcc version. If you think it's MPlayer's fault, please read\n"\
"  DOCS/HTML/en/bugreports.html and follow the instructions there. We can't and\n"\
"  won't help unless you provide this information when reporting a possible bug.\n"


// mencoder.c:

#define MSGTR_CannotOpenFile_Device "ãƒ•ã‚¡ã‚¤ãƒ«å?Šã?³ãƒ‡ãƒ?ã‚¤ã‚¹ã?Œé–‹ã?‘ã?¾ã?›ã‚“.\n"
#define MSGTR_CannotOpenDemuxer "demuxerã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"
#define MSGTR_NoAudioEncoderSelected "\néŸ³å£°ã‚¨ãƒ³ã‚³ãƒ¼ãƒ€(-oac)ã?ŒæŒ‡å®šã?•ã‚Œã?¦ã?„ã?¾ã?›ã‚“ã€? ä½•ã?‹æŒ‡å®šã?™ã‚‹ã?‹ã€?ç„¡æŒ‡å®š(-nosound)ã‚’ä¸Žã?ˆã?¦ä¸‹ã?•ã?„ã€‚è©³ç´°ã?¯ '-oac help'\n"
#define MSGTR_NoVideoEncoderSelected "\næ˜ åƒ?ã‚¨ãƒ³ã‚³ãƒ¼ãƒ€(-ovc)ã?ŒæŒ‡å®šã?•ã‚Œã?¦ã?„ã?¾ã?›ã‚“ã€? ä½•ã?‹æŒ‡å®šã?—ã?¦ä¸‹ã?•ã?„ã€‚ è©³ç´°ã?¯ '-ovc help'\n"
#define MSGTR_CannotOpenOutputFile "å‡ºåŠ›ãƒ•ã‚¡ã‚¤ãƒ«'%s'ã‚’é–‹ã??äº‹ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"
#define MSGTR_EncoderOpenFailed "ã‚¨ãƒ³ã‚³ãƒ¼ãƒ€ã‚’é–‹ã??ã?“ã?¨ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ.\n"
#define MSGTR_ForcingOutputFourcc "fourccã‚’ %x [%.4s] ã?«æŒ‡å®šã?—ã?¾ã?™\n"
#define MSGTR_DuplicateFrames "\n%d é‡?è¤‡ã?—ã?Ÿãƒ•ãƒ¬ãƒ¼ãƒ \n"
#define MSGTR_SkipFrame "\nãƒ•ãƒ¬ãƒ¼ãƒ ã‚’ã‚¹ã‚­ãƒƒãƒ—ã?—ã?¦ã?„ã?¾ã?™\n"
#define MSGTR_ErrorWritingFile "%s: ãƒ•ã‚¡ã‚¤ãƒ«æ›¸ã??è¾¼ã?¿ã‚¨ãƒ©ãƒ¼.\n"
#define MSGTR_VideoStreamResult "\næ˜ åƒ?ã‚¹ãƒˆãƒªãƒ¼ãƒ : %8.3f kbit/s  (%d B/s)  ã‚µã‚¤ã‚º: %"PRIu64" bytes  %5.3f secs  %d ãƒ•ãƒ¬ãƒ¼ãƒ \n"
#define MSGTR_AudioStreamResult "\néŸ³å£°ã‚¹ãƒˆãƒªãƒ¼ãƒ : %8.3f kbit/s  (%d B/s)  ã‚µã‚¤ã‚º: %"PRIu64" bytes  %5.3f secs\n"

// cfg-mencoder.h:

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM ãƒ‡ãƒ?ã‚¤ã‚¹ '%s' ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“.\n"
#define MSGTR_ReadSTDIN "æ¨™æº–å…¥åŠ›ã?‹ã‚‰èª­ã?¿è¾¼ã‚“ã?§ã?„ã?¾ã?™...\n"
#define MSGTR_UnableOpenURL "æŒ‡å®šã?•ã‚Œã?ŸURLã‚’èª­ã?¿è¾¼ã‚?ã?¾ã?›ã‚“: %s\n"
#define MSGTR_ConnToServer "ã‚µãƒ¼ãƒ?ã?«æŽ¥ç¶šä¸­: %s\n"
#define MSGTR_FileNotFound "ãƒ•ã‚¡ã‚¤ãƒ«ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“: '%s'\n"

#define MSGTR_SMBInitError "libsmbclient ã?®åˆ?æœŸåŒ–å¤±æ•—: %d\n"
#define MSGTR_SMBFileNotFound "ãƒ­ãƒ¼ã‚«ãƒ«ã‚¨ãƒªã‚¢ãƒ?ãƒƒãƒˆãƒ¯ãƒ¼ã‚¯ã?‹ã‚‰é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ã?§ã?—ã?Ÿ: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer ã?¯SMB reading support ã‚’ç„¡åŠ¹ã?«ã?—ã?¦ã‚³ãƒ³ãƒ‘ã‚¤ãƒ«ã?•ã‚Œã?¦ã?„ã?¾ã?™\n"

#define MSGTR_CantOpenDVD "DVDãƒ‡ãƒ?ã‚¤ã‚¹ã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ã?§ã?—ã?Ÿ: %s (%s)\n"
#define MSGTR_DVDnumTitles "ã?“ã?®DVDã?«ã?¯ %d ã‚¿ã‚¤ãƒˆãƒ«è¨˜éŒ²ã?•ã‚Œã?¦ã?„ã?¾ã?™.\n"
#define MSGTR_DVDinvalidTitle "ä¸?æ­£ã?ª DVD ã‚¿ã‚¤ãƒˆãƒ«ç•ªå?·ã?§ã?™: %d\n"
#define MSGTR_DVDnumChapters "ã?“ã?®DVDã?¯ %d ã‚­ãƒ£ãƒ—ã‚¿ãƒ¼ã?‚ã‚Šã?¾ã?™.\n"
#define MSGTR_DVDinvalidChapter "ä¸?æ­£ã?ªDVDã‚­ãƒ£ãƒ—ã‚¿ãƒ¼ç•ªå?·ã?§ã?™r: %d\n"
#define MSGTR_DVDnumAngles "ã?“ã?®DVDã?«ã?¯ %d ã‚¢ãƒ³ã‚°ãƒ«ã?‚ã‚Šã?¾ã?™.\n"
#define MSGTR_DVDinvalidAngle "ä¸?æ­£ã?ªDVDã‚¢ãƒ³ã‚°ãƒ«ç•ªå?·ã?§ã?™: %d\n"
#define MSGTR_DVDnoIFO "Cannot open the IFO file for DVD title %d.\n"
#define MSGTR_DVDnoVOBs "Cannot open title VOBS (VTS_%02d_1.VOB).\n"

// muxer_*.c:
#define MSGTR_TooManyStreams "ã‚¹ãƒˆãƒªãƒ¼ãƒ ã?Œå¤šé?Žã?Žã‚‹!"
#define MSGTR_RawMuxerOnlyOneStream "Rawaudio muxerã?Œä¸€ã?¤ã? ã?‘ã?®éŸ³å£°ã‚¹ãƒˆãƒªãƒ¼ãƒ ã‚’ã‚µãƒ?ãƒ¼ãƒˆã?—ã?¾ã?™!\n"
#define MSGTR_IgnoringVideoStream "æ˜ åƒ?ã‚¹ãƒˆãƒªãƒ¼ãƒ ã‚’å·®ã?—ç½®ã??ã?¾ã?™!\n"
#define MSGTR_UnknownStreamType "è­¦å‘Š: æœªçŸ¥ã?®ã‚¹ãƒˆãƒªãƒ¼ãƒ ã‚¿ã‚¤ãƒ—: %d\n"
#define MSGTR_WarningLenIsntDivisible "è­¦å‘Š: samplesizeã?§lenã?Œæ•´é™¤ã?§ã??ã?ªã?„!\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "è­¦å‘Š: Audio stream header %d redefined.\n"
#define MSGTR_VideoStreamRedefined "è­¦å‘Š: Video stream header %d redefined.\n"
#define MSGTR_TooManyAudioInBuffer "\nãƒ?ãƒƒãƒ•ã‚¡ã?«å¤šé?Žã?Žã‚‹éŸ³å£°ãƒ‘ã‚±ãƒƒãƒˆã?Œä¸Žã?ˆã‚‰ã‚Œã?¦ã?¾ã?™: (%d in %d bytes).\n"
#define MSGTR_TooManyVideoInBuffer "\nãƒ?ãƒƒãƒ•ã‚¡ã?«å¤šé?Žã?Žã‚‹æ˜ åƒ?ãƒ‘ã‚±ãƒƒãƒˆã?Œä¸Žã?ˆã‚‰ã‚Œã?¦ã?¾ã?™: (%d in %d bytes).\n"
#define MSGTR_Detected_XXX_FileFormat "%s ãƒ•ã‚¡ã‚¤ãƒ«ãƒ•ã‚©ãƒ¼ãƒžãƒƒãƒˆã?¨åˆ¤æ–­.\n"
#define MSGTR_DetectedAudiofile "éŸ³å£°ãƒ•ã‚¡ã‚¤ãƒ«ã?¨åˆ¤æ–­.\n"
#define MSGTR_FormatNotRecognized "============ ã?“ã?®ãƒ•ã‚¡ã‚¤ãƒ«ãƒ•ã‚©ãƒ¼ãƒžãƒƒãƒˆã?¯ ã‚µãƒ?ãƒ¼ãƒˆã?—ã?¦ã?„ã?¾ã?›ã‚“ =============\n"\
                                  "======= ã‚‚ã?—ã?“ã?®ãƒ•ã‚¡ã‚¤ãƒ«ã?Œ AVIã€?ASFã€?MPEGã?ªã‚‰ä½œæˆ?è€…ã?«é€£çµ¡ã?—ã?¦ä¸‹ã?•ã?„ ======\n"
#define MSGTR_MissingVideoStream "æ˜ åƒ?ã‚¹ãƒˆãƒªãƒ¼ãƒ ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“.\n"
#define MSGTR_MissingAudioStream "éŸ³å£°ã‚¹ãƒˆãƒªãƒ¼ãƒ ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“ -> ç„¡éŸ³å£°ã?«ã?ªã‚Šã?¾ã?™\n"
#define MSGTR_MissingVideoStreamBug "Missing video stream!? ä½œæˆ?è€…ã?«é€£çµ¡ã?—ã?¦ä¸‹ã?•ã?„ã€?æ??ã‚‰ã??ã?“ã‚Œã?¯ãƒ?ã‚°ã?§ã?™ :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: é?¸æŠžã?•ã‚Œã?Ÿ æ˜ åƒ?ã?‹éŸ³å£°ã‚’æ ¼ç´?ã?™ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"

#define MSGTR_NI_Forced "Forced"
#define MSGTR_NI_Detected "Detected"
#define MSGTR_NI_Message "%s NON-INTERLEAVED AVI ãƒ•ã‚¡ã‚¤ãƒ« ãƒ•ã‚©ãƒ¼ãƒžãƒƒãƒˆ.\n"

#define MSGTR_CantSeekRawAVI "Cannot seek in raw AVI streams. (Indexã?Œå¿…è¦?ã?§ã?™, -idx ã‚’è©¦ã?—ã?¦ä¸‹ã?•ã?„.)\n"
#define MSGTR_CantSeekFile "ã?“ã?®ãƒ•ã‚¡ã‚¤ãƒ«ã?¯ã‚·ãƒ¼ã‚¯ã?™ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"

#define MSGTR_MOVcomprhdr "MOV: åœ§ç¸®ã?•ã‚Œã?Ÿãƒ˜ãƒƒãƒ€(Compressed headers)ã‚’ã‚µãƒ?ãƒ¼ãƒˆã?™ã‚‹ã?«ã?¯ ZLIB ã?Œå¿…è¦?ã?§ã?™\n"
#define MSGTR_MOVvariableFourCC "MOV: è­¦å‘Š: Variable FOURCC detected!?\n"
#define MSGTR_MOVtooManyTrk "MOV: è­¦å‘Š: too many tracks"
#define MSGTR_DetectedTV "TV detected! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "ogg demuxer ã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"
#define MSGTR_CannotOpenAudioStream "éŸ³å£°ã‚¹ãƒˆãƒªãƒ¼ãƒ ã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«ã‚¹ãƒˆãƒªãƒ¼ãƒ ã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "audio demuxerã‚’é–‹ã??ã?“ã?¨é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "subtitle demuxerã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“: %s\n"
#define MSGTR_TVInputNotSeekable "TVå…¥åŠ›ã?¯ã‚·ãƒ¼ã‚¯ã?™ã‚‹ã?“ã?¨ã?¯å‡ºæ?¥ã?¾ã?›ã‚“(ã‚·ãƒ¼ã‚¯ã?¯æ??ã‚‰ã??ãƒ?ãƒ£ãƒ³ãƒ?ãƒ«é?¸æŠžã?«ç›¸å½“ã?™ã‚‹ã?®ã?§ã?¯? ;)\n"
#define MSGTR_ClipInfo "ã‚¯ãƒªãƒƒãƒ—æƒ…å ±:\n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: 30fps NTSC ã‚³ãƒ³ãƒ†ãƒ³ãƒ„æ¤œå‡º, ãƒ•ãƒ¬ãƒ¼ãƒ ãƒ¬ãƒ¼ãƒˆå¤‰æ›´ä¸­.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: 24fps ãƒ—ãƒ­ã‚°ãƒ¬ãƒƒã‚·ãƒ– NTSC ã‚³ãƒ³ãƒ†ãƒ³ãƒ„æ¤œå‡º, ãƒ•ãƒ¬ãƒ¼ãƒ ãƒ¬ãƒ¼ãƒˆå¤‰æ›´ä¸­.\n"


// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"
#define MSGTR_CantCloseCodec "ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã‚’é–‰ã?˜ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"

#define MSGTR_MissingDLLcodec "ã‚¨ãƒ©ãƒ¼: è¦?æ±‚ã?•ã‚Œã?Ÿ DirectShow ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ %s ã‚’é–‹ã??ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“.\n"
#define MSGTR_ACMiniterror "Win32/ACMéŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã?®èª­ã?¿è¾¼ã?¿å?Šã?³åˆ?æœŸåŒ–ã‚’ã?™ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ (DLLãƒ•ã‚¡ã‚¤ãƒ«ã?¯å¤§ä¸ˆå¤«ã?§ã?™ã?‹?).\n"
#define MSGTR_MissingLAVCcodec "'%s' ã‚’ libavcodecã?‹ã‚‰è¦‹ä»˜ã?‘ã‚‹ã?“ã?¨ã?Œå‡ºæ?¥ã?¾ã?›ã‚“ ...\n"

#define MSGTR_CannotReadMpegSequHdr "FATAL: ã‚·ãƒ¼ã‚±ãƒ³ã‚¹ãƒ˜ãƒƒãƒ€(sequence header)ã‚’èª­ã?¿è¾¼ã‚?ã?¾ã?›ã‚“.\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: æ‹¡å¼µã‚·ãƒ¼ã‚±ãƒ³ã‚¹ãƒ˜ãƒƒãƒ€(sequence header extension)ã‚’èª­ã?¿è¾¼ã‚?ã?¾ã?›ã‚“.\n"
#define MSGTR_BadMpegSequHdr "MPEG: ä¸?æ­£ã?ªã‚·ãƒ¼ã‚±ãƒ³ã‚¹ãƒ˜ãƒƒãƒ€(sequence header)\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: ä¸?æ­£ã?ªæ‹¡å¼µã‚·ãƒ¼ã‚±ãƒ³ã‚¹ãƒ˜ãƒƒãƒ€(sequence header)\n"

#define MSGTR_ShMemAllocFail "å…±æœ‰ãƒ¡ãƒ¢ãƒªã?®ç¢ºä¿?ã?«å¤±æ•—\n"
#define MSGTR_CantAllocAudioBuf "éŸ³å£°å‡ºåŠ›ãƒ?ãƒƒãƒ•ã‚¡ãƒ¼ã?®ç¢ºä¿?ã?«å¤±æ•—\n"

#define MSGTR_UnknownAudio "æœªçŸ¥ã?®ã€?ã‚‚ã?—ã??ã?¯å£Šã‚Œã?ŸéŸ³å£°ãƒ•ã‚©ãƒ¼ãƒžãƒƒãƒˆã?§ã?™ -> ç„¡éŸ³å£°ã?«ã?ªã‚Šã?¾ã?™\n"

#define MSGTR_VideoCodecFamilyNotAvailableStr "è¦?æ±‚ã?•ã‚Œã?Ÿæ˜ åƒ?ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ [%s] (vfm=%s) ã?¯ç„¡åŠ¹ã?§ã?™ (æœ‰åŠ¹ã?«ã?™ã‚‹ã?«ã?¯ã‚³ãƒ³ãƒ‘ã‚¤ãƒ«æ™‚ã?«æŒ‡å®šã?—ã?¾ã?™)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "è¦?æ±‚ã?•ã‚Œã?ŸéŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ [%s] (afm=%s) ã?¯ç„¡åŠ¹ã?§ã?™ (æœ‰åŠ¹ã?«ã?™ã‚‹ã?«ã?¯ã‚³ãƒ³ãƒ‘ã‚¤ãƒ«æ™‚ã?«æŒ‡å®šã?—ã?¾ã?™)\n"
#define MSGTR_OpeningVideoDecoder "æ˜ åƒ?ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã‚’é–‹ã?„ã?¦ã?„ã?¾ã?™: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "éŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ã‚’é–‹ã?„ã?¦ã?„ã?¾ã?™: [%s] %s\n"
#define MSGTR_VDecoderInitFailed "æ˜ åƒ?ãƒ‡ã‚³ãƒ¼ãƒ€ã?®åˆ?æœŸåŒ–ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ :(\n"
#define MSGTR_ADecoderInitFailed "éŸ³å£°ãƒ‡ã‚³ãƒ¼ãƒ€ã?®åˆ?æœŸåŒ–ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ :(\n"
#define MSGTR_ADecoderPreinitFailed "éŸ³å£°ãƒ‡ã‚³ãƒ¼ãƒ€ã?®å‰?å‡¦ç?†ã?«å¤±æ•— :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "LIRC ã‚µãƒ?ãƒ¼ãƒˆã‚’é–‹ã??äº‹ã?«å¤±æ•—.\n"
#define MSGTR_LIRCcfgerr "LIRC è¨­å®šãƒ•ã‚¡ã‚¤ãƒ« %s ã‚’é–‹ã??ã?“ã?¨ã?«å¤±æ•—ã?—ã?¾ã?—ã?Ÿ.\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "æ˜ åƒ?ãƒ•ã‚£ãƒ«ã‚¿ '%s' ã?Œè¦‹ä»˜ã?‹ã‚Šã?¾ã?›ã‚“\n"
#define MSGTR_CouldNotOpenVideoFilter "éŸ³å£°ãƒ•ã‚£ãƒ«ã‚¿ '%s' ã?Œè¦‹ä»˜ã?‹ã‚Šã?¾ã?›ã‚“\n"
#define MSGTR_OpeningVideoFilter "æ˜ åƒ?ãƒ•ã‚£ãƒ«ã‚¿ã‚’é–‹ã?„ã?¦ã?„ã?¾ã?™: "
#define MSGTR_CannotFindColorspace "common colorspaceã?Œè¦‹ä»˜ã?‹ã‚Šã?¾ã?›ã‚“, even by inserting 'scale' :(\n"

// vd.c

// ====================== GUI messages/buttons ========================

#ifdef CONFIG_GUI

// --- labels ---
#define MSGTR_About "ã‚¢ãƒ?ã‚¦ãƒˆ"
#define MSGTR_FileSelect "ãƒ•ã‚¡ã‚¤ãƒ«é?¸æŠž ..."
#define MSGTR_SubtitleSelect "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«é?¸æŠž ..."
#define MSGTR_OtherSelect "é?¸æŠž ..."
#define MSGTR_FontSelect "ãƒ•ã‚©ãƒ³ãƒˆé?¸æŠž ..."
#define MSGTR_PlayList "ãƒ—ãƒ¬ã‚¤ãƒªã‚¹ãƒˆ"
#define MSGTR_Equalizer "ã‚¨ã‚³ãƒ©ã‚¤ã‚¶ãƒ¼"
#define MSGTR_SkinBrowser "ã‚¹ã‚­ãƒ³ãƒ–ãƒ©ã‚¦ã‚¶"
#define MSGTR_Preferences "è¨­å®š"
#define MSGTR_NoMediaOpened "ãƒ¡ãƒ‡ã‚£ã‚¢ã?Œé–‹ã?‹ã‚Œã?¦ã?„ã?¾ã?›ã‚“."
#define MSGTR_NoChapter "ã‚­ãƒ£ãƒ—ã‚¿ãƒ¼ã?Œã?‚ã‚Šã?¾ã?›ã‚“"
#define MSGTR_Chapter "ã‚­ãƒ£ãƒ—ã‚¿ãƒ¼ %d"
#define MSGTR_NoFileLoaded "ãƒ•ã‚¡ã‚¤ãƒ«ã?Œèª­ã?¿è¾¼ã?¾ã‚Œã?¦ã?„ã?¾ã?›ã‚“."

// --- buttons ---
#define MSGTR_Ok "OK"
#define MSGTR_Cancel "ã‚­ãƒ£ãƒ³ã‚»ãƒ«"
#define MSGTR_Add "è¿½åŠ "
#define MSGTR_Remove "å‰Šé™¤"
#define MSGTR_Clear "ã‚¯ãƒªã‚¢"
#define MSGTR_Config "è¨­å®š"
#define MSGTR_ConfigDriver "ãƒ‰ãƒ©ã‚¤ãƒ?è¨­å®š"
#define MSGTR_Browse "ãƒ–ãƒ©ã‚¦ã‚º"

// --- error messages ---
#define MSGTR_NEMDB "æ??ç”»ã?«å¿…è¦?ã?ªãƒ?ãƒƒãƒ•ã‚¡ã‚’ç¢ºä¿?ã?™ã‚‹ã?Ÿã‚?ã?®ãƒ¡ãƒ¢ãƒªã?Œè¶³ã‚Šã?¾ã?›ã‚“."
#define MSGTR_NEMFMR "ãƒ¡ãƒ‹ãƒ¥ãƒ¼ã‚’æ??ç”»ã?«å¿…è¦?ã?ªãƒ¡ãƒ¢ãƒªã?Œè¶³ã‚Šã?¾ã?›ã‚“."

// --- skin loader error messages
#define MSGTR_SKIN_ERRORMESSAGE "[skin] ã‚¨ãƒ©ãƒ¼: ã‚¹ã‚­ãƒ³è¨­å®šãƒ•ã‚¡ã‚¤ãƒ« %d è¡Œ: %s"
#define MSGTR_SKIN_BITMAP_16bit  "16 ãƒ“ãƒƒãƒˆä»¥ä¸‹ã?®è§£æ¶ˆåº¦ã?¯ã‚µãƒ?ãƒ¼ãƒˆã?•ã‚Œã?¦ã?„ã?¾ã?›ã‚“ (%s).\n"
#define MSGTR_SKIN_BITMAP_FileNotFound  "ãƒ•ã‚¡ã‚¤ãƒ«ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“ (%s)\n"
#define MSGTR_SKIN_BITMAP_PNGReadError "png èª­ã?¿è¾¼ã?¿ã‚¨ãƒ©ãƒ¼ (%s)\n"
#define MSGTR_SKIN_BITMAP_ConversionError "24bitã?‹ã‚‰32bitã?¸ã?®å¤‰æ?›ã‚¨ãƒ©ãƒ¼ (%s)\n"
#define MSGTR_SKIN_UnknownMessage "æœªçŸ¥ã?®ãƒ¡ãƒƒã‚»ãƒ¼ã‚¸: %s\n"
#define MSGTR_SKIN_NotEnoughMemory "ãƒ¡ãƒ¢ãƒªã?Œä¸?è¶³ã?—ã?¦ã?„ã?¾ã?™\n"
#define MSGTR_SKIN_FONT_FontFileNotFound "ãƒ•ã‚©ãƒ³ãƒˆãƒ•ã‚¡ã‚¤ãƒ«ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“\n"
#define MSGTR_SKIN_FONT_FontImageNotFound "ãƒ•ã‚©ãƒ³ãƒˆã‚¤ãƒ¡ãƒ¼ã‚¸ãƒ•ã‚¡ã‚¤ãƒ«ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“\n"
#define MSGTR_SKIN_UnknownParameter "æœªçŸ¥ã?®ãƒ‘ãƒ©ãƒ¡ãƒ¼ã‚¿(%s)\n"
#define MSGTR_SKIN_SKINCFG_SkinNotFound "ã‚¹ã‚­ãƒ³ã?Œå­˜åœ¨ã?—ã?¾ã?›ã‚“( %s ).\n"
#define MSGTR_SKIN_LABEL "ã‚¹ã‚­ãƒ³:"

// --- gtk menus
#define MSGTR_MENU_AboutMPlayer "MPlayerã?«ã?¤ã?„ã?¦"
#define MSGTR_MENU_Open "é–‹ã?? ..."
#define MSGTR_MENU_PlayFile "ãƒ•ã‚¡ã‚¤ãƒ«å†?ç”Ÿ ..."
#define MSGTR_MENU_PlayVCD "VCD å†?ç”Ÿ ..."
#define MSGTR_MENU_PlayDVD "DVD å†?ç”Ÿ ..."
#define MSGTR_MENU_PlayURL "URL å†?ç”Ÿ ..."
#define MSGTR_MENU_LoadSubtitle "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«èª­ã?¿è¾¼ã?¿ ..."
#define MSGTR_MENU_DropSubtitle "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«ç ´æ£„ ..."
#define MSGTR_MENU_Playing "ç?¾åœ¨ã?®ãƒ•ã‚¡ã‚¤ãƒ«"
#define MSGTR_MENU_Play "å†?ç”Ÿ"
#define MSGTR_MENU_Pause "ä¸€æ™‚å?œæ­¢"
#define MSGTR_MENU_Stop "å?œæ­¢"
#define MSGTR_MENU_NextStream "æ¬¡ã?®ã‚¹ãƒˆãƒªãƒ¼ãƒ "
#define MSGTR_MENU_PrevStream "å‰?ã?®ã‚¹ãƒˆãƒªãƒ¼ãƒ "
#define MSGTR_MENU_Size "ã‚µã‚¤ã‚º"
#define MSGTR_MENU_NormalSize "é€šå¸¸ã‚µã‚¤ã‚º"
#define MSGTR_MENU_DoubleSize "2å€?ã‚µã‚¤ã‚º"
#define MSGTR_MENU_FullScreen "ãƒ•ãƒ«ã‚¹ã‚¯ãƒªãƒ¼ãƒ³"
#define MSGTR_MENU_DVD "DVD"
#define MSGTR_MENU_VCD "VCD"
#define MSGTR_MENU_PlayDisc "ãƒ‡ã‚£ã‚¹ã‚¯å†?ç”Ÿ ..."
#define MSGTR_MENU_ShowDVDMenu "DVD ãƒ¡ãƒ‹ãƒ¥ãƒ¼ã?®è¡¨ç¤º"
#define MSGTR_MENU_Titles "ã‚¿ã‚¤ãƒˆãƒ«"
#define MSGTR_MENU_Title "ã‚¿ã‚¤ãƒˆãƒ« %2d"
#define MSGTR_MENU_None "(ç„¡ã?—)"
#define MSGTR_MENU_AudioLanguages "éŸ³å£°è¨€èªž"
#define MSGTR_MENU_SubtitleLanguages "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«è¨€èªž"
#define MSGTR_MENU_SkinBrowser "ã‚¹ã‚­ãƒ³ãƒ–ãƒ©ã‚¦ã‚¶"
#define MSGTR_MENU_Exit "çµ‚äº† "
#define MSGTR_MENU_Mute "æ¶ˆéŸ³"
#define MSGTR_MENU_Original "ã‚ªãƒªã‚¸ãƒŠãƒ«"
#define MSGTR_MENU_Track "ãƒˆãƒ©ãƒƒã‚¯ %d"
#define MSGTR_MENU_VideoTrack "æ˜ åƒ?ãƒˆãƒ©ãƒƒã‚¯"

// --- equalizer
#define MSGTR_EQU_Audio "éŸ³å£°"
#define MSGTR_EQU_Video "æ˜ åƒ?"
#define MSGTR_EQU_Contrast "æ˜Žæš—: "
#define MSGTR_EQU_Brightness "å…‰åº¦: "
#define MSGTR_EQU_Front_Left "å‰?æ–¹ å·¦"
#define MSGTR_EQU_Front_Right "å‰?æ–¹ å?³"
#define MSGTR_EQU_Back_Left "å¾Œæ–¹ å·¦"
#define MSGTR_EQU_Back_Right "å¾Œæ–¹ å?³"
#define MSGTR_EQU_Center "ä¸­å¤®"
#define MSGTR_EQU_Bass "ãƒ?ã‚¹"
#define MSGTR_EQU_All "All"
#define MSGTR_EQU_Channel1 "ãƒ?ãƒ£ãƒ³ãƒ?ãƒ« 1:"
#define MSGTR_EQU_Channel2 "ãƒ?ãƒ£ãƒ³ãƒ?ãƒ« 2:"
#define MSGTR_EQU_Channel3 "ãƒ?ãƒ£ãƒ³ãƒ?ãƒ« 3:"
#define MSGTR_EQU_Channel4 "ãƒ?ãƒ£ãƒ³ãƒ?ãƒ« 4:"
#define MSGTR_EQU_Channel5 "ãƒ?ãƒ£ãƒ³ãƒ?ãƒ« 5:"
#define MSGTR_EQU_Channel6 "ãƒ?ãƒ£ãƒ³ãƒ?ãƒ« 6:"

// --- playlist
#define MSGTR_PLAYLIST_Path "ãƒ‘ã‚¹"
#define MSGTR_PLAYLIST_Selected "é?¸æŠžã?•ã‚Œã?Ÿãƒ•ã‚¡ã‚¤ãƒ«"
#define MSGTR_PLAYLIST_Files "ãƒ•ã‚¡ã‚¤ãƒ«"
#define MSGTR_PLAYLIST_DirectoryTree "ãƒ‡ã‚£ãƒ¬ã‚¯ãƒˆãƒªãƒ„ãƒªãƒ¼"

// --- preferences
#define MSGTR_PREFERENCES_SubtitleOSD "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ« & OSD"
#define MSGTR_PREFERENCES_Codecs "ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯ & demuxer"
#define MSGTR_PREFERENCES_Misc "Misc"

#define MSGTR_PREFERENCES_None "ç„¡ã?—"
#define MSGTR_PREFERENCES_AvailableDrivers "æœ‰åŠ¹ã?ªãƒ‰ãƒ©ã‚¤ãƒ?:"
#define MSGTR_PREFERENCES_EnableEqualizer "ã‚¤ã‚³ãƒ©ã‚¤ã‚¶ãƒ¼ã?®æœ‰åŠ¹"
#define MSGTR_PREFERENCES_DoubleBuffer "double buffering æœ‰åŠ¹"
#define MSGTR_PREFERENCES_DirectRender "direct rendering æœ‰åŠ¹"
#define MSGTR_PREFERENCES_FrameDrop "frame dropping æœ‰åŠ¹"
#define MSGTR_PREFERENCES_HFrameDrop "HARD frame dropping (å?±é™ºã?§ã?™) æœ‰åŠ¹"
#define MSGTR_PREFERENCES_Subtitle "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«:"
#define MSGTR_PREFERENCES_SUB_Delay "Delay: "
#define MSGTR_PREFERENCES_SUB_FPS "FPS:"
#define MSGTR_PREFERENCES_SUB_POS "ä½?ç½®: "
#define MSGTR_PREFERENCES_SUB_AutoLoad "subtitle è‡ªå‹•èª­ã?¿è¾¼ã?¿ç„¡åŠ¹"
#define MSGTR_PREFERENCES_SUB_Unicode "Unicode ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«"
#define MSGTR_PREFERENCES_SUB_MPSUB "ä¸Žã?ˆã‚‰ã‚Œã?Ÿã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«ã‚’MPLayerã?®ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«ãƒ•ã‚©ãƒ¼ãƒžãƒƒãƒˆã?«å¤‰æ?›"
#define MSGTR_PREFERENCES_Font "ãƒ•ã‚©ãƒ³ãƒˆ:"
#define MSGTR_PREFERENCES_PostProcess "postprocessing æœ‰åŠ¹"
#define MSGTR_PREFERENCES_NI "non-interleaved AVI ãƒ‘ãƒ¼ã‚µä½¿ç”¨"
#define MSGTR_PREFERENCES_IDX "å¿…è¦?ã?ªã‚‰index tableã?®å†?æ§‹ç¯‰"
#define MSGTR_PREFERENCES_VideoCodecFamily "æ˜ åƒ?ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯:"
#define MSGTR_PREFERENCES_AudioCodecFamily "éŸ³å£°ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯:"
#define MSGTR_PREFERENCES_FRAME_OSD_Level "OSD ãƒ¬ãƒ™ãƒ«"
#define MSGTR_PREFERENCES_FRAME_Subtitle "ã‚µãƒ–ã‚¿ã‚¤ãƒˆãƒ«"
#define MSGTR_PREFERENCES_FRAME_Font "ãƒ•ã‚©ãƒ³ãƒˆ"
#define MSGTR_PREFERENCES_FRAME_CodecDemuxer "ã‚³ãƒ¼ãƒ‡ãƒƒã‚¯& demuxer"
#define MSGTR_PREFERENCES_FRAME_Cache "ã‚­ãƒ£ãƒƒã‚·ãƒ¥"
#define MSGTR_PREFERENCES_DXR3_VENC "Video ã‚¨ãƒ³ã‚³ãƒ¼ãƒ€:"
#define MSGTR_PREFERENCES_DXR3_LAVC "LAVC (FFmpeg)ä½¿ç”¨"
#define MSGTR_PREFERENCES_FontEncoding1 "ãƒ¦ãƒ‹ã‚³ãƒ¼ãƒ‰"
#define MSGTR_PREFERENCES_FontEncoding "ã‚¨ãƒ³ã‚³ãƒ¼ãƒ‡ã‚£ãƒ³ã‚°:"
#define MSGTR_PREFERENCES_Cache "ã‚­ãƒ£ãƒƒã‚·ãƒ¥ on/off"
#define MSGTR_PREFERENCES_CacheSize "ã‚­ãƒ£ãƒƒã‚·ãƒ¥ã‚µã‚¤ã‚º: "
#define MSGTR_PREFERENCES_LoadFullscreen "é–‹å§‹æ™‚ã?«ãƒ•ãƒ«ã‚¹ã‚¯ãƒªãƒ¼ãƒ³"
#define MSGTR_PREFERENCES_SaveWinPos "ã‚¦ã‚£ãƒ³ãƒ‰ã‚¦ä½?ç½®ã‚’ä¿?æŒ?"
#define MSGTR_PREFERENCES_XSCREENSAVER "XScreenSaverã‚’ã‚¹ãƒˆãƒƒãƒ—"
#define MSGTR_PREFERENCES_PlayBar "ãƒ—ãƒ¬ã‚¤ãƒ?ãƒ¼æœ‰åŠ¹"
#define MSGTR_PREFERENCES_CDROMDevice "CD-ROM ãƒ‡ãƒ?ã‚¤ã‚¹:"
#define MSGTR_PREFERENCES_DVDDevice "DVD ãƒ‡ãƒ?ã‚¤ã‚¹:"
#define MSGTR_PREFERENCES_FPS "Movie FPS:"

// --- messagebox
#define MSGTR_MSGBOX_LABEL_FatalError "è‡´å‘½çš„ã‚¨ãƒ©ãƒ¼!"
#define MSGTR_MSGBOX_LABEL_Error "ã‚¨ãƒ©ãƒ¼"
#define MSGTR_MSGBOX_LABEL_Warning "è­¦å‘Š"

#endif
