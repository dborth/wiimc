// Spanish translation by
//
// Diego Biurrun
// Reynaldo H. Verdejo Pinochet
//
// Original work done by:
//
// Leandro Lucarella <leandro at lucarella.com.ar>,
// Jesús Climent <jesus.climent at hispalinux.es>,
// Sefanja Ruijsenaars <sefanja at gmx.net>,
// Andoni Zubimendi <andoni at lpsat.net>
//
// In sync with r27967
// FIXME: Necesita una revisión a fondo hay muchas faltas de ortografía


// ========================= MPlayer help ===========================

static const char help_text[]=
"Uso:   mplayer [opciones] [url o ruta del archivo]\n"
"\n"
"Opciones básicas: ('man mplayer' para una lista completa)\n"
" -vo <driver[:disp]>  Seleccionar driver y dispositivo de salida de video ('-vo help' para obtener una lista).\n"
" -ao <driver[:disp]>  Seleccionar driver y dispositivo de salida de audio ('-ao help' para obtener una lista).\n"
#ifdef CONFIG_VCD
" vcd://<numpista>      Reproducir pista de (S)VCD (Super Video CD) (acceso directo al dispositivo, no montado)\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<número>        Reproducir título o pista de DVD desde un dispositivo en vez de un archivo regular.\n"
" -alang <idioma>      Seleccionar idioma para el audio del DVD (con código de país de dos caracteres. p. ej. 'es').\n"
" -alang <idioma>      Seleccionar idioma para los subtítulos del DVD.\n"
#endif
" -ss <tiempo>         Saltar a una posición determindada (en segundos o hh:mm:ss).\n"
" -nosound             No reproducir sonido.\n"
" -fs, -vm, -zoom      Opciones de pantalla completa (pantalla completa, cambio de modo de video, escalado por software).\n"
" -x <x> -y <y>        Escalar imagen a resolución dada (para usar con -vm o -zoom).\n"
" -sub <archivo>       Indicar que archivo de subtítulos debe utilizarse (revise también -subfps, -subdelay).\n"
" -playlist <archivo>  Especificar archivo con la lista de reproducción.\n"
" -vid <x> -aid <y>    Opciones para seleccionar los stream de video (x) y audio (y) a reproducir.\n"
" -fps <x> -srate <y>  Opciones para cambiar las tasas de refresco/muestreo de video (x fps) y audio (y Hz) respectivamente.\n"
" -pp <calidad>        Activar filtro de postproceso (revise el manual para para obtener más información).\n"
" -framedrop           Activar el descarte de cuadros/frames (para máquinas lentas).\n\n"

"Teclas básicas ('man mplayer' para una lista completa, revise también input.conf):\n"
" <-  ó  ->       Avanzar o retroceder diez segundos.\n"
" arriba ó abajo  Avanzar o retroceder un minuto.\n"
" pgup ó pgdown   Avanzar o retroceder diez minutos.\n"
" < ó >           Avanzar o retroceder en la lista de reproducción.\n"
" p ó ESPACIO     Pausar (presione cualquier tecla para continuar).\n"
" q ó ESC         Detener la reproducción y salir del programa.\n"
" + ó -           Ajustar el retardo de audio en más o menos 0.1 segundos.\n"
" o               Cambiar modo OSD: nada / búsqueda / búsqueda + tiempo.\n"
" * ó /           Aumentar o disminuir el volumen (presione 'm' para elegir entre master/pcm).\n"
" z ó x           Ajustar el retardo de la subtítulación en más o menos 0.1 segundos.\n"
" r ó t           Ajustar posición de la subtítulación (mira también -vf expand).\n"
"\n"
" *** REVISE EL MANUAL PARA OTROS DETALLES, OPCIONES (AVANZADAS) Y TECLAS DE CONTROL ***\n"
"\n";

// ========================= MPlayer messages ===========================

// mplayer.c
#define MSGTR_Exiting "\nSaliendo...\n"
#define MSGTR_ExitingHow "\nSaliendo... (%s)\n"
#define MSGTR_Exit_quit "Salida."
#define MSGTR_Exit_eof "Fin de archivo."
#define MSGTR_Exit_error "Error fatal."
#define MSGTR_IntBySignal "\nMPlayer fue interrumpido con señal %d en el módulo: %s \n"
#define MSGTR_NoHomeDir "No se puede encontrar el directorio HOME.\n"
#define MSGTR_GetpathProblem "Problema en get_path(\"config\").\n"
#define MSGTR_CreatingCfgFile "Creando archivo de configuración: %s.\n"
#define MSGTR_CantLoadFont "No se pudo cargar typografía: %s.\n"
#define MSGTR_CantLoadSub "No se pudo cargar subtítulo: %s.\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: No se encuentró el stream seleccionado.\n"
#define MSGTR_CantOpenDumpfile "No se puede abrir el archivo de dump/volcado.\n"
#define MSGTR_CoreDumped "Core dumped ;)\n"
#define MSGTR_FPSnotspecified "FPS no especificado (o inválido) en la cabecera! Usa la opción -fps.\n"
#define MSGTR_TryForceAudioFmtStr "Tratando de forzar la familia de codecs de audio %s...\n"
#define MSGTR_CantFindAudioCodec "No se encontró codec para el formato de audio 0x%X!\n"
#define MSGTR_TryForceVideoFmtStr "Tratando de forzar la familia de codecs de video %s...\n"
#define MSGTR_CantFindVideoCodec "No se encontró codec para el formato de video 0x%X!\n"
#define MSGTR_CannotInitVO "FATAL: No se puede inicializar el driver de video!\n"
#define MSGTR_CannotInitAO "No se puede abrir o inicializar dispositivo de audio, no se reproducirá sonido.\n"
#define MSGTR_StartPlaying "Comenzando la reproducción...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"       **************************************************************\n"\
"       **** Su sistema es demasiado lento para reproducir esto!  ****\n"\
"       **************************************************************\n"\
"Posibles razones, problemas, soluciones:\n"\
"- Más común: controlador de _audio_ con errores o arruinado.\n"\
"  - Intenta con -ao sdl o la emulación OSS de ALSA.\n"\
"  - Pruebe con diferentes valores para -autosync, 30 es un buen comienzo.\n"\
"- Salida de video lenta\n"\
"  - Pruebe otro driver -vo (para obtener una lista, -vo help) o intente\n"\
"    iniciar con la opción -framedrop!\n"\
"- CPU lenta\n"\
"  - No intente reproducir DVDs o DivX grandes en una CPU lenta. Intente\n"\
"    iniciar con la opción -hardframedrop.\n"\
"- Archivo erróneo o arruinado\n"\
"  - Pruebe combinaciones de -nobps -ni -mc 0.\n"\
"- Medios lentos (unidad NFS/SMB, DVD, VCD, etc)\n"\
"  - Intente con -cache 8192.\n"\
"- Esta utilizando -cache para reproducir archivos AVI no entrelazados?\n"\
"  - Intente con -nocache.\n"\
"Lea DOCS/HTML/es/video.html para consejos de ajuste/mejora de la velocidad.\n"\
"Si nada de eso sirve, revise DOCS/HTML/es/bugreports.html\n\n"

#define MSGTR_NoGui "MPlayer fue compilado sin soporte para interfaz gráfica.\n"
#define MSGTR_GuiNeedsX "La interfaz gráfica de MPlayer requiere X11!\n"
#define MSGTR_Playing "Reproduciendo %s.\n"
#define MSGTR_NoSound "Audio: sin sonido.\n"
#define MSGTR_FPSforced "FPS forzado a %5.3f  (ftime: %5.3f).\n"
#define MSGTR_AvailableVideoOutputDrivers "Controladores de salida de video disponibles:\n"
#define MSGTR_AvailableAudioOutputDrivers "Controladores de salida de audio disponibles:\n"
#define MSGTR_AvailableAudioCodecs "Codecs de audio disponibles:\n"
#define MSGTR_AvailableVideoCodecs "Codecs de video disponibles:\n"
#define MSGTR_AvailableAudioFm "Familias/drivers de codecs de audio (compilados dentro de MPlayer) disponibles:\n"
#define MSGTR_AvailableVideoFm "Familias/drivers de codecs de video (compilados dentro de MPlayer) disponibles:\n"
#define MSGTR_AvailableFsType "Modos disponibles de cambio a pantalla completa:\n"
#define MSGTR_CannotReadVideoProperties "Vídeo: no se puede leer las propiedades.\n"
#define MSGTR_NoStreamFound "No se ha encontrado stream.\n"
#define MSGTR_ErrorInitializingVODevice "Error abriendo/inicializando el dispositivo de la salida de video (-vo)!\n"
#define MSGTR_ForcedVideoCodec "Codec de video forzado: %s.\n"
#define MSGTR_ForcedAudioCodec "Codec de audio forzado: %s\n"
#define MSGTR_Video_NoVideo "Vídeo: no hay video!\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: No fue posible inicializar los filtros (-vf) o la salida de video (-vo)!\n"
#define MSGTR_Paused "  =====  PAUSA  ====="
#define MSGTR_PlaylistLoadUnable "\nNo fue posible cargar la lista de reproducción %s.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer se detuvo por una 'Instrucción Ilegal'.\n"\
"  Esto puede deberse a un defecto en nuestra nueva rutina de autodetección de CPU...\n"\
"  Por favor lee DOCS/HTML/es/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer se detuvo por una 'Instrucción Ilegal'.\n"\
"  Esto ocurre normalmente cuando ejecuta el programa en una CPU diferente de\n"\
"  la usada para compilarlo o para la cual fue optimizado.\n"\
"  ¡Verifique eso!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer se detuvo por mal uso de CPU/FPU/RAM.\n"\
"  Recompile MPlayer con la opción --enable-debug y haga un backtrace con\n"\
"  'gdb' y un desensamblado. Para más detalles, revise\n"\
"  DOCS/HTML/es/bugreports_what.html#bugreports_crash\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer se detuvo. Esto no debería pasar.\n"\
"  Puede ser un defecto en el código de MPlayer, en sus controladores\n"\
"  _o_ en su versión de gcc. Si piensa que es culpa de MPlayer, por\n"\
"  favor revise DOCS/HTML/es/bugreports.html y siga las instrucciones que allí\n"\
"  se encuentran. No podemos ayudarle a menos que nos provea esa\n"\
"  información cuando reporte algún posible defecto.\n"
#define MSGTR_LoadingConfig "Cargando configuración '%s'\n"
#define MSGTR_LoadingProtocolProfile "Cargando el perfil protocol-related '%s'\n"
#define MSGTR_LoadingExtensionProfile "Cargando el perfil extension-related '%s'\n"
#define MSGTR_AddedSubtitleFile "SUB: se agregó el archivo de subtítulo (%d): %s\n"
#define MSGTR_RemovedSubtitleFile "SUB: Archivo de subtítulos borrado (%d): %s\n"
#define MSGTR_ErrorOpeningOutputFile "Error abriendo archivo [%s] en modo escritura!\n"
#define MSGTR_RTCDeviceNotOpenable "Fallo al abrir %s: %s (el usuario debe tener permisos de lectura)\n"
#define MSGTR_LinuxRTCInitErrorIrqpSet "Error iniciando Linux RTC en llamada a ioctl (rtc_irqp_set %lu): %s\n"
#define MSGTR_IncreaseRTCMaxUserFreq "Pruebe agregando \"echo %lu > /proc/sys/dev/rtc/max-user-freq\" a los scripts de inicio de su sistema.\n"
#define MSGTR_LinuxRTCInitErrorPieOn "Error iniciando Linux RTC en llamada a ioctl (rtc_pie_on): %s\n"
#define MSGTR_UsingTimingType "Usando temporización %s.\n"
#define MSGTR_Getch2InitializedTwice "ADVERTENCIA: getch2_init llamada dos veces!\n"
#define MSGTR_DumpstreamFdUnavailable "No puedo volcar este stream - no está disponible 'fd'.\n"
#define MSGTR_CantOpenLibmenuFilterWithThisRootMenu "No puedo abrir filtro de video libmenu con el menú principal %s.\n"
#define MSGTR_AudioFilterChainPreinitError "Error en pre-inicialización de cadena de filtros de audio!\n"
#define MSGTR_LinuxRTCReadError "Error de lectura de Linux RTC: %s\n"
#define MSGTR_SoftsleepUnderflow "Advertencia! Softsleep underflow!\n"
#define MSGTR_DvdnavNullEvent "Evento DVDNAV NULO?!\n"
#define MSGTR_DvdnavHighlightEventBroken "Evento DVDNAV: Evento de destacado erroneo\n"
#define MSGTR_DvdnavEvent "Evento DVDNAV: %s\n"
#define MSGTR_DvdnavHighlightHide "Evento DVDNAV: Ocultar destacado\n"
#define MSGTR_DvdnavStillFrame "######################################## Evento DVDNAV: Frame fijo: %d seg(s)\n"
#define MSGTR_DvdnavNavStop "Evento DVDNAV: Nav Stop\n"
#define MSGTR_DvdnavNavNOP "Evento DVDNAV: Nav NOP\n"
#define MSGTR_DvdnavNavSpuStreamChangeVerbose "Evento DVDNAV: Cambio de Nav SPU Stream Change: phys: %d/%d/%d logical: %d\n"
#define MSGTR_DvdnavNavSpuStreamChange "Evento DVDNAV: Cambio de Nav SPU Stream: phys: %d logical: %d\n"
#define MSGTR_DvdnavNavAudioStreamChange "Evento DVDNAV: Cambio de Nav Audio Stream: phys: %d logical: %d\n"
#define MSGTR_DvdnavNavVTSChange "Evento DVDNAV: Cambio de Nav VTS\n"
#define MSGTR_DvdnavNavCellChange "Evento DVDNAV: Cambio de Nav Cell\n"
#define MSGTR_DvdnavNavSpuClutChange "Evento DVDNAV: Cambio de Nav SPU CLUT\n"
#define MSGTR_DvdnavNavSeekDone "Evento DVDNAV: Busqueda Nav hecha\n"
#define MSGTR_MenuCall "Llamada a menú\n"

// --- edit decision lists
#define MSGTR_EdlOutOfMem "No hay memoria suficiente para almacenar los datos EDL.\n"
#define MSGTR_EdlRecordsNo "Leidas %d acciones EDL.\n"
#define MSGTR_EdlQueueEmpty "No hay acciones EDL de las que ocuparse.\n"
#define MSGTR_EdlCantOpenForWrite "Error tratando de escribir en [%s].\n"
#define MSGTR_EdlCantOpenForRead "Error tratando de leer desde [%s].\n"
#define MSGTR_EdlNOsh_video "Imposible usar EDL sin video.\n"
#define MSGTR_EdlNOValidLine "Linea EDL inválida: %s\n"
#define MSGTR_EdlBadlyFormattedLine "Ignorando linea EDL mal formateada [%d].\n"
#define MSGTR_EdlBadLineOverlap "Última posición de parada fue [%f]; próxima "\
"posición de partida es [%f]. Las operaciones deben estar en orden cronológico"\
"y sin sobreponerse, ignorando.\n"
#define MSGTR_EdlBadLineBadStop "La posición de parada debe ser posterior a la"\
" posición de partida.\n"
#define MSGTR_EdloutBadStop "EDL skip cancelado, último comienzo > parada\n"
#define MSGTR_EdloutStartSkip "EDL skip comenzado, presione 'i' denuevo para terminar con el bloque.\n"
#define MSGTR_EdloutEndSkip "EDL skip terminado, operación guardada.\n"

// mplayer.c OSD
#define MSGTR_OSDenabled "habilitado"
#define MSGTR_OSDdisabled "deshabilitado"
#define MSGTR_OSDAudio "Audio: %s"
#define MSGTR_OSDVideo "Vídeo: %s"
#define MSGTR_OSDChannel "Canal: %s"
#define MSGTR_OSDSubDelay "Sub delay: %d ms"
#define MSGTR_OSDSpeed "Velocidad: x %6.2f"
#define MSGTR_OSDosd "OSD: %s"
#define MSGTR_OSDChapter "Capítulo: (%d) %s"
#define MSGTR_OSDAngle "�?ngulo: %d/%d"

// property values
#define MSGTR_Enabled "habilitado"
#define MSGTR_EnabledEdl "deshabilitado (EDL)"
#define MSGTR_Disabled "deshabilitado"
#define MSGTR_HardFrameDrop "hard"
#define MSGTR_Unknown "desconocido"
#define MSGTR_Bottom "abajo"
#define MSGTR_Center "centro"
#define MSGTR_Top "arriba"
#define MSGTR_SubSourceFile "archivo"
#define MSGTR_SubSourceVobsub "vobsub"
#define MSGTR_SubSourceDemux "incluido"

// OSD bar names
#define MSGTR_Volume "Volumen"
#define MSGTR_Panscan "Panscan"
#define MSGTR_Gamma "Gamma"
#define MSGTR_Brightness "Brillo"
#define MSGTR_Contrast "Contraste"
#define MSGTR_Saturation "Saturación"
#define MSGTR_Hue "Hue"
#define MSGTR_Balance "Balance"

// property state
#define MSGTR_LoopStatus "Loop: %s"
#define MSGTR_MuteStatus "Mudo: %s"
#define MSGTR_AVDelayStatus "A-V delay: %s"
#define MSGTR_OnTopStatus "Quedarse arriba: %s"
#define MSGTR_RootwinStatus "Rootwin: %s"
#define MSGTR_BorderStatus "Borde: %s"
#define MSGTR_FramedroppingStatus "Framedropping: %s"
#define MSGTR_VSyncStatus "VSync: %s"
#define MSGTR_SubSelectStatus "Subtítulos: %s"
#define MSGTR_SubSourceStatus "Fuente de subtítulos: %s"
#define MSGTR_SubPosStatus "Posición de subtítulos: %s/100"
#define MSGTR_SubAlignStatus "Alineación de subtítulos: %s"
#define MSGTR_SubDelayStatus "Retraso de subtítulos: %s"
#define MSGTR_SubScale "Escalado de subtítulos: %s"
#define MSGTR_SubVisibleStatus "Subtítulos: %s"
#define MSGTR_SubForcedOnlyStatus "Sólo subtítulos forzados: %s"

// mencoder.c
#define MSGTR_UsingPass3ControlFile "Usando el archivo de control pass3: %s\n"
#define MSGTR_MissingFilename "\nFalta el nombre del archivo.\n\n"
#define MSGTR_CannotOpenFile_Device "No se pudo abrir el archivo o el dispositivo.\n"
#define MSGTR_CannotOpenDemuxer "No pude abrir el demuxer.\n"
#define MSGTR_NoAudioEncoderSelected "\nNo se ha seleccionado un codificador de audio (-oac). Escoja uno (mire -oac help) o use -nosound.\n"
#define MSGTR_NoVideoEncoderSelected "\nNo se ha selecciono un codificador de video (-ovc). Escoge uno (mira -ovc help)\n"
#define MSGTR_CannotOpenOutputFile "No se puede abrir el archivo de salida'%s'.\n"
#define MSGTR_EncoderOpenFailed "No pude abrir el codificador.\n"
#define MSGTR_MencoderWrongFormatAVI "\nAVISO: EL FORMATO DEL FICHERO DE SALIDA ES _AVI_. Ver ayuda sobre el parámetro -of.\n"
#define MSGTR_MencoderWrongFormatMPG "\nAVISO: EL FORMATO DEL FICHERO DE SALIDA ES _MPEG_. Ver ayuda sobre el parámetro -of.\n"
#define MSGTR_MissingOutputFilename "No se ha especificado ningún fichero de salida, por favor verifique la opción -o"
#define MSGTR_ForcingOutputFourcc "Forzando salida fourcc a %x [%.4s].\n"
#define MSGTR_ForcingOutputAudiofmtTag "Forzando la etiqueta del formato de la salida de audio a 0x%x.\n"
#define MSGTR_DuplicateFrames "\n%d frame(s) duplicados.\n"
#define MSGTR_SkipFrame "\nSaltando frame/cuadro...\n"
#define MSGTR_ResolutionDoesntMatch "\nEl nuevo archivo de video tiene diferente resolución o espacio de colores que el anterior.\n"
#define MSGTR_FrameCopyFileMismatch "\nTodos los archivos de video deben tener fps, resolución y codec identicos para -ovc copy.\n"
#define MSGTR_AudioCopyFileMismatch "\nTodos los archivos deben tener codec de audio y formato identicos para -oac copy.\n"
#define MSGTR_NoAudioFileMismatch "\nNo se puede mezclar archivos de solo video y archivos con video y audio, Pruebe -nosound.\n"
#define MSGTR_NoSpeedWithFrameCopy "ADVERTENCIA: No se garantiza que -speed funcione adecuadamente con -oac copy!\n"\
"Su codificación puede salir mal!\n"
#define MSGTR_ErrorWritingFile "%s: error escribiendo el archivo.\n"
#define MSGTR_FlushingVideoFrames "\nVolcando los frames de vídeo.\n"
#define MSGTR_FiltersHaveNotBeenConfiguredEmptyFile "¡Los filtros no han sido configurados! ¿Fichero vacío?\n"
#define MSGTR_RecommendedVideoBitrate "Bitrate recomendado para %s CD: %d.\n"
#define MSGTR_VideoStreamResult "\nStream de video: %8.3f kbit/s (%d B/s), tamaño: %"PRIu64" bytes, %5.3f segundos, %d frames\n"
#define MSGTR_AudioStreamResult "\nStream de audio: %8.3f kbit/s (%d B/s), tamaño: %"PRIu64" bytes, %5.3f segundos\n"
#define MSGTR_EdlSkipStartEndCurrent "EDL SKIP: Inicio: %.2f  Final: %.2f   Actual: V: %.2f  A: %.2f     \r"
#define MSGTR_OpenedStream "exito: formato: %d  datos: 0x%X - 0x%x\n"
#define MSGTR_VCodecFramecopy "Codec de video: framecopy (%dx%d %dbpp fourcc=%x)\n"
#define MSGTR_ACodecFramecopy "Codec de audio: framecopy (formato=%x canales=%d razón=%d bits=%d B/s=%d muestra-%d)\n"
#define MSGTR_CBRPCMAudioSelected "Audio PCM CBR seleccionado\n"
#define MSGTR_MP3AudioSelected "Audio MP3 seleccionado\n"
#define MSGTR_CannotAllocateBytes "No se pueden asignar %d bytes\n"
#define MSGTR_SettingAudioDelay "Ajustando el RETRASO DEL AUDIO a %5.3f\n"
#define MSGTR_SettingVideoDelay "Estableciendo el retardo del vídeo en %5.3fs.\n"
#define MSGTR_LimitingAudioPreload "Limitando la pre-carda de audio a 0.4s\n"
#define MSGTR_IncreasingAudioDensity "Incrementando la densidad de audio a 4\n"
#define MSGTR_ZeroingAudioPreloadAndMaxPtsCorrection "Forzando la precarga de audio a 0, corrección pts a 0\n"
#define MSGTR_LameVersion "Versión de LAME %s (%s)\n\n"
#define MSGTR_InvalidBitrateForLamePreset "Error: La tasa de bits especificada esta fuera de los rangos de valor para esta preconfiguración\n"\
"\n"\
"Cuando utilice este modo debe ingresar un valor entre \"8\" y \"320\"\n"\
"\n"\
"Para mayor información pruebe: \"-lameopts preset=help\"\n"
#define MSGTR_InvalidLamePresetOptions "Error: No ingresó un perfil válido y/o opciones con una preconfiguración\n"\
"\n"\
"Los perfiles disponibles son:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (Modo ABR) - Implica el Modo ABR Mode. Para usarlo,\n"\
"                      solamente especifique la tasa de bits.. Por ejemplo:\n"\
"                      \"preset=185\" activates esta\n"\
"                      preconfiguración y usa 185 como el kbps promedio.\n"\
"\n"\
"    Algunos ejemplos:\n"\
"\n"\
"    \"-lameopts fast:preset=standard  \"\n"\
" o  \"-lameopts  cbr:preset=192       \"\n"\
" o  \"-lameopts      preset=172       \"\n"\
" o  \"-lameopts      preset=extreme   \"\n"\
"\n"\
"Para mayor información pruebe: \"-lameopts preset=help\"\n"
#define MSGTR_LamePresetsLongInfo "\n"\
"Las opciones de preconfiguración estan hechas para proveer la mejor calidad posible.\n"\
"\n"\
"Estas han sido en su mayor parte sometidas y ajustadas por medio de pruebas rigurosas de\n"\
"doble escucha ciega (double blind listening) para verificar y lograr este objetivo\n"\
"\n"\
"Son continuamente actualizadas con el desarrollo actual y como resultado\n"\
"deberían proveer practicamente la mejor calidad actualmente posible con\n"\
"LAME.\n"\
"\n"\
"Para activar estas preconfiguracines:\n"\
"\n"\
"   For modos VBR (en general de mejor calidad):\n"\
"\n"\
"     \"preset=standard\" Esta preconfiguración generalmente debería ser transparente\n"\
"                             para la mayoría de la gente en la música y ya es bastante\n"\
"                             buena en calidad.\n"\
"\n"\
"     \"preset=extreme\"  Si tiene un oido extramademente bueno y un equipo\n"\
"                             similar, esta preconfiguración normalmente le\n"\
"                             proveerá una calidad levemente superior al modo "\
"                            \"standard\"\n"\
"\n"\
"   Para 320kbps CBR (la mejor calidad posible desde las preconfiguraciones):\n"\
"\n"\
"     \"preset=insane\"   Esta preconfiguración será, en casi cualquier\n"\
"                             caso, excesiva para la mayoría de la gente.\n"\
"                             Pero si debe tener la mejor calidad posible\n"\
"                             sin importar el tamaño del archivo, ésta es la\n"\
"                             opción definitiva.\n"\
"\n"\
"   Para modos ABR (alta calidad por tasa de bits dado pero no tan alto como el modo VBR):\n"\
"\n"\
"  \"preset=<kbps>\"      Utilizando esta preconfiguración normalmente\n"\
"                             obtendrá buena calidad a la tasa de bits\n"\
"                             especificada. Dependiendo de ésta la\n"\
"                             preconfiguración determinará las opciones\n"\
"                             óptimas para esa situación particular.\n"\
"                             Esta configuración no es tan flexible como VBR\n"\
"                             y normalmente no llegará a obtener el mismo\n"\
"                             nivel de calidad que VBR a mayores tasas de\n"\
"                             bits.\n"\
"\n"\
"Las siguientes opciones también están disponibles para los correspondientes perfiles:\n"\
"\n"\
"   <fast>        standard\n"\
"   <fast>        extreme\n"\
"                 insane\n"\
"   <cbr> (Modo ABR) - Implica el Modo ABR Mode. Para usarlo,\n"\
"                      solamente especifique la tasa de bits.. Por ejemplo:\n"\
"                      \"preset=185\" activates esta\n"\
"                      preconfiguración y usa 185 como el kbps promedio.\n"\
"\n"\
"   \"fast\" - Activa el nuevo modo rápido VBR para un perfil en particular. La\n"\
"            desventaja al cambio de velocidad es que muchas veces la tasa de\n"\
"            bits será levemente más alta respecto del modo normal y la calidad\n"\
"            puede llegar a ser un poco más baja también.\n"\
"Advertencia: con la versión actual las preconfiguraciones \"fast\" pueden llegar a\n"\
"             dar como resultado tasas de bits demasiado altas comparadas a los normales.\n"\
"\n"\
"   \"cbr\"  - Si usa el modo ABR (ver más arriba) con una tasa de bits significativa\n"\
"            como 80, 96, 112, 128, 160, 192, 224, 256, 320,\n"\
"            puede usar la opción \"cbr\" para forzar la codificación en modo CBR\n"\
"            en lugar del modo por omisión ABR. ABR provee mejor calidad pero\n"\
"            CBR podría llegar a ser util en  situaciones tales como cuando se\n"\
"            recibe un flujo mp3 a través de internet.\n"\
"\n"\
"    Por ejemplo:\n"\
"\n"\
"    \"-lameopts fast:preset=standard  \"\n"\
" o  \"-lameopts  cbr:preset=192       \"\n"\
" o  \"-lameopts      preset=172       \"\n"\
" o  \"-lameopts      preset=extreme   \"\n"\
"\n"\
"\n"\
"Unos poco alias estan disponibles para el modo ABR:\n"\
"phone => 16kbps/mono        phon+/lw/mw-eu/sw => 24kbps/mono\n"\
"mw-us => 40kbps/mono        voice => 56kbps/mono\n"\
"fm/radio/tape => 112kbps    hifi => 160kbps\n"\
"cd => 192kbps               studio => 256kbps"
#define MSGTR_LameCantInit "No fue posible setear las opciones de LAME, revise el"\
" bitrate/samplerate. Algunos bitrates muy bajos (<32) necesitan una tasa de"\
" muestreo más baja (ej. -srate 8000). Si todo falla, pruebe con un preset."
#define MSGTR_ConfigFileError "error en archivo de configuración"
#define MSGTR_ErrorParsingCommandLine "error en parametros de la línea de comando"
#define MSGTR_VideoStreamRequired "¡El flujo de video es obligatorio!\n"
#define MSGTR_ForcingInputFPS "en su lugar el fps de entrada será interpretado como %5.3f\n"
#define MSGTR_RawvideoDoesNotSupportAudio "El formato de archivo de salida RAWVIDEO no soporta audio - desactivando audio\n"
#define MSGTR_DemuxerDoesntSupportNosound "Este demuxer todavía no soporta -nosound.\n"
#define MSGTR_MemAllocFailed "falló la asignación de memoria"
#define MSGTR_NoMatchingFilter "¡No se encontró filtro o formato de salida concordante!\n"
#define MSGTR_MP3WaveFormatSizeNot30 "sizeof(MPEGLAYER3WAVEFORMAT)==%d!=30, ¿quiza este fallado el compilador de C?\n"
#define MSGTR_NoLavcAudioCodecName "LAVC Audio,¡falta el nombre del codec!\n"
#define MSGTR_LavcAudioCodecNotFound "LAVC Audio, no se encuentra el codificador para el codec %s\n"
#define MSGTR_CouldntAllocateLavcContext "LAVC Audio, ¡no se puede asignar contexto!\n"
#define MSGTR_CouldntOpenCodec "No se puede abrir el codec %s, br=%d\n"
#define MSGTR_CantCopyAudioFormat "El formato de audio 0x%x no es compatible con '-oac copy', por favor pruebe '-oac pcm' o use '-fafmttag' para sobreescribirlo.\n"

// cfg-mencoder.h
#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     método de tasa de bits variable\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(default)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           tasa de bits media\n"\
"\n"\
" cbr           tasa de bits constante\n"\
"               Forzar también modo de codificación CBR en modos ABR \n"\
"               preseleccionados subsecuentemente.\n"\
"\n"\
" br=<0-1024>   especifica tasa de bits en kBit (solo CBR y ABR)\n"\
"\n"\
" q=<0-9>       calidad (0-mejor, 9-peor) (solo para VBR)\n"\
"\n"\
" aq=<0-9>      calidad del algoritmo (0-mejor/lenta, 9-peor/rápida)\n"\
"\n"\
" ratio=<1-100> razón de compresión\n"\
"\n"\
" vol=<0-10>    configura ganancia de entrada de audio\n"\
"\n"\
" mode=<0-3>    (por defecto: auto)\n"\
"                0: estéreo\n"\
"                1: estéreo-junto\n"\
"                2: canal dual\n"\
"                3: mono\n"\
"\n"\
" padding=<0-2>\n"\
"                0: no\n"\
"                1: todo\n"\
"                2: ajustar\n"\
"\n"\
" fast          Activa codificación rápida en modos VBR preseleccionados\n"\
"               subsecuentes, más baja calidad y tasas de bits más altas.\n"\
"\n"\
" preset=<value> Provee configuración con la mejor calidad posible.\n"\
"                 medium: codificación VBR, buena calidad\n"\
"                 (rango de 150-180 kbps de tasa de bits)\n"\
"                 standard:  codificación VBR, alta calidad\n"\
"                 (rango de 170-210 kbps de tasa de bits)\n"\
"                 extreme: codificación VBR, muy alta calidad\n"\
"                 (rango de 200-240 kbps de tasa de bits)\n"\
"                 insane:  codificación CBR, la mejor calidad configurable\n"\
"                 (320 kbps de tasa de bits)\n"\
"                 <8-320>: codificación ABR con tasa de bits en promedio en los kbps dados.\n\n"

// codec-cfg.c
#define MSGTR_DuplicateFourcc "FourCC duplicado"
#define MSGTR_TooManyFourccs "demasiados FourCCs/formatos..."
#define MSGTR_ParseError "error en el analísis"
#define MSGTR_ParseErrorFIDNotNumber "error en el analísis (¿ID de formato no es un número?)"
#define MSGTR_ParseErrorFIDAliasNotNumber "error en el analísis (¿el alias del ID de formato no es un número?)"
#define MSGTR_DuplicateFID "ID de formato duplicado"
#define MSGTR_TooManyOut "demasiados out..."
#define MSGTR_InvalidCodecName "\n¡el nombre del codec(%s) no es valido!\n"
#define MSGTR_CodecLacksFourcc "\n¡el codec(%s) no tiene FourCC/formato!\n"
#define MSGTR_CodecLacksDriver "\ncodec(%s) does not have a driver!\n"
#define MSGTR_CodecNeedsDLL "\n¡codec(%s) necesita una 'dll'!\n"
#define MSGTR_CodecNeedsOutfmt "\n¡codec(%s) necesita un 'outfmt'!\n"
#define MSGTR_CantAllocateComment "No puedo asignar memoria para el comentario. "
#define MSGTR_GetTokenMaxNotLessThanMAX_NR_TOKEN "get_token(): max >= MAX_MR_TOKEN!"
#define MSGTR_CantGetMemoryForLine "No puedo asignar memoria para 'line': %s\n"
#define MSGTR_CantReallocCodecsp "No puedo reasignar '*codecsp': %s\n"
#define MSGTR_CodecNameNotUnique "El nombre del Codec '%s' no es único."
#define MSGTR_CantStrdupName "No puedo strdup -> 'name': %s\n"
#define MSGTR_CantStrdupInfo "No puedo strdup -> 'info': %s\n"
#define MSGTR_CantStrdupDriver "No puedo strdup -> 'driver': %s\n"
#define MSGTR_CantStrdupDLL "No puedo strdup -> 'dll': %s"
#define MSGTR_AudioVideoCodecTotals "%d codecs de audio & %d codecs de video\n"
#define MSGTR_CodecDefinitionIncorrect "Codec no esta definido correctamente."
#define MSGTR_OutdatedCodecsConf "¡El archivo codecs.conf es demasiado viejo y es incompatible con esta versión de MPlayer!"

// fifo.c
#define MSGTR_CannotMakePipe "No puedo hacer un PIPE!\n"

// parser-mecmd.c, parser-mpcmd.c
#define MSGTR_NoFileGivenOnCommandLine "'--' indica que no hay más opciones, pero no se ha especificado el nombre de ningún fichero en la línea de comandos\n"
#define MSGTR_TheLoopOptionMustBeAnInteger "La opción del bucle debe ser un entero: %s\n"
#define MSGTR_UnknownOptionOnCommandLine "Opción desconocida en la línea de comandos: -%s\n"
#define MSGTR_ErrorParsingOptionOnCommandLine "Error analizando la opción en la línea de comandos: -%s\n"
#define MSGTR_InvalidPlayEntry "Entrada de reproducción inválida %s\n"
#define MSGTR_NotAnMEncoderOption "-%s no es una opción de MEncoder\n"
#define MSGTR_NoFileGiven "No se ha especificado ningún fichero"

// m_config.c
#define MSGTR_SaveSlotTooOld "Encontrada casilla demasiado vieja del lvl %d: %d !!!\n"
#define MSGTR_InvalidCfgfileOption "La opción %s no puede ser usada en un archivo de configuración.\n"
#define MSGTR_InvalidCmdlineOption "La opción %s no puede ser usada desde la línea de comandos.\n"
#define MSGTR_InvalidSuboption "Error: opción '%s' no tiene la subopción '%s'.\n"
#define MSGTR_MissingSuboptionParameter "Error: ¡subopción '%s' de '%s' tiene que tener un parámetro!\n"
#define MSGTR_MissingOptionParameter "Error: ¡opción '%s' debe tener un parámetro!\n"
#define MSGTR_OptionListHeader "\n Nombre               Tipo            Min        Max      Global  LC    Cfg\n\n"
#define MSGTR_TotalOptions "\nTotal: %d opciones\n"
#define MSGTR_ProfileInclusionTooDeep "ADVERTENCIA: Inclusion de perfil demaciado profunda.\n"
#define MSGTR_NoProfileDefined "No se han definido perfiles.\n"
#define MSGTR_AvailableProfiles "Perfiles disponibles:\n"
#define MSGTR_UnknownProfile "Perfil desconocido '%s'.\n"
#define MSGTR_Profile "Perfil %s: %s\n"

// m_property.c
#define MSGTR_PropertyListHeader "\n Nombre                 Tipo            Min      Max\n\n"
#define MSGTR_TotalProperties "\nTotal: %d propiedades\n"

// loader/ldt_keeper.c
#define MSGTR_LOADER_DYLD_Warning "AVISO: Se está intentando usar los codecs DLL pero la variable de entorno\n         DYLD_BIND_AT_LAUNCH no está establecida. Probablemente falle.\n"

// ====================== GUI messages/buttons ========================

#ifdef CONFIG_GUI

// --- labels ---
#define MSGTR_About "Acerca de"
#define MSGTR_FileSelect "Seleccionar archivo..."
#define MSGTR_SubtitleSelect "Seleccionar subtítulos..."
#define MSGTR_OtherSelect "Seleccionar..."
#define MSGTR_AudioFileSelect "Seleccionar canal de audio externo..."
#define MSGTR_FontSelect "Seleccionar fuente..."
#define MSGTR_PlayList "Lista de reproducción"
#define MSGTR_Equalizer "Equalizador"
#define MSGTR_ConfigureEqualizer "Configurar el equalizador"
#define MSGTR_SkinBrowser "Navegador de skins"
#define MSGTR_Network "Streaming por red..."
#define MSGTR_Preferences "Preferencias"
#define MSGTR_AudioPreferences "Configuración de controlador de Audio"
#define MSGTR_NoMediaOpened "no se abrió audio/video"
#define MSGTR_NoChapter "sin capítulo"
#define MSGTR_Chapter "capítulo %d"
#define MSGTR_NoFileLoaded "no se ha cargado ningún archivo"

// --- buttons ---
#define MSGTR_Ok "OK"
#define MSGTR_Cancel "Cancelar"
#define MSGTR_Add "Agregar"
#define MSGTR_Remove "Quitar"
#define MSGTR_Clear "Limpiar"
#define MSGTR_Config "Configurar"
#define MSGTR_ConfigDriver "Configurar driver"
#define MSGTR_Browse "Navegar"

// --- error messages ---
#define MSGTR_NEMDB "No hay suficiente memoria para dibujar el búfer."
#define MSGTR_NEMFMR "No hay suficiente memoria para dibujar el menú."
#define MSGTR_IDFGCVD "No se encuentra driver -vo compatible con la interfaz gráfica."
#define MSGTR_NEEDLAVC "No puede reproducir archivos no MPEG con su DXR3/H+ sin recodificación. Activa lavc en la configuración del DXR3/H+."

// --- skin loader error messages
#define MSGTR_SKIN_ERRORMESSAGE "[skin] error en configuración de skin en la línea %d: %s"
#define MSGTR_SKIN_SkinFileNotFound "[skin] no se encontró archivo ( %s ).\n"
#define MSGTR_SKIN_SkinFileNotReadable "[skin] file no leible ( %s ).\n"
#define MSGTR_SKIN_BITMAP_16bit  "Mapa de bits de 16 bits o menos no soportado (%s).\n"
#define MSGTR_SKIN_BITMAP_FileNotFound  "Archivo no encontrado (%s).\n"
#define MSGTR_SKIN_BITMAP_PNGReadError "Error al leer PNG (%s).\n"
#define MSGTR_SKIN_BITMAP_ConversionError "Error de conversión de 24 bit a 32 bit (%s).\n"
#define MSGTR_SKIN_UnknownMessage "Mensaje desconocido: %s.\n"
#define MSGTR_SKIN_NotEnoughMemory "No hay suficiente memoria.\n"
#define MSGTR_SKIN_FONT_TooManyFontsDeclared "Demasiadas fuentes declaradas.\n"
#define MSGTR_SKIN_FONT_FontFileNotFound "Archivo de fuentes no encontrado.\n"
#define MSGTR_SKIN_FONT_FontImageNotFound "Archivo de imagen de fuente no encontrado.\n"
#define MSGTR_SKIN_FONT_NonExistentFont "identificador de fuente no existente (%s).\n"
#define MSGTR_SKIN_UnknownParameter "parámetro desconocido (%s)\n"
#define MSGTR_SKIN_SKINCFG_SkinNotFound "Skin no encontrado (%s).\n"
#define MSGTR_SKIN_SKINCFG_SelectedSkinNotFound "Skin elegida ( %s ) no encontrada, probando 'default'...\n"
#define MSGTR_SKIN_LABEL "Skins:"

// --- GTK menus
#define MSGTR_MENU_AboutMPlayer "Acerca de MPlayer"
#define MSGTR_MENU_Open "Abrir..."
#define MSGTR_MENU_PlayFile "Reproducir archivo..."
#define MSGTR_MENU_PlayVCD "Reproducir VCD..."
#define MSGTR_MENU_PlayDVD "Reproducir DVD..."
#define MSGTR_MENU_PlayURL "Reproducir URL..."
#define MSGTR_MENU_LoadSubtitle "Cargar subtítulos..."
#define MSGTR_MENU_DropSubtitle "Cancelar subtitulos..."
#define MSGTR_MENU_LoadExternAudioFile "Cargar archivo de audio externo..."
#define MSGTR_MENU_Playing "Reproduciendo"
#define MSGTR_MENU_Play "Reproducir"
#define MSGTR_MENU_Pause "Pausa"
#define MSGTR_MENU_Stop "Parar"
#define MSGTR_MENU_NextStream "Siguiente stream"
#define MSGTR_MENU_PrevStream "Anterior stream"
#define MSGTR_MENU_Size "Tamaño"
#define MSGTR_MENU_HalfSize   "Mitad del Tamaño"
#define MSGTR_MENU_NormalSize "Tamaño normal"
#define MSGTR_MENU_DoubleSize "Tamaño doble"
#define MSGTR_MENU_FullScreen "Pantalla completa"
#define MSGTR_MENU_DVD "DVD"
#define MSGTR_MENU_VCD "VCD"
#define MSGTR_MENU_PlayDisc "Reproducir disco..."
#define MSGTR_MENU_ShowDVDMenu "Mostrar menú DVD"
#define MSGTR_MENU_Titles "Títulos"
#define MSGTR_MENU_Title "Título %2d"
#define MSGTR_MENU_None "(ninguno)"
#define MSGTR_MENU_Chapters "Capítulos"
#define MSGTR_MENU_Chapter "Capítulo %2d"
#define MSGTR_MENU_AudioLanguages "Idiomas de audio"
#define MSGTR_MENU_SubtitleLanguages "Idiomas de subtítulos"
#define MSGTR_MENU_PlayList MSGTR_PlayList
#define MSGTR_MENU_SkinBrowser "Navegador de skins"
#define MSGTR_MENU_Preferences MSGTR_Preferences
#define MSGTR_MENU_Exit "Salir"
#define MSGTR_MENU_Mute "Mudo"
#define MSGTR_MENU_Original "Original"
#define MSGTR_MENU_AspectRatio "Relación de aspecto"
#define MSGTR_MENU_AudioTrack "Pista de Audio"
#define MSGTR_MENU_Track "Pista %d"
#define MSGTR_MENU_VideoTrack "Pista de Video"
#define MSGTR_MENU_Subtitles "Subtítulos"

// --- equalizer
#define MSGTR_EQU_Audio "Audio"
#define MSGTR_EQU_Video "Video"
#define MSGTR_EQU_Contrast "Contraste: "
#define MSGTR_EQU_Brightness "Brillo: "
#define MSGTR_EQU_Hue "Hue: "
#define MSGTR_EQU_Saturation "Saturación: "
#define MSGTR_EQU_Front_Left "Frente izquierdo"
#define MSGTR_EQU_Front_Right "Frente derecho"
#define MSGTR_EQU_Back_Left "Fondo izquierdo"
#define MSGTR_EQU_Back_Right "Fondo dercho"
#define MSGTR_EQU_Center "Centro"
#define MSGTR_EQU_Bass "Bajo"
#define MSGTR_EQU_All "Todos"
#define MSGTR_EQU_Channel1 "Canal 1:"
#define MSGTR_EQU_Channel2 "Canal 2:"
#define MSGTR_EQU_Channel3 "Canal 3:"
#define MSGTR_EQU_Channel4 "Canal 4:"
#define MSGTR_EQU_Channel5 "Canal 5:"
#define MSGTR_EQU_Channel6 "Canal 6:"

// --- playlist
#define MSGTR_PLAYLIST_Path "Ubicación"
#define MSGTR_PLAYLIST_Selected "Archivos seleccionados"
#define MSGTR_PLAYLIST_Files "Archivos"
#define MSGTR_PLAYLIST_DirectoryTree "�?rbol de directorios"

// --- preferences
#define MSGTR_PREFERENCES_Audio MSGTR_EQU_Audio
#define MSGTR_PREFERENCES_Video MSGTR_EQU_Video
#define MSGTR_PREFERENCES_SubtitleOSD "Subtítulos y OSD"
#define MSGTR_PREFERENCES_Codecs "Codecs y demuxer"
#define MSGTR_PREFERENCES_Misc "Misc"
#define MSGTR_PREFERENCES_None "Ninguno"
#define MSGTR_PREFERENCES_DriverDefault "controlador por omisión"
#define MSGTR_PREFERENCES_AvailableDrivers "Controladores disponibles:"
#define MSGTR_PREFERENCES_DoNotPlaySound "No reproducir sonido"
#define MSGTR_PREFERENCES_NormalizeSound "Normalizar sonido"
#define MSGTR_PREFERENCES_EnableEqualizer "Activar equalizer"
#define MSGTR_PREFERENCES_SoftwareMixer "Activar mezclador por software"
#define MSGTR_PREFERENCES_ExtraStereo "Activar estereo extra"
#define MSGTR_PREFERENCES_Coefficient "Coeficiente:"
#define MSGTR_PREFERENCES_AudioDelay "Retraso de audio"
#define MSGTR_PREFERENCES_DoubleBuffer "Activar buffering doble"
#define MSGTR_PREFERENCES_DirectRender "Activar renderización directa"
#define MSGTR_PREFERENCES_FrameDrop "Activar frame dropping"
#define MSGTR_PREFERENCES_HFrameDrop "Activar frame dropping DURO (peligroso)"
#define MSGTR_PREFERENCES_Flip "Visualizar imagen al revés"
#define MSGTR_PREFERENCES_Panscan "Panscan: "
#define MSGTR_PREFERENCES_Subtitle "Subtítulo:"
#define MSGTR_PREFERENCES_SUB_Delay "Retraso: "
#define MSGTR_PREFERENCES_SUB_FPS "FPS:"
#define MSGTR_PREFERENCES_SUB_POS "Posición: "
#define MSGTR_PREFERENCES_SUB_AutoLoad "Desactivar carga automática de subtítulos"
#define MSGTR_PREFERENCES_SUB_Unicode "Subtítulo unicode"
#define MSGTR_PREFERENCES_SUB_MPSUB "Convertir el subtítulo dado al formato de subtítulos de MPlayer"
#define MSGTR_PREFERENCES_SUB_SRT "Convertir el subtítulo dado al formato basado en tiempo SubViewer (SRT)"
#define MSGTR_PREFERENCES_SUB_Overlap "Superposición de subtitulos"
#define MSGTR_PREFERENCES_SUB_USE_ASS "SSA/ASS renderizado de subtítulos"
#define MSGTR_PREFERENCES_SUB_ASS_USE_MARGINS "Usar márgenes"
#define MSGTR_PREFERENCES_SUB_ASS_TOP_MARGIN "Superior: "
#define MSGTR_PREFERENCES_SUB_ASS_BOTTOM_MARGIN "Inferior: "
#define MSGTR_PREFERENCES_Font "Fuente:"
#define MSGTR_PREFERENCES_FontFactor "Factor de fuente:"
#define MSGTR_PREFERENCES_PostProcess "Activar postprocesado"
#define MSGTR_PREFERENCES_AutoQuality "Calidad automática: "
#define MSGTR_PREFERENCES_NI "Usar non-interleaved AVI parser"
#define MSGTR_PREFERENCES_IDX "Reconstruir tabla de índices, si se necesita"
#define MSGTR_PREFERENCES_VideoCodecFamily "Familia de codec de video:"
#define MSGTR_PREFERENCES_AudioCodecFamily "Familia de codec de audio:"
#define MSGTR_PREFERENCES_FRAME_OSD_Level "Nivel OSD"
#define MSGTR_PREFERENCES_FRAME_Subtitle "Subtítulos"
#define MSGTR_PREFERENCES_FRAME_Font "Fuente"
#define MSGTR_PREFERENCES_FRAME_PostProcess "Postprocesado"
#define MSGTR_PREFERENCES_FRAME_CodecDemuxer "Codec y demuxer"
#define MSGTR_PREFERENCES_FRAME_Cache "Cache"
#define MSGTR_PREFERENCES_FRAME_Misc MSGTR_PREFERENCES_Misc
#define MSGTR_PREFERENCES_Audio_Device "Dispositivo:"
#define MSGTR_PREFERENCES_Audio_Mixer "Mezclador:"
#define MSGTR_PREFERENCES_Audio_MixerChannel "Canal del Mezclador:"
#define MSGTR_PREFERENCES_Message "Algunas opciones requieren reiniciar la reproducción."
#define MSGTR_PREFERENCES_DXR3_VENC "Codificador de video:"
#define MSGTR_PREFERENCES_DXR3_LAVC "Usar LAVC (FFmpeg)"
#define MSGTR_PREFERENCES_FontEncoding1 "Unicode"
#define MSGTR_PREFERENCES_FontEncoding2 "Occidental (ISO-8859-1)"
#define MSGTR_PREFERENCES_FontEncoding3 "Occidental con euro (ISO-8859-15)"
#define MSGTR_PREFERENCES_FontEncoding4 "Eslavo/Centroeuropeo (ISO-8859-2)"
#define MSGTR_PREFERENCES_FontEncoding5 "Esperanto, Gallego, Maltés, Turco (ISO-8859-3)"
#define MSGTR_PREFERENCES_FontEncoding6 "Báltico (ISO-8859-4)"
#define MSGTR_PREFERENCES_FontEncoding7 "Cirílico (ISO-8859-5)"
#define MSGTR_PREFERENCES_FontEncoding8 "�?rabe (ISO-8859-6)"
#define MSGTR_PREFERENCES_FontEncoding9 "Griego moderno (ISO-8859-7)"
#define MSGTR_PREFERENCES_FontEncoding10 "Turco (ISO-8859-9)"
#define MSGTR_PREFERENCES_FontEncoding11 "Báltico (ISO-8859-13)"
#define MSGTR_PREFERENCES_FontEncoding12 "Céltico (ISO-8859-14)"
#define MSGTR_PREFERENCES_FontEncoding13 "Hebreo (ISO-8859-8)"
#define MSGTR_PREFERENCES_FontEncoding14 "Ruso (KOI8-R)"
#define MSGTR_PREFERENCES_FontEncoding15 "Belaruso (KOI8-U/RU)"
#define MSGTR_PREFERENCES_FontEncoding16 "Chino simplificado (CP936)"
#define MSGTR_PREFERENCES_FontEncoding17 "Chino tradicional (BIG5)"
#define MSGTR_PREFERENCES_FontEncoding18 "Japanés(SHIFT-JIS)"
#define MSGTR_PREFERENCES_FontEncoding19 "Coreano (CP949)"
#define MSGTR_PREFERENCES_FontEncoding20 "Thai (CP874)"
#define MSGTR_PREFERENCES_FontEncoding21 "Cirílico (Windows) (CP1251)"
#define MSGTR_PREFERENCES_FontEncoding22 "Eslavo/Centroeuropeo (Windows) (CP1250)"
#define MSGTR_PREFERENCES_FontEncoding23 "Arabic Windows (CP1256)"
#define MSGTR_PREFERENCES_FontNoAutoScale "Sin autoescalado"
#define MSGTR_PREFERENCES_FontPropWidth "Proporcional a la anchura de película"
#define MSGTR_PREFERENCES_FontPropHeight "Proporcional a la altura de película"
#define MSGTR_PREFERENCES_FontPropDiagonal "Proporcional al diagonal de película"
#define MSGTR_PREFERENCES_FontEncoding "Codificación:"
#define MSGTR_PREFERENCES_FontBlur "Blur:"
#define MSGTR_PREFERENCES_FontOutLine "Outline:"
#define MSGTR_PREFERENCES_FontTextScale "Escalado de texto:"
#define MSGTR_PREFERENCES_FontOSDScale "Escalado de OSD:"
#define MSGTR_PREFERENCES_SubtitleOSD "Subtítulos y OSD"
#define MSGTR_PREFERENCES_Cache "Cache si/no"
#define MSGTR_PREFERENCES_CacheSize "Tamaño de Cache: "
#define MSGTR_PREFERENCES_LoadFullscreen "Empezar en pantalla completa"
#define MSGTR_PREFERENCES_SaveWinPos "Guardar posición de la ventana"
#define MSGTR_PREFERENCES_XSCREENSAVER "Detener Salvador de Pantallas de X"
#define MSGTR_PREFERENCES_PlayBar "Habilitar barra de reproducción"
#define MSGTR_PREFERENCES_AutoSync "AutoSync si/no"
#define MSGTR_PREFERENCES_AutoSyncValue "Autosync: "
#define MSGTR_PREFERENCES_CDROMDevice "Dispositivo de CD-ROM:"
#define MSGTR_PREFERENCES_DVDDevice "Dispositivo de DVD:"
#define MSGTR_PREFERENCES_FPS "Cuadros por segundo de la Pelicula:"
#define MSGTR_PREFERENCES_ShowVideoWindow "Mostrar Ventana de Video cuando este inactiva"
#define MSGTR_PREFERENCES_ArtsBroken "Las versiones nuevas de aRts no son compatibles con GTK 1.x y botan GMPlayer!"

// -- aboutbox
#define MSGTR_ABOUT_UHU " Desarrollo de GUI patrocinado por UHU Linux\n"
#define MSGTR_ABOUT_Contributors "Contribuyentes al código y documentación\n"
#define MSGTR_ABOUT_Codecs_libs_contributions "Codecs y librerías de terceros\n"
#define MSGTR_ABOUT_Translations "Traducciones\n"
#define MSGTR_ABOUT_Skins "Skins\n"

// --- messagebox
#define MSGTR_MSGBOX_LABEL_FatalError "Error fatal"
#define MSGTR_MSGBOX_LABEL_Error "Error"
#define MSGTR_MSGBOX_LABEL_Warning "Advertencia"

// cfg.c
#define MSGTR_UnableToSaveOption "[cfg] No se puede guardar la opción '%s'.\n"

// interface.c
#define MSGTR_DeletingSubtitles "[GUI] Borrando subtítulos.\n"
#define MSGTR_LoadingSubtitles "[GUI] Carganado subtítulos: %s\n"
#define MSGTR_AddingVideoFilter "[GUI] Agregando filtro de video: %s\n"

// mw.c
#define MSGTR_NotAFile "Esto no parece ser un archivo: %s !\n"

// ws.c
#define MSGTR_WS_RemoteDisplay "[ws] Display remoto, desactivando XMITSHM.\n"
#define MSGTR_WS_NoXshm "[ws] Lo lamento, su sistema no soporta la extensión de memoria compartida X.\n"
#define MSGTR_WS_NoXshape "[ws] Lo lamento, su sistema no soporta la extensión XShape.\n"
#define MSGTR_WS_ColorDepthTooLow "[ws] Lo lamento, la profundidad de color es demasiado baja.\n"
#define MSGTR_WS_TooManyOpenWindows "[ws] Hay demasiadas ventanas abiertas.\n"
#define MSGTR_WS_ShmError "[ws] Error en la extensión de memoria compartida\n"
#define MSGTR_WS_NotEnoughMemoryDrawBuffer "[ws] Lo lamento, no hay suficiente memoria para el buffer de dibujo.\n"
#define MSGTR_WS_DpmsUnavailable "¿DPMS no disponible?\n"
#define MSGTR_WS_DpmsNotEnabled "No se pudo activar DPMS.\n"

// wsxdnd.c
#define MSGTR_WS_NotAFile "Esto no parece ser un archivo...\n"
#define MSGTR_WS_DDNothing "D&D: ¡No retorno nada!\n"

#endif

// ======================= video output drivers ========================

#define MSGTR_VOincompCodec "Disculpe, el dispositivo de salida de video es incompatible con este codec.\n"
#define MSGTR_VO_GenericError "Este error ha ocurrido"
#define MSGTR_VO_UnableToAccess "No es posible acceder"
#define MSGTR_VO_ExistsButNoDirectory "ya existe, pero no es un directorio."
#define MSGTR_VO_DirExistsButNotWritable "El directorio ya existe, pero no se puede escribir en él."
#define MSGTR_VO_DirExistsAndIsWritable "El directorio ya existe y se puede escribir en él."
#define MSGTR_VO_CantCreateDirectory "No es posible crear el directorio de salida."
#define MSGTR_VO_CantCreateFile "No es posible crear archivo de salida."
#define MSGTR_VO_DirectoryCreateSuccess "Directorio de salida creado exitosamente."
#define MSGTR_VO_ValueOutOfRange "Valor fuera de rango"
#define MSGTR_VO_NoValueSpecified "Valor no especificado."
#define MSGTR_VO_UnknownSuboptions "Subopción(es) desconocida(s)"

// aspect.c
#define MSGTR_LIBVO_ASPECT_NoSuitableNewResFound "[ASPECT] Aviso: ¡No se ha encontrado ninguna resolución nueva adecuada!\n"
#define MSGTR_LIBVO_ASPECT_NoNewSizeFoundThatFitsIntoRes "[ASPECT] Error: No new size found that fits into res!\n"

// font_load_ft.c
#define MSGTR_LIBVO_FONT_LOAD_FT_NewFaceFailed "Fallo en New_Face. Quizas el font path no es correcto.\nPor favor proporcione el archivo de fuentes de texto (~/.mplayer/subfont.ttf).\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_NewMemoryFaceFailed "Fallo en New_Memory_Face ..\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_SubFaceFailed "Fuente de subtítulo: fallo en load_sub_face.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_SubFontCharsetFailed "Fuente de subtítulo: fallo en prepare_charset.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_CannotPrepareSubtitleFont "Imposible preparar la fuente para subtítulos.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_CannotPrepareOSDFont "Imposible preparar la fuente para el OSD.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_CannotGenerateTables "Imposible generar tablas.\n"
#define MSGTR_LIBVO_FONT_LOAD_FT_DoneFreeTypeFailed "Fallo en FT_Done_FreeType.\n"

// sub.c
#define MSGTR_VO_SUB_Seekbar "Barra de navegación"
#define MSGTR_VO_SUB_Play "Play"
#define MSGTR_VO_SUB_Pause "Pausa"
#define MSGTR_VO_SUB_Stop "Stop"
#define MSGTR_VO_SUB_Rewind "Rebobinar"
#define MSGTR_VO_SUB_Forward "Avanzar"
#define MSGTR_VO_SUB_Clock "Reloj"
#define MSGTR_VO_SUB_Contrast "Contraste"
#define MSGTR_VO_SUB_Saturation "Saturación"
#define MSGTR_VO_SUB_Volume "Volumen"
#define MSGTR_VO_SUB_Brightness "Brillo"
#define MSGTR_VO_SUB_Hue "Hue"
#define MSGTR_VO_SUB_Balance "Balance"

// vo_3dfx.c
#define MSGTR_LIBVO_3DFX_Only16BppSupported "[VO_3DFX] Solo 16bpp soportado!"
#define MSGTR_LIBVO_3DFX_VisualIdIs "[VO_3DFX] El id visual es  %lx.\n"
#define MSGTR_LIBVO_3DFX_UnableToOpenDevice "[VO_3DFX] No pude abrir /dev/3dfx.\n"
#define MSGTR_LIBVO_3DFX_Error "[VO_3DFX] Error: %d.\n"
#define MSGTR_LIBVO_3DFX_CouldntMapMemoryArea "[VO_3DFX] No pude mapear las areas de memoria 3dfx: %p,%p,%d.\n"
#define MSGTR_LIBVO_3DFX_DisplayInitialized "[VO_3DFX] Inicializado: %p.\n"
#define MSGTR_LIBVO_3DFX_UnknownSubdevice "[VO_3DFX] sub-dispositivo desconocido: %s.\n"

// vo_aa.c
#define MSGTR_VO_AA_HelpHeader "\n\nAquí estan las subopciones del vo_aa aalib:\n"
#define MSGTR_VO_AA_AdditionalOptions "Opciones adicionales provistas por vo_aa:\n" \
"  help        mostrar esta ayuda\n" \
"  osdcolor    elegir color de osd\n  subcolor    elegir color de subtitlos\n" \
"        los parámetros de color son:\n           0 : normal\n" \
"           1 : oscuro\n           2 : bold\n           3 : boldfont\n" \
"           4 : reverso\n           5 : especial\n\n\n"

// vo_dxr3.c
#define MSGTR_LIBVO_DXR3_UnableToLoadNewSPUPalette "[VO_DXR3] No pude cargar la nueva paleta SPU!\n"
#define MSGTR_LIBVO_DXR3_UnableToSetPlaymode "[VO_DXR3] No pude setear el playmode!\n"
#define MSGTR_LIBVO_DXR3_UnableToSetSubpictureMode "[VO_DXR3] No pude setear el modo del subpicture!\n"
#define MSGTR_LIBVO_DXR3_UnableToGetTVNorm "[VO_DXR3] No pude obtener la norma de TV!\n"
#define MSGTR_LIBVO_DXR3_AutoSelectedTVNormByFrameRate "[VO_DXR3] Norma de TV autoeleccionada a partir del frame rate: "
#define MSGTR_LIBVO_DXR3_UnableToSetTVNorm "[VO_DXR3] Imposible setear la norma de TV!\n"
#define MSGTR_LIBVO_DXR3_SettingUpForNTSC "[VO_DXR3] Configurando para NTSC.\n"
#define MSGTR_LIBVO_DXR3_SettingUpForPALSECAM "[VO_DXR3] Configurando para PAL/SECAM.\n"
#define MSGTR_LIBVO_DXR3_SettingAspectRatioTo43 "[VO_DXR3] Seteando la relación de aspecto a 4:3.\n"
#define MSGTR_LIBVO_DXR3_SettingAspectRatioTo169 "[VO_DXR3] Seteando la relación de aspecto a 16:9.\n"
#define MSGTR_LIBVO_DXR3_OutOfMemory "[VO_DXR3] Ouch! me quede sin memoria!\n"
#define MSGTR_LIBVO_DXR3_UnableToAllocateKeycolor "[VO_DXR3] No pude disponer el keycolor!\n"
#define MSGTR_LIBVO_DXR3_UnableToAllocateExactKeycolor "[VO_DXR3] No pude disponer el keycolor exacto, voy a utilizar el mas parecido (0x%lx).\n"
#define MSGTR_LIBVO_DXR3_Uninitializing "[VO_DXR3] Uninitializing.\n"
#define MSGTR_LIBVO_DXR3_FailedRestoringTVNorm "[VO_DXR3] No pude restaurar la norma de TV!\n"
#define MSGTR_LIBVO_DXR3_EnablingPrebuffering "[VO_DXR3] Habilitando prebuffering.\n"
#define MSGTR_LIBVO_DXR3_UsingNewSyncEngine "[VO_DXR3] Utilizando el nuevo motor de syncronía.\n"
#define MSGTR_LIBVO_DXR3_UsingOverlay "[VO_DXR3] Utilizando overlay.\n"
#define MSGTR_LIBVO_DXR3_ErrorYouNeedToCompileMplayerWithX11 "[VO_DXR3] Error: Necesitas compilar MPlayer con las librerias x11 y sus headers para utilizar overlay.\n"
#define MSGTR_LIBVO_DXR3_WillSetTVNormTo "[VO_DXR3] Voy a setear la norma de TV a: "
#define MSGTR_LIBVO_DXR3_AutoAdjustToMovieFrameRatePALPAL60 "Auto-adjustando al framerate del video (PAL/PAL-60)"
#define MSGTR_LIBVO_DXR3_AutoAdjustToMovieFrameRatePALNTSC "Auto-adjustando al framerate del video (PAL/NTSC)"
#define MSGTR_LIBVO_DXR3_UseCurrentNorm "Utilizar norma actual."
#define MSGTR_LIBVO_DXR3_UseUnknownNormSuppliedCurrentNorm "Norma sugerida: desconocida. Utilizando norma actual."
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingTrying "[VO_DXR3] Error abriendo %s para escribir, intentando /dev/em8300 en su lugar.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingTryingMV "[VO_DXR3] Error abriendo %s para escribir, intentando /dev/em8300_mv en su lugar.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingAsWell "[VO_DXR3] Nuevamente error abriendo /dev/em8300 para escribir!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingAsWellMV "[VO_DXR3] Nuevamente error abriendo /dev/em8300_mv para escribir!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_Opened "[VO_DXR3] Abierto: %s.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingTryingSP "[VO_DXR3] Error abriendo %s para escribir, intentando /dev/em8300_sp en su lugar.\n"
#define MSGTR_LIBVO_DXR3_ErrorOpeningForWritingAsWellSP "[VO_DXR3] Nuevamente error abriendo /dev/em8300_sp para escribir!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_UnableToOpenDisplayDuringHackSetup "[VO_DXR3] Unable to open display during overlay hack setup!\n"
#define MSGTR_LIBVO_DXR3_UnableToInitX11 "[VO_DXR3] No puede inicializar x11!\n"
#define MSGTR_LIBVO_DXR3_FailedSettingOverlayAttribute "[VO_DXR3] Fallé tratando de setear el atributo overlay.\n"
#define MSGTR_LIBVO_DXR3_FailedSettingOverlayScreen "[VO_DXR3] Fallé tratando de setear el overlay screen!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedEnablingOverlay "[VO_DXR3] Fallé habilitando overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedResizingOverlayWindow "[VO_DXR3] Fallé tratando de redimiencionar la ventana overlay!\n"
#define MSGTR_LIBVO_DXR3_FailedSettingOverlayBcs "[VO_DXR3] Fallé seteando el bcs overlay!\n"
#define MSGTR_LIBVO_DXR3_FailedGettingOverlayYOffsetValues "[VO_DXR3] Fallé tratando de obtener los valores Y-offset del overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedGettingOverlayXOffsetValues "[VO_DXR3] Fallé tratando de obtener los valores X-offset del overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_FailedGettingOverlayXScaleCorrection "[VO_DXR3] Fallé obteniendo la corrección X scale del overlay!\nSaliendo.\n"
#define MSGTR_LIBVO_DXR3_YOffset "[VO_DXR3] Yoffset: %d.\n"
#define MSGTR_LIBVO_DXR3_XOffset "[VO_DXR3] Xoffset: %d.\n"
#define MSGTR_LIBVO_DXR3_XCorrection "[VO_DXR3] Xcorrection: %d.\n"
#define MSGTR_LIBVO_DXR3_FailedSetSignalMix "[VO_DXR3] Fallé seteando el signal mix!\n"

// vo_jpeg.c
#define MSGTR_VO_JPEG_ProgressiveJPEG "JPEG progresivo habilitado."
#define MSGTR_VO_JPEG_NoProgressiveJPEG "JPEG progresivo deshabilitado."
#define MSGTR_VO_JPEG_BaselineJPEG "Baseline JPEG habilitado."
#define MSGTR_VO_JPEG_NoBaselineJPEG "Baseline JPEG deshabilitado."

// vo_mga.c
#define MSGTR_LIBVO_MGA_AspectResized "[VO_MGA] aspect(): redimencionado a %dx%d.\n"
#define MSGTR_LIBVO_MGA_Uninit "[VO] uninit!\n"

// mga_template.c
#define MSGTR_LIBVO_MGA_ErrorInConfigIoctl "[MGA] Error en mga_vid_config ioctl (versión de mga_vid.o erronea?)"
#define MSGTR_LIBVO_MGA_CouldNotGetLumaValuesFromTheKernelModule "[MGA] No pude obtener los valores de luma desde el módulo del kernel!\n"
#define MSGTR_LIBVO_MGA_CouldNotSetLumaValuesFromTheKernelModule "[MGA] No pude setear los valores de luma que obtuve desde el módulo del kernel!\n"
#define MSGTR_LIBVO_MGA_ScreenWidthHeightUnknown "[MGA] Ancho/Alto de la pantalla desconocidos!\n"
#define MSGTR_LIBVO_MGA_InvalidOutputFormat "[MGA] Formáto de salida inválido %0X\n"
#define MSGTR_LIBVO_MGA_IncompatibleDriverVersion "[MGA] La versión de tu driver mga_vid no es compatible con esta versión de MPlayer!\n"
#define MSGTR_LIBVO_MGA_CouldntOpen "[MGA] No pude abrir: %s\n"
#define MSGTR_LIBVO_MGA_ResolutionTooHigh "[MGA] La resolución de la fuente es en por lo menos una dimensión mas grande que 1023x1023. Por favor escale en software o use -lavdopts lowres=1\n"
#define MSGTR_LIBVO_MGA_mgavidVersionMismatch "[MGA] Las versiones del controlador mga_vid del kernel (%u) y MPlayer (%u) no coinciden\n"

// vo_null.c
#define MSGTR_LIBVO_NULL_UnknownSubdevice "[VO_NULL] Sub-dispositivo desconocido: %s.\n"

// vo_png.c
#define MSGTR_LIBVO_PNG_Warning1 "[VO_PNG] Advertencia: nivel de compresión seteado a 0, compresión desabilitada!\n"
#define MSGTR_LIBVO_PNG_Warning2 "[VO_PNG] Info: Utiliza -vo png:z=<n> para setear el nivel de compresión desde 0 a 9.\n"
#define MSGTR_LIBVO_PNG_Warning3 "[VO_PNG] Info: (0 = sin compresión, 1 = la más rápida y baja - 9 la más lenta y alta)\n"
#define MSGTR_LIBVO_PNG_ErrorOpeningForWriting "\n[VO_PNG] Error abriendo '%s' para escribir!\n"
#define MSGTR_LIBVO_PNG_ErrorInCreatePng "[VO_PNG] Error en create_png.\n"

// vo_pnm.c
#define MSGTR_VO_PNM_ASCIIMode "modo ASCII habilitado."
#define MSGTR_VO_PNM_RawMode "Raw mode habilitado."
#define MSGTR_VO_PNM_PPMType "Escribiré archivos PPM."
#define MSGTR_VO_PNM_PGMType "Escribiré archivos PGM."
#define MSGTR_VO_PNM_PGMYUVType "Escribiré archivos PGMYUV."

// vo_sdl.c
#define MSGTR_LIBVO_SDL_CouldntGetAnyAcceptableSDLModeForOutput "[VO_SDL] No pude obtener ni un solo modo SDL aceptable para la salida.\n"
#define MSGTR_LIBVO_SDL_SetVideoModeFailed "[VO_SDL] set_video_mode: SDL_SetVideoMode fallido: %s.\n"
#define MSGTR_LIBVO_SDL_SetVideoModeFailedFull "[VO_SDL] Set_fullmode: SDL_SetVideoMode fallido: %s.\n"
#define MSGTR_LIBVO_SDL_MappingI420ToIYUV "[VO_SDL] Mapeando I420 a IYUV.\n"
#define MSGTR_LIBVO_SDL_UnsupportedImageFormat "[VO_SDL] Formato de imagen no soportado (0x%X).\n"
#define MSGTR_LIBVO_SDL_InfoPleaseUseVmOrZoom "[VO_SDL] Info - por favor utiliza -vm ó -zoom para cambiar a la mejor resolución.\n"
#define MSGTR_LIBVO_SDL_FailedToSetVideoMode "[VO_SDL] Fallè tratando de setear el modo de video: %s.\n"
#define MSGTR_LIBVO_SDL_CouldntCreateAYUVOverlay "[VO_SDL] No pude crear un overlay YUV: %s.\n"
#define MSGTR_LIBVO_SDL_CouldntCreateARGBSurface "[VO_SDL] No pude crear una superficie RGB: %s.\n"
#define MSGTR_LIBVO_SDL_UsingDepthColorspaceConversion "[VO_SDL] Utilizando conversión de depth/colorspace, va a andar un poco más lento.. (%ibpp -> %ibpp).\n"
#define MSGTR_LIBVO_SDL_UnsupportedImageFormatInDrawslice "[VO_SDL] Formato no soportado de imagen en draw_slice, contacta a los desarrolladores de MPlayer!\n"
#define MSGTR_LIBVO_SDL_BlitFailed "[VO_SDL] Blit falló: %s.\n"
#define MSGTR_LIBVO_SDL_InitializationFailed "[VO_SDL] Fallo la inicialización de SDL: %s.\n"
#define MSGTR_LIBVO_SDL_UsingDriver "[VO_SDL] Utilizando el driver: %s.\n"

// vo_svga.c
#define MSGTR_LIBVO_SVGA_ForcedVidmodeNotAvailable "[VO_SVGA] El vid_mode forzado %d (%s) no esta disponible.\n"
#define MSGTR_LIBVO_SVGA_ForcedVidmodeTooSmall "[VO_SVGA] El vid_mode forzado %d (%s) es muy pequeño.\n"
#define MSGTR_LIBVO_SVGA_Vidmode "[VO_SVGA] Vid_mode: %d, %dx%d %dbpp.\n"
#define MSGTR_LIBVO_SVGA_VgasetmodeFailed "[VO_SVGA] Vga_setmode(%d) fallido.\n"
#define MSGTR_LIBVO_SVGA_VideoModeIsLinearAndMemcpyCouldBeUsed "[VO_SVGA] El modo de vídeo es lineal y memcpy se puede usar para la transferencia de imágenes.\n"
#define MSGTR_LIBVO_SVGA_VideoModeHasHardwareAcceleration "[VO_SVGA] El modo de video dispone de aceleración por hardware y put_image puede ser utilizada.\n"
#define MSGTR_LIBVO_SVGA_IfItWorksForYouIWouldLikeToKnow "[VO_SVGA] Si le ha funcionado nos gustaría saberlo.\n[VO_SVGA] (mande un registro con `mplayer test.avi -v -v -v -v &> svga.log`). ¡Gracias!\n"
#define MSGTR_LIBVO_SVGA_VideoModeHas "[VO_SVGA] El modo de video tiene %d pagina(s).\n"
#define MSGTR_LIBVO_SVGA_CenteringImageStartAt "[VO_SVGA] Centrando imagen, comenzando en (%d,%d)\n"
#define MSGTR_LIBVO_SVGA_UsingVidix "[VO_SVGA] Utilizando VIDIX. w=%i h=%i  mw=%i mh=%i\n"

// vo_tdfx_vid.c
#define MSGTR_LIBVO_TDFXVID_Move "[VO_TDXVID] Move %d(%d) x %d => %d.\n"
#define MSGTR_LIBVO_TDFXVID_AGPMoveFailedToClearTheScreen "[VO_TDFXVID] El AGP move falló al tratar de limpiar la pantalla.\n"
#define MSGTR_LIBVO_TDFXVID_BlitFailed "[VO_TDFXVID] Fallo el Blit.\n"
#define MSGTR_LIBVO_TDFXVID_NonNativeOverlayFormatNeedConversion "[VO_TDFXVID] El formato overlay no-nativo requiere conversión.\n"
#define MSGTR_LIBVO_TDFXVID_UnsupportedInputFormat "[VO_TDFXVID] Formato de entrada no soportado 0x%x.\n"
#define MSGTR_LIBVO_TDFXVID_OverlaySetupFailed "[VO_TDFXVID] Fallo en la configuración del overlay.\n"
#define MSGTR_LIBVO_TDFXVID_OverlayOnFailed "[VO_TDFXVID] Falló el Overlay on .\n"
#define MSGTR_LIBVO_TDFXVID_OverlayReady "[VO_TDFXVID] Overlay listo: %d(%d) x %d @ %d => %d(%d) x %d @ %d.\n"
#define MSGTR_LIBVO_TDFXVID_TextureBlitReady "[VO_TDFXVID] Blit de textura listo: %d(%d) x %d @ %d => %d(%d) x %d @ %d.\n"
#define MSGTR_LIBVO_TDFXVID_OverlayOffFailed "[VO_TDFXVID] Falló el Overlay off\n"
#define MSGTR_LIBVO_TDFXVID_CantOpen "[VO_TDFXVID] No pude abrir %s: %s.\n"
#define MSGTR_LIBVO_TDFXVID_CantGetCurrentCfg "[VO_TDFXVID] No pude obtener la cfg actual: %s.\n"
#define MSGTR_LIBVO_TDFXVID_MemmapFailed "[VO_TDFXVID] Falló el Memmap!!!!!\n"
#define MSGTR_LIBVO_TDFXVID_GetImageTodo "Get image todo.\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailed "[VO_TDFXVID] Fallo el AGP move.\n"
#define MSGTR_LIBVO_TDFXVID_SetYuvFailed "[VO_TDFXVID] Falló set yuv\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailedOnYPlane "[VO_TDFXVID] El AGP move falló en el plano Y\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailedOnUPlane "[VO_TDFXVID] El AGP move falló en el plano U\n"
#define MSGTR_LIBVO_TDFXVID_AgpMoveFailedOnVPlane "[VO_TDFXVID] El AGP move fallo en el plano V\n"
#define MSGTR_LIBVO_TDFXVID_UnknownFormat "[VO_TDFXVID] Qué es esto como formato 0x%x?\n"

// vo_tdfxfb.c
#define MSGTR_LIBVO_TDFXFB_CantOpen "[VO_TDFXFB] No pude abrir %s: %s.\n"
#define MSGTR_LIBVO_TDFXFB_ProblemWithFbitgetFscreenInfo "[VO_TDFXFB] Problema con el ioctl FBITGET_FSCREENINFO: %s.\n"
#define MSGTR_LIBVO_TDFXFB_ProblemWithFbitgetVscreenInfo "[VO_TDFXFB] Problema con el ioctl FBITGET_VSCREENINFO: %s.\n"
#define MSGTR_LIBVO_TDFXFB_ThisDriverOnlySupports "[VO_TDFXFB] Este driver solo soporta las 3Dfx Banshee, Voodoo3 y Voodoo 5.\n"
#define MSGTR_LIBVO_TDFXFB_OutputIsNotSupported "[VO_TDFXFB] La salida %d bpp no esta soporatda.\n"
#define MSGTR_LIBVO_TDFXFB_CouldntMapMemoryAreas "[VO_TDFXFB] No pude mapear las áreas de memoria: %s.\n"
#define MSGTR_LIBVO_TDFXFB_BppOutputIsNotSupported "[VO_TDFXFB] La salida %d bpp no esta soportada (esto de debería haber pasado).\n"
#define MSGTR_LIBVO_TDFXFB_SomethingIsWrongWithControl "[VO_TDFXFB] Uh! algo anda mal con control().\n"
#define MSGTR_LIBVO_TDFXFB_NotEnoughVideoMemoryToPlay "[VO_TDFXFB] No hay suficiente memoria de video para reproducir esta pelicula. Intenta con una resolución más baja.\n"
#define MSGTR_LIBVO_TDFXFB_ScreenIs "[VO_TDFXFB] La pantalla es %dx%d a %d bpp, en %dx%d a %d bpp, la norma es %dx%d.\n"

// vo_tga.c
#define MSGTR_LIBVO_TGA_UnknownSubdevice "[VO_TGA] Sub-dispositivo desconocido: %s\n"

// vo_vesa.c
#define MSGTR_LIBVO_VESA_FatalErrorOccurred "[VO_VESA] Error fatal! no puedo continuar.\n"
#define MSGTR_LIBVO_VESA_UnknownSubdevice "[VO_VESA] Sub-dispositivo desconocido: '%s'.\n"
#define MSGTR_LIBVO_VESA_YouHaveTooLittleVideoMemory "[VO_VESA] Tienes muy poca memoria de video para este modo:\n[VO_VESA] Requiere: %08lX tienes: %08lX.\n"
#define MSGTR_LIBVO_VESA_YouHaveToSpecifyTheCapabilitiesOfTheMonitor "[VO_VESA] Tienes que especificar las capacidades del monitor. No voy a cambiar la tasa de refresco.\n"
#define MSGTR_LIBVO_VESA_UnableToFitTheMode "[VO_VESA] No pude encajar este modo en las limitaciones del  monitor. No voy a cambiar la tasa de refresco.\n"
#define MSGTR_LIBVO_VESA_DetectedInternalFatalError "[VO_VESA] Error fatal interno detectado: init se llamado despues de preinit.\n"
#define MSGTR_LIBVO_VESA_SwitchFlipIsNotSupported "[VO_VESA] Switch -flip no esta soportado.\n"
#define MSGTR_LIBVO_VESA_PossibleReasonNoVbe2BiosFound "[VO_VESA] Razón posible: No se encontró una BIOS VBE2.\n"
#define MSGTR_LIBVO_VESA_FoundVesaVbeBiosVersion "[VO_VESA] Enontré una versión de BIOS VESA VBE %x.%x Revisión: %x.\n"
#define MSGTR_LIBVO_VESA_VideoMemory "[VO_VESA] Memoria de video: %u Kb.\n"
#define MSGTR_LIBVO_VESA_Capabilites "[VO_VESA] Capacidades VESA: %s %s %s %s %s.\n"
#define MSGTR_LIBVO_VESA_BelowWillBePrintedOemInfo "[VO_VESA] !!! abajo se imprimira información OEM. !!!\n"
#define MSGTR_LIBVO_VESA_YouShouldSee5OemRelatedLines "[VO_VESA] Deberias ver 5 lineas con respecto a OEM abajo; sino, has arruinado vm86.\n"
#define MSGTR_LIBVO_VESA_OemInfo "[VO_VESA] Información OEM: %s.\n"
#define MSGTR_LIBVO_VESA_OemRevision "[VO_VESA] OEM Revisión: %x.\n"
#define MSGTR_LIBVO_VESA_OemVendor "[VO_VESA] OEM vendor: %s.\n"
#define MSGTR_LIBVO_VESA_OemProductName "[VO_VESA] OEM Product Name: %s.\n"
#define MSGTR_LIBVO_VESA_OemProductRev "[VO_VESA] OEM Product Rev: %s.\n"
#define MSGTR_LIBVO_VESA_Hint "[VO_VESA] Consejo: Para que el TV_Out te funcione tienes que conectarla\n"\
"[VO_VESA] antes de que el pc bootee por que la BIOS VESA solo se inicializa a si misma durante el POST.\n"
#define MSGTR_LIBVO_VESA_UsingVesaMode "[VO_VESA] Utilizando el modo VESA (%u) = %x [%ux%u@%u]\n"
#define MSGTR_LIBVO_VESA_CantInitializeSwscaler "[VO_VESA] No pude inicializar el SwScaler.\n"
#define MSGTR_LIBVO_VESA_CantUseDga "[VO_VESA] No puedo utilizar DGA. fuersa el modo bank switching. :(\n"
#define MSGTR_LIBVO_VESA_UsingDga "[VO_VESA] Utilizand DGA (recursos físicos: %08lXh, %08lXh)"
#define MSGTR_LIBVO_VESA_CantUseDoubleBuffering "[VO_VESA] No puedo utilizar double buffering: No hay suficiente memoria de video.\n"
#define MSGTR_LIBVO_VESA_CantFindNeitherDga "[VO_VESA] No pude encontrar DGA o un cuadro de ventana relocatable.\n"
#define MSGTR_LIBVO_VESA_YouveForcedDga "[VO_VESA] Tenemos DGA forzado. Saliendo.\n"
#define MSGTR_LIBVO_VESA_CantFindValidWindowAddress "[VO_VESA] No pude encontrar una dirección de ventana válida.\n"
#define MSGTR_LIBVO_VESA_UsingBankSwitchingMode "[VO_VESA] Utilizando modo bank switching (recursos físicos: %08lXh, %08lXh).\n"
#define MSGTR_LIBVO_VESA_CantAllocateTemporaryBuffer "[VO_VESA] No pude disponer de un buffer temporal.\n"
#define MSGTR_LIBVO_VESA_SorryUnsupportedMode "[VO_VESA] Sorry, formato no soportado -- trata con -x 640 -zoom.\n"
#define MSGTR_LIBVO_VESA_OhYouReallyHavePictureOnTv "[VO_VESA] Oh! mira! realmente tienes una imagen en la TV!\n"
#define MSGTR_LIBVO_VESA_CantInitialozeLinuxVideoOverlay "[VO_VESA] No pude inicializar el Video Overlay Linux.\n"
#define MSGTR_LIBVO_VESA_UsingVideoOverlay "[VO_VESA] Utilizando overlay de video: %s.\n"
#define MSGTR_LIBVO_VESA_CantInitializeVidixDriver "[VO_VESA] No pude inicializar el driver VIDIX.\n"
#define MSGTR_LIBVO_VESA_UsingVidix "[VO_VESA] Utilizando VIDIX.\n"
#define MSGTR_LIBVO_VESA_CantFindModeFor "[VO_VESA] No pude encontrar un modo para: %ux%u@%u.\n"
#define MSGTR_LIBVO_VESA_InitializationComplete "[VO_VESA] Inicialización VESA completa.\n"

// vesa_lvo.c
#define MSGTR_LIBVO_VESA_ThisBranchIsNoLongerSupported "[VESA_LVO] Este branch ya no esta soportado.\n[VESA_LVO] Por favor utiliza -vo vesa:vidix en su lugar.\n"
#define MSGTR_LIBVO_VESA_CouldntOpen "[VESA_LVO] No pude abrir: '%s'\n"
#define MSGTR_LIBVO_VESA_InvalidOutputFormat "[VESA_LVI] Formato de salida inválido: %s(%0X)\n"
#define MSGTR_LIBVO_VESA_IncompatibleDriverVersion "[VESA_LVO] La version de tu driver fb_vid no es compatible con esta versión de MPlayer!\n"

// vo_x11.c
#define MSGTR_LIBVO_X11_DrawFrameCalled "[VO_X11] draw_frame() llamada!!!!!!\n"

// vo_xv.c
#define MSGTR_LIBVO_XV_DrawFrameCalled "[VO_XV] draw_frame() llamada!!!!!!\n"
#define MSGTR_LIBVO_XV_SharedMemoryNotSupported "[VO_XV] Memoria compartida no soportada\nVolviendo al Xv normal.\n"
#define MSGTR_LIBVO_XV_XvNotSupportedByX11 "[VO_XV] Perdone, Xv no está soportado por esta versión/driver de X11\n[VO_XV] ******** Pruebe con  -vo x11  o  -vo sdl  *********\n"
#define MSGTR_LIBVO_XV_XvQueryAdaptorsFailed  "[VO_XV] XvQueryAdaptors ha fallado.\n"
#define MSGTR_LIBVO_XV_InvalidPortParameter "[VO_XV] Parámetro de puerto inválido, invalidándolo con el puerto 0.\n"
#define MSGTR_LIBVO_XV_CouldNotGrabPort "[VO_XV] No se ha podido fijar el puerto %i.\n"
#define MSGTR_LIBVO_XV_CouldNotFindFreePort "[VO_XV] No se ha podido encontrar ningún puerto Xvideo libre - quizás otro proceso ya lo está usando\n"\
"[VO_XV] usándolo. Cierre todas las aplicaciones de vídeo, y pruebe otra vez\n"\
"[VO_XV] Si eso no ayuda, vea 'mplayer -vo help' para otros (no-xv) drivers de salida de vídeo.\n"
#define MSGTR_LIBVO_XV_NoXvideoSupport "[VO_XV] Parece que no hay soporte Xvideo para su tarjeta gráfica.\n"\
"[VO_XV] Ejecute 'xvinfo' para verificar el soporte Xv y lea\n"\
"[VO_XV] DOCS/HTML/en/video.html#xv!\n"\
"[VO_XV] Revise otros drivers de salida de video (no-xv) con 'mplayer -vo help'.\n"\
"[VO_XV] Intente -vo x11.\n"
#define MSGTR_VO_XV_ImagedimTooHigh "Las dimensiones de la imagen de origen son demasiado grandes: %ux%u (el máximo es %ux%u)\n"

// vo_yuv4mpeg.c
#define MSGTR_VO_YUV4MPEG_InterlacedHeightDivisibleBy4 "Modo interlaceado requiere que la altura de la imagen sea divisible por 4."
#define MSGTR_VO_YUV4MPEG_InterlacedLineBufAllocFail "No se pudo reservar buffer de línea para el modo interlaceado."
#define MSGTR_VO_YUV4MPEG_WidthDivisibleBy2 "El ancho de la imagen debe ser divisible por 2."
#define MSGTR_VO_YUV4MPEG_OutFileOpenError "Imposible obtener memoria o descriptor de archivos para escribir \"%s\"!"
#define MSGTR_VO_YUV4MPEG_OutFileWriteError "Error escribiendo imagen a la salida!"
#define MSGTR_VO_YUV4MPEG_UnknownSubDev "Se desconoce el subdevice: %s"
#define MSGTR_VO_YUV4MPEG_InterlacedTFFMode "Usando modo de salida interlaceado, top-field primero."
#define MSGTR_VO_YUV4MPEG_InterlacedBFFMode "Usando modo de salida interlaceado, bottom-field primero."
#define MSGTR_VO_YUV4MPEG_ProgressiveMode "Usando (por defecto) modo de cuadro progresivo."

// vobsub_vidix.c
#define MSGTR_LIBVO_SUB_VIDIX_CantStartPlayback "[VO_SUB_VIDIX] No puedo iniciar la reproducción: %s\n"
#define MSGTR_LIBVO_SUB_VIDIX_CantStopPlayback "[VO_SUB_VIDIX] No puedo parar la reproducción: %s\n"
#define MSGTR_LIBVO_SUB_VIDIX_InterleavedUvForYuv410pNotSupported "[VO_SUB_VIDIX] Interleaved uv para yuv410p no soportado.\n"
#define MSGTR_LIBVO_SUB_VIDIX_DummyVidixdrawsliceWasCalled "[VO_SUB_VIDIX] Dummy vidix_draw_slice() fue llamada.\n"
#define MSGTR_LIBVO_SUB_VIDIX_DummyVidixdrawframeWasCalled "[VO_SUB_VIDIX] Dummy vidix_draw_frame() fue llamada.\n"
#define MSGTR_LIBVO_SUB_VIDIX_UnsupportedFourccForThisVidixDriver "[VO_SUB_VIDIX] fourcc no soportado para este driver vidix: %x (%s).\n"
#define MSGTR_LIBVO_SUB_VIDIX_VideoServerHasUnsupportedResolution "[VO_SUB_VIDIX] El servidor de video server tiene una resolución no soportada (%dx%d), soportadas: %dx%d-%dx%d.\n"
#define MSGTR_LIBVO_SUB_VIDIX_VideoServerHasUnsupportedColorDepth "[VO_SUB_VIDIX] Video server has unsupported color depth by vidix (%d).\n"
#define MSGTR_LIBVO_SUB_VIDIX_DriverCantUpscaleImage "[VO_SUB_VIDIX] El driver Vidix no puede ampliar la imagen (%d%d -> %d%d).\n"
#define MSGTR_LIBVO_SUB_VIDIX_DriverCantDownscaleImage "[VO_SUB_VIDIX] El driver Vidix no puede reducir la imagen (%d%d -> %d%d).\n"
#define MSGTR_LIBVO_SUB_VIDIX_CantConfigurePlayback "[VO_SUB_VIDIX] No pude configurar la reproducción: %s.\n"
#define MSGTR_LIBVO_SUB_VIDIX_YouHaveWrongVersionOfVidixLibrary "[VO_SUB_VIDIX] Tienes una versión erronea de la librería VIDIX.\n"
#define MSGTR_LIBVO_SUB_VIDIX_CouldntFindWorkingVidixDriver "[VO_SUB_VIDIX] No pude encontrar un driver VIDIX que funcione.\n"
#define MSGTR_LIBVO_SUB_VIDIX_CouldntGetCapability "[VO_SUB_VIDIX] No pude obtener la capacidad: %s.\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: ¡No se pudo enviar evento de pantalla completa EWMH!\n"
#define MSGTR_CouldNotFindXScreenSaver "xscreensaver_disable: no se pudo encontrar ventana XScreenSaver.\n"
#define MSGTR_SelectedVideoMode "XF86VM: Modo de video seleccionado %dx%d para tamaño de imagen %dx%d.\n"
#define MSGTR_InsertingAfVolume "[Mixer] No se ecnontró mezclador de volumen por hardware, insertando filtro de volumen.\n"
#define MSGTR_NoVolume "[Mixer] No hay un control de volumen disponible.\n"
#define MSGTR_NoBalance "[Mixer] No hay un control de balance disponible.\n"

// Controladores vo viejos que han sido reemplazados
#define MSGTR_VO_PGM_HasBeenReplaced "El controlador de salida pgm ha sido reemplazado por -vo pnm:pgmyuv.\n"
#define MSGTR_VO_MD5_HasBeenReplaced "El controlador de salida md5 ha sido reemplazado por -vo md5sum.\n"


// ======================= audio output drivers ========================

// audio_out.c
#define MSGTR_AO_ALSA9_1x_Removed "audio_out: los módulos alsa9 y alsa1x fueron eliminados, usa -ao alsa.\n"
#define MSGTR_AO_NoSuchDriver "No existe ese controlador de audio '%.*s'\n"
#define MSGTR_AO_FailedInit "Fallo al inicializar controlador de audio '%s'\n"

// ao_oss.c
#define MSGTR_AO_OSS_CantOpenMixer "[AO OSS] audio_setup: Imposible abrir dispositivo mezclador %s: %s\n"
#define MSGTR_AO_OSS_ChanNotFound "[AO OSS] audio_setup: El mezclador de la tarjeta de audio no tiene el canal '%s' usando valor por omisión.\n"
#define MSGTR_AO_OSS_CantOpenDev "[AO OSS] audio_setup: Imposible abrir dispositivo de audio %s: %s\n"
#define MSGTR_AO_OSS_CantMakeFd "[AO OSS] audio_setup: Imposible crear descriptor de archivo, bloqueando: %s\n"
#define MSGTR_AO_OSS_CantSet "[AO OSS] No puedo configurar el dispositivo de audio %s para salida %s, tratando %s...\n"
#define MSGTR_AO_OSS_CantSetChans "[AO OSS] audio_setup: Imposible configurar dispositivo de audio a %d channels.\n"
#define MSGTR_AO_OSS_CantUseGetospace "[AO OSS] audio_setup: El controlador no soporta SNDCTL_DSP_GETOSPACE :-(\n"
#define MSGTR_AO_OSS_CantUseSelect "[AO OSS]\n   ***  Su controlador de audio no soporta select()  ***\n Recompile MPlayer con #undef HAVE_AUDIO_SELECT en config.h !\n\n"
#define MSGTR_AO_OSS_CantReopen "[AO OSS]\n Error fatal: *** Imposible RE-ABRIR / RESETEAR DISPOSITIVO DE AUDIO *** %s\n"
#define MSGTR_AO_OSS_UnknownUnsupportedFormat "[AO OSS] Formato OSS Desconocido/No-soportado: %x.\n"

// ao_arts.c
#define MSGTR_AO_ARTS_CantInit "[AO ARTS] %s\n"
#define MSGTR_AO_ARTS_ServerConnect "[AO ARTS] Conectado al servidor de sonido.\n"
#define MSGTR_AO_ARTS_CantOpenStream "[AO ARTS] Imposible abrir stream.\n"
#define MSGTR_AO_ARTS_StreamOpen "[AO ARTS] Stream Abierto.\n"
#define MSGTR_AO_ARTS_BufferSize "[AO ARTS] Tamaño del buffer: %d\n"

// ao_dxr2.c
#define MSGTR_AO_DXR2_SetVolFailed "[AO DXR2] Fallo Seteando volumen en %d.\n"
#define MSGTR_AO_DXR2_UnsupSamplerate "[AO DXR2] dxr2: %d Hz no soportado, trate \"-aop list=resample\"\n"

// ao_esd.c
#define MSGTR_AO_ESD_CantOpenSound "[AO ESD] Fallo en esd_open_sound: %s\n"
#define MSGTR_AO_ESD_LatencyInfo "[AO ESD] latencia: [servidor: %0.2fs, red: %0.2fs] (ajuste %0.2fs)\n"
#define MSGTR_AO_ESD_CantOpenPBStream "[AO ESD] Fallo abriendo playback stream esd: %s\n"

// ao_mpegpes.c
#define MSGTR_AO_MPEGPES_CantSetMixer "[AO MPEGPES] Fallo configurando mezclador de audio DVB:%s\n"
#define MSGTR_AO_MPEGPES_UnsupSamplerate "[AO MPEGPES] %d Hz no soportado, trate de resamplear...\n"

// ao_pcm.c
#define MSGTR_AO_PCM_FileInfo "[AO PCM] Archivo: %s (%s)\nPCM: Samplerate: %iHz Canales: %s Formato %s\n"
#define MSGTR_AO_PCM_HintInfo "[AO PCM] Info: El volcado más rápido se logra con -vc null -vo null\nPCM: Info: Para escribir archivos de onda (WAVE) use -ao pcm:waveheader (valor por omisión).\n"
#define MSGTR_AO_PCM_CantOpenOutputFile "[AO PCM] Imposible abrir %s para escribir!\n"

// ao_sdl.c
#define MSGTR_AO_SDL_INFO "[AO SDL] Samplerate: %iHz Canales: %s Formato %s\n"
#define MSGTR_AO_SDL_DriverInfo "[AO SDL] usando controlador de audio: %s .\n"
#define MSGTR_AO_SDL_UnsupportedAudioFmt "[AO SDL] Formato de audio no soportado: 0x%x.\n"
#define MSGTR_AO_SDL_CantInit "[AO SDL] Error inicializando audio SDL: %s\n"
#define MSGTR_AO_SDL_CantOpenAudio "[AO SDL] Imposible abrir audio: %s\n"

// ao_sgi.c
#define MSGTR_AO_SGI_INFO "[AO SGI] control.\n"
#define MSGTR_AO_SGI_InitInfo "[AO SGI] init: Samplerate: %iHz Canales: %s Formato %s\n"
#define MSGTR_AO_SGI_InvalidDevice "[AO SGI] play: dispositivo inválido.\n"
#define MSGTR_AO_SGI_CantSetParms_Samplerate "[AO SGI] init: fallo en setparams: %s\nImposble configurar samplerate deseado.\n"
#define MSGTR_AO_SGI_CantSetAlRate "[AO SGI] init: AL_RATE No fue aceptado en el recurso dado.\n"
#define MSGTR_AO_SGI_CantGetParms "[AO SGI] init: fallo en getparams: %s\n"
#define MSGTR_AO_SGI_SampleRateInfo "[AO SGI] init: samplerate es ahora %f (el deseado es %f)\n"
#define MSGTR_AO_SGI_InitConfigError "[AO SGI] init: %s\n"
#define MSGTR_AO_SGI_InitOpenAudioFailed "[AO SGI] init: Imposible abrir canal de audio: %s\n"
#define MSGTR_AO_SGI_Uninit "[AO SGI] uninit: ...\n"
#define MSGTR_AO_SGI_Reset "[AO SGI] reseteando: ...\n"
#define MSGTR_AO_SGI_PauseInfo "[AO SGI] audio_pausa: ...\n"
#define MSGTR_AO_SGI_ResumeInfo "[AO SGI] audio_continuar: ...\n"

// ao_sun.c
#define MSGTR_AO_SUN_RtscSetinfoFailed "[AO SUN] rtsc: Fallo en SETINFO.\n"
#define MSGTR_AO_SUN_CantOpenAudioDev "[AO SUN] Imposible abrir dispositivo de audio %s, %s -> nosound.\n"
#define MSGTR_AO_SUN_UnsupSampleRate "[AO SUN] audio_setup: Su tarjeta no soporta el canal %d, %s, %d Hz samplerate.\n"
#define MSGTR_AO_SUN_CantUseSelect "[AO SUN]\n   ***  Su controlador de audio no soporta select()  ***\nRecompile MPlayer con #undef HAVE_AUDIO_SELECT en config.h !\n\n"
#define MSGTR_AO_SUN_CantReopenReset "[AO SUN]\nError fatal: *** IMPOSIBLE RE-ABRIR / RESETEAR DISPOSITIVO DE AUDIO (%s) ***\n"

// ao_alsa.c
#define MSGTR_AO_ALSA_InvalidMixerIndexDefaultingToZero "[AO_ALSA] �?ndice del mezclador inválido. Usando 0.\n"
#define MSGTR_AO_ALSA_MixerOpenError "[AO_ALSA] Mezclador, error abriendo: %s\n"
#define MSGTR_AO_ALSA_MixerAttachError "[AO_ALSA] Mezclador, adjunto %s error: %s\n"
#define MSGTR_AO_ALSA_MixerRegisterError "[AO_ALSA] Mezclador, error de registro: %s\n"
#define MSGTR_AO_ALSA_MixerLoadError "[AO_ALSA] Mezclador, error de carga: %s\n"
#define MSGTR_AO_ALSA_UnableToFindSimpleControl "[AO_ALSA] Incapaz de encontrar un control simple '%s',%i.\n"
#define MSGTR_AO_ALSA_ErrorSettingLeftChannel "[AO_ALSA] Error estableciendo el canal izquierdo, %s\n"
#define MSGTR_AO_ALSA_ErrorSettingRightChannel "[AO_ALSA] Error estableciendo el canal derecho, %s\n"
#define MSGTR_AO_ALSA_CommandlineHelp "\n[AO_ALSA] -ao alsa ayuda línea de comandos:\n"\
"[AO_ALSA] Ejemplo: mplayer -ao alsa:dispositivo=hw=0.3\n"\
"[AO_ALSA]   Establece como primera tarjeta el cuarto dispositivo de hardware.\n\n"\
"[AO_ALSA] Opciones:\n"\
"[AO_ALSA]   noblock\n"\
"[AO_ALSA]     Abre el dispositivo en modo sin bloqueo.\n"\
"[AO_ALSA]   device=<nombre-dispositivo>\n"\
"[AO_ALSA]     Establece el dispositivo (cambiar , por . y : por =)\n"
#define MSGTR_AO_ALSA_ChannelsNotSupported "[AO_ALSA] %d canales no están soportados.\n"
#define MSGTR_AO_ALSA_OpenInNonblockModeFailed "[AO_ALSA] La apertura en modo sin bloqueo ha fallado, intentando abrir en modo bloqueo.\n"
#define MSGTR_AO_ALSA_PlaybackOpenError "[AO_ALSA] Error de apertura en la reproducción: %s\n"
#define MSGTR_AO_ALSA_ErrorSetBlockMode "[AL_ALSA] Error estableciendo el modo bloqueo %s.\n"
#define MSGTR_AO_ALSA_UnableToGetInitialParameters "[AO_ALSA] Incapaz de obtener los parámetros iniciales: %s\n"
#define MSGTR_AO_ALSA_UnableToSetAccessType "[AO_ALSA] Incapaz de establecer el tipo de acceso: %s\n"
#define MSGTR_AO_ALSA_FormatNotSupportedByHardware "[AO_ALSA] Formato %s no soportado por el hardware, intentando la opción por defecto.\n"
#define MSGTR_AO_ALSA_UnableToSetFormat "[AO_ALSA] Incapaz de establecer el formato: %s\n"
#define MSGTR_AO_ALSA_UnableToSetChannels "[AO_ALSA] Incapaz de establecer los canales: %s\n"
#define MSGTR_AO_ALSA_UnableToDisableResampling "[AO_ALSA] Incapaz de deshabilitar el resampling:  %s\n"
#define MSGTR_AO_ALSA_UnableToSetSamplerate2 "[AO_ALSA] Incapaz de establecer el samplerate-2: %s\n"
#define MSGTR_AO_ALSA_UnableToSetBufferTimeNear "[AO_ALSA] Incapaz de establecer el tiempo del buffer: %s\n"
#define MSGTR_AO_ALSA_UnableToGetPeriodSize "[AO ALSA] Incapaz de obtener el tamaño del período: %s\n"
#define MSGTR_AO_ALSA_UnableToSetPeriods "[AO_ALSA] Incapaz de establecer períodos: %s\n"
#define MSGTR_AO_ALSA_UnableToSetHwParameters "[AO_ALSA] Incapaz de establecer parámatros de hw: %s\n"
#define MSGTR_AO_ALSA_UnableToGetBufferSize "[AO_ALSA] Incapaz de obtener el tamaño del buffer: %s\n"
#define MSGTR_AO_ALSA_UnableToGetSwParameters "[AO_ALSA] Incapaz de obtener parámatros de sw: %s\n"
#define MSGTR_AO_ALSA_UnableToSetSwParameters "[AO_ALSA] Incapaz de establecer parámetros de sw: %s\n"
#define MSGTR_AO_ALSA_UnableToGetBoundary "[AO_ALSA] Incapaz de obtener el límite: %s\n"
#define MSGTR_AO_ALSA_UnableToSetStartThreshold "[AO_ALSA] Incapaz de establecer el umbral de comienzo: %s\n"
#define MSGTR_AO_ALSA_UnableToSetStopThreshold "[AO_ALSA] Incapaz de establecer el umbral de parada: %s\n"
#define MSGTR_AO_ALSA_UnableToSetSilenceSize "[AO_ALSA] Incapaz de establecer el tamaño del silencio: %s\n"
#define MSGTR_AO_ALSA_PcmCloseError "[AO_ALSA] pcm error de clausura: %s\n"
#define MSGTR_AO_ALSA_NoHandlerDefined "[AO_ALSA] ¡Ningún manejador definido!\n"
#define MSGTR_AO_ALSA_PcmPrepareError "[AO_ALSA] pcm error de preparación: %s\n"
#define MSGTR_AO_ALSA_PcmPauseError "[AO_ALSA] pcm error de pausa: %s\n"
#define MSGTR_AO_ALSA_PcmDropError "[AO_ALSA] pcm error de pérdida: %s\n"
#define MSGTR_AO_ALSA_PcmResumeError "[AO_ALSA] pcm error volviendo al estado normal: %s\n"
#define MSGTR_AO_ALSA_DeviceConfigurationError "[AO_ALSA] Error de configuración del dispositivo."
#define MSGTR_AO_ALSA_PcmInSuspendModeTryingResume "[AO_ALSA] Pcm en modo suspendido, intentando volver al estado normal.\n"
#define MSGTR_AO_ALSA_WriteError "[AO_ALSA] Error de escritura: %s\n"
#define MSGTR_AO_ALSA_TryingToResetSoundcard "[AO_ALSA] Intentando resetear la tarjeta de sonido.\n"
#define MSGTR_AO_ALSA_CannotGetPcmStatus "[AO_ALSA] No se puede obtener el estado pcm: %s\n"

// ao_plugin.c
#define MSGTR_AO_PLUGIN_InvalidPlugin "[AO PLUGIN] Plugin inválido: %s\n"


// ======================= audio filters ================================

// af_scaletempo.c
#define MSGTR_AF_ValueOutOfRange MSGTR_VO_ValueOutOfRange

// af_ladspa.c
#define MSGTR_AF_LADSPA_AvailableLabels "Etiquetas disponibles en"
#define MSGTR_AF_LADSPA_WarnNoInputs "ADVERTENCIA! Este plugin LADSPA no tiene entradas de audio.\n La señal de entrada de audio se perderá."
#define MSGTR_AF_LADSPA_ErrMultiChannel "Plugins Multi-canal (>2) no están soportados (todavía).\n  Use solo plugins mono y estereo."
#define MSGTR_AF_LADSPA_ErrNoOutputs "Este plugín LADSPA no tiene salidas de audio."
#define MSGTR_AF_LADSPA_ErrInOutDiff "El número de entradas y de salidas  de audio de este plugin LADSPA son distintas."
#define MSGTR_AF_LADSPA_ErrFailedToLoad "Fallo la carga."
#define MSGTR_AF_LADSPA_ErrNoDescriptor "No se pudo encontrar la función ladspa_descriptor() en el archivo de biblioteca especificado."
#define MSGTR_AF_LADSPA_ErrLabelNotFound "No se puede encontrar la etiqueta en la biblioteca del plugin."
#define MSGTR_AF_LADSPA_ErrNoSuboptions "No se especificaron subopciones"
#define MSGTR_AF_LADSPA_ErrNoLibFile "No se especificó archivo de biblioteca"
#define MSGTR_AF_LADSPA_ErrNoLabel "No se especificó etiqueta del filtro"
#define MSGTR_AF_LADSPA_ErrNotEnoughControls "No se especificaron suficientes controles en la línea de comando"
#define MSGTR_AF_LADSPA_ErrControlBelow "%s: Control de entrada #%d esta abajo del límite inferior que es %0.4f.\n"
#define MSGTR_AF_LADSPA_ErrControlAbove "%s: Control de entrada #%d esta por encima del límite superior que es %0.4f.\n"

// format.c
#define MSGTR_AF_FORMAT_UnknownFormat "formato desconocido "


// ========================== INPUT =========================================

// joystick.c
#define MSGTR_INPUT_JOYSTICK_CantOpen "Imposible abrir joystick %s : %s\n"
#define MSGTR_INPUT_JOYSTICK_ErrReading "Error leyendo dispositivo joystick : %s\n"
#define MSGTR_INPUT_JOYSTICK_LoosingBytes "Joystick : perdimos %d bytes de datos\n"
#define MSGTR_INPUT_JOYSTICK_WarnLostSync "Joystick : Advertencia, init event, perdimos sync con el driver\n"
#define MSGTR_INPUT_JOYSTICK_WarnUnknownEvent "Joystick : Advertencia, tipo de evento desconocido %d\n"

// appleir.c
#define MSGTR_INPUT_APPLE_IR_CantOpen "Imposible abrir dispositivo Apple IR: %s\n"

// input.c
#define MSGTR_INPUT_INPUT_ErrCantRegister2ManyCmdFds "Demaciados fds de comandos, imposible registrar fd %d.\n"
#define MSGTR_INPUT_INPUT_ErrCantRegister2ManyKeyFds "Demaciados fds de teclas, imposible registrar fd %d.\n"
#define MSGTR_INPUT_INPUT_ErrArgMustBeInt "Comando %s: argumento %d no es un entero.\n"
#define MSGTR_INPUT_INPUT_ErrArgMustBeFloat "Comando %s: argumento %d no es punto flotante.\n"
#define MSGTR_INPUT_INPUT_ErrUnterminatedArg "Commando %s: argumento %d no está terminado.\n"
#define MSGTR_INPUT_INPUT_ErrUnknownArg "Argumento desconocido %d\n"
#define MSGTR_INPUT_INPUT_Err2FewArgs "Comando %s requiere a lo menos %d argumentos, solo encontramos %d hasta ahora.\n"
#define MSGTR_INPUT_INPUT_ErrReadingCmdFd "Error leyendo fd de comandos %d: %s\n"
#define MSGTR_INPUT_INPUT_ErrCmdBufferFullDroppingContent "Buffer de comandos de fd %d lleno: botando contenido\n"
#define MSGTR_INPUT_INPUT_ErrInvalidCommandForKey "Comando inválido asignado a la tecla %s"
#define MSGTR_INPUT_INPUT_ErrSelect "Error en Select: %s\n"
#define MSGTR_INPUT_INPUT_ErrOnKeyInFd "Error en fd %d de entrada de teclas\n"
#define MSGTR_INPUT_INPUT_ErrDeadKeyOnFd "Ingreso de tecla muerta en fd %d\n"
#define MSGTR_INPUT_INPUT_Err2ManyKeyDowns "Demaciados eventos de keydown al mismo tiempo\n"
#define MSGTR_INPUT_INPUT_ErrOnCmdFd "Error en fd de comandos %d\n"
#define MSGTR_INPUT_INPUT_ErrReadingInputConfig "Error leyendo archivo de configuración de input %s: %s\n"
#define MSGTR_INPUT_INPUT_ErrUnknownKey "Tecla desconocida '%s'\n"
#define MSGTR_INPUT_INPUT_ErrUnfinishedBinding "Asignación no terminada %s\n"
#define MSGTR_INPUT_INPUT_ErrBuffer2SmallForKeyName "El buffer es demaciado pequeño para este nombre de tecla: %s\n"
#define MSGTR_INPUT_INPUT_ErrNoCmdForKey "No se econtró un comando para la tecla %s"
#define MSGTR_INPUT_INPUT_ErrBuffer2SmallForCmd "Buffer demaciado pequeño para el comando %s\n"
#define MSGTR_INPUT_INPUT_ErrWhyHere "Que estamos haciendo aqui?\n"
#define MSGTR_INPUT_INPUT_ErrCantInitJoystick "No se puede inicializar la entrada del joystick\n"
#define MSGTR_INPUT_INPUT_ErrCantOpenFile "No se puede abrir %s: %s\n"
#define MSGTR_INPUT_INPUT_ErrCantInitAppleRemote "No se puede inicializar la entrada del Apple Remote.\n"

// lirc.c
#define MSGTR_LIRCopenfailed "Fallo al abrir el soporte para LIRC.\n"
#define MSGTR_LIRCcfgerr "Fallo al leer archivo de configuración de LIRC %s.\n"


// ========================== LIBMPDEMUX ===================================

// muxer.c, muxer_*.c
#define MSGTR_TooManyStreams "¡Demasiados streams!"
#define MSGTR_RawMuxerOnlyOneStream "El muxer rawaudio soporta sólo un stream de audio!\n"
#define MSGTR_IgnoringVideoStream "Ignorando stream de video!\n"
#define MSGTR_UnknownStreamType "Advertencia! tipo de stream desconocido: %d\n"
#define MSGTR_WarningLenIsntDivisible "¡Advertencia! ¡La longitud no es divisible por el tamaño del muestreo!\n"
#define MSGTR_MuxbufMallocErr "No se puede asignar memoria para el frame buffer del Muxer!\n"
#define MSGTR_MuxbufReallocErr "No se puede reasignar memoria para el frame buffer de del Muxer!\n"
#define MSGTR_WritingHeader "Escribiendo la cabecera...\n"
#define MSGTR_WritingTrailer "Escribiendo el índice...\n"

// demuxer.c, demux_*.c
#define MSGTR_AudioStreamRedefined "Advertencia! Cabecera de stream de audio %d redefinida!\n"
#define MSGTR_VideoStreamRedefined "Advertencia! Cabecera de stream de video %d redefinida!\n"
#define MSGTR_TooManyAudioInBuffer "\nDEMUXER: Demasiados (%d en %d bytes) paquetes de audio en el buffer!\n"
#define MSGTR_TooManyVideoInBuffer "\nDEMUXER: Demasiados (%d en %d bytes) paquetes de video en el buffer!\n"
#define MSGTR_MaybeNI "¿Estás reproduciendo un stream o archivo 'non-interleaved' o falló el codec?\n " \
                "Para archivos .AVI, intente forzar el modo 'non-interleaved' con la opción -ni.\n"
#define MSGTR_WorkAroundBlockAlignHeaderBug "AVI: Rodeo CBR-MP3 nBlockAlign"
#define MSGTR_SwitchToNi "\nDetectado .AVI mal interleaveado - cambiando al modo -ni!\n"
#define MSGTR_InvalidAudioStreamNosound "AVI: flujo de audio inválido ID: %d - ignorado (sin sonido)\n"
#define MSGTR_InvalidAudioStreamUsingDefault "AVI: flujo de audio inválido ID: %d - ignorado (usando default)\n"
#define MSGTR_ON2AviFormat "Formato ON2 AVI"
#define MSGTR_Detected_XXX_FileFormat "Detectado formato de archivo %s.\n"
#define MSGTR_DetectedAudiofile "Detectado archivo de audio.\n"
#define MSGTR_InvalidMPEGES "Stream MPEG-ES inválido? Contacta con el autor, podría ser un fallo.\n"
#define MSGTR_FormatNotRecognized "Este formato no está soportado o reconocido. Si este archivo es un AVI, ASF o MPEG, por favor contacte con el autor.\n"
#define MSGTR_SettingProcessPriority "Estableciendo la prioridad del proceso: %s\n"
#define MSGTR_FilefmtFourccSizeFpsFtime "[V] filefmt:%d  fourcc:0x%X  tamaño:%dx%d  fps:%5.3f  ftime:=%6.4f\n"
#define MSGTR_CannotInitializeMuxer "No se puede inicializar el muxer."
#define MSGTR_MissingVideoStream "¡No se encontró stream de video!\n"
#define MSGTR_MissingAudioStream "No se encontró el stream de audio, no se reproducirá sonido.\n"
#define MSGTR_MissingVideoStreamBug "¡¿Stream de video perdido!? Contacta con el autor, podría ser un fallo.\n"

#define MSGTR_DoesntContainSelectedStream "demux: El archivo no contiene el stream de audio o video seleccionado.\n"

#define MSGTR_NI_Forced "Forzado"
#define MSGTR_NI_Detected "Detectado"
#define MSGTR_NI_Message "%s formato de AVI 'NON-INTERLEAVED'.\n"

#define MSGTR_UsingNINI "Usando formato de AVI arruinado 'NON-INTERLEAVED'.\n"
#define MSGTR_CouldntDetFNo "No se puede determinar el número de cuadros (para una búsqueda absoluta).\n"
#define MSGTR_CantSeekRawAVI "No se puede avanzar o retroceder en un stream crudo .AVI (se requiere índice, prueba con -idx).\n"
#define MSGTR_CantSeekFile "No se puede avanzar o retroceder en este archivo.\n"
#define MSGTR_MOVcomprhdr "MOV: ¡Soporte de Cabecera comprimida requiere ZLIB!.\n"
#define MSGTR_MOVvariableFourCC "MOV: Advertencia. ¡Variable FOURCC detectada!\n"
#define MSGTR_MOVtooManyTrk "MOV: Advertencia. ¡Demasiadas pistas!"
#define MSGTR_DetectedTV "Detectado TV.\n"
#define MSGTR_ErrorOpeningOGGDemuxer "No se puede abrir el demuxer ogg.\n"
#define MSGTR_CannotOpenAudioStream "No se puede abrir stream de audio: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "No se puede abrir stream de subtítulos: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "No se pudo abrir el demuxer de audio: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "No se pudo abrir demuxer de subtítulos: %s\n"
#define MSGTR_TVInputNotSeekable "No se puede buscar en la entrada de TV.\n"
#define MSGTR_DemuxerInfoChanged "Demuxer la información %s ha cambiado a %s\n"
#define MSGTR_ClipInfo "Información de clip: \n"
#define MSGTR_LeaveTelecineMode "\ndemux_mpg: contenido NTSC de 30000/1001cps detectado, cambiando cuadros por segundo.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: contenido NTSC progresivo de 24000/1001cps detectado, cambiando cuadros por segundo.\n"
#define MSGTR_CacheFill "\rLlenando cache: %5.2f%% (%"PRId64" bytes)   "
#define MSGTR_NoBindFound "No se econtró una asignación para la tecla '%s'\n"
#define MSGTR_FailedToOpen "No se pudo abrir %s\n"
#define MSGTR_VideoID "[%s] Stream de video encontrado, -vid %d\n"
#define MSGTR_AudioID "[%s] Stream de audio encontrado, -aid %d\n"
#define MSGTR_SubtitleID "[%s] Stream de subtítulos encontrado, -sid %d\n"

// asfheader.c
#define MSGTR_MPDEMUX_ASFHDR_HeaderSizeOver1MB "FATAL: más de 1 MB de tamaño del header (%d)!\nPor favor contacte a los autores de MPlayer y suba/envie este archivo.\n"
#define MSGTR_MPDEMUX_ASFHDR_HeaderMallocFailed "Imposible obtener %d bytes para la cabecera\n"
#define MSGTR_MPDEMUX_ASFHDR_EOFWhileReadingHeader "EOF Mientras leia la cabecera ASF, archivo dañado o incompleto?\n"
#define MSGTR_MPDEMUX_ASFHDR_DVRWantsLibavformat "DVR quizas solo funcione con libavformat, intente con -demuxer 35 si tiene problemas\n"
#define MSGTR_MPDEMUX_ASFHDR_NoDataChunkAfterHeader "No hay un chunk de datos despues de la cabecera!\n"
#define MSGTR_MPDEMUX_ASFHDR_AudioVideoHeaderNotFound "ASF: No encuentro cabeceras de audio o video, archivo dañado?\n"
#define MSGTR_MPDEMUX_ASFHDR_InvalidLengthInASFHeader "Largo inválido en la cabecera ASF!\n"
#define MSGTR_MPDEMUX_ASFHDR_DRMLicenseURL "URL de la licencia DRM: %s\n"
#define MSGTR_MPDEMUX_ASFHDR_DRMProtected "Este archivo tiene DRM encription, MPlayer no lo va a reproducir!\n"

// aviheader.c
#define MSGTR_MPDEMUX_AVIHDR_EmptyList "** Lista vacia?!\n"
#define MSGTR_MPDEMUX_AVIHDR_WarnNotExtendedAVIHdr "Advertencia: esta no es una cabecera AVI extendida..\n"
#define MSGTR_MPDEMUX_AVIHDR_BuildingODMLidx "AVI: ODML: Construyendo el índice odml (%d superindexchunks).\n"
#define MSGTR_MPDEMUX_AVIHDR_BrokenODMLfile "AVI: ODML: Archivo arruinado (incompleto?) detectado. Utilizaré el índice tradicional.\n"
#define MSGTR_MPDEMUX_AVIHDR_CantReadIdxFile "No pude leer el archivo de índice %s: %s\n"
#define MSGTR_MPDEMUX_AVIHDR_NotValidMPidxFile "%s No es un archivo de índice MPlayer válido.\n"
#define MSGTR_MPDEMUX_AVIHDR_FailedMallocForIdxFile "Imposible disponer de memoria suficiente para los datos de índice de %s\n"
#define MSGTR_MPDEMUX_AVIHDR_PrematureEOF "El archivo de índice termina prematuramente %s\n"
#define MSGTR_MPDEMUX_AVIHDR_IdxFileLoaded "El archivo de índice: %s fue cargado.\n"
#define MSGTR_MPDEMUX_AVIHDR_GeneratingIdx "Generando Indice: %3lu %s     \r"
#define MSGTR_MPDEMUX_AVIHDR_IdxGeneratedForHowManyChunks "AVI: tabla de índices generada para %d chunks!\n"
#define MSGTR_MPDEMUX_AVIHDR_Failed2WriteIdxFile "Imposible escribir el archivo de índice %s: %s\n"
#define MSGTR_MPDEMUX_AVIHDR_IdxFileSaved "Archivo de índice guardado: %s\n"

// demux_audio.c
#define MSGTR_MPDEMUX_AUDIO_UnknownFormat "Audio demuxer: Formato desconocido %d.\n"

// demux_demuxers.c
#define MSGTR_MPDEMUX_DEMUXERS_FillBufferError "fill_buffer error: demuxer erroneo: no vd, ad o sd.\n"

// demux_mkv.c
#define MSGTR_MPDEMUX_MKV_ZlibInitializationFailed "[mkv] zlib la inicialización ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_ZlibDecompressionFailed "[mkv] zlib la descompresión ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_LzoInitializationFailed "[mkv] lzo la inicialización ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_LzoDecompressionFailed "[mkv] lzo la descompresión ha fallado.\n"
#define MSGTR_MPDEMUX_MKV_TrackEncrypted "[mkv] La pista número %u ha sido encriptada y la desencriptación no ha sido\n[mkv] todavía implementada. Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_UnknownContentEncoding "[mkv] Tipo de codificación desconocida para la pista %u. Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_UnknownCompression "[mkv] La pista %u ha sido comprimida con un algoritmo de compresión\n[mkv] desconocido o no soportado algoritmo (%u). Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_ZlibCompressionUnsupported "[mkv] La pista %u ha sido comprimida con zlib pero mplayer no ha sido\n[mkv] compilado con soporte para compresión con zlib. Omitiendo la pista.\n"
#define MSGTR_MPDEMUX_MKV_TrackIDName "[mkv] Pista ID %u: %s (%s) \"%s\", %s\n"
#define MSGTR_MPDEMUX_MKV_TrackID "[mkv] Pista ID %u: %s (%s), %s\n"
#define MSGTR_MPDEMUX_MKV_UnknownCodecID "[mkv] CodecID (%s) desconocido/no soportado/erróneo/faltante Codec privado\n[mkv] datos (pista %u).\n"
#define MSGTR_MPDEMUX_MKV_FlacTrackDoesNotContainValidHeaders "[mkv] La pista FLAC no contiene cabeceras válidas.\n"
#define MSGTR_MPDEMUX_MKV_UnknownAudioCodec "[mkv] Codec de audio ID '%s' desconocido/no soportado para la pista %u\n[mkv] o erróneo/falta Codec privado.\n"
#define MSGTR_MPDEMUX_MKV_SubtitleTypeNotSupported "[mkv] El tipo de subtítulos '%s' no está soportado.\n"
#define MSGTR_MPDEMUX_MKV_WillPlayVideoTrack "[mkv] Reproducirá la pista de vídeo %u.\n"
#define MSGTR_MPDEMUX_MKV_NoVideoTrackFound "[mkv] Ninguna pista de vídeo encontrada/deseada.\n"
#define MSGTR_MPDEMUX_MKV_NoAudioTrackFound "[mkv] Ninguna pista de sonido encontrada/deseada.\n"
#define MSGTR_MPDEMUX_MKV_WillDisplaySubtitleTrack "[mkv] Mostrará la pista de subtítulos %u.\n"
#define MSGTR_MPDEMUX_MKV_NoBlockDurationForSubtitleTrackFound "[mkv] Aviso: No se ha encontrado la duración del bloque para los subtítulos de la pista.\n"
#define MSGTR_MPDEMUX_MKV_TooManySublines "[mkv] Aviso: demasiados líneas para renderizar, omitiendo.\n"
#define MSGTR_MPDEMUX_MKV_TooManySublinesSkippingAfterFirst "\n[mkv] Aviso: demasiadas líneas para renderizar, omitiendo depués de la primera %i.\n"

// demux_nuv.c
#define MSGTR_MPDEMUX_NUV_NoVideoBlocksInFile "Este archivo no tiene blocks de video.\n"

// demux_xmms.c
#define MSGTR_MPDEMUX_XMMS_FoundPlugin "Plugin encontrado: %s (%s).\n"
#define MSGTR_MPDEMUX_XMMS_ClosingPlugin "Cerrando plugin: %s.\n"
#define MSGTR_MPDEMUX_XMMS_WaitForStart "Esperando a que el plugin XMMS comience la reprodución de '%s'...\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "Dispositivo de CD-ROM '%s' no encontrado.\n"
#define MSGTR_ErrTrackSelect "Error seleccionando la pista de VCD!"
#define MSGTR_ReadSTDIN "Leyendo desde la entrada estándar (stdin)...\n"
#define MSGTR_UnableOpenURL "No se puede abrir URL: %s\n"
#define MSGTR_ConnToServer "Connectado al servidor: %s\n"
#define MSGTR_FileNotFound "Archivo no encontrado: '%s'\n"

#define MSGTR_SMBInitError "No se puede inicializar la librería libsmbclient: %d\n"
#define MSGTR_SMBFileNotFound "No se puede abrir desde la RED: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer no fue compilado con soporte de lectura de SMB.\n"

#define MSGTR_CantOpenDVD "No se puede abrir el dispositivo de DVD: %s (%s)\n"

// stream_dvd.c
#define MSGTR_DVDspeedCantOpen "No se ha podido abrir el dispositivo de DVD para escritura, cambiar la velocidad del DVD requiere acceso de escritura\n"
#define MSGTR_DVDrestoreSpeed "Restableciendo la velocidad del DVD"
#define MSGTR_DVDlimitSpeed "Limitando la velocidad del DVD a %dKB/s... "
#define MSGTR_DVDlimitFail "La limitación de la velocidad del DVD ha fallado.\n"
#define MSGTR_DVDlimitOk "Se ha limitado la velocidad del DVD con éxito.\n"
#define MSGTR_NoDVDSupport "MPlayer fue compilado sin soporte para DVD, saliendo.\n"
#define MSGTR_DVDnumTitles "Hay %d títulos en este DVD.\n"
#define MSGTR_DVDinvalidTitle "Número de título de DVD inválido: %d\n"
#define MSGTR_DVDnumChapters "Hay %d capítulos en este título de DVD.\n"
#define MSGTR_DVDinvalidChapter "Número de capítulo de DVD inválido: %d\n"
#define MSGTR_DVDinvalidChapterRange "Especificación inválida de rango de capítulos %s\n"
#define MSGTR_DVDinvalidLastChapter "Número del último capítulo del DVD inválido: %d\n"
#define MSGTR_DVDnumAngles "Hay %d ángulos en este título de DVD.\n"
#define MSGTR_DVDinvalidAngle "Número de ángulo de DVD inválido: %d\n"
#define MSGTR_DVDnoIFO "No se pudo abrir archivo IFO para el título de DVD %d.\n"
#define MSGTR_DVDnoVMG "No se pudo abrir la información VMG!\n"
#define MSGTR_DVDnoVOBs "No se pudo abrir VOBS del título (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDnoMatchingAudio "DVD, no se encontró un idioma coincidente!\n"
#define MSGTR_DVDaudioChannel "DVD, canal de audio seleccionado: %d idioma: %c%c\n"
#define MSGTR_DVDaudioStreamInfo "stream de audio: %d formato: %s (%s) idioma: %s aid: %d.\n"
#define MSGTR_DVDnumAudioChannels "Número de canales de audio en el disco: %d.\n"
#define MSGTR_DVDnoMatchingSubtitle "DVD, no se encontró un idioma de subtitulo coincidente!\n"
#define MSGTR_DVDsubtitleChannel "DVD, canal de subtitulos seleccionado: %d idioma: %c%c\n"
#define MSGTR_DVDsubtitleLanguage "subtítulo ( sid ): %d idioma: %s\n"
#define MSGTR_DVDnumSubtitles "Número de subtítulos en el disco: %d\n"


// dec_video.c & dec_audio.c
#define MSGTR_CantOpenCodec "No se pudo abrir codec.\n"
#define MSGTR_CantCloseCodec "No se pudo cerrar codec.\n"

#define MSGTR_MissingDLLcodec "ERROR: No se pudo abrir el codec DirectShow requerido: %s\n"
#define MSGTR_ACMiniterror "No se puede cargar/inicializar codecs de audio Win32/ACM (falta archivo DLL?)\n"
#define MSGTR_MissingLAVCcodec "No se encuentra codec '%s' en libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF mientras buscaba la cabecera de secuencia.\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: No se puede leer cabecera de secuencia.\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: No se puede leer la extensión de la cabecera de secuencia.\n"
#define MSGTR_BadMpegSequHdr "MPEG: Mala cabecera de secuencia.\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: Mala extensión de la cabecera de secuencia.\n"

#define MSGTR_ShMemAllocFail "No se puede alocar memoria compartida.\n"
#define MSGTR_CantAllocAudioBuf "No se puede alocar buffer de la salida de audio.\n"
#define MSGTR_UnknownAudio "Formato de audio desconocido/faltante, no se reproducirá sonido.\n"


#define MSGTR_UsingExternalPP "[PP] Usando filtro de postprocesado externo, max q = %d.\n"
#define MSGTR_UsingCodecPP "[PP] Usando postprocesado del codec, max q = %d.\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Familia de codec de video solicitada [%s] (vfm=%s) no está disponible (actívalo al compilar).\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Familia de codec de audio solicitada [%s] (afm=%s) no está disponible (actívalo al compilar).\n"
#define MSGTR_OpeningVideoDecoder "Abriendo decodificador de video: [%s] %s.\n"
#define MSGTR_SelectedVideoCodec "Video codec seleccionado: [%s] vfm: %s (%s)\n"
#define MSGTR_OpeningAudioDecoder "Abriendo decodificador de audio: [%s] %s.\n"
#define MSGTR_SelectedAudioCodec "Audio codec seleccionado: [%s] afm: %s (%s)\n"
#define MSGTR_VDecoderInitFailed "Inicialización del VDecoder ha fallado.\n"
#define MSGTR_ADecoderInitFailed "Inicialización del ADecoder ha fallado.\n"
#define MSGTR_ADecoderPreinitFailed "Preinicialización del ADecoder ha fallado.\n"

// vf.c
#define MSGTR_CouldNotFindVideoFilter "No se pudo encontrar el filtro de video '%s'.\n"
#define MSGTR_CouldNotOpenVideoFilter "No se pudo abrir el filtro de video '%s'.\n"
#define MSGTR_OpeningVideoFilter "Abriendo filtro de video: "
#define MSGTR_CannotFindColorspace "No se pudo encontrar espacio de color concordante, ni siquiera insertando 'scale' :(.\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: el codec no declaró sh->disp_w y sh->disp_h, intentando solucionarlo!\n"
#define MSGTR_CouldNotFindColorspace "No se pudo encontrar colorspace concordante - reintentando escalado -vf...\n"
#define MSGTR_MovieAspectIsSet "Aspecto es %.2f:1 - prescalando a aspecto correcto.\n"
#define MSGTR_MovieAspectUndefined "Aspecto de película no es definido - no se ha aplicado prescalado.\n"

// vd_dshow.c, vd_dmo.c
#define MSGTR_DownloadCodecPackage "Necesita actualizar/instalar el paquete binario con codecs.\n Dirijase a http://www.mplayerhq.hu/dload.html\n"


// url.c
#define MSGTR_MPDEMUX_URL_StringAlreadyEscaped "Al parecer el string ya ha sido escapado en url_scape %c%c1%c2\n"

// ai_alsa.c
#define MSGTR_MPDEMUX_AIALSA_CannotSetSamplerate "No puedo setear el samplerate.\n"
#define MSGTR_MPDEMUX_AIALSA_CannotSetBufferTime "No puedo setear el tiempo del buffer.\n"
#define MSGTR_MPDEMUX_AIALSA_CannotSetPeriodTime "No puedo setear el tiempo del periodo.\n"

// ai_alsa.c
#define MSGTR_MPDEMUX_AIALSA_PcmBrokenConfig "Configuración erronea para este PCM: no hay configuraciones disponibles.\n"
#define MSGTR_MPDEMUX_AIALSA_UnavailableAccessType "Tipo de acceso no disponible.\n"
#define MSGTR_MPDEMUX_AIALSA_UnavailableSampleFmt "Formato de muestreo no disponible.\n"
#define MSGTR_MPDEMUX_AIALSA_UnavailableChanCount "Conteo de canales no disponible, usando el valor por omisión: %d\n"
#define MSGTR_MPDEMUX_AIALSA_CannotInstallHWParams "Imposible instalar los parametros de hardware: %s"
#define MSGTR_MPDEMUX_AIALSA_PeriodEqualsBufferSize "Imposible usar un periodo igual al tamaño del buffer (%u == %lu).\n"
#define MSGTR_MPDEMUX_AIALSA_CannotInstallSWParams "Imposible instalar los parametros de software:\n"
#define MSGTR_MPDEMUX_AIALSA_ErrorOpeningAudio "Error tratando de abrir el sonido: %s\n"
#define MSGTR_MPDEMUX_AIALSA_AlsaXRUN "ALSA xrun!!! (por lo menos %.3f ms de largo)\n"
#define MSGTR_MPDEMUX_AIALSA_AlsaXRUNPrepareError "ALSA xrun: error de preparación: %s"
#define MSGTR_MPDEMUX_AIALSA_AlsaReadWriteError "ALSA Error de lectura/escritura"

// ai_oss.c

#define MSGTR_MPDEMUX_AIOSS_Unable2SetChanCount "Imposible setear el conteo de canales: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2SetStereo "Imposible setear stereo: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2Open "Imposible abrir '%s': %s\n"
#define MSGTR_MPDEMUX_AIOSS_UnsupportedFmt "Formato no soportado\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2SetAudioFmt "Imposible setear formato de audio."
#define MSGTR_MPDEMUX_AIOSS_Unable2SetSamplerate "Imposible setear el samplerate: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2SetTrigger "Imposible setear el trigger: %d\n"
#define MSGTR_MPDEMUX_AIOSS_Unable2GetBlockSize "No pude obtener el tamaño del bloque!\n"
#define MSGTR_MPDEMUX_AIOSS_AudioBlockSizeZero "El tamaño del bloque de audio es cero, utilizando %d!\n"
#define MSGTR_MPDEMUX_AIOSS_AudioBlockSize2Low "Tamaño del bloque de audio muy bajo, utilizando %d!\n"

// asf_mmst_streaming.c

#define MSGTR_MPDEMUX_MMST_WriteError "Error de escritura\n"
#define MSGTR_MPDEMUX_MMST_EOFAlert "\nAlerta! EOF\n"
#define MSGTR_MPDEMUX_MMST_PreHeaderReadFailed "Falló la lectura pre-header\n"
#define MSGTR_MPDEMUX_MMST_InvalidHeaderSize "Tamaño de cabecera inválido, me rindo!\n"
#define MSGTR_MPDEMUX_MMST_HeaderDataReadFailed "Falló la lectura de los datos de la cabecera\n"
#define MSGTR_MPDEMUX_MMST_packet_lenReadFailed "Falló la lectura del packet_len\n"
#define MSGTR_MPDEMUX_MMST_InvalidRTSPPacketSize "Tamaño inválido de paquete RTSP, me rindo!\n"
#define MSGTR_MPDEMUX_MMST_CmdDataReadFailed "Fallo el comando 'leer datos'.\n"
#define MSGTR_MPDEMUX_MMST_HeaderObject "Objeto de cabecera.\n"
#define MSGTR_MPDEMUX_MMST_DataObject "Objeto de datos.\n"
#define MSGTR_MPDEMUX_MMST_FileObjectPacketLen "Objeto de archivo, largo del paquete = %d (%d)\n"
#define MSGTR_MPDEMUX_MMST_StreamObjectStreamID "Objeto de stream, id: %d\n"
#define MSGTR_MPDEMUX_MMST_2ManyStreamID "Demaciados id, ignorando stream"
#define MSGTR_MPDEMUX_MMST_UnknownObject "Objeto desconocido\n"
#define MSGTR_MPDEMUX_MMST_MediaDataReadFailed "Falló la lectura de los datos desde el medio\n"
#define MSGTR_MPDEMUX_MMST_MissingSignature "Firma no encontrada.\n"
#define MSGTR_MPDEMUX_MMST_PatentedTechnologyJoke "Todo listo, gracias por bajar un archivo de medios que contiene tecnología patentada y propietaria ;)\n"
#define MSGTR_MPDEMUX_MMST_UnknownCmd "Comando desconocido %02x\n"
#define MSGTR_MPDEMUX_MMST_GetMediaPacketErr "Error en get_media_packet: %s\n"
#define MSGTR_MPDEMUX_MMST_Connected "Conectado.\n"

// asf_streaming.c

#define MSGTR_MPDEMUX_ASF_StreamChunkSize2Small "Ahhhh, el tamaño del stream_chunck es muy pequeño: %d\n"
#define MSGTR_MPDEMUX_ASF_SizeConfirmMismatch "size_confirm erroneo!: %d %d\n"
#define MSGTR_MPDEMUX_ASF_WarnDropHeader "Advertencia : botar la cabecera ????\n"
#define MSGTR_MPDEMUX_ASF_ErrorParsingChunkHeader "Error mientras se parseaba la cabecera del chunk.\n"
#define MSGTR_MPDEMUX_ASF_NoHeaderAtFirstChunk "No me llego la cabecera como primer chunk !!!!\n"
#define MSGTR_MPDEMUX_ASF_BufferMallocFailed "Error imposible obtener un buffer de %d bytes\n"
#define MSGTR_MPDEMUX_ASF_ErrReadingNetworkStream "Error leyendo el network stream\n"
#define MSGTR_MPDEMUX_ASF_ErrChunk2Small "Error, el chunk es muy pequeño\n"
#define MSGTR_MPDEMUX_ASF_ErrSubChunkNumberInvalid "Error, el número de sub chunks es inválido.\n"
#define MSGTR_MPDEMUX_ASF_Bandwidth2SmallCannotPlay "Imposible mostrarte este archivo con tu bandwidth!\n"
#define MSGTR_MPDEMUX_ASF_Bandwidth2SmallDeselectedAudio "Bandwidth muy pequeño, deselecionando este stream de audio.\n"
#define MSGTR_MPDEMUX_ASF_Bandwidth2SmallDeselectedVideo "Bandwidth muy pequeño, deselecionando este stream de video.\n"
#define MSGTR_MPDEMUX_ASF_InvalidLenInHeader "Largo inválido den cabecera ASF!\n"
#define MSGTR_MPDEMUX_ASF_ErrReadingChunkHeader "Error mientras leía la cabecera del chunk.\n"
#define MSGTR_MPDEMUX_ASF_ErrChunkBiggerThanPacket "Error chunk_size > packet_size.\n"
#define MSGTR_MPDEMUX_ASF_ErrReadingChunk "Error mientras leía el chunk\n"
#define MSGTR_MPDEMUX_ASF_ASFRedirector "=====> Redireccionador ASF\n"
#define MSGTR_MPDEMUX_ASF_InvalidProxyURL "URL del proxy inválida.\n"
#define MSGTR_MPDEMUX_ASF_UnknownASFStreamType "Tipo de ASF stream desconocido.\n"
#define MSGTR_MPDEMUX_ASF_Failed2ParseHTTPResponse "No pude procesar la respuesta HTTP\n"
#define MSGTR_MPDEMUX_ASF_ServerReturn "El servidor retornó %d:%s\n"
#define MSGTR_MPDEMUX_ASF_ASFHTTPParseWarnCuttedPragma "ASF HTTP PARSE WARNING : Pragma %s cortado desde %zu bytes a %zu\n"
#define MSGTR_MPDEMUX_ASF_SocketWriteError "Error escribiendo en el socket : %s\n"
#define MSGTR_MPDEMUX_ASF_HeaderParseFailed "Imposible procesar la cabecera.\n"
#define MSGTR_MPDEMUX_ASF_NoStreamFound "No encontre ningún stream.\n"
#define MSGTR_MPDEMUX_ASF_UnknownASFStreamingType "Desconozco este tipo de streaming ASF\n"
#define MSGTR_MPDEMUX_ASF_InfoStreamASFURL "STREAM_ASF, URL: %s\n"
#define MSGTR_MPDEMUX_ASF_StreamingFailed "Fallo, saliendo..\n"

// audio_in.c

#define MSGTR_MPDEMUX_AUDIOIN_ErrReadingAudio "\nError leyendo el audio: %s\n"
#define MSGTR_MPDEMUX_AUDIOIN_XRUNSomeFramesMayBeLeftOut "Recuperandome de un cross-run, puede que hayamos perdido algunos frames!\n"
#define MSGTR_MPDEMUX_AUDIOIN_ErrFatalCannotRecover "Error fatal, imposible reponerme!\n"
#define MSGTR_MPDEMUX_AUDIOIN_NotEnoughSamples "\nNo hay suficientes samples de audio!\n"

// cache2.c

#define MSGTR_MPDEMUX_CACHE2_NonCacheableStream "\rEste stream no es cacheable.\n"
#define MSGTR_MPDEMUX_CACHE2_ReadFileposDiffers "!!! read_filepos diffiere!!! reporta este bug...\n"

// cdda.c

#define MSGTR_MPDEMUX_CDDA_CantOpenCDDADevice "No puede abrir el dispositivo CDA.\n"
#define MSGTR_MPDEMUX_CDDA_CantOpenDisc "No pude abrir el disco.\n"
#define MSGTR_MPDEMUX_CDDA_AudioCDFoundWithNTracks "Encontre un disco de audio con %d pistas.\n"

// cddb.c

#define MSGTR_MPDEMUX_CDDB_FailedToReadTOC "Fallé al tratar de leer el TOC.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToOpenDevice "Falle al tratar de abrir el dispositivo %s\n"
#define MSGTR_MPDEMUX_CDDB_NotAValidURL "No es un URL válido\n"
#define MSGTR_MPDEMUX_CDDB_FailedToSendHTTPRequest "Fallé al tratar de enviar la solicitud HTTP.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToReadHTTPResponse "Fallé al tratar de leer la respuesta HTTP.\n"
#define MSGTR_MPDEMUX_CDDB_HTTPErrorNOTFOUND "No encontrado.\n"
#define MSGTR_MPDEMUX_CDDB_HTTPErrorUnknown "Código de error desconocido.\n"
#define MSGTR_MPDEMUX_CDDB_NoCacheFound "No encontre cache.\n"
#define MSGTR_MPDEMUX_CDDB_NotAllXMCDFileHasBeenRead "No todo el archivo xmcd ha sido leido.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToCreateDirectory "No pude crear el directorio %s.\n"
#define MSGTR_MPDEMUX_CDDB_NotAllXMCDFileHasBeenWritten "No todo el archivo xmcd ha sido escrito.\n"
#define MSGTR_MPDEMUX_CDDB_InvalidXMCDDatabaseReturned "Archivo de base de datos xmcd inválido retornado.\n"
#define MSGTR_MPDEMUX_CDDB_UnexpectedFIXME "FIXME Inesperado.\n"
#define MSGTR_MPDEMUX_CDDB_UnhandledCode "Unhandled code.\n"
#define MSGTR_MPDEMUX_CDDB_UnableToFindEOL "No encontré el EOL\n"
#define MSGTR_MPDEMUX_CDDB_ParseOKFoundAlbumTitle "Procesado OK, encontrado: %s\n"
#define MSGTR_MPDEMUX_CDDB_AlbumNotFound "No encontré el album.\n"
#define MSGTR_MPDEMUX_CDDB_ServerReturnsCommandSyntaxErr "El servidor retornó: Error en la sintaxis del comando.\n"
#define MSGTR_MPDEMUX_CDDB_NoSitesInfoAvailable "No hay disponible información sobre los sitios.\n"
#define MSGTR_MPDEMUX_CDDB_FailedToGetProtocolLevel "Fallé tratando de obtener el nivel del protocolo.\n"
#define MSGTR_MPDEMUX_CDDB_NoCDInDrive "No hay un CD en la unidad.\n"

// cue_read.c

#define MSGTR_MPDEMUX_CUEREAD_UnexpectedCuefileLine "[bincue] Línea cuefile inesperada: %s\n"
#define MSGTR_MPDEMUX_CUEREAD_BinFilenameTested "[bincue] Nombre de archivo bin testeado: %s\n"
#define MSGTR_MPDEMUX_CUEREAD_CannotFindBinFile "[bincue] No enccuentro el archivo bin - me rindo.\n"
#define MSGTR_MPDEMUX_CUEREAD_UsingBinFile "[bincue] Utilizando el archivo bin %s\n"
#define MSGTR_MPDEMUX_CUEREAD_UnknownModeForBinfile "[bincue] Modo desconocido para el archivo bin, esto no debería pasar. Abortando.\n"
#define MSGTR_MPDEMUX_CUEREAD_CannotOpenCueFile "[bincue] No puedo abrir %s\n"
#define MSGTR_MPDEMUX_CUEREAD_ErrReadingFromCueFile "[bincue] Error leyendo desde  %s\n"
#define MSGTR_MPDEMUX_CUEREAD_ErrGettingBinFileSize "[bincue] Error obteniendo el tamaño del archivo bin.\n"
#define MSGTR_MPDEMUX_CUEREAD_InfoTrackFormat "pista %02d:  formato=%d  %02d:%02d:%02d\n"
#define MSGTR_MPDEMUX_CUEREAD_UnexpectedBinFileEOF "[bincue] Final inesperado en el archivo bin.\n"
#define MSGTR_MPDEMUX_CUEREAD_CannotReadNBytesOfPayload "[bincue] No pude leer %d bytes de payload.\n"
#define MSGTR_MPDEMUX_CUEREAD_CueStreamInfo_FilenameTrackTracksavail "CUE stream_open, archivo=%s, pista=%d, pistas disponibles: %d -> %d\n"

// network.c

#define MSGTR_MPDEMUX_NW_UnknownAF "Familia de direcciones desconocida %d\n"
#define MSGTR_MPDEMUX_NW_ResolvingHostForAF "Resoliendo %s para %s...\n"
#define MSGTR_MPDEMUX_NW_CantResolv "No pude resolver el nombre para %s: %s\n"
#define MSGTR_MPDEMUX_NW_ConnectingToServer "Connectando con el servidor %s[%s]: %d...\n"
#define MSGTR_MPDEMUX_NW_CantConnect2Server "No pude conectarme con el servidor con %s\n"
#define MSGTR_MPDEMUX_NW_SelectFailed "Select fallido.\n"
#define MSGTR_MPDEMUX_NW_ConnTimeout "La conección expiró.\n"
#define MSGTR_MPDEMUX_NW_GetSockOptFailed "getsockopt fallido: %s\n"
#define MSGTR_MPDEMUX_NW_ConnectError "Error de conección: %s\n"
#define MSGTR_MPDEMUX_NW_InvalidProxySettingTryingWithout "Configuración del proxy inválida... probando sin proxy.\n"
#define MSGTR_MPDEMUX_NW_CantResolvTryingWithoutProxy "No pude resolver el nombre del host para AF_INET. probando sin proxy.\n"
#define MSGTR_MPDEMUX_NW_ErrSendingHTTPRequest "Error enviando la solicitud HTTP: no alcancé a enviarla completamente.\n"
#define MSGTR_MPDEMUX_NW_ReadFailed "Falló la lectura.\n"
#define MSGTR_MPDEMUX_NW_Read0CouldBeEOF "http_read_response leí 0 (ej. EOF)\n"
#define MSGTR_MPDEMUX_NW_AuthFailed "Fallo la autentificación, por favor usa las opciones -user y -passwd con tus respectivos nombre de usuario y contraseña\n"\
"para una lista de URLs o construye unu URL con la siguiente forma:\n"\
"http://usuario:contraseña@servidor/archivo\n"
#define MSGTR_MPDEMUX_NW_AuthRequiredFor "Se requiere autentificación para %s\n"
#define MSGTR_MPDEMUX_NW_AuthRequired "Se requiere autentificación.\n"
#define MSGTR_MPDEMUX_NW_NoPasswdProvidedTryingBlank "No especificaste una contraseña, voy a utilizar una contraseña en blanco.\n"
#define MSGTR_MPDEMUX_NW_ErrServerReturned "El servidor retornó %d: %s\n"
#define MSGTR_MPDEMUX_NW_CacheSizeSetTo "Se seteo el tamaño del caché a %d KBytes.\n"


// ========================== LIBMENU ===================================

// common
#define MSGTR_LIBMENU_NoEntryFoundInTheMenuDefinition "[MENU] Noencontre una entrada e n la definición del menú.\n"

// libmenu/menu.c
#define MSGTR_LIBMENU_SyntaxErrorAtLine "[MENU] Error de sintáxis en la línea: %d\n"
#define MSGTR_LIBMENU_MenuDefinitionsNeedANameAttrib "[MENU] Las definiciones de menú necesitan un nombre de atributo (linea %d)\n"
#define MSGTR_LIBMENU_BadAttrib "[MENU] Atributo erroneo %s=%s en el menú '%s' en la línea %d\n"
#define MSGTR_LIBMENU_UnknownMenuType "[MENU] Tipo de menú desconocido '%s' en la línea %d\n"
#define MSGTR_LIBMENU_CantOpenConfigFile "[MENU] No puedo abrir el archivo de configuración del menú: %s\n"
#define MSGTR_LIBMENU_ConfigFileIsTooBig "[MENU] El archivo de configuración es muy grande (> %d KB).\n"
#define MSGTR_LIBMENU_ConfigFileIsEmpty "[MENU] El archivo de configuración esta vacío\n"
#define MSGTR_LIBMENU_MenuNotFound "[MENU] No encontré el menú %s\n"
#define MSGTR_LIBMENU_MenuInitFailed "[MENU] Fallo en la inicialización del menú '%s'.\n"
#define MSGTR_LIBMENU_UnsupportedOutformat "[MENU] Formato de salida no soportado!!!!\n"

// libmenu/menu_cmdlist.c
#define MSGTR_LIBMENU_ListMenuEntryDefinitionsNeedAName "[MENU] Las definiciones de Lista del menu necesitan un nombre (line %d).\n"
#define MSGTR_LIBMENU_ListMenuNeedsAnArgument "[MENU] El menú de lista necesita un argumento.\n"

// libmenu/menu_console.c
#define MSGTR_LIBMENU_WaitPidError "[MENU] Error Waitpid: %s.\n"
#define MSGTR_LIBMENU_SelectError "[MENU] Error en Select.\n"
#define MSGTR_LIBMENU_ReadErrorOnChildFD "[MENU] Error de lectura en el descriptor de archivo hijo: %s.\n"
#define MSGTR_LIBMENU_ConsoleRun "[MENU] Consola corriendo: %s ...\n"
#define MSGTR_LIBMENU_AChildIsAlreadyRunning "[MENU] Ya hay un hijo/child corriendo.\n"
#define MSGTR_LIBMENU_ForkFailed "[MENU] Falló el Fork!!!\n"
#define MSGTR_LIBMENU_WriteError "[MENU] Error de escritura.\n"

// libmenu/menu_filesel.c
#define MSGTR_LIBMENU_OpendirError "[MENU] Error en Opendir: %s.\n"
#define MSGTR_LIBMENU_ReallocError "[MENU] Error en Realloc: %s.\n"
#define MSGTR_LIBMENU_MallocError "[MENU] Error tratando de disponer de memoria: %s.\n"
#define MSGTR_LIBMENU_ReaddirError "[MENU] Error en Readdir: %s.\n"
#define MSGTR_LIBMENU_CantOpenDirectory "[MENU] No pude abrir el directorio %s\n"

// libmenu/menu_param.c
#define MSGTR_LIBMENU_SubmenuDefinitionNeedAMenuAttribut "[MENU] Submenu definition needs a 'menu' attribute.\n"
#define MSGTR_LIBMENU_InvalidProperty "[MENU] '%s', Propiedad inválida en entrada de menu de preferencias. (linea %d).\n"
#define MSGTR_LIBMENU_PrefMenuEntryDefinitionsNeed "[MENU] Las definiciones de las entradas de 'Preferencia' en el menu necesitan un atributo 'property' válido (linea %d).\n"
#define MSGTR_LIBMENU_PrefMenuNeedsAnArgument "[MENU] El menu 'Pref' necesita un argumento.\n"

// libmenu/menu_pt.c
#define MSGTR_LIBMENU_CantfindTheTargetItem "[MENU] Imposible encontrar el item objetivo ????\n"
#define MSGTR_LIBMENU_FailedToBuildCommand "[MENU] Fallé tratando de construir el comando: %s.\n"

// libmenu/menu_txt.c
#define MSGTR_LIBMENU_MenuTxtNeedATxtFileName "[MENU] El menu 'Text' necesita un nombre de archivo txt (param file).\n"
#define MSGTR_LIBMENU_MenuTxtCantOpen "[MENU] No pude abrir: %s.\n"
#define MSGTR_LIBMENU_WarningTooLongLineSplitting "[MENU] Advertencia, Línea muy larga, cortandola...\n"
#define MSGTR_LIBMENU_ParsedLines "[MENU] %d líneas procesadas.\n"

// libmenu/vf_menu.c
#define MSGTR_LIBMENU_UnknownMenuCommand "[MENU] Comando desconocido: '%s'.\n"
#define MSGTR_LIBMENU_FailedToOpenMenu "[MENU] No pude abrir el menú: '%s'.\n"

// ========================== LIBMPCODECS ===================================

// ad_dvdpcm.c:
#define MSGTR_SamplesWanted "Se necesitan muestras de este formato para mejorar el soporte. Por favor contacte a los desarrolladores.\n"

// libmpcodecs/ad_libdv.c
#define MSGTR_MPCODECS_AudioFramesizeDiffers "[AD_LIBDV] Advertencia! El framezise de audio difiere! leidos=%d  hdr=%d.\n"

// libmpcodecs/vd_dmo.c vd_dshow.c vd_vfw.c
#define MSGTR_MPCODECS_CouldntAllocateImageForCinepakCodec "[VD_DMO] No pude disponer imagen para el códec cinepak.\n"

// libmpcodecs/vd_ffmpeg.c
#define MSGTR_MPCODECS_XVMCAcceleratedCodec "[VD_FFMPEG] Códec acelerado con XVMC.\n"
#define MSGTR_MPCODECS_ArithmeticMeanOfQP "[VD_FFMPEG] Significado aritmético de QP: %2.4f, significado armónico de QP: %2.4f\n"
#define MSGTR_MPCODECS_DRIFailure "[VD_FFMPEG] Falla DRI.\n"
#define MSGTR_MPCODECS_CouldntAllocateImageForCodec "[VD_FFMPEG] No pude disponer imagen para el códec.\n"
#define MSGTR_MPCODECS_XVMCAcceleratedMPEG2 "[VD_FFMPEG] MPEG-2 acelerado con XVMC.\n"
#define MSGTR_MPCODECS_TryingPixfmt "[VD_FFMPEG] Intentando pixfmt=%d.\n"
#define MSGTR_MPCODECS_McGetBufferShouldWorkOnlyWithXVMC "[VD_FFMPEG] El mc_get_buffer funcionará solo con aceleración XVMC!!"
#define MSGTR_MPCODECS_UnexpectedInitVoError "[VD_FFMPEG] Error inesperado en init_vo.\n"
#define MSGTR_MPCODECS_UnrecoverableErrorRenderBuffersNotTaken "[VD_FFMPEG] Error insalvable, no se tomaron los render buffers.\n"
#define MSGTR_MPCODECS_OnlyBuffersAllocatedByVoXvmcAllowed "[VD_FFMPEG] Solo buffers dispuestos por vo_xvmc estan permitidos.\n"

// libmpcodecs/ve_lavc.c
#define MSGTR_MPCODECS_HighQualityEncodingSelected "[VE_LAVC] Encoding de alta calidad seleccionado (no real-time)!\n"
#define MSGTR_MPCODECS_UsingConstantQscale "[VE_LAVC] Utilizando qscale = %f constante (VBR).\n"

// libmpcodecs/ve_raw.c
#define MSGTR_MPCODECS_OutputWithFourccNotSupported "[VE_RAW] La salida en bruto (raw) con fourcc [%x] no está soportada!\n"
#define MSGTR_MPCODECS_NoVfwCodecSpecified "[VE_RAW] No se especificó el codec VfW requerido!!\n"

// libmpcodecs/vf_crop.c
#define MSGTR_MPCODECS_CropBadPositionWidthHeight "[CROP] posición/ancho/alto inválido(s) - el área a cortar esta fuera del original!\n"

// libmpcodecs/vf_cropdetect.c
#define MSGTR_MPCODECS_CropArea "[CROP] Area de corte: X: %d..%d  Y: %d..%d  (-vf crop=%d:%d:%d:%d).\n"

// libmpcodecs/vf_format.c, vf_palette.c, vf_noformat.c
#define MSGTR_MPCODECS_UnknownFormatName "[VF_FORMAT] Nombre de formato desconocido: '%s'.\n"

// libmpcodecs/vf_framestep.c vf_noformat.c vf_palette.c vf_tile.c
#define MSGTR_MPCODECS_ErrorParsingArgument "[VF_FRAMESTEP] Error procesando argumento.\n"

// libmpcodecs/ve_vfw.c
#define MSGTR_MPCODECS_CompressorType "Tipo de compresor: %.4lx\n"
#define MSGTR_MPCODECS_CompressorSubtype "Subtipo de compresor: %.4lx\n"
#define MSGTR_MPCODECS_CompressorFlags "Compressor flags: %lu, versión %lu, versión ICM: %lu\n"
#define MSGTR_MPCODECS_Flags "Flags:"
#define MSGTR_MPCODECS_Quality " Calidad"

// libmpcodecs/vf_expand.c
#define MSGTR_MPCODECS_FullDRNotPossible "DR completo imposible, tratando con SLICES en su lugar!\n"
#define MSGTR_MPCODECS_WarnNextFilterDoesntSupportSlices  "Advertencia! El próximo filtro no soporta SLICES, preparate para el sig11 (SEGV)...\n"
#define MSGTR_MPCODECS_FunWhydowegetNULL "Por qué obtenemos NULL??\n"


// libmpcodecs/vf_test.c, vf_yuy2.c, vf_yvu9.c
#define MSGTR_MPCODECS_WarnNextFilterDoesntSupport "%s No soportado por el próximo filtro/vo :(\n"

// ================================== LIBMPVO ====================================

// stream/stream_radio.c

#define MSGTR_RADIO_ChannelNamesDetected "[radio] Nombre de canales de radio detectados.\n"
#define MSGTR_RADIO_WrongFreqForChannel "[radio] Frecuencia erronea para canal %s\n"
#define MSGTR_RADIO_WrongChannelNumberFloat "[radio] Número de canal erroneo: %.2f\n"
#define MSGTR_RADIO_WrongChannelNumberInt "[radio] Número de canal erroneo: %d\n"
#define MSGTR_RADIO_WrongChannelName "[radio] Nombre de canal erroneo: %s\n"
#define MSGTR_RADIO_FreqParameterDetected "[radio] Parametro de frequencia de radio detectado.\n"
#define MSGTR_RADIO_GetTunerFailed "[radio] Advertencia: ioctl get tuner failed: %s. Setting frac to %d.\n"
#define MSGTR_RADIO_NotRadioDevice "[radio] %s no es un dispositivo de radio!\n"
#define MSGTR_RADIO_SetFreqFailed "[radio] ioctl set frequency 0x%x (%.2f) fallido: %s\n"
#define MSGTR_RADIO_GetFreqFailed "[radio] ioctl get frequency fallido: %s\n"
#define MSGTR_RADIO_SetMuteFailed "[radio] ioctl set mute fallido: %s\n"
#define MSGTR_RADIO_QueryControlFailed "[radio] ioctl query control fallido: %s\n"
#define MSGTR_RADIO_GetVolumeFailed "[radio] ioctl get volume fallido: %s\n"
#define MSGTR_RADIO_SetVolumeFailed "[radio] ioctl set volume fallido: %s\n"
#define MSGTR_RADIO_DroppingFrame "\n[radio] Muy malo - desechando frame de audio(%d bytes)!\n"
#define MSGTR_RADIO_BufferEmpty "[radio] grab_audio_frame: buffer vacio, esperando por %d bytes de datos.\n"
#define MSGTR_RADIO_AudioInitFailed "[radio] audio_in_init fallido: %s\n"
#define MSGTR_RADIO_AllocateBufferFailed "[radio] Imposible reservar buffer de audio (bloque=%d,buf=%d): %s\n"
#define MSGTR_RADIO_CurrentFreq "[radio] Frecuencia actual: %.2f\n"
#define MSGTR_RADIO_SelectedChannel "[radio] Canal seleccionado: %d - %s (freq: %.2f)\n"
#define MSGTR_RADIO_ChangeChannelNoChannelList "[radio] No puedo cambiar canal, no se ha entregado una lista de canales.\n"
#define MSGTR_RADIO_UnableOpenDevice "[radio] Imposible abrir '%s': %s\n"
#define MSGTR_RADIO_InitFracFailed "[radio] init_frac fallido.\n"
#define MSGTR_RADIO_WrongFreq "[radio] Frecuencia erronea: %.2f\n"
#define MSGTR_RADIO_UsingFreq "[radio] Utilizando frecuencia: %.2f.\n"
#define MSGTR_RADIO_AudioInInitFailed "[radio] audio_in_init fallido.\n"
#define MSGTR_RADIO_AudioInSetupFailed "[radio] audio_in_setup llamada fallida: %s\n"
#define MSGTR_RADIO_ClearBufferFailed "[radio] Fallo al limpiar el buffer: %s\n"
#define MSGTR_RADIO_StreamEnableCacheFailed "[radio] Llamada fallida a stream_enable_cache: %s\n"
#define MSGTR_RADIO_DriverUnknownStr "[radio] Nombre de driver desconocido: %s\n"
#define MSGTR_RADIO_DriverV4L2 "[radio] Utilizando interfaz de radio V4Lv2.\n"
#define MSGTR_RADIO_DriverV4L "[radio] Utilizando interfaz de radio V4Lv1.\n"
#define MSGTR_RADIO_DriverBSDBT848 "[radio] Usando la interfaz de radio *BSD BT848.\n"

//tv.c
#define MSGTR_TV_BogusNormParameter "tv.c: norm_from_string(%s): Parametro de normal inválido al configurar %s.\n"
#define MSGTR_TV_NoVideoInputPresent "Error: Entrada de video no encontrada!\n"
#define MSGTR_TV_UnknownImageFormat ""\
"==================================================================\n"\
" ADVERTENCIA: HA SELECCIONADO UN FORMATO DE SALIDA DE IMAGEN NO  \n"\
" CONOCIDO Y/O NO PROBADO (0x%x)                                  \n"\
" Esto podría causarle problemas de reproducción o que el programa\n"\
" se cayera! Los Bug reports serán ignorados! Intentelo nuevamente\n"\
" con YV12 (el colorspace por omisión) y leer la documentación.   \n"\
"==================================================================\n"
#define MSGTR_TV_CannotSetNorm "Error: No puedo configurar la norma!\n"
#define MSGTR_TV_MJP_WidthHeight "  MJP: ancho %d alto %d\n"
#define MSGTR_TV_UnableToSetWidth "Imposible configurar ancho requerido: %d\n"
#define MSGTR_TV_UnableToSetHeight "Imposible configurar alto requerido: %d\n"
#define MSGTR_TV_NoTuner "No hay un sintonizador en la entrada seleccionada!\n"
#define MSGTR_TV_UnableFindChanlist "Imposible encontrar lista de canales seleccionada! (%s)\n"
#define MSGTR_TV_ChannelFreqParamConflict "No puede configurar simultaneamente frecuencia y canal!\n"
#define MSGTR_TV_ChannelNamesDetected "Nombres de canales de TV detectados.\n"
#define MSGTR_TV_NoFreqForChannel "Imposible encontrar frecuencia para el canal %s (%s)\n"
#define MSGTR_TV_SelectedChannel3 "Canal seleccionado: %s - %s (freq: %.3f)\n"
#define MSGTR_TV_SelectedChannel2 "Canal seleccionado: %s (freq: %.3f)\n"
#define MSGTR_TV_UnsupportedAudioType "Tipo de audio '%s (%x)' no soportado!\n"
#define MSGTR_TV_AvailableDrivers "Drivers disponibles:\n"
#define MSGTR_TV_DriverInfo "Driver seleccionado: %s\n nombre: %s\n autor: %s\n comentario: %s\n"
#define MSGTR_TV_NoSuchDriver "Ese driver no existe: %s\n"
#define MSGTR_TV_DriverAutoDetectionFailed "Falló la utodetección del driver de TV.\n"
#define MSGTR_TV_UnknownColorOption "Se especificó una opción de color desconodida (%d)!\n"
#define MSGTR_TV_NoTeletext "Sin teletexto"
#define MSGTR_TV_Bt848IoctlFailed "tvi_bsdbt848: llamada a %s ioctl fallida. Error: %s\n"
#define MSGTR_TV_Bt848InvalidAudioRate "tvi_bsdbt848: Audio rate inválido. Error: %s\n"
#define MSGTR_TV_Bt848ErrorOpeningBktrDev "tvi_bsdbt848: Imposible abrir dispositivo bktr. Error: %s\n"
#define MSGTR_TV_Bt848ErrorOpeningTunerDev "tvi_bsdbt848: Imposible abrir dispositivo sintonizador. Error: %s\n"
#define MSGTR_TV_Bt848ErrorOpeningDspDev "tvi_bsdbt848: Imposible abrir dispositivo dsp. Error: %s\n"
#define MSGTR_TV_Bt848ErrorConfiguringDsp "tvi_bsdbt848: Falló la configuración del dsp. Error: %s\n"
#define MSGTR_TV_Bt848ErrorReadingAudio "tvi_bsdbt848: Error leyendo datos de audio. Error: %s\n"
#define MSGTR_TV_Bt848MmapFailed "tvi_bsdbt848: mmap fallido. Error: %s\n"
#define MSGTR_TV_Bt848FrameBufAllocFailed "tvi_bsdbt848: Fallo al reservar frame buffer (allocation failed). Error: %s\n"
#define MSGTR_TV_Bt848ErrorSettingWidth "tvi_bsdbt848: Error configurando el ancho del picture. Error: %s\n"
#define MSGTR_TV_Bt848ErrorSettingHeight "tvi_bsdbt848: Error configurando el alto del picture. Error: %s\n"
#define MSGTR_TV_Bt848UnableToStopCapture "tvi_bsdbt848: Imposible detener la captura. Error: %s\n"
#define MSGTR_TV_TTSupportedLanguages "Idiomas de teletexto soportados:\n"
#define MSGTR_TV_TTSelectedLanguage "Se seleccionó el idioma de teletexto por omisión: %s\n"
#define MSGTR_TV_ScannerNotAvailableWithoutTuner "No se puede utilizar el scanner de canales sin un sintonizador\n"

//tvi_dshow.c
#define MSGTR_TVI_DS_UnableConnectInputVideoDecoder  "Imposible conectar la entrada seleccionada con el decodificador de video. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableConnectInputAudioDecoder  "Imposible conectar la entrada seleccionada con el decodificador de audio. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableSelectVideoFormat "tvi_dshow: Imposible seleccioar formato de video. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableSelectAudioFormat "tvi_dshow: Imposible seleccionar formato de audio. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableGetMediaControlInterface "tvi_dshow: Imposible obtener interfaz IMediaControl. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableStartGraph "tvi_dshow: Imposible comenzar graph! Error:0x%x\n"
#define MSGTR_TVI_DS_DeviceNotFound "tvi_dshow: No se encontró el dispositivo #%d\n"
#define MSGTR_TVI_DS_UnableGetDeviceName "tvi_dshow: Imposible obtener nombre para dispositivo #%d\n"
#define MSGTR_TVI_DS_UsingDevice "tvi_dshow: Utilizando dispositivo #%d: %s\n"
#define MSGTR_TVI_DS_DirectGetFreqFailed "tvi_dshow: Imposible obtener la frecuencia directamente. Se utilizará la tabla de canales incluída con el OS.\n"
//following phrase will be printed near the selected audio/video input
#define MSGTR_TVI_DS_UnableExtractFreqTable "tvi_dshow: Imposible cargar tabla de frecuencias desde kstvtune.ax\n"
#define MSGTR_TVI_DS_WrongDeviceParam "tvi_dshow: Parametro de dispositivo inválido: %s\n"
#define MSGTR_TVI_DS_WrongDeviceIndex "tvi_dshow: Indice de dispositivo inválido: %d\n"
#define MSGTR_TVI_DS_WrongADeviceParam "tvi_dshow: Parametro de adevice inválido: %s\n"
#define MSGTR_TVI_DS_WrongADeviceIndex "tvi_dshow: Indice de adevice inválido: %d\n"
#define MSGTR_TVI_DS_SamplerateNotsupported "tvi_dshow: El dispositivo no soporta el Samplerate %d. Probando con el primero disponible.\n"
#define MSGTR_TVI_DS_VideoAdjustigNotSupported "tvi_dshow: El dispositivo no soporta el ajuste de brillo/hue/saturación/contraste\n"
#define MSGTR_TVI_DS_ChangingWidthHeightNotSupported "tvi_dshow: El dispositivo no soporta cambios de ancho/largo.\n"
#define MSGTR_TVI_DS_SelectingInputNotSupported  "tvi_dshow: El dispositivo no soporta la selección de la fuente de captura\n"
#define MSGTR_TVI_DS_ErrorParsingAudioFormatStruct "tvi_dshow: Imposible procesar la estructura del formato de audio.\n"
#define MSGTR_TVI_DS_ErrorParsingVideoFormatStruct "tvi_dshow: Imposible procesar la estructura del formato de video.\n"
#define MSGTR_TVI_DS_UnableSetAudioMode "tvi_dshow: Imposible seleccionar modo de audio %d. Error:0x%x\n"
#define MSGTR_TVI_DS_UnsupportedMediaType "tvi_dshow: A %s se le ha pasado un tipo de medios no soportado.\n"
#define MSGTR_TVI_DS_UnableGetsupportedVideoFormats "tvi_dshow: Imposible obtener los formatos de medios soportados desde el video pin. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableGetsupportedAudioFormats "tvi_dshow: Imposible obtener los formatos de medios soportados desde el audio pin. Error:0x%x Desabilitando audio.\n"
#define MSGTR_TVI_DS_UnableFindNearestChannel "tvi_dshow: Imposible encontrar el canal más cercano en la tabla de frecuencias del sistema\n"
#define MSGTR_TVI_DS_UnableToSetChannel "tvi_dshow: Imposible cambiar al canal más cercano en la tabla de frecuencias del sistema. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableTerminateVPPin "tvi_dshow: Imposible terminar pin VideoPort con cualquiera de los filtros en el graph. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableBuildVideoSubGraph "tvi_dshow: Imposible construir cadena de video del graph de captura. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableBuildAudioSubGraph "tvi_dshow: Imposible construir cadena de audio del graph de captura. Error:0x%x\n"
#define MSGTR_TVI_DS_UnableBuildVBISubGraph "tvi_dshow: Imposible construir cadena VBI del graph de captura. Error:0x%x\n"
#define MSGTR_TVI_DS_GraphInitFailure "tvi_dshow: Falló la inicialización del graph Directshow.\n"
#define MSGTR_TVI_DS_NoVideoCaptureDevice "tvi_dshow: No se pudo encontrar un dispositivo de captura de video\n"
#define MSGTR_TVI_DS_NoAudioCaptureDevice "tvi_dshow: No se pudo encontrar un dispositivo de captura de audio\n"
#define MSGTR_TVI_DS_GetActualMediatypeFailed "tvi_dshow: Imposible obtener el tipo de medios actual (Error:0x%x). Se asumirá que es igual al requerido.\n"


// ================================== LIBASS ====================================

// ass_bitmap.c
#define MSGTR_LIBASS_FT_Glyph_To_BitmapError "[ass] FT_Glyph_To_Bitmap error %d \n"
#define MSGTR_LIBASS_UnsupportedPixelMode "[ass] Modo píxel no soportado: %d\n"
#define MSGTR_LIBASS_GlyphBBoxTooLarge "[ass] Glyph bounding box muy grande: %dx%dpx\n"


// ass.c
#define MSGTR_LIBASS_NoStyleNamedXFoundUsingY "[ass] [%p] Aviso: no se ha encontrado ningun estilo llamado '%s', usando '%s'\n"
#define MSGTR_LIBASS_BadTimestamp "[ass] fecha/hora errónea\n"
#define MSGTR_LIBASS_BadEncodedDataSize "[ass] tamaño de datos erróneamente codificado\n"
#define MSGTR_LIBASS_FontLineTooLong "[ass] Línea demasiado grande para la fuente actual: %d, %s\n"
#define MSGTR_LIBASS_EventFormatHeaderMissing "[ass] No se encuentra el formato de la cabecera del evento\n"
#define MSGTR_LIBASS_ErrorOpeningIconvDescriptor "[ass] error abriendo el descriptor iconv.\n"
#define MSGTR_LIBASS_ErrorRecodingFile "[ass] error recodificando el fichero.\n"
#define MSGTR_LIBASS_FopenFailed "[ass] ass_read_file(%s): fopen ha fallado\n"
#define MSGTR_LIBASS_FseekFailed "[ass] ass_read_file(%s): fseek ha fallado\n"
#define MSGTR_LIBASS_RefusingToLoadSubtitlesLargerThan100M "[ass] ass_read_file(%s): No se pueden cargar ficheros de subtítulos mayores de 100M\n"
#define MSGTR_LIBASS_ReadFailed "La lectura ha fallado, %d: %s\n"
#define MSGTR_LIBASS_AddedSubtitleFileMemory "[ass] Añadido fichero de subtítulos: <memoria> (%d estilos, %d eventos)\n"
#define MSGTR_LIBASS_AddedSubtitleFileFname "[ass] Añadido fichero de subtítulos: %s (%d estilos, %d eventos)\n"
#define MSGTR_LIBASS_FailedToCreateDirectory "[ass] Ha fallado la creación del directorio %s\n"
#define MSGTR_LIBASS_NotADirectory "[ass] No es un directorio: %s\n"

// ass_cache.c
#define MSGTR_LIBASS_TooManyFonts "[ass] Demasiadas fuentes\n"
#define MSGTR_LIBASS_ErrorOpeningFont "[ass] Error abriendo la fuente: %s, %d\n"

// ass_fontconfig.c
#define MSGTR_LIBASS_SelectedFontFamilyIsNotTheRequestedOne "[ass] fontconfig: La familia de la fuente seleccionada no es la que se ha pedido: '%s' != '%s'\n"
#define MSGTR_LIBASS_UsingDefaultFontFamily "[ass] fontconfig_select: Usando el grupo de fuentes por defecto: (%s, %d, %d) -> %s, %d\n"
#define MSGTR_LIBASS_UsingDefaultFont "[ass] fontconfig_select: Usando la fuente por defecto : (%s, %d, %d) -> %s, %d\n"
#define MSGTR_LIBASS_UsingArialFontFamily "[ass] fontconfig_select: Usando el grupo de fuentes 'Arial': (%s, %d, %d) -> %s, %d\n"
#define MSGTR_LIBASS_FcInitLoadConfigAndFontsFailed "[ass] FcInitLoadConfigAndFonts ha fallado.\n"
#define MSGTR_LIBASS_UpdatingFontCache "[ass] Actualizando la caché de la fuente.\n"
#define MSGTR_LIBASS_BetaVersionsOfFontconfigAreNotSupported "[ass] Las versiones Beta de fontconfig no están soportadas.\n[ass] Actualiza la version antes de enviar ningún fallo.\n"
#define MSGTR_LIBASS_FcStrSetAddFailed "[ass] FcStrSetAdd ha fallado.\n"
#define MSGTR_LIBASS_FcDirScanFailed "[ass] FcDirScan ha fallado.\n"
#define MSGTR_LIBASS_FcDirSave "[ass] FcDirSave ha fallado.\n"
#define MSGTR_LIBASS_FcConfigAppFontAddDirFailed "[ass] FcConfigAppFontAddDir ha fallado\n"
#define MSGTR_LIBASS_FontconfigDisabledDefaultFontWillBeUsed "[ass] Fontconfig deshabilitado, sólo se utilizará la fuente por defecto.\n"
#define MSGTR_LIBASS_FunctionCallFailed "[ass] %s ha fallado\n"

// ass_render.c
#define MSGTR_LIBASS_NeitherPlayResXNorPlayResYDefined "[ass] Ni PlayResX ni PlayResY están definidos. Asumiendo 384x288.\n"
#define MSGTR_LIBASS_PlayResYUndefinedSettingY "[ass] PlayResY no está definido, estableciendo %d.\n"
#define MSGTR_LIBASS_PlayResXUndefinedSettingX "[ass] PlayResX no está definido, estableciendo %d.\n"
#define MSGTR_LIBASS_FT_Init_FreeTypeFailed "[ass] FT_Init_FreeType ha fallado.\n"
#define MSGTR_LIBASS_Init "[ass] Inicialización\n"
#define MSGTR_LIBASS_InitFailed "[ass] La inicialización ha fallado.\n"
#define MSGTR_LIBASS_BadCommand "[ass] Comando erróneo: %c%c\n"
#define MSGTR_LIBASS_ErrorLoadingGlyph  "[ass] Error cargando el glifo.\n"
#define MSGTR_LIBASS_FT_Glyph_Stroke_Error "[ass] FT_Glyph_Stroke error %d \n"
#define MSGTR_LIBASS_UnknownEffectType_InternalError "[ass] Tipo de efecto desconocido (error interno)\n"
#define MSGTR_LIBASS_NoStyleFound "[ass] No se ha encontrado ningún estilo!\n"
#define MSGTR_LIBASS_EmptyEvent "[ass] Evento vacío!\n"
#define MSGTR_LIBASS_MAX_GLYPHS_Reached "[ass] MAX_GLYPHS alcanzado: evento %d, comienzo = %llu, duración = %llu\n Texto = %s\n"
#define MSGTR_LIBASS_EventHeightHasChanged "[ass] ¡Aviso! ¡El tamaño del evento ha cambiado!  \n"

// ass_font.c
#define MSGTR_LIBASS_GlyphNotFoundReselectingFont "[ass] Glifo 0x%X no encontrado, seleccionando la fuente para (%s, %d, %d)\n"
#define MSGTR_LIBASS_GlyphNotFound "[ass] No se ha encontrado el glifo 0x%X en la fuente para (%s, %d, %d)\n"
#define MSGTR_LIBASS_ErrorOpeningMemoryFont "[ass] Error abriendo la fuente en memoria: %s\n"
#define MSGTR_LIBASS_NoCharmaps "[ass] Este font face no tiene charmaps\n"
#define MSGTR_LIBASS_NoCharmapAutodetected "[ass] No se detectó un charmap automaticamente, probando con el primero\n"
