// sync'ed with help_mp-en.h 1.121
// Translated by: Ioannis Panteleakis <pioann@csd.auth.gr>
// Various corrections and additions by: Haris Kouzinopoulos <ironhell3@hotmail.com>

// ========================= MPlayer help ===========================

static const char help_text[]=
"Usage:   mplayer [επιλογές] [url|διαδ�?ομή/]όνομα_α�?χείου\n"
"\n"
"Βασικές επιλογές: (Ανατ�?έξτε στη  σελίδα εγχει�?ιδίου για ολοκλη�?ωμένη λίστα με επιλογές)\n"
" -vo <drv[:dev]> επιλέξτε τον οδηγό εξόδου βίντεο και τη συσκευή (βλέπε '-vo help' για τη λίστα)\n"
" -ao <drv[:dev]> επιλέξτε τον οδηγό εξόδου ήχου και τη συσκευή (βλέπε '-ao help' για τη λίστα)\n"
#ifdef CONFIG_VCD
" vcd://<α�?ιθμός track>  αναπα�?αγωγή track VCD (video cd)  από συσκευή αντί για α�?χείο\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>  αναπα�?αγωγή του τίτλου/track DVD από τη συσκευή αντί για α�?χείο\n"
" -alang/-slang   επιλογή της γλώσσας του ήχου/υποτίτλων του DVD (2 χα�?ακτή�?ες του κωδικο�? της χώ�?ας)\n"
#endif
" -ss <timepos>   αναζήτηση σε δεδομένη θέση (δευτε�?όλεπτα ή ωω:λλ:δδ)\n"
" -nosound        μη αναπα�?αγωγή του ήχου\n"
" -fs   επιλογές για αναπα�?αγωγή σε πλή�?η οθόνη (ή -vm -zoom, δείτε man page για πε�?ισσότε�?ες πλη�?οφο�?ίες)\n"
" -x <x> -y <y>   κλιμάκωση εικόνας σε <x> * <y> αναλ�?σεις [αν ο -vo οδηγός το υποστη�?ίζει!]\n"
" -sub <α�?χείο>   επιλογή του α�?χείου υποτίτλων για χ�?ήση (βλέπε επίσης -subfps, -subdelay)\n"
" -playlist <α�?χείο> ο�?ίζει το α�?χείο της λίστας αναπα�?αγωγής\n"
" -vid x -aid y   επιλογή καναλιο�? βίντεο (x) και ήχου (y) για αναπα�?αγωγή\n"
" -fps x -srate y επιλογή  συχνότητας του βίντεο (x fps) και  ήχου (y Hz)\n"
" -pp <ποιότητα>  ενε�?γοποίηση του φίλτ�?ου postprocessing (0-4 για DivX, 0-63 για mpeg)\n"
" -framedrop      ενε�?γοποίηση του frame-dropping (για α�?γά μηχανήματα)\n"
"\n"
"Βασικά πλήκτ�?α: (Ανατ�?έξτε στη  σελίδα εγχει�?ιδίου για ολοκλη�?ωμένη λίστα , καθώς επίσης και  στο α�?χείο input.conf)\n"
" <-  ή  ->      αναζήτηση μπ�?ος/πίσω κατά 10 δευτε�?όλεπτα\n"
" up ή down      αναζήτηση μπ�?ος/πίσω κατά 1 λεπτό\n"
" pgup ή pgdown  αναζήτηση μπ�?ος/πίσω κατά 10 λεπτά\n"
" < ή >          αναζήτηση μπ�?ος/πίσω στην λίστα αναπα�?αγωγής\n"
" p ή SPACE      πα�?ση ταινίας (πατήστε οποιοδήποτε πλήκτ�?ο για να συνεχίσετε)\n"
" q ή ESC        στοπ την αναπα�?αγωγή και έξοδος π�?ογ�?άμματος\n"
" + ή -          �?�?θμιση καθυστέ�?ησης ήχου κατά +/- 0.1 δευτε�?όλεπτα\n"
" o               αλλαγή της OSD μεθόδου:  τίποτα / μπά�?α π�?οόδου / μπά�?α π�?οόδου+χ�?όνος\n"
" * ή /          α�?ξηση ή μείωση της έντασης του ήχου (πατήστε 'm' για επιλογή master/pcm)\n"
" z ή x          �?�?θμιση καθυστέ�?ησης υποτίτλων κατά +/- 0.1 δευτε�?όλεπτα\n"
" r ή t          �?�?θμιση της θέσης των υποτίτλων πάνω/κάτω, βλέπε επίσης -vf expand!\n"
"\n"
" Ανατ�?έξτε στη σελίδα εγχει�?ιδίου για πε�?ισσότε�?ες λεπτομέ�?ειες, πιο π�?οχω�?ημένες επιλογές και  λίστα με πλήκτ�?α \n"
"\n";

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\n Έξοδος...\n"
#define MSGTR_ExitingHow "\n Έξοδος... (%s)\n"
#define MSGTR_Exit_quit "Κλείσιμο"
#define MSGTR_Exit_eof "Τέλος του α�?χείου"
#define MSGTR_Exit_error "Κ�?ίσιμο σφάλμα"
#define MSGTR_IntBySignal "\n Το MPlayer τε�?ματίστηκε από το σήμα %d στο module: %s \n"
#define MSGTR_NoHomeDir "Μη δυνατή η ε�?�?εση του HOME φακέλου\n"
#define MSGTR_GetpathProblem "get_path(\"config\") π�?όβλημα\n"
#define MSGTR_CreatingCfgFile "Δημιου�?γία του α�?χείου config: %s\n"
#define MSGTR_CantLoadFont "Μη δυνατότητα φό�?τωσης της γ�?αμματοσει�?άς: %s\n"
#define MSGTR_CantLoadSub "Μη δυνατότητα φό�?τωσης των υποτίτλων: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: Σφάλμα: λείπει το επιλεγμένο κανάλι!\n"
#define MSGTR_CantOpenDumpfile "Αδ�?νατο το άνοιγμα του dump α�?χείου!!!\n"
#define MSGTR_CoreDumped "core dumped :)\n"
#define MSGTR_FPSnotspecified "Μη ο�?ισμένα FPS (ή λάθος) στην επικεφαλίδα! Χ�?ησιμοποιήστε την επιλογή -fps!\n"
#define MSGTR_TryForceAudioFmtStr "Π�?οσπάθεια  επιβολής της οικογένειας του οδηγο�? του ήχου %s...\n"
#define MSGTR_CantFindAudioCodec "Δεν είναι δυνατή η ε�?�?εση του format του οδηγο�? του ήχου 0x%X!\n"
#define MSGTR_TryForceVideoFmtStr "Π�?οσπάθεια  επιβολής της οικογένειας του οδηγο�? του βίντεο %s...\n"
#define MSGTR_CantFindVideoCodec "Δεν είναι δυνατή η ε�?�?εση του οδηγο�?  για τον συγκεκ�?ιμένο -vo και το format του βίντεο 0x%X!\n"
#define MSGTR_VOincompCodec "Λυπάμαι, η επιλεγμένη συσκευή video_out δεν είναι συμβατή με αυτό τον οδηγό.\n"
#define MSGTR_CannotInitVO "Σφάλμα: Αδ�?νατη η α�?χικοποίηση του οδηγο�? του βίντεο!\n"
#define MSGTR_CannotInitAO "Αδ�?νατο το άνοιγμα/α�?χικοποίηση του οδηγο�? του ήχου -> ΧΩΡΙΣ-ΗΧΟ\n"
#define MSGTR_StartPlaying "Εκκίνηση αναπα�?αγωγής...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"         **************************************************************************\n"\
"         **** Το σ�?στημά σας είναι πολ�? α�?γό για την αναπα�?αγωγή του α�?χείου!  ****\n"\
"         **************************************************************************\n\n"\
"Πιθανές αιτίες, π�?οβλήματα, λ�?σεις: \n"\
"- Συνήθη αιτία: π�?όβλημα με τον οδηγό του ήχου\n"\
"  - Δοκιμάστε -ao sdl ή χ�?ησιμοποιήστε ALSA 0.5 ή oss π�?οσομοίωση του οδηγο�? ALSA 0.9.\n"\
"  - Μπο�?είτε επίσης να πει�?αματιστείτε με διάφο�?ες τιμές του -autosync, η τιμή  30 είναι μια καλή α�?χή.\n"\
"- Α�?γή έξοδος του βίντεο\n"\
"  - Δοκιμάστε διαφο�?ετικό -vo οδηγό (για λίστα: -vo help) ή δοκιμάστε με -framedrop\n"\
"- Α�?γός επεξε�?γαστής\n"\
"  - Μην αναπα�?άγετε μεγάλα DVD/DivX α�?χεία σε α�?γο�?ς επεξε�?γαστές! Δοκιμάστε με -hardframedrop\n"\
"- Π�?οβληματικό α�?χείο\n"\
"  - Δοκιμάστε με διάφο�?ους συνδυασμο�?ς από τους πα�?ακάτω: -nobps  -ni  -mc 0  -forceidx\n"\
"- Α�?γά μέσα αναπα�?αγωγή (NFS/SMB mounts, DVD, VCD κτλ) \n"\
"  - Δοκιμάστε -cache 8192\n"\
"- Μήπως χ�?ησιμοποιείται -cache για την αναπα�?αγωγή ενός non-interleaved α�?χείου;\n"\
"  - Δοκιμάστε με -nocache\n"\
"Διαβάστε το DOCS/HTML/en/video.html για �?�?θμιση/επιτάχυνση του βίντεο.\n"\
"Αν κανένα από αυτά δεν βοηθάει, τότε διαβάστε το DOCS/HTML/en/bugreports.html!\n\n"

#define MSGTR_NoGui "Το MPlayer μεταφ�?άστηκε ΧΩΡΙΣ υποστή�?ιξη για GUI!\n"
#define MSGTR_GuiNeedsX "Το GUI του MPlayer χ�?ειάζεται X11!\n"
#define MSGTR_Playing "Αναπα�?αγωγή του %s\n"
#define MSGTR_NoSound "Ήχος: μη διαθέσιμο!!!\n"
#define MSGTR_FPSforced "Τα FPS �?υθμίστηκαν να είναι %5.3f  (ftime: %5.3f)\n"
#define MSGTR_AvailableVideoOutputDrivers "Διαθέσιμοι οδηγοί για έξοδο βίντεο:\n"
#define MSGTR_AvailableAudioOutputDrivers "Διαθέσιμοι οδηγοί για έξοδο ήχου:\n"
#define MSGTR_AvailableAudioCodecs "Διαθέσιμα codecs ήχου:\n"
#define MSGTR_AvailableVideoCodecs "Διαθέσιμα codecs βίντεο:\n"
#define MSGTR_AvailableAudioFm "\nΔιαθέσιμοι (compiled-in) οδηγοί/οικογένειες codec ήχου:\n"
#define MSGTR_AvailableVideoFm "\nΔιαθέσιμοι (compiled-in) οδηγοί/οικογένειες codec βίντεο:\n"
#define MSGTR_AvailableFsType "Διαθέσιμα επίπεδα αλλαγής σε πλή�?η οθόνη:\n"
#define MSGTR_CannotReadVideoProperties "Βίντεο: αδ�?νατη η ανάγνωση ιδιοτήτων\n"
#define MSGTR_NoStreamFound "Δεν β�?έθηκε κανάλι\n"
#define MSGTR_ErrorInitializingVODevice "Σφάλμα κατά το άνοιγμα/α�?χικοποίηση της επιλεγμένης video_out (-vo) συσκευή!\n"
#define MSGTR_ForcedVideoCodec "Εξαναγκασμός χ�?ήσης του βίντεο codec: %s\n"
#define MSGTR_ForcedAudioCodec "Εξαναγκασμός χ�?ήσης του codec ήχου: %s\n"
#define MSGTR_Video_NoVideo "Βίντεο: δεν υπά�?χει βίντεο!!!\n"
#define MSGTR_NotInitializeVOPorVO "\n Σφάλμα: Αδ�?νατη η α�?χικοποίηση του φίλτ�?ου βίντεο (-vf) ή της εξόδου βίντεο (-vo)!\n"
#define MSGTR_Paused "  =====  ΠΑΥΣΗ  ====="
#define MSGTR_PlaylistLoadUnable "\n Αδ�?νατη η φό�?τωση  της λίστας αναπα�?αγωγής %s\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- Το MPlayer κατέ�?�?ευσε από ένα 'Illegal Instruction'.\n"\
"  Μπο�?εί να είναι π�?όβλημα στον νέο κώδικα για runtime CPU-αναγνώ�?ιση...\n"\
"  Πα�?ακαλο�?με διαβάστε το DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- Το MPlayer κατέ�?�?ευσε από ένα 'Illegal Instruction'.\n"\
"  Συνήθως συμβαίνει όταν τ�?έχετε το π�?όγ�?αμμα σε διαφο�?ετικό επεξε�?γαστή από αυτόν στον οποίο έγινε\n"\
"  η μεταγλώττιση/βελτιστοποίηση.\n"\
"  Ελέγξτε το!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- Το MPlayer κατέ�?�?ευσε  από κακή χ�?ήση του επεξε�?γαστή ή της μνήμης.\n"\
"  Αναμεταγλωττίστε το MPlayer με --enable-debug και τ�?έξτε 'gdb' backtrace και\n"\
"  disassembly. Για λεπτομέ�?ειες, δείτε το DOCS/HTML/en/bugreports_what.html#bugreports_crash\n"
#define MSGTR_Exit_SIGCRASH \
"- Το MPlayer κατέ�?�?ευσε. Αυτό δεν θα έπ�?επε να είχε συμβεί.\n"\
"  Μπο�?εί να είναι ένα π�?όβλημα στον κώδικα του MPlayer _ή_ στους οδηγο�?ς σας _ή_ στην έκδοση\n"\
"  του gcc σας. Αν νομίζετε ότι φταίει το MPlayer, πα�?ακαλώ διαβάστε το DOCS/HTML/en/bugreports.html\n"\
"  και ακολουθήστε της οδηγίες. Δεν μπο�?ο�?με και δεν θα π�?οσφέ�?ουμε βοήθεια εκτός και αν στείλετε\n"\
"  τις πλη�?οφο�?ίες όταν αναφέ�?ετε το π�?όβλημα.\n"


// mencoder.c:

#define MSGTR_UsingPass3ControlFile "Χ�?ήση του α�?χείου ελέγχου pass3: %s\n"
#define MSGTR_MissingFilename "\n Πα�?άλειψη ονόματος α�?χείου!\n\n"
#define MSGTR_CannotOpenFile_Device "Αδ�?νατο το άνοιγμα του α�?χείου/συσκευή\n"
#define MSGTR_CannotOpenDemuxer "Αδ�?νατο το άνοιγμα του demuxer\n"
#define MSGTR_NoAudioEncoderSelected "\n Δεν επιλέχτηκε κωδικοποιητής ήχου (-oac)! Επιλέξτε έναν ή χ�?ησιμοποιήστε -nosound. Χ�?ησιμοποιήστε -oac help!\n"
#define MSGTR_NoVideoEncoderSelected "\n Δεν επιλέχτηκε κωδικοποιητής βίντεο (-ovc)! Επιλέξτε έναν, Χ�?ησιμοποιήστε -ovc help!\n"
#define MSGTR_CannotOpenOutputFile "Αδ�?νατο το άνοιγμα του α�?χείου εξόδου '%s'\n"
#define MSGTR_EncoderOpenFailed "Αποτυχία κατά το άνοιγμα του κωδικοποιητή \n"
#define MSGTR_ForcingOutputFourcc "Εξαναγκασμός χ�?ήσης εξόδου fourcc σε %x [%.4s]\n"
#define MSGTR_DuplicateFrames "\n διπλασιασμός %d κα�?έ!!!    \n"
#define MSGTR_SkipFrame "\n πα�?άλειψη κα�?έ!!!    \n"
#define MSGTR_ErrorWritingFile "%s: σφάλμα εγγ�?αφής α�?χείου.\n"
#define MSGTR_RecommendedVideoBitrate "Π�?οτεινόμενο bitrate του βίντεο για %s CD: %d\n"
#define MSGTR_VideoStreamResult "\n Κανάλι βίντεο: %8.3f kbit/s  (%d B/s)  μέγεθος: %"PRIu64" bytes  %5.3f δευτε�?όλεπτα  %d κα�?έ\n"
#define MSGTR_AudioStreamResult "\n Κανάλι ήχου: %8.3f kbit/s  (%d B/s)  μέγεθος: %"PRIu64" bytes  %5.3f δευτε�?όλεπτα\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     μέθοδος μεταβλητο�? bitrate\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(π�?οεπιλεγμένο)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           μέσο bitrate\n"\
"\n"\
" cbr           σταθε�?ό bitrate\n"\
"               Αναγκάζει την κωδικοποίηση σε CBR mode σε subsequent ABR presets modes\n"\
"\n"\
" br=<0-1024>   ο�?ισμός του bitrate σε kBit (CBR και ABR μόνο)\n"\
"\n"\
" q=<0-9>       ποιότητα (0-υψηλότε�?η, 9-χαμηλότε�?η) (μόνο για VBR)\n"\
"\n"\
" aq=<0-9>      αλγο�?ιθμική ποιότητα (0-καλ�?τε�?ο/α�?γό, 9-χει�?ότε�?ο/γ�?ηγο�?ότε�?ο)\n"\
"\n"\
" ratio=<1-100> αναλογία συμπίεσης\n"\
"\n"\
" vol=<0-10>    ο�?ισμός του audio gain εισόδου\n"\
"\n"\
" mode=<0-3>    (π�?οεπιλεγμένο: auto)\n"\
"                0: stereo\n"\
"                1: joint-stereo\n"\
"                2: dualchannel\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: όχι\n"\
"                1: όλα\n"\
"                2: �?�?θμιση\n"\
"\n"\
" fast          εναλλαγή σε γ�?ηγο�?ότε�?η κωδικοποίηση σε subsequent VBR presets modes,\n"\
"               ελαφ�?ότε�?α χαμηλότε�?η ποιότητα και υψηλότε�?α bitrates.\n"\
"\n"\
" preset=<value> π�?οσφέ�?ει τις υψηλότε�?ες δυνατές επιλογές ποιότητας.\n"\
"                 μεσαία: VBR  κωδικοποίηση, καλή ποιότητα\n"\
"                 (150-180 kbps ε�?�?ος bitrate)\n"\
"                 στάντα�?:  VBR κωδικοποίηση, υψηλή ποιότητα\n"\
"                 (170-210 kbps ε�?�?ος bitrate)\n"\
"                 extreme: VBR κωδικοποίηση, πολ�? υψηλή ποιότητα\n"\
"                 (200-240 kbps ε�?�?ος bitrate)\n"\
"                 insane:  CBR  κωδικοποίηση, υψηλότε�?η preset ποιότητα\n"\
"                 (320 kbps ε�?�?ος bitrate)\n"\
"                 <8-320>: ABR κωδικοποίηση στο μέσο bitrate που δόθηκε σε kbps.\n\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "Η CD-ROM συσκευή '%s' δεν β�?έθηκε!\n"
#define MSGTR_ErrTrackSelect "Σφάλμα στην επιλογή του VCD track!"
#define MSGTR_ReadSTDIN "Διαβάζοντας από το stdin...\n"
#define MSGTR_UnableOpenURL "Αδ�?νατο το άνοιγμα του URL: %s\n"
#define MSGTR_ConnToServer "Π�?αγματοποιήθηκε σ�?νδεση με τον server: %s\n"
#define MSGTR_FileNotFound "Το α�?χείο: '%s' δεν β�?έθηκε\n"

#define MSGTR_SMBInitError "Αδ�?νατη η α�?χικοποίηση της βιβλιοθήκης libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "Δεν μπό�?εσα να ανοίξω από το τοπικό δίκτυο: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer δεν μεταφ�?άστηκε με υποστή�?ιξη ανάγνωσης SMB\n"

#define MSGTR_CantOpenDVD "Δεν μπό�?εσα να ανοίξω την συσκευή DVD: %s (%s)\n"
#define MSGTR_DVDnumTitles "Υπά�?χουν %d τίτλοι στο DVD.\n"
#define MSGTR_DVDinvalidTitle "Άκυ�?ος α�?ιθμός για τον τίτλο του DVD: %d\n"
#define MSGTR_DVDnumChapters "Υπά�?χουν %d κεφάλαια σε αυτόν τον τίτλο του DVD.\n"
#define MSGTR_DVDinvalidChapter "Λάθος α�?ιθμός των κεφαλαίων του DVD: %d\n"
#define MSGTR_DVDnumAngles "Υπά�?χουν %d γωνίες σε αυτό τον τίτλο του DVD.\n"
#define MSGTR_DVDinvalidAngle "Λάθος α�?ιθμός των γωνιών του DVD: %d\n"
#define MSGTR_DVDnoIFO "Δεν είναι δυνατό το άνοιγμα του IFO α�?χείο για τον τίτλο του DVD %d.\n"
#define MSGTR_DVDnoVOBs "Δεν είναι δυνατό το άνοιγμα των VOB (VTS_%02d_1.VOB).\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "Π�?οειδοποίηση! Η επικεφαλίδα του καναλιο�? ήχου %d ο�?ίζεται ξανά!\n"
#define MSGTR_VideoStreamRedefined "Π�?οειδοποίηση! Η επικεφαλίδα του καναλιο�? βίντεο %d ο�?ίζεται ξανά!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: Πολλαπλά (%d σε %d bytes) πακέτα ήχου στον buffer!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: Πολλαπλά (%d σε %d bytes) πακέτα βίντεο στον buffer!\n"
#define MSGTR_MaybeNI "(ίσως αναπα�?άγεται έναν non-interleaved κανάλι/α�?χείο ή απέτυχε το codec)\n" \
                      "Για .AVI α�?χεία, ενε�?γοποιήστε τη μέθοδο non-interleaved με την επιλογή -ni\n"
#define MSGTR_SwitchToNi "\n Αναγνω�?ίστηκε λάθος interleaved .AVI - εναλλαγή στη μέθοδο -ni!\n"
#define MSGTR_Detected_XXX_FileFormat "Αναγνω�?ίστηκε α�?χείο τ�?που %s!\n"
#define MSGTR_DetectedAudiofile "Αναγνω�?ίστηκε α�?χείο ήχου!\n"
#define MSGTR_InvalidMPEGES "Μη Αναγνω�?ίσιμο κανάλι MPEG-ES??? Επικοινώνησε με τον δημιου�?γό, μπο�?εί να είναι ένα bug :(\n"
#define MSGTR_FormatNotRecognized "============= Λυπάμαι, αυτό το είδος α�?χείου δεν αναγνω�?ίζεται/υποστη�?ίζεται ===============\n"\
                                  "=== Αν το α�?χείο είναι ένα AVI, ASF ή MPEG κανάλι, πα�?ακαλώ επικοινωνήστε με τον δημιου�?γό! ===\n"
#define MSGTR_MissingVideoStream "Δεν β�?έθηκε κανάλι βίντεο!\n"
#define MSGTR_MissingAudioStream "Δεν β�?έθηκε κανάλι ήχου...  ->χω�?ίς-ήχο\n"
#define MSGTR_MissingVideoStreamBug "Λείπει το κανάλι βίντεο!? Επικοινώνησε με τον δημιου�?γό, μπο�?εί να είναι ένα bug :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: το α�?χείο δεν πε�?ιέχει το επιλεγμένο κανάλι ήχου ή βίντεο\n"

#define MSGTR_NI_Forced "Εξαναγκασμένο"
#define MSGTR_NI_Detected "Β�?έθηκε"
#define MSGTR_NI_Message "%s NON-INTERLEAVED AVI format α�?χείου!\n"

#define MSGTR_UsingNINI "Χ�?ήση ενός NON-INTERLEAVED φθα�?μένου α�?χείου τ�?που AVI!\n"
#define MSGTR_CouldntDetFNo "Δεν μπό�?εσε να διευκ�?ινιστεί ο α�?ιθμός των frames (για απόλυτη αναζήτηση)  \n"
#define MSGTR_CantSeekRawAVI "Μη δυνατή αναζήτηση σε raw .AVI κανάλια! (το index είναι απα�?αίτητο, δοκιμάστε με την επιλογή -idx!)  \n"
#define MSGTR_CantSeekFile "Αδ�?νατη η αναζήτηση σε αυτό το α�?χείο!  \n"

#define MSGTR_MOVcomprhdr "MOV: Συμπιεσμένες επικεφαλίδες δεν υποστη�?ίζονται (ακόμα)!\n"
#define MSGTR_MOVvariableFourCC "MOV: ΠΡΟΕΙΔΟΠΟΙΗΣΗ! μεταβλητό FOURCC β�?έθηκε!?\n"
#define MSGTR_MOVtooManyTrk "MOV: Π�?οειδοποίηση! β�?έθηκαν πολλά tracks!"
#define MSGTR_DetectedTV "Β�?έθηκε TV! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Δεν είναι δυνατό το άνοιγμα του ogg demuxer\n"
#define MSGTR_CannotOpenAudioStream "Δεν είναι δυνατό το άνοιγμα του καναλιο�? ήχου: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Δεν είναι δυνατό το άνοιγμα του καναλιο�? υποτίτλων: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Αποτυχία κατά το άνοιγμα του demuxer ήχου: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Αποτυχία κατά το άνοιγμα του demuxer υποτίτλων: %s\n"
#define MSGTR_TVInputNotSeekable "TV input δεν είναι αναζητήσιμο! (πιθανόν η αναζήτηση να γίνει για την αλλαγή σταθμών ;)\n"
#define MSGTR_ClipInfo "Πλη�?οφο�?ίες του μέσου: \n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: β�?έθηκε πε�?ιεχόμενο 30fps NTSC, αλλαγή του framerate.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: β�?έθηκε πε�?ιεχόμενο 24fps progressive NTSC, αλλαγή του framerate.\n"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "Αδ�?νατο το άνοιγμα του codec\n"
#define MSGTR_CantCloseCodec "Αδ�?νατο το κλείσιμο του codec\n"

#define MSGTR_MissingDLLcodec "Σφάλμα: Δεν είναι δυνατό το άνοιγμα του απαιτο�?μενο DirectShow codec: %s\n"
#define MSGTR_ACMiniterror "Δεν είναι δυνατό να φο�?τωθεί/α�?χικοποιηθεί το Win32/ACM codec ήχου (λείπει το DLL α�?χείο?)\n"
#define MSGTR_MissingLAVCcodec "Δεν είναι δυνατό να β�?εθεί το '%s' στο libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: Σφάλμα: β�?έθηκε τέλος α�?χείου στην αναζήτηση για ακολουθία της επικεφαλίδας\n"
#define MSGTR_CannotReadMpegSequHdr "Σφάλμα: Δεν είναι δυνατό να διαβαστεί η ακολουθία της επικεφαλίδας!\n"
#define MSGTR_CannotReadMpegSequHdrEx "Σφάλμα: Δεν είναι δυνατό να διαβαστεί η ακολουθία της επέκτασης της επικεφαλίδας!\n"
#define MSGTR_BadMpegSequHdr "MPEG: Κακή ακολουθία της επικεφαλίδας!\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Κακή ακολουθία της επέκτασης της επικεφαλίδας!\n"

#define MSGTR_ShMemAllocFail "Δεν μπο�?εί να π�?οσδιο�?ιστεί διαμοι�?αζόμενη μνήμη\n"
#define MSGTR_CantAllocAudioBuf "Δεν μπο�?εί να π�?οσδιο�?ιστεί buffer για έξοδο ήχου\n"

#define MSGTR_UnknownAudio "Άγνωστο/απών format ήχου, χ�?ήση του χω�?ίς-ήχο\n"

#define MSGTR_UsingExternalPP "[PP] Χ�?ήση εξωτε�?ικο�? φίλτ�?ου π�?οεπεξε�?γασίας, μέγιστο q = %d\n"
#define MSGTR_UsingCodecPP "[PP] Χ�?ήση φίλτ�?ου π�?οεπεξε�?γασίας για το codec, μέγιστο q = %d\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Η αίτηση για την οικογένεια του codec βίντεο [%s] (vfm=%s) δεν διατίθεται (ενε�?γοποιήστε το κατά την μετάφ�?αση του π�?ογ�?άμματος!)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Η αίτηση για την οικογένεια του codec ήχου [%s] (afm=%s) δεν διατίθεται (ενε�?γοποιήστε το κατά την μετάφ�?αση του π�?ογ�?άμματος!)\n"
#define MSGTR_OpeningVideoDecoder "Άνοιγμα αποκωδικοποιητή βίντεο: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "Άνοιγμα αποκωδικοποιητή ήχου: [%s] %s\n"
#define MSGTR_VDecoderInitFailed "Αποτυχία α�?χικοποίησης του VDecoder :(\n"
#define MSGTR_ADecoderInitFailed "Αποτυχία α�?χικοποίησης του ADecoder :(\n"
#define MSGTR_ADecoderPreinitFailed "Αποτυχία π�?οα�?χικοποίησης του ADecoder :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "Αποτυχία στην α�?χικοποίηση της υποστή�?ιξης του lirc!\n"
#define MSGTR_LIRCcfgerr "Αποτυχία κατά το διάβασμα του α�?χείου πα�?αμέτ�?ων του lirc %s!\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "Αδ�?νατη η ε�?�?εση του φίλτ�?ου βίντεο '%s'\n"
#define MSGTR_CouldNotOpenVideoFilter "Αδ�?νατο το άνοιγμα του φίλτ�?ου βίντεο '%s'\n"
#define MSGTR_OpeningVideoFilter "Άνοιγμα του φίλτ�?ου βίντεο: "
#define MSGTR_CannotFindColorspace "Αδ�?νατη ε�?�?εση για colorspace, ακόμη και με την εισαγωγή 'scale' :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: το codec δεν ό�?ισε sh->disp_w και sh->disp_h, π�?οσπάθεια επίλυσης!\n"
#define MSGTR_CouldNotFindColorspace "Δεν β�?έθηκε αντίστοιχο colorspace - π�?οσπάθεια με -vf scale...\n"
#define MSGTR_MovieAspectIsSet "Η αναλογία της ταινίας είναι %.2f:1 - π�?οκλιμάκωση για την διό�?θωση της εμφάνισης της ταινίας.\n"
#define MSGTR_MovieAspectUndefined "Η αναλογία της ταινίας δεν είναι ο�?ισμένη - δεν εφα�?μόζεται π�?οκλιμάκωση.\n"

// vd_dshow.c, vd_dmo.c
#define MSGTR_DownloadCodecPackage "Θα π�?έπει να αναβαθμήσετε ή να εγκαταστήσετε το πακέτο με τα codecs.\nΔείτε τη διε�?θυνση http://www.mplayerhq.hu/dload.html\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: Αδ�?νατη η αποστολή του γεγονότος EWMH fullscreen!\n"


// ====================== GUI messages/buttons ========================

#ifdef CONFIG_GUI

// --- labels ---
#define MSGTR_About "Πε�?ί"
#define MSGTR_FileSelect "Επιλογή α�?χείου..."
#define MSGTR_SubtitleSelect "Επιλογή υποτίτλου..."
#define MSGTR_OtherSelect "Επιλογή..."
#define MSGTR_AudioFileSelect "Επιλογή εξωτε�?ικο�? α�?χείου ήχου..."
#define MSGTR_FontSelect "Επιλογή γ�?αμματοσει�?άς..."
#define MSGTR_PlayList "Λίστα Αναπα�?αγωγής"
#define MSGTR_Equalizer "Equalizer"
#define MSGTR_SkinBrowser "Λίστα  skins"
#define MSGTR_Network "Streaming δικτ�?ου."
#define MSGTR_Preferences "Ιδιότητες"
#define MSGTR_NoMediaOpened "Δεν φο�?τώθηκαν α�?χεία"
#define MSGTR_NoChapter "Μη χ�?ήση κεφαλαίου"
#define MSGTR_Chapter "Κεφάλαιο %d"
#define MSGTR_NoFileLoaded "δεν φο�?τώθηκε α�?χείο"

// --- buttons ---
#define MSGTR_Ok "Εντάξει"
#define MSGTR_Cancel "Άκυ�?ο"
#define MSGTR_Add "Π�?οσθήκη"
#define MSGTR_Remove "Αφαί�?εση"
#define MSGTR_Clear "Καθά�?ισμα"
#define MSGTR_Config "Π�?οτιμήσεις"
#define MSGTR_ConfigDriver "Π�?οτίμηση οδηγο�?"
#define MSGTR_Browse "Αναζήτηση α�?χείου"

// --- error messages ---
#define MSGTR_NEMDB "Λυπάμαι, δεν υπά�?χει α�?κετή μνήμη για εγγ�?αφή  στον buffer."
#define MSGTR_NEMFMR "Λυπάμαι, δεν υπά�?χει α�?κετή μνήμη για την εμφάνιση του μενο�?."
#define MSGTR_IDFGCVD "Λυπάμαι, δεν β�?έθηκε οδηγός εξόδου βίντεο που να είναι συμβατός με το GUI."
#define MSGTR_NEEDLAVC "Λυπάμαι, δεν μπο�?είτε να αναπα�?άγετε α�?χεία που δεν είναι mpeg με τη συσκευή DXR3/H+ χω�?ίς επανακωδικοποίηση.\n Πα�?ακαλώ ενε�?γοποιήστε lavc στο DXR3/H+ κουτί-διαλόγου."

// --- skin loader error messages
#define MSGTR_SKIN_ERRORMESSAGE "[skin] σφάλμα στο α�?χείο π�?οτιμήσεων του skin στη γ�?αμμή %d: %s"
#define MSGTR_SKIN_BITMAP_16bit  "το βάθος χ�?ώματος εικόνας των 16 bits ή λιγότε�?ο δεν υποστη�?ίζεται (%s).\n"
#define MSGTR_SKIN_BITMAP_FileNotFound  "το α�?χείο (%s) δεν β�?έθηκε\n"
#define MSGTR_SKIN_BITMAP_PNGReadError "σφάλμα κατά την ανάγνωση του PNG (%s)\n"
#define MSGTR_SKIN_BITMAP_ConversionError "σφάλμα κατά τη μετατ�?οπή από 24 bit σε 32 bit (%s)\n"
#define MSGTR_SKIN_UnknownMessage "μη αναγνω�?ίσιμο μήνυμα: %s\n"
#define MSGTR_SKIN_NotEnoughMemory "δεν υπά�?χει α�?κετή μνήμη διαθέσιμη\n"
#define MSGTR_SKIN_FONT_TooManyFontsDeclared "πολλαπλές ο�?ισμένες γ�?αμματοσει�?ές\n"
#define MSGTR_SKIN_FONT_FontFileNotFound "δεν β�?έθηκε α�?χείο γ�?αμματοσει�?άς\n"
#define MSGTR_SKIN_FONT_FontImageNotFound "δεν β�?έθηκε εικόνα του α�?χείου γ�?αμματοσει�?άς\n"
#define MSGTR_SKIN_FONT_NonExistentFont "μη-υπα�?κτή η ταυτότητα της γ�?αμματοσει�?άς (%s)\n"
#define MSGTR_SKIN_UnknownParameter "μη αναγνω�?ίσιμη πα�?άμετ�?ος (%s)\n"
#define MSGTR_SKIN_SKINCFG_SkinNotFound "Δεν β�?έθηκε skin (%s).\n"
#define MSGTR_SKIN_LABEL "Skins:"

// --- gtk menus
#define MSGTR_MENU_AboutMPlayer "Πε�?ί του MPlayer"
#define MSGTR_MENU_Open "Άνοιγμα..."
#define MSGTR_MENU_PlayFile "Αναπα�?αγωγή α�?χείου..."
#define MSGTR_MENU_PlayVCD "Αναπα�?αγωγή VCD..."
#define MSGTR_MENU_PlayDVD "Αναπα�?αγωγή DVD..."
#define MSGTR_MENU_PlayURL "Αναπα�?αγωγή URL..."
#define MSGTR_MENU_LoadSubtitle "Άνοιγμα υποτίτλου..."
#define MSGTR_MENU_DropSubtitle "Αφαί�?εση υποτίτλου..."
#define MSGTR_MENU_LoadExternAudioFile "Άνοιγμα εξωτε�?ικο�? α�?χείου ήχου..."
#define MSGTR_MENU_Playing "Αναπα�?αγωγή..."
#define MSGTR_MENU_Play "Αναπα�?αγωγή"
#define MSGTR_MENU_Pause "Πα�?ση"
#define MSGTR_MENU_Stop "Σταμάτημα"
#define MSGTR_MENU_NextStream "Επόμενο κανάλι"
#define MSGTR_MENU_PrevStream "Π�?οηγο�?μενο κανάλι"
#define MSGTR_MENU_Size "Μέγεθος"
#define MSGTR_MENU_NormalSize "Κανονικό μέγεθος"
#define MSGTR_MENU_DoubleSize "Διπλάσιο μέγεθος"
#define MSGTR_MENU_FullScreen "Πλή�?ης οθόνη"
#define MSGTR_MENU_DVD "DVD"
#define MSGTR_MENU_VCD "VCD"
#define MSGTR_MENU_PlayDisc "Αναπα�?αγωγή δίσκου..."
#define MSGTR_MENU_ShowDVDMenu "Εμφάνιση του μενο�? του DVD"
#define MSGTR_MENU_Titles "Τίτλοι"
#define MSGTR_MENU_Title "Τίτλος %2d"
#define MSGTR_MENU_None "(τίποτα)"
#define MSGTR_MENU_Chapters "Κεφάλαια"
#define MSGTR_MENU_Chapter "Κεφάλαιο %2d"
#define MSGTR_MENU_AudioLanguages "Γλώσσες ήχου"
#define MSGTR_MENU_SubtitleLanguages "Γλώσσες υποτίτλων"
#define MSGTR_MENU_SkinBrowser "Λίστα skins"
// TODO: Why is this different from MSGTR_Preferences?
#define MSGTR_MENU_Preferences "Ρυθμίσεις"
#define MSGTR_MENU_Exit "Έξοδος"
#define MSGTR_MENU_Mute "Απενε�?γοποίηση ήχου"
#define MSGTR_MENU_Original "Α�?χικό"
#define MSGTR_MENU_AspectRatio "Αναλογία εμφάνισης"
#define MSGTR_MENU_AudioTrack "Track ήχου"
#define MSGTR_MENU_Track "Track %d"
#define MSGTR_MENU_VideoTrack "Track βίντεο"

// --- equalizer
#define MSGTR_EQU_Audio "Ήχος"
#define MSGTR_EQU_Video "Βίντεο"
#define MSGTR_EQU_Contrast "Αντίθεση: "
#define MSGTR_EQU_Brightness "Φωτεινότητα: "
#define MSGTR_EQU_Hue "Hue: "
#define MSGTR_EQU_Saturation "Saturation: "
#define MSGTR_EQU_Front_Left "Μπ�?οστά Α�?ιστε�?ά"
#define MSGTR_EQU_Front_Right "Μπ�?οστά Δεξιά"
#define MSGTR_EQU_Back_Left "Πίσω α�?ιστε�?ά"
#define MSGTR_EQU_Back_Right "Πίσω δεξιά"
#define MSGTR_EQU_Center "Κέντ�?ο"
#define MSGTR_EQU_Bass "Μπάσο"
#define MSGTR_EQU_All "Όλα"
#define MSGTR_EQU_Channel1 "Κανάλι 1:"
#define MSGTR_EQU_Channel2 "Κανάλι 2:"
#define MSGTR_EQU_Channel3 "Κανάλι 3:"
#define MSGTR_EQU_Channel4 "Κανάλι 4:"
#define MSGTR_EQU_Channel5 "Κανάλι 5:"
#define MSGTR_EQU_Channel6 "Κανάλι 6:"

// --- playlist
#define MSGTR_PLAYLIST_Path "Διαδ�?ομή"
#define MSGTR_PLAYLIST_Selected "Επιλεγμένα α�?χεία"
#define MSGTR_PLAYLIST_Files "Α�?χεία"
#define MSGTR_PLAYLIST_DirectoryTree "Δένδ�?ο καταλόγων"

// --- preferences
#define MSGTR_PREFERENCES_SubtitleOSD "Υπότιτλοι και OSD"
#define MSGTR_PREFERENCES_Codecs "Codecs και demuxer"
#define MSGTR_PREFERENCES_Misc "Διάφο�?α"

#define MSGTR_PREFERENCES_None "Τίποτα"
#define MSGTR_PREFERENCES_AvailableDrivers "Διαθέσιμοι οδηγοί:"
#define MSGTR_PREFERENCES_DoNotPlaySound "Μη-αναπα�?αγωγή ήχου"
#define MSGTR_PREFERENCES_NormalizeSound "Κανονικοποίηση ήχου"
#define MSGTR_PREFERENCES_EnableEqualizer "Ενε�?γοποίηση του equalizer"
#define MSGTR_PREFERENCES_ExtraStereo "Ενε�?γοποίηση του extra stereo"
#define MSGTR_PREFERENCES_Coefficient "Coefficient:"
#define MSGTR_PREFERENCES_AudioDelay "Καθυστέ�?ηση ήχου"
#define MSGTR_PREFERENCES_DoubleBuffer "Ενε�?γοποίηση double buffering"
#define MSGTR_PREFERENCES_DirectRender "Ενε�?γοποίηση direct rendering"
#define MSGTR_PREFERENCES_FrameDrop "Ενε�?γοποίηση κατά�?γησης κα�?έ"
#define MSGTR_PREFERENCES_HFrameDrop "Ενε�?γοποίηση σκλη�?ής κατά�?γησης κα�?έ (επικίνδυνο)"
#define MSGTR_PREFERENCES_Flip "Flip της εικόνας πάνω-κάτω"
#define MSGTR_PREFERENCES_Panscan "Panscan: "
#define MSGTR_PREFERENCES_Subtitle "Υπότιτλοι:"
#define MSGTR_PREFERENCES_SUB_Delay "Καθυστέ�?ηση:"
#define MSGTR_PREFERENCES_SUB_FPS "FPS:"
#define MSGTR_PREFERENCES_SUB_POS "Θέση:"
#define MSGTR_PREFERENCES_SUB_AutoLoad "Απενε�?γοποίηση αυτόματου φο�?τώματος υποτίτλων"
#define MSGTR_PREFERENCES_SUB_Unicode "Unicode υπότιτλος"
#define MSGTR_PREFERENCES_SUB_MPSUB "Μετατ�?οπή εισαγόμενου υπότιτλου σε υπότιτλο τ�?που MPlayer"
#define MSGTR_PREFERENCES_SUB_SRT "Μετατ�?οπή εισαγόμενου υπότιτλου σε υπότιτλο τ�?πο SubViewer(SRT)"
#define MSGTR_PREFERENCES_SUB_Overlap "Εναλλαγή του overlapping υποτίτλου"
#define MSGTR_PREFERENCES_Font "Γ�?αμματοσει�?ά:"
#define MSGTR_PREFERENCES_FontFactor "Πα�?άγοντας της γ�?αμματοσει�?άς:"
#define MSGTR_PREFERENCES_PostProcess "Ενε�?γοποίηση π�?οεπεξε�?γασίας"
#define MSGTR_PREFERENCES_AutoQuality "Αυτόματη ποιότητα: "
#define MSGTR_PREFERENCES_NI "Χ�?ήση του non-interleaved AVI parser"
#define MSGTR_PREFERENCES_IDX "Αναδημιου�?γία του πίνακα index, αν χ�?ειάζεται"
#define MSGTR_PREFERENCES_VideoCodecFamily "Οικογένεια του βίντεο codec:"
#define MSGTR_PREFERENCES_AudioCodecFamily "Οικογένεια του codec ήχου:"
#define MSGTR_PREFERENCES_FRAME_OSD_Level "Επίπεδο OSD"
#define MSGTR_PREFERENCES_FRAME_Subtitle "Υπότιτλος"
#define MSGTR_PREFERENCES_FRAME_Font "Γ�?αμματοσει�?ά"
#define MSGTR_PREFERENCES_FRAME_PostProcess "Π�?οεπεξε�?γασία"
#define MSGTR_PREFERENCES_FRAME_CodecDemuxer "Codec και demuxer"
#define MSGTR_PREFERENCES_FRAME_Cache "Cache"
#define MSGTR_PREFERENCES_Message "Π�?οσοχή, με�?ικές λειτου�?γίες χ�?ειάζονται επανεκκίνιση αναπα�?αγωγής."
#define MSGTR_PREFERENCES_DXR3_VENC "Κωδικοποιητής βίντεο:"
#define MSGTR_PREFERENCES_DXR3_LAVC "Χ�?ήση του LAVC (FFmpeg)"
#define MSGTR_PREFERENCES_FontEncoding1 "Unicode"
#define MSGTR_PREFERENCES_FontEncoding2 "Δυτικές Ευ�?ωπαϊκές γλώσσες (ISO-8859-1)"
#define MSGTR_PREFERENCES_FontEncoding3 "Δυτικές Ευ�?ωπαϊκές γλώσσες με Ευ�?ώ (ISO-8859-15)"
#define MSGTR_PREFERENCES_FontEncoding4 "Slavic/Central European Languages (ISO-8859-2)"
#define MSGTR_PREFERENCES_FontEncoding5 "Esperanto, Galician, Maltese, Το�?�?κικα (ISO-8859-3)"
#define MSGTR_PREFERENCES_FontEncoding6 "Παλιά Βαλτική κωδικοσει�?ά (ISO-8859-4)"
#define MSGTR_PREFERENCES_FontEncoding7 "Κυ�?ιλλική (ISO-8859-5)"
#define MSGTR_PREFERENCES_FontEncoding8 "Α�?αβικά (ISO-8859-6)"
#define MSGTR_PREFERENCES_FontEncoding9 "�?έα Ελληνικά (ISO-8859-7)"
#define MSGTR_PREFERENCES_FontEncoding10 "Το�?�?κικα (ISO-8859-9)"
#define MSGTR_PREFERENCES_FontEncoding11 "Βαλτική (ISO-8859-13)"
#define MSGTR_PREFERENCES_FontEncoding12 "Κέλτικα (ISO-8859-14)"
#define MSGTR_PREFERENCES_FontEncoding13 "Εβ�?αϊκά (ISO-8859-8)"
#define MSGTR_PREFERENCES_FontEncoding14 "Ρώσικα (KOI8-R)"
#define MSGTR_PREFERENCES_FontEncoding15 "Ukrainian, Belarusian (KOI8-U/RU)"
#define MSGTR_PREFERENCES_FontEncoding16 "Απλοποιημένα Κινέζικα (CP936)"
#define MSGTR_PREFERENCES_FontEncoding17 "Πα�?αδοσιακά Κινέζικα (BIG5)"
#define MSGTR_PREFERENCES_FontEncoding18 "Γιαπωνέζικα (SHIFT-JIS)"
#define MSGTR_PREFERENCES_FontEncoding19 "Κο�?εάτικα (CP949)"
#define MSGTR_PREFERENCES_FontEncoding20 "Ταϊλανδέζικη κωδικοσει�?ά (CP874)"
#define MSGTR_PREFERENCES_FontEncoding21 "Κυ�?ιλλική Windows (CP1251)"
#define MSGTR_PREFERENCES_FontEncoding22 "Σλάβικα/Κεντ�?ικά Ευ�?ωπαϊκά Windows (CP1250)"
#define MSGTR_PREFERENCES_FontNoAutoScale "Χω�?ίς αυτόματη κλιμάκωση"
#define MSGTR_PREFERENCES_FontPropWidth "Αναλογία με το πλάτος της ταινίας"
#define MSGTR_PREFERENCES_FontPropHeight "Αναλογία με το �?ψος της ταινίας"
#define MSGTR_PREFERENCES_FontPropDiagonal "Αναλογία με τη διαγώνιο της ταινίας"
#define MSGTR_PREFERENCES_FontEncoding "Κωδικοποίηση:"
#define MSGTR_PREFERENCES_FontBlur "Blur:"
#define MSGTR_PREFERENCES_FontOutLine "Outline:"
#define MSGTR_PREFERENCES_FontTextScale "Κλιμάκωση του Κειμένου:"
#define MSGTR_PREFERENCES_FontOSDScale "OSD κλιμάκωση:"
#define MSGTR_PREFERENCES_Cache "Ενε�?γοποίηση/απενε�?γοποίηση της cache"
#define MSGTR_PREFERENCES_CacheSize "Μέγεθος της cache: "
#define MSGTR_PREFERENCES_LoadFullscreen "Εκκίνηση σε πλή�?η οθόνη"
#define MSGTR_PREFERENCES_SaveWinPos "Αποθήκευση της θέσης του πα�?αθ�?�?ου"
#define MSGTR_PREFERENCES_XSCREENSAVER "Απενε�?γοποίηση της π�?οστασίας οθόνης"
#define MSGTR_PREFERENCES_PlayBar "Ενε�?γοποίηση της playbar"
#define MSGTR_PREFERENCES_AutoSync "Ενε�?γοποίηση/απενε�?γοποίηση του αυτόματου συγχ�?ονισμο�?"
#define MSGTR_PREFERENCES_AutoSyncValue "Αυτόματος συγχ�?ονισμός: "
#define MSGTR_PREFERENCES_CDROMDevice "Συσκευή CD-ROM:"
#define MSGTR_PREFERENCES_DVDDevice "Συσκευή DVD:"
#define MSGTR_PREFERENCES_FPS "FPS ταινίας:"
#define MSGTR_PREFERENCES_ShowVideoWindow "Εμφάνιση του Video Window όταν δεν είναι ενε�?γοποιημένο"

#define MSGTR_ABOUT_UHU "Η ανάπτυξη του GUI π�?οωθείται από την UHU Linux\n"

// --- messagebox
#define MSGTR_MSGBOX_LABEL_FatalError "Κ�?ίσιμο σφάλμα..."
#define MSGTR_MSGBOX_LABEL_Error "Σφάλμα..."
#define MSGTR_MSGBOX_LABEL_Warning "Π�?οειδοποίηση..."

#endif
