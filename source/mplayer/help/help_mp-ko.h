// Fully sync'ed with help_mp-en.h 1.120
// Translated by: DongCheon Park <dcpark@kaist.ac.kr>

// ========================= MPlayer help ===========================

static const char help_text[]=
"사용법:   mplayer [선�?사항] [url|경로/]파�?�명\n"
"\n"
"기본 선�?사항: (전체 목�?�?� man 페�?�지 참조)\n"
" -vo <drv[:dev]>  비디오 출력 드�?��?�버 �? 장치 선�? (목�?보기는 '-vo help')\n"
" -ao <drv[:dev]>  오디오 출력 드�?��?�버 �? 장치 선�? (목�?보기는 '-ao help')\n"
#ifdef CONFIG_VCD
" vcd://<trackno>  (S)VCD (Super Video CD) 트랙 재�? (장치로부터, 마운트 없�?�)\n"
#endif
#ifdef CONFIG_DVDREAD
" dvd://<titleno>  �?�반 파�?��?� 아닌 장치로부터 DVD 타�?�틀 재�?\n"
" -alang/-slang    DVD 오디오/�?막 언어 선�? (�? 글�?�?� 국가 코드)\n"
#endif
" -ss <timepos>    특정 위치로 찾아가기 (초 �?는 시:분:초)\n"
" -nosound         소리 재�? 안함\n"
" -fs              전체화면 재�? (�?는 -vm, -zoom, �?세한 사항�?� man 페�?�지)\n"
" -x <x> -y <y>    화면�?� <x>*<y>해�?�?�로 설정 (-vm�?�나 -zoom과 함께 사용함)\n"
" -sub <file>      사용할 �?막 파�?� 지정 (-subfps, -subdelay�?� 참고할 것)\n"
" -playlist <file> 재�?목�? 파�?� 지정\n"
" -vid x -aid y    재�?할 비디오(x) 와 오디오(y) 스트림 선�?\n"
" -fps x -srate y  비디오(x fps)와 오디오(y Hz) 비율 변경\n"
" -pp <quality>    후행처리 필터 사용 (�?세한 사항�?� man 페�?�지 참조)\n"
" -framedrop       프레임 건너뛰기 사용 (�?린 컴퓨터용 선�?사항)\n"
"\n"
"기본 조정키: (전체 조정키 목�?�?� man 페�?�지 참조, input.conf�?� 확�?�할 것)\n"
" <-  �?는  ->     10초 뒤로/앞으로 �?��?�\n"
" up �?는 down     1분 뒤로/앞으로 �?��?�\n"
" pgup �?는 pgdown 10분 뒤로/앞으로 �?��?�\n"
" < �?는 >         재�?목�?�?서 뒤로/앞으로 �?��?�\n"
" p �?는 SPACE     잠시 멈춤 (아무키나 누르면 계�?)\n"
" q �?는 ESC       재�?�?� 멈추고 프로그램�?� �??냄\n"
" + �?는 -         +/- 0.1초씩 오디오 지연 조정\n"
" o                OSD모드 변경:  없�?�/�?색줄/�?색줄+타�?�머\n"
" * �?는 /         PCM �?�량�?� 높임/낮춤\n"
" z �?는 x         +/- 0.1초씩 �?막 지연 조정\n"
" r �?는 t         �?막 위치를 위로/아래로 조정, -vf expand�?� 참고할 것\n"
"\n"
" * * * �?� �?세한 (고급) 선�?사항 �? 조정키는 MAN 페�?�지를 참조하세요. * * *\n"
"\n";

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\n종료합니다.\n"
#define MSGTR_ExitingHow "\n종료합니다... (%s)\n"
#define MSGTR_Exit_quit "종료"
#define MSGTR_Exit_eof "파�?��?� �??"
#define MSGTR_Exit_error "치명�? 오류"
#define MSGTR_IntBySignal "\nMPlayer가 %d시그�?�? �?�해 �?�터럽트�?�었습니다. - 모듈: %s\n"
#define MSGTR_NoHomeDir "홈디렉토리를 찾�?� 수 없습니다.\n"
#define MSGTR_GetpathProblem "get_path(\"config\") 문제 발�?\n"
#define MSGTR_CreatingCfgFile "설정파�?��?� 만듭니다.: %s\n"
#define MSGTR_CantLoadFont "�?�트를 �?�어 들�?� 수 없습니다.: %s\n"
#define MSGTR_CantLoadSub "�?막�?� �?�어 들�?� 수 없습니다.: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: 치명�? : 선�?�?� 스트림�?� 없습니다!\n"
#define MSGTR_CantOpenDumpfile "dump파�?��?� 열 수 없습니다.\n"
#define MSGTR_CoreDumped "Core dumped :)\n"
#define MSGTR_FPSnotspecified "헤�?��? FPS가 지정�?�지 않았거나 유효하지 않습니다. -fps 옵션�?� 사용하세요.\n"
#define MSGTR_TryForceAudioFmtStr "오디오 코�?� 드�?��?�버 집합 %s를 시�?�하고 있습니다...\n"
#define MSGTR_CantFindAudioCodec "오디오 형�? 0x%X를 위한 코�?��?� 찾�?� 수 없습니다.\n"
#define MSGTR_TryForceVideoFmtStr "비디오 코�?� 드�?��?�버 집합 %s를 시�?�하고 있습니다...\n"
#define MSGTR_CantFindVideoCodec "선�?한 -vo �? 비디오 형�? 0x%X과 �?�치하는 코�?��?� 찾�?� 수 없습니다.\n"
#define MSGTR_VOincompCodec "선�?한 비디오 출력 장치는 �?� 코�?�과 호환�?�지 않습니다.\n"
#define MSGTR_CannotInitVO "치명�? 오류: 비디오 드�?��?�버를 초기화할 수 없습니다.\n"
#define MSGTR_CannotInitAO "오디오 장치를 열거나 초기화할 수 없습니다. -> 소리없�?�\n"
#define MSGTR_StartPlaying "재�?�?� 시작합니다...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"         ************************************************\n"\
"         **** 재�?하기�?는 시스템�?� 너무 �?립니다.!  ****\n"\
"         ************************************************\n"\
"가능성있는 �?�?�과 문제 �? 대처방안: \n"\
"- 대부분�?� 경우: 깨진/버그가 많�?� 오디오 드�?��?�버\n"\
"  - -ao sdl�?� 시�?�하거나 ALSA 0.5, 혹�?� ALSA 0.9�?� OSS �?뮬레�?�션�?� 사용해보세요.\n"\
"  - -autosync를으로 여러가지 값으로 실험해보세요. 시작 값으로는 30�?� �?당합니다.\n"\
"- 비디오 출력�?� �?림\n"\
"  - 다른 -vo driver를 시�?�하거나 (목�?보기는 -vo help), -framedrop�?� 시�?�해보세요!\n"\
"- �?린 CPU\n"\
"  - �?�치 �?� DVD나 DivX를 �?린 CPU�?서 재�?하지 마세요! -hardframedrop�?� 시�?�해보세요.\n"\
"- 깨진 파�?�\n"\
"  - -nobps -ni -forceidx -mc 0 등�?� 여러가지 조합�?� 시�?�해보세요.\n"\
"- �?린 미디어 (NFS/SMB 마운트, DVD, VCD 등)\n"\
"  - -cache 8192를 시�?�해보세요.\n"\
"- non-interleaved AVI 파�?��?� -cache 옵션으로 재�?하고 있나요?\n"\
"  - -nocache를 시�?�해보세요.\n"\
"미세조정/�?�?�향�? �?�?� DOCS/HTML/en/video.html과 DOCS/HTML/en/audio.html�?� 참조하세요.\n"\
"위�?� 어떤 사항�?� �?�움�?� �?�지 않는다면, DOCS/HTML/en/bugreports.html�?� 참조하세요.\n\n"

#define MSGTR_NoGui "MPlayer가 GUI를 사용할 수 있�?��? 컴파�?��?�지 않았습니다.\n"
#define MSGTR_GuiNeedsX "MPlayer GUI는 X11�?� 필요로합니다!\n"
#define MSGTR_Playing "%s 재�? 중...\n"
#define MSGTR_NoSound "오디오: 소리없�?�\n"
#define MSGTR_FPSforced "FPS가 %5.3f (ftime: %5.3f)으로 변경�?�었습니다.\n"
#define MSGTR_AvailableVideoOutputDrivers "가능한 비디오 출력 드�?��?�버:\n"
#define MSGTR_AvailableAudioOutputDrivers "가능한 오디오 출력 드리아버:\n"
#define MSGTR_AvailableAudioCodecs "가능한 오디오 코�?�:\n"
#define MSGTR_AvailableVideoCodecs "가능한 비디오 코�?�:\n"
#define MSGTR_AvailableAudioFm "\n가능한 (컴파�?��?�) 오디오 코�?� 집합/드�?��?�버:\n"
#define MSGTR_AvailableVideoFm "\n가능한 (컴파�?��?�) 비디오 코�?� 집합/드�?��?�버:\n"
#define MSGTR_AvailableFsType "가능한 전체화면 레�?�어 변경 모드:\n"
#define MSGTR_CannotReadVideoProperties "비디오: �?성�?� �?��?� 수 없습니다.\n"
#define MSGTR_NoStreamFound "스티림�?� 찾�?� 수 없습니다.\n"
#define MSGTR_ErrorInitializingVODevice "선�?한 비디오 출력 (-vo) 장치를 열거나 초기화할 수 없습니다.\n"
#define MSGTR_ForcedVideoCodec "강제로 사용�?� 비디오 코�?�: %s\n"
#define MSGTR_ForcedAudioCodec "강제로 사용�?� 오디오 코�?�: %s\n"
#define MSGTR_Video_NoVideo "비디오: 비디오 없�?�\n"
#define MSGTR_NotInitializeVOPorVO "\n치명�? 오류: 비디오 필터(-vf) �?는 비디오 출력(-vo)�?� 초기화할 수 없습니다.\n"
#define MSGTR_Paused "  =====  잠시멈춤  ====="
#define MSGTR_PlaylistLoadUnable "\n재�?목�? %s�?�(를) 열 수 없습니다.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer가 '잘못�?� 연산'으로 종료�?�었습니다.\n"\
"  런타임 CPU �?지 코드�? 버그가 있�?� 지�?� 모릅니다...\n"\
"  DOCS/HTML/en/bugreports.html�?� 참조하세요.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer가 '잘못�?� 연산'으로 종료�?�었습니다.\n"\
"  컴파�?�/최�?화�?� CPU와 다른 모�?��?� CPU�?서 실행�?� 때\n"\
"  종종 �?�어나는 현�?입니다.\n"\
"  확�?�해 보세요!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer가 잘못�?� CPU/FPU/RAM�?� 사용으로 종료�?�었습니다.\n"\
"  MPlayer를 --enable-debug 옵션으로 다시 컴파�?�하고, 'gdb' 백트레�?�스 �?\n"\
"  디스어셈블해보세요. �?세한 사항�?� DOCS/HTML/en/bugreports_what.html#bugreports_crash를 참조하세요.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer가 알 수 없는 �?�유로 종료�?�었습니다.\n"\
"  MPlayer 코드나 드�?��?�버�?� 버그, 혹�?� gcc버전�?� 문제�?� 수�?� 있습니다.\n"\
"  MPlayer�?� 문제�?�고 �?�?한다면, DOCS/HTML/en/bugreports.html�?� �?�고 거기있는\n"\
"  설명대로 하시기 바�?니다. 가능한 버그를 보고할 �?, �?� 정보를 �?�함하세요.\n"\
"  그렇지 않으면, �?�와줄 방법�?� 없습니다.\n"


// mencoder.c:

#define MSGTR_UsingPass3ControlFile "pass3 컨트롤 파�?��?� 사용합니다.: %s\n"
#define MSGTR_MissingFilename "\n파�?��?�름�?� 없습니다.\n\n"
#define MSGTR_CannotOpenFile_Device "파�?�/장치를 열 수 없습니다.\n"
#define MSGTR_CannotOpenDemuxer "해�?기를 열 수 없습니다.\n"
#define MSGTR_NoAudioEncoderSelected "\n선�?�?� 오디오 �?�코�?�(-oac)가 없습니다. 하나를 선�?하거나, -nosound 옵션�?� 사용하세요. -oac help를 참조하세요!\n"
#define MSGTR_NoVideoEncoderSelected "\n선�?�?� 비디오 �?�코�?�(-ovc)가 없습니다. 하나를 선�?거나, -ovc help를 참조하세요!\n"
#define MSGTR_CannotOpenOutputFile "출력 파�?� '%s'�?�(를) 열 수 없습니다.\n"
#define MSGTR_EncoderOpenFailed "�?�코�?� 열기�? 실패했습니다.\n"
#define MSGTR_ForcingOutputFourcc "fourcc를 %x [%.4s](으)로 강제출력합니다.\n"
#define MSGTR_DuplicateFrames "\n%d 프레임(들)�?� 중복�?�었습니다!\n"
#define MSGTR_SkipFrame "\n프레임�?� 건너 �?니다!\n"
#define MSGTR_ErrorWritingFile "%s: 파�?� 쓰기 오류가 발�?했습니다.\n"
#define MSGTR_RecommendedVideoBitrate "%s CD용으로 추천할 만한 비디오 주사율: %d\n"
#define MSGTR_VideoStreamResult "\n비디오 스트림: %8.3f kbit/s  (%d B/s)  �?�기: %"PRIu64" 바�?�트  %5.3f 초  %d 프레임\n"
#define MSGTR_AudioStreamResult "\n오디오 스트림: %8.3f kbit/s  (%d B/s)  �?�기: %"PRIu64" 바�?�트  %5.3f 초\n"

// cfg-mencoder.h:

#define MSGTR_MEncoderMP3LameHelp "\n\n"\
" vbr=<0-4>     가변 비트레�?�트 방�?\n"\
"                0: cbr\n"\
"                1: mt\n"\
"                2: rh(기본값)\n"\
"                3: abr\n"\
"                4: mtrh\n"\
"\n"\
" abr           �?�균 비트레�?�트\n"\
"\n"\
" cbr           고정 비트레�?�트\n"\
"               �?�련�?� ABR 프리셋 모드들�?서 CBR모드 강제 사용함.\n"\
"\n"\
" br=<0-1024>   비트레�?�트를 kBit단위로 지정 (CBR �? ABR�?서만)\n"\
"\n"\
" q=<0-9>       �?�질 (0-최고, 9-최저) (VBR�?서만)\n"\
"\n"\
" aq=<0-9>      연산 �?�질 (0-최고/�?림, 9-최저/빠름)\n"\
"\n"\
" ratio=<1-100> 압축률\n"\
"\n"\
" vol=<0-10>    오디오 입력 �?�량 조절\n"\
"\n"\
" mode=<0-3>    (기본값: �?�?�)\n"\
"                0: 스테레오\n"\
"                1: 조�?�트-스테레오\n"\
"                2: 듀얼채�?\n"\
"                3: 모노\n"\
"\n"\
" padding=<0-2>\n"\
"                0: 안함\n"\
"                1: 모�?\n"\
"                2: 조정\n"\
"\n"\
" fast          �?�련�?� VBR 프리셋 모드들�?서 �?� 빠른 �?�코딩 사용,\n"\
"               �?�질�?� 조금 저하�?�고 비트레�?�트가 조금 �?� 높아�?.\n"\
"\n"\
" preset=<value> 최�?�?� 가능한 �?�질 세팅들.\n"\
"                 medium: VBR  �?�코딩, 좋�?� �?�질\n"\
"                 (150-180 kbps 비트레�?�트 범위)\n"\
"                 standard:  VBR �?�코딩, 높�?� �?�질\n"\
"                 (170-210 kbps 비트레�?�트 범위)\n"\
"                 extreme: VBR �?�코딩, 매우 높�?� �?�질\n"\
"                 (200-240 kbps 비트레�?�트 범위)\n"\
"                 insane:  CBR  �?�코딩, 가장 높�?� �?�질\n"\
"                 (320 kbps 비트레�?�트 고정)\n"\
"                 <8-320>: 주어진 kbps 비트레�?�트�?� �?�균치로 ABR �?�코딩.\n\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM 장치 '%s'를 찾�?� 수 없습니다!\n"
#define MSGTR_ErrTrackSelect "VCD 트랙�?� 선�?하는 �?�중 �?러가 발�?했습니다."
#define MSGTR_ReadSTDIN "표준입력(stdin)으로 부터 �?�고 있습니다...\n"
#define MSGTR_UnableOpenURL "URL�?� 열 수 없습니다.: %s\n"
#define MSGTR_ConnToServer "서버�? 연결�?�었습니다.: %s\n"
#define MSGTR_FileNotFound "파�?��?� 찾�?� 수 없습니다.: '%s'\n"

#define MSGTR_SMBInitError "libsmbclient �?��?�브러리를 초기화할 수 없습니다.: %d\n"
#define MSGTR_SMBFileNotFound "lan으로 부터 열 수 없습니다.: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer가 SMB�?�기를 할 수 있�?��? 컴파�?��?�지 않았습니다.\n"

#define MSGTR_CantOpenDVD "DVD 장치를 열 수 없습니다.: %s (%s)\n"
#define MSGTR_DVDnumTitles "�?� DVD�?는 %d개�?� 타�?�틀�?� 있습니다.\n"
#define MSGTR_DVDinvalidTitle "유효하지 않�?� DVD 타�?�틀 번호입니다.: %d\n"
#define MSGTR_DVDnumChapters "�?� DVD 타�?�틀�?는 %d개�?� 챕터가 있습니다.\n"
#define MSGTR_DVDinvalidChapter "유효하지 않�?� DVD 챕터 번호입니다.: %d\n"
#define MSGTR_DVDnumAngles "�?� DVD 타�?�틀�?는 %d개�?� 앵글�?� 있습니다.\n"
#define MSGTR_DVDinvalidAngle "유효하지 않�?� DVD 앵글 번호입니다.: %d\n"
#define MSGTR_DVDnoIFO "DVD 타�?�틀 %d를 위한 IFO파�?��?� 열 수 없습니다.\n"
#define MSGTR_DVDnoVOBs "타�?�틀 VOBS (VTS_%02d_1.VOB)를 열 수 없습니다.\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "경고: 오디오 스트림 헤�?� %d가 재정�?��?�었습니다.\n"
#define MSGTR_VideoStreamRedefined "경고: 비디오 스트림 헤�?� %d가 재정�?��?�었습니다.\n"
#define MSGTR_TooManyAudioInBuffer "\n버�?��? 너무 많�?� 오디오 패킷�?� 있습니다.: (%d in %d bytes)\n"
#define MSGTR_TooManyVideoInBuffer "\n버�?��? 너무 많�?� 비디오 패킷�?� 있습니다.: (%d in %d bytes)\n"
#define MSGTR_MaybeNI "non-interleaved 스트림/파�?��?� 재�?하고있거나 코�?��? 문제가 있나요?\n" \
                      "AVI 파�?��?� 경우, -ni 옵션으로 non-interleaved 모드로 강제 시�?�해보세요.\n"
#define MSGTR_SwitchToNi "\n잘못�?� interleaved AVI 파�?��?� 발견했습니다. -ni 모드로 변경합니다...\n"
#define MSGTR_Detected_XXX_FileFormat "%s 파�?� 형�?�?� 발견했습니다.\n"
#define MSGTR_DetectedAudiofile "오디오 파�?��?� �?지하였습니다.\n"
#define MSGTR_InvalidMPEGES "유효하지 않�?� MPEG-ES 스트림??? 저작�?�?게 문�?�하세요, 버그�?�지�?� 모릅니다. :(\n"
#define MSGTR_FormatNotRecognized "============= 죄송합니다. �?� 파�?�형�?�?� �?��?하지못했거나 지�?하지않습니다 ===============\n"\
                                  "=== 만약 �?� 파�?��?� AVI, ASF �?는 MPEG 스트림�?��?�면, 저작�?�?게 문�?�하세요! ===\n"
#define MSGTR_MissingVideoStream "비디오 스트림�?� 찾지 못했습니다.\n"
#define MSGTR_MissingAudioStream "오디오 스트림�?� 찾지 못했습니다. -> 소리없�?�\n"
#define MSGTR_MissingVideoStreamBug "찾�?� 수 없는 비디오 스트림!? 저작�?�?게 문�?�하세요, 버그�?�지�?� 모릅니다. :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: 파�?��? 선�?�?� 오디오 �? 비디오 스트림�?� 없습니다.\n"

#define MSGTR_NI_Forced "강제로"
#define MSGTR_NI_Detected "발견�?�"
#define MSGTR_NI_Message "%s NON-INTERLEAVED AVI 파�?� 형�?입니다.\n"

#define MSGTR_UsingNINI "NON-INTERLEAVED 깨진 AVI 파�?� 형�?�?� 사용중입니다.\n"
#define MSGTR_CouldntDetFNo "(절대 �?색�?� 위한) 프레임 수를 결정할 수 없습니다.\n"
#define MSGTR_CantSeekRawAVI "raw AVI 스트림�?서는 �?색할 수 없습니다. (�?��?�스가 필요합니다. -idx 스위치로 시�?�해보세요.)  \n"
#define MSGTR_CantSeekFile "�?� 파�?��?서는 �?색할 수 없습니다.\n"

#define MSGTR_MOVcomprhdr "MOV: 압축�?� 헤�?�는 (아�?) 지�?�?�지않습니다.\n"
#define MSGTR_MOVvariableFourCC "MOV: 경고: 가변�?�?� FOURCC 발견!?\n"
#define MSGTR_MOVtooManyTrk "MOV: 경고: 트랙�?� 너무 많습니다."
#define MSGTR_DetectedTV "TV를 발견하였습니다! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "ogg 해�?기를 열 수 없습니다.\n"
#define MSGTR_CannotOpenAudioStream "오디오 스트림�?� 열 수 없습니다.: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "�?막 스트림�?� 열 수 없습니다.: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "오디오 해�?기를 여는�?� 실패했습니다.: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "�?막 해�?기를 여는�?� 실패했습니다.: %s\n"
#define MSGTR_TVInputNotSeekable "TV 입력�?� 찾�?� 수 없습니다! (채�?�?� 바꾸고 하면 �?�수�?� 있습니다. ;)\n"
#define MSGTR_ClipInfo "�?�립 정보: \n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: 30fps NTSC 항목�?� �?지하여, 프레임�?�?�를 바꿉니다.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: 24fps progressive NTSC 항목�?� �?지하여, 프레임�?�?�를 바꿉니다.\n"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "코�?��?� 열 수 없습니다.\n"
#define MSGTR_CantCloseCodec "코�?��?� 닫�?� 수 없습니다.\n"

#define MSGTR_MissingDLLcodec "오류: 요청한 DirectShow 코�?� %s를 열 수 없습니다.\n"
#define MSGTR_ACMiniterror "Win32/ACM 오디오 코�?��?� 열거나 초기화할 수 없습니다. (DLL 파�?��?� 없나요?)\n"
#define MSGTR_MissingLAVCcodec "libavcodec�?서 '%s' 코�?��?� 찾�?� 수 없습니다...\n"

#define MSGTR_MpegNoSequHdr "MPEG: 치명�? 오류: 시퀀스 헤�?�를 찾는 �?�중 EOF.\n"
#define MSGTR_CannotReadMpegSequHdr "치명�? 오류: 시퀀스 헤�?�를 �?��?� 수 없습니다.\n"
#define MSGTR_CannotReadMpegSequHdrEx "치명�? 오류: 시퀀스 헤�?� 확장�?� �?��?� 수 없습니다.\n"
#define MSGTR_BadMpegSequHdr "MPEG: 잘못�?� 시퀀스 헤�?�입니다.\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: 잘못�?� 시퀀스 헤�?� 확장입니다.\n"

#define MSGTR_ShMemAllocFail "공유 메모리를 할당할 수 없습니다.\n"
#define MSGTR_CantAllocAudioBuf "오디오 출력 버�?�를 할당할 수 없습니다.\n"

#define MSGTR_UnknownAudio "알 수 없는 오디오 형�?입니다. -> 소리없�?�\n"

#define MSGTR_UsingExternalPP "[PP] 외부 후행처리 필터를 사용합니다. max q = %d\n"
#define MSGTR_UsingCodecPP "[PP] 코�?��?� 후행처리를 사용합니다. max q = %d\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "요청한 비디오 코�?� 집합 [%s] (vfm=%s)�?�(를) 사용할 수 없습니다. (컴파�?�시�? 가능하�?��? 설정하세요.)\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "요청한 오디오 코�? 집합 [%s] (afm=%s)�?�(를) 사용할 수 없습니다. (컴파�?�시�? 가능하�?��? 설정하세요.)\n"
#define MSGTR_OpeningVideoDecoder "비디오 디코�?�를 열고 있습니다.: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "오디오 디코�?�를 열고 있습니다.: [%s] %s\n"
#define MSGTR_VDecoderInitFailed "비디오 디코�?� 초기화를 실패했습니다. :(\n"
#define MSGTR_ADecoderInitFailed "오디오 디코�?� 초기화를 실패했습니다. :(\n"
#define MSGTR_ADecoderPreinitFailed "오디오 디코�?� 사전 초기화를 실패했습니다. :(\n"

// LIRC:
#define MSGTR_LIRCopenfailed "LIRC 지�? 시작�?� 실패했습니다.\n"
#define MSGTR_LIRCcfgerr "LIRC 설정파�?� %s를 �?�는�?� 실패했습니다.\n"

// vf.c:
#define MSGTR_CouldNotFindVideoFilter "비디오 필터 '%s'�?�(를) 찾�?� 수 없습니다.\n"
#define MSGTR_CouldNotOpenVideoFilter "비디오 필터 '%s'�?�(를) 열 수 없습니다.\n"
#define MSGTR_OpeningVideoFilter "비디오 필터를 열고 있습니다.: "
#define MSGTR_CannotFindColorspace "'scale'�?� 시�?�했지만, 맞는 컬러공간�?� 찾�?� 수 없습니다. :(\n"

// vd.c
#define MSGTR_CodecDidNotSet "VDec: 코�?��?� sh->disp_w와 sh->disp_h로 설정�?�지 않아서, 다시 시�?�합니다.\n"
#define MSGTR_CouldNotFindColorspace "어울리는 컬러공간�?� 찾�?� 수 없습니다. -vf �?�기조절로 다시 시�?�합니다...\n"
#define MSGTR_MovieAspectIsSet "화면비율�?� %.2f:1 입니다. - 화면비율�?� 조정하기위해 사전 �?�기조절�?� 합니다.\n"
#define MSGTR_MovieAspectUndefined "화면비율�?� 정�?��?�지 않았습니다. - 사전 �?�기조절�?� �?용�?�지 않았습니다.\n"

// x11_common.c
#define MSGTR_EwmhFullscreenStateFailed "\nX11: EWMH 전체화면 �?�벤트를 보낼 수 없습니다!\n"


// ====================== GUI messages/buttons ========================

#ifdef CONFIG_GUI

// --- labels ---
#define MSGTR_About "정보"
#define MSGTR_FileSelect "파�?� 선�?..."
#define MSGTR_SubtitleSelect "�?막 선�?..."
#define MSGTR_OtherSelect "선�?..."
#define MSGTR_AudioFileSelect "�?�악 파�?� 선�?..."
#define MSGTR_FontSelect "글꼴 선�?..."
#define MSGTR_PlayList "재�?목�?"
#define MSGTR_Equalizer "�?�퀄�?��?�저"
#define MSGTR_SkinBrowser "스킨 찾기"
#define MSGTR_Network "네트워�?� 스트리�?..."
#define MSGTR_Preferences "선�?사항"
#define MSGTR_NoMediaOpened "미디어 없�?�"
#define MSGTR_NoChapter "챕터 없�?�"
#define MSGTR_Chapter "챕터 %d"
#define MSGTR_NoFileLoaded "파�?� 없�?�"

// --- buttons ---
#define MSGTR_Ok "확�?�"
#define MSGTR_Cancel "취소"
#define MSGTR_Add "추가"
#define MSGTR_Remove "삭제"
#define MSGTR_Clear "지움"
#define MSGTR_Config "설정"
#define MSGTR_ConfigDriver "드�?��?�버 설정"
#define MSGTR_Browse "열기"

// --- error messages ---
#define MSGTR_NEMDB "죄송합니다. 그리기 버�?�를 위한 충분한 메모리가 없습니다."
#define MSGTR_NEMFMR "죄송합니다. 메뉴 렌�?��?�?� 위한 충분한 메모리가 없습니다."
#define MSGTR_IDFGCVD "죄송합니다. GUI 호환 비디오 출력 드�?��?�버를 찾지 못했습니다."

// --- skin loader error messages
#define MSGTR_SKIN_ERRORMESSAGE "[스킨] 스킨 설정파�?��?� %d번째 줄�? 오류가 있습니다.: %s"
#define MSGTR_SKIN_BITMAP_16bit  "16 비트 혹�?� �?� 낮�?� 품질�?� 비트맵�?� 지�?�?�지 않습니다. (%s)\n"
#define MSGTR_SKIN_BITMAP_FileNotFound  "파�?��?� 찾�?� 수 없습니다. (%s)\n"
#define MSGTR_SKIN_BITMAP_PNGReadError "PNG �?�기 오류입니다. (%s)\n"
#define MSGTR_SKIN_BITMAP_ConversionError "24 비트�?서 32 비트로 전환 오류 (%s)\n"
#define MSGTR_SKIN_UnknownMessage "알 수 없는 메세지입니다.: %s\n"
#define MSGTR_SKIN_NotEnoughMemory "메모리가 부족합니다.\n"
#define MSGTR_SKIN_FONT_TooManyFontsDeclared "�?�트가 너무 많�?� 선언�?�어 있습니다.\n"
#define MSGTR_SKIN_FONT_FontFileNotFound "�?�트파�?��?� 찾�?� 수 없습니다.\n"
#define MSGTR_SKIN_FONT_FontImageNotFound "�?�트 �?�미지파�?��?� 찾�?� 수 없습니다.\n"
#define MSGTR_SKIN_FONT_NonExistentFont "�?�트 지정�?가 존재하지 않습니다. (%s)\n"
#define MSGTR_SKIN_UnknownParameter "알 수 없는 매개변수입니다. (%s)\n"
#define MSGTR_SKIN_SKINCFG_SkinNotFound "스킨�?� 찾�?� 수 없습니다. (%s)\n"
#define MSGTR_SKIN_LABEL "스킨:"

// --- gtk menus
#define MSGTR_MENU_AboutMPlayer "MPlayer 정보"
#define MSGTR_MENU_Open "열기..."
#define MSGTR_MENU_PlayFile "파�?� 재�?..."
#define MSGTR_MENU_PlayVCD "VCD 재�?..."
#define MSGTR_MENU_PlayDVD "DVD 재�?..."
#define MSGTR_MENU_PlayURL "URL 재�?..."
#define MSGTR_MENU_LoadSubtitle "�?막 선�?..."
#define MSGTR_MENU_DropSubtitle "�?막 없앰..."
#define MSGTR_MENU_LoadExternAudioFile "�?�악 파�?�..."
#define MSGTR_MENU_Playing "작�?�"
#define MSGTR_MENU_Play "재�?"
#define MSGTR_MENU_Pause "멈춤"
#define MSGTR_MENU_Stop "정지"
#define MSGTR_MENU_NextStream "다�?�"
#define MSGTR_MENU_PrevStream "�?�전"
#define MSGTR_MENU_Size "�?�기"
#define MSGTR_MENU_NormalSize "보통 �?�기"
#define MSGTR_MENU_DoubleSize "�?배 �?�기"
#define MSGTR_MENU_FullScreen "전체 화면"
#define MSGTR_MENU_DVD "DVD"
#define MSGTR_MENU_VCD "VCD"
#define MSGTR_MENU_PlayDisc "디스�?� 열기..."
#define MSGTR_MENU_ShowDVDMenu "DVD 메뉴보기"
#define MSGTR_MENU_Titles "타�?�틀"
#define MSGTR_MENU_Title "타�?�틀 %2d"
#define MSGTR_MENU_None "(없�?�)"
#define MSGTR_MENU_Chapters "챕터"
#define MSGTR_MENU_Chapter "챕터 %2d"
#define MSGTR_MENU_AudioLanguages "오디오 언어"
#define MSGTR_MENU_SubtitleLanguages "�?막 언어"
#define MSGTR_MENU_SkinBrowser "스킨선�?"
#define MSGTR_MENU_Exit "종료"
#define MSGTR_MENU_Mute "�?�소거"
#define MSGTR_MENU_Original "�?래대로"
#define MSGTR_MENU_AspectRatio "화면비율"
#define MSGTR_MENU_AudioTrack "오디오 트랙"
#define MSGTR_MENU_Track "트랙 %d"
#define MSGTR_MENU_VideoTrack "비디오 트랙"

// --- equalizer
#define MSGTR_EQU_Audio "오디오"
#define MSGTR_EQU_Video "비디오"
#define MSGTR_EQU_Contrast "대비: "
#define MSGTR_EQU_Brightness "�?기: "
#define MSGTR_EQU_Hue "색�?: "
#define MSGTR_EQU_Saturation "채�?�: "
#define MSGTR_EQU_Front_Left "왼쪽 앞"
#define MSGTR_EQU_Front_Right "오른쪽 앞"
#define MSGTR_EQU_Back_Left "왼쪽 뒤"
#define MSGTR_EQU_Back_Right "오른쪽 뒤"
#define MSGTR_EQU_Center "가운�?�"
#define MSGTR_EQU_Bass "베�?�스"
#define MSGTR_EQU_All "모�?"
#define MSGTR_EQU_Channel1 "채�? 1:"
#define MSGTR_EQU_Channel2 "채�? 2:"
#define MSGTR_EQU_Channel3 "채�? 3:"
#define MSGTR_EQU_Channel4 "채�? 4:"
#define MSGTR_EQU_Channel5 "채�? 5:"
#define MSGTR_EQU_Channel6 "채�? 6:"

// --- playlist
#define MSGTR_PLAYLIST_Path "경로"
#define MSGTR_PLAYLIST_Selected "선�?�?� 파�?�"
#define MSGTR_PLAYLIST_Files "파�?�"
#define MSGTR_PLAYLIST_DirectoryTree "디렉토리"

// --- preferences
#define MSGTR_PREFERENCES_SubtitleOSD "�?막 & OSD"
#define MSGTR_PREFERENCES_Codecs "코�?� & 해�?기"
#define MSGTR_PREFERENCES_Misc "기타"

#define MSGTR_PREFERENCES_None "없�?�"
#define MSGTR_PREFERENCES_AvailableDrivers "가능한 드�?��?�버:"
#define MSGTR_PREFERENCES_DoNotPlaySound "사운드 재�? 안함"
#define MSGTR_PREFERENCES_NormalizeSound "사운드 표준화"
#define MSGTR_PREFERENCES_EnableEqualizer "�?�퀄�?��?�저 사용"
#define MSGTR_PREFERENCES_ExtraStereo "외부 스테레오 사용"
#define MSGTR_PREFERENCES_Coefficient "계수:"
#define MSGTR_PREFERENCES_AudioDelay "오디오 지연:"
#define MSGTR_PREFERENCES_DoubleBuffer "�?�중 버�?��? 사용"
#define MSGTR_PREFERENCES_DirectRender "다�?�렉트 렌�?��? 사용"
#define MSGTR_PREFERENCES_FrameDrop "프레임 건너뛰기 사용"
#define MSGTR_PREFERENCES_HFrameDrop "강제 프레임 건너뛰기 사용(위험함)"
#define MSGTR_PREFERENCES_Flip "�?�미지 �?하 반전"
#define MSGTR_PREFERENCES_Panscan "팬스캔: "
#define MSGTR_PREFERENCES_Subtitle "�?막:"
#define MSGTR_PREFERENCES_SUB_Delay "지연: "
#define MSGTR_PREFERENCES_SUB_FPS "FPS:"
#define MSGTR_PREFERENCES_SUB_POS "위치: "
#define MSGTR_PREFERENCES_SUB_AutoLoad "�?�?�으로 �?막 열지 않기"
#define MSGTR_PREFERENCES_SUB_Unicode "유니코드 �?막"
#define MSGTR_PREFERENCES_SUB_MPSUB "주어진 �?막�?� MPlayer용 �?막 형�?으로 바꿈"
#define MSGTR_PREFERENCES_SUB_SRT "주어진 �?막�?� SRT 형�?으로 바꿈"
#define MSGTR_PREFERENCES_SUB_Overlap "�?막 겹침 켜기"
#define MSGTR_PREFERENCES_Font "글꼴:"
#define MSGTR_PREFERENCES_FontFactor "글꼴 팩터:"
#define MSGTR_PREFERENCES_PostProcess "후행처리 사용"
#define MSGTR_PREFERENCES_AutoQuality "�?�?� 품질조정: "
#define MSGTR_PREFERENCES_NI "non-interleaved AVI 파서 사용"
#define MSGTR_PREFERENCES_IDX "필요한 경우, �?��?�스 테�?�블 다시 만들기"
#define MSGTR_PREFERENCES_VideoCodecFamily "비디오 코�?� 집합:"
#define MSGTR_PREFERENCES_AudioCodecFamily "오디오 코�?� 집합:"
#define MSGTR_PREFERENCES_FRAME_OSD_Level "OSD 레벨"
#define MSGTR_PREFERENCES_FRAME_Subtitle "�?막"
#define MSGTR_PREFERENCES_FRAME_Font "글꼴"
#define MSGTR_PREFERENCES_FRAME_PostProcess "후행처리"
#define MSGTR_PREFERENCES_FRAME_CodecDemuxer "코�?� & 해�?기"
#define MSGTR_PREFERENCES_FRAME_Cache "�?시"
#define MSGTR_PREFERENCES_Message "선�?사항들�?� �?용하려면 재�?기를 다시 시작해야 합니다!"
#define MSGTR_PREFERENCES_DXR3_VENC "비디오 �?�코�?�:"
#define MSGTR_PREFERENCES_DXR3_LAVC "LAVC 사용 (FFmpeg)"
#define MSGTR_PREFERENCES_FontEncoding1 "유니코드"
#define MSGTR_PREFERENCES_FontEncoding2 "서유럽어 (ISO-8859-1)"
#define MSGTR_PREFERENCES_FontEncoding3 "Euro �?�함 서유럽어 (ISO-8859-15)"
#define MSGTR_PREFERENCES_FontEncoding4 "슬�?�브/중앙 유럽어 (ISO-8859-2)"
#define MSGTR_PREFERENCES_FontEncoding5 "�?스페란토, 갈리시아, 몰타, 터키어 (ISO-8859-3)"
#define MSGTR_PREFERENCES_FontEncoding6 "고대 발트 문�?셋 (ISO-8859-4)"
#define MSGTR_PREFERENCES_FontEncoding7 "키릴 �?모 (ISO-8859-5)"
#define MSGTR_PREFERENCES_FontEncoding8 "아�?어 (ISO-8859-6)"
#define MSGTR_PREFERENCES_FontEncoding9 "현대 그리스어 (ISO-8859-7)"
#define MSGTR_PREFERENCES_FontEncoding10 "터키어 (ISO-8859-9)"
#define MSGTR_PREFERENCES_FontEncoding11 "발트어 (ISO-8859-13)"
#define MSGTR_PREFERENCES_FontEncoding12 "켈트어 (ISO-8859-14)"
#define MSGTR_PREFERENCES_FontEncoding13 "히브리어 (ISO-8859-8)"
#define MSGTR_PREFERENCES_FontEncoding14 "러시아어 (KOI8-R)"
#define MSGTR_PREFERENCES_FontEncoding15 "우�?��?��?�나, 벨로루시어 (KOI8-U/RU)"
#define MSGTR_PREFERENCES_FontEncoding16 "중국어 간체 (CP936)"
#define MSGTR_PREFERENCES_FontEncoding17 "중국어 번체 (BIG5)"
#define MSGTR_PREFERENCES_FontEncoding18 "�?�본어 (SHIFT-JIS)"
#define MSGTR_PREFERENCES_FontEncoding19 "한국어 (CP949)"
#define MSGTR_PREFERENCES_FontEncoding20 "태국어 (CP874)"
#define MSGTR_PREFERENCES_FontEncoding21 "발트어 Windows (CP1251)"
#define MSGTR_PREFERENCES_FontEncoding22 "슬�?�브/중앙 유럽어 Windows (CP1250)"
#define MSGTR_PREFERENCES_FontNoAutoScale "�?�?� �?�기조절 �?�"
#define MSGTR_PREFERENCES_FontPropWidth "스�?�린�?� 너비�? 비례"
#define MSGTR_PREFERENCES_FontPropHeight "스�?�린�?� 높�?��? 비례"
#define MSGTR_PREFERENCES_FontPropDiagonal "스�?�린�?� 대�?선�? 비례"
#define MSGTR_PREFERENCES_FontEncoding "�?�코딩:"
#define MSGTR_PREFERENCES_FontBlur "�??림:"
#define MSGTR_PREFERENCES_FontOutLine "외곽선:"
#define MSGTR_PREFERENCES_FontTextScale "�?스트 �?�기조절:"
#define MSGTR_PREFERENCES_FontOSDScale "OSD �?�기조절:"
#define MSGTR_PREFERENCES_Cache "�?쉬 켜기/�?�기"
#define MSGTR_PREFERENCES_CacheSize "�?시 �?�기: "
#define MSGTR_PREFERENCES_LoadFullscreen "전체화면으로 시작"
#define MSGTR_PREFERENCES_SaveWinPos "창�?� 위치 저장"
#define MSGTR_PREFERENCES_XSCREENSAVER "X스�?�린세�?�버 정지"
#define MSGTR_PREFERENCES_PlayBar "재�?표시줄 사용"
#define MSGTR_PREFERENCES_AutoSync "�?�?� �?�기화 켜기/�?�기"
#define MSGTR_PREFERENCES_AutoSyncValue "�?�?� �?�기화: "
#define MSGTR_PREFERENCES_CDROMDevice "CD-ROM 장치:"
#define MSGTR_PREFERENCES_DVDDevice "DVD 장치:"
#define MSGTR_PREFERENCES_FPS "�?��?�? FPS:"
#define MSGTR_PREFERENCES_ShowVideoWindow "정지 중�?� 때 비디오 창 보�?�기"

#define MSGTR_ABOUT_UHU "GUI 개발 지�?: UHU Linux\n"

// --- messagebox
#define MSGTR_MSGBOX_LABEL_FatalError "치명�? 오류!"
#define MSGTR_MSGBOX_LABEL_Error "오류!"
#define MSGTR_MSGBOX_LABEL_Warning "경고!"

#endif
