/* See LICENSE file for copyright and license details. */

/* appearance */
#define BARPOS			BarBot // BarTop /* BarBot, BarOff */
#define BORDERPX		0
#define FLOATBORDERPX		2
#define FONT			"-misc-fixed-medium-r-normal-*-13-*-*-*-*-*-*-*"
#define NORMBORDERCOLOR		"#000"
#define NORMBGCOLOR		"#222"
#define NORMFGCOLOR		"#ccc"
#define SELBORDERCOLOR		"#000"
#define SELBGCOLOR		"#555"
#define SELFGCOLOR		"#fff"

/* workspaces */
/* Query class:instance:title for regex matching info with following command:
 * xprop | awk -F '"' '/^WM_CLASS/ { printf("%s:%s:",$4,$2) }; /^WM_NAME/ { printf("%s\n",$2) }' */
Rule rules[] = {
	/* class:instance:title regex		isfloat	workspace (0=current) */ \
	{ "MPlayer:",				True,	0 }, \
	{ "[Vv]ncviewer:",			True,	0 }, \
	{ "VNC Viewer:",			True,	0 }, \
	{ "VNC::",				True,	0 }, \
	{ "Gimp",				True,	0 }, \
	{ ":xsane:",				True,	0 }, \
	{ ":Xdialog:",				True,	0 }, \
	{ ":zenity:",				True,	0 }, \
	{ ":VICE:",				True,	0 }, \
	{ "Xnest",				True,	0 }, \
	{ "Wine:Diablo III.exe",		True,	0 }, \
	{ "de-cgarbs-knittr-Knittr:",		True,	0 }, \
};
#define INITIALWORKSPACES	 2
#define MAXWORKSPACES		99
#define MAXWSTEXTWIDTH		 6	/* must be 2*(strlen(MAXWORKSPACES)+1)  */
#define MAXXINERAMASCREENS       2


/* rotate layouts on swapscreen() - undefine SWAPSCREEN_LAYOUT to deactivate */
#define SWAPSCREEN_LAYOUT

/* volume management via status bar - undefine VOLUME to deactivate */
#define VOLUME
/* volume management configuration: */
#ifdef VOLUME
const char *volup   = "amixer set Master 5%+";
const char *voldown = "amixer set Master 5%-";
int vw = 16; /* pixels from the right to activate volume management on bar */
#endif

/* ugly: depending on constants above but needed by layouts below */
double mwfact[MAXXINERAMASCREENS][MAXWORKSPACES];
Bool domwfact[MAXXINERAMASCREENS] = {True};
Bool dozoom[MAXXINERAMASCREENS] = {True};

/* layout(s) */
#define MWFACT			0.65	/* default master width factor [0.1 .. 0.9] */
#define RESIZEHINTS		False	/* False - respect size hints in tiled resizals */
#define SNAP			32	/* snap pixel */
#include "bstack.c"
#include "maximize.c"
#include "widescreen.c"
#include "wow.c"
Layout layouts[] = {
	/* symbol		function */
	{ "TTT",		bstack },
	{ "[ ]",		maximize },
	{ "<><",		floating },
	{ " |-",		tile },
	{ "-| ",		tileleft },
	{ "= =",		widescreen },
	{ "WoW",		wow },
};

/* key definitions */
#define MODKEY			Mod1Mask
Key keys[] = {
	/* modifier			key		function	argument */
	{ MODKEY|ShiftMask,		XK_a,		spawn, "exec x-terminal-emulator" },
	{ MODKEY|ShiftMask,		XK_s,		spawn, "exec dwm-choose" },
	{ MODKEY|ControlMask,		XK_s,		spawn, "exec ssh -X yggdrasil.mitch.h.shuttle.de dwm-choose" },
	{ MODKEY|ShiftMask|ControlMask,	XK_r,		spawn, "exec killall -SIGUSR1 dwm" },
	{ MODKEY,			XK_Tab,		focusnext,	NULL },
	{ MODKEY|ShiftMask,		XK_Tab,		focusprev,	NULL },
	{ MODKEY,			XK_Return,	zoom,		NULL },
	{ MODKEY,			XK_Return,	togglemax,	NULL },
	{ MODKEY|ControlMask,		XK_c,		togglebar,	NULL },
	{ MODKEY|ControlMask,		XK_e,		setmwfact,	"-0.032" },
	{ MODKEY|ControlMask,		XK_d,		setmwfact,	"+0.032" },
	{ MODKEY|ShiftMask,		XK_e,		popstack,	NULL },
	{ MODKEY|ShiftMask,		XK_d,		pushstack,	NULL },
	{ MODKEY|ShiftMask,		XK_y,		wscount,	"1" },
	{ MODKEY|ShiftMask,		XK_x,		wscount,	"-1" },
	{ MODKEY|ShiftMask,		XK_c,		killclient,	NULL },
	{ MODKEY|ShiftMask,		XK_f,		togglefloating,	NULL },
	{ MODKEY|ShiftMask,		XK_w,		viewrel,	"1" },
	{ MODKEY|ShiftMask,		XK_q,		viewrel,	"-1" },
	{ MODKEY|ShiftMask|ControlMask,	XK_q,		quit,		NULL },
	{ MODKEY|ShiftMask,		XK_1,		setlayout,	"TTT" },
	{ MODKEY|ShiftMask,		XK_2,		setlayout,	"[ ]" },
	{ MODKEY|ShiftMask,		XK_3,		setlayout,	"<><" },
	{ MODKEY|ShiftMask,		XK_4,		setlayout,	" |-" },
	{ MODKEY|ShiftMask,		XK_5,		setlayout,	"-| " },
	{ MODKEY|ShiftMask,		XK_6,		setlayout,	"= =" },
	{ MODKEY|ShiftMask,		XK_7,		setlayout,	"WoW" },
	{ MODKEY|ControlMask,		XK_2,		warpmouserel,	"1" },
	{ MODKEY|ControlMask,		XK_1,		warpmouserel,	"-1" },
	{ MODKEY|ControlMask,		XK_x,		swapscreen,	"1" },

	/* "macros" using multiple commands on the same keybinding */
	{ MODKEY|ControlMask,		XK_w,		pushstack,	NULL },
	{ MODKEY|ControlMask,		XK_w,		viewrel,	"1" },
	{ MODKEY|ControlMask,		XK_w,		popstack,	NULL },

	{ MODKEY|ControlMask,		XK_q,		pushstack,	NULL },
	{ MODKEY|ControlMask,		XK_q,		viewrel,	"-1" },
	{ MODKEY|ControlMask,		XK_q,		popstack,	NULL },

	{ MODKEY|ShiftMask|ControlMask,	XK_2,		pushstack,	NULL },
	{ MODKEY|ShiftMask|ControlMask,	XK_2,		warpmouserel,	"1" },
	{ MODKEY|ShiftMask|ControlMask,	XK_2,		popstack,	NULL },

	{ MODKEY|ShiftMask|ControlMask,	XK_1,		pushstack,	NULL },
	{ MODKEY|ShiftMask|ControlMask,	XK_1,		warpmouserel,	"-1" },
	{ MODKEY|ShiftMask|ControlMask,	XK_1,		popstack,	NULL },


	/* Thinkpad special keys XF86Forward/XF86Back */
	{ 0,				0x1008ff27,	viewrel,	"1" },
	{ 0,				0x1008ff26,	viewrel,	"-1" },

	{ ControlMask,			0x1008ff27,	pushstack,	NULL },
	{ ControlMask,			0x1008ff27,	viewrel,	"1" },
	{ ControlMask,			0x1008ff27,	popstack,	NULL },

	{ ControlMask,			0x1008ff26,	pushstack,	NULL },
	{ ControlMask,			0x1008ff26,	viewrel,	"-1" },
	{ ControlMask,			0x1008ff26,	popstack,	NULL },

	/* ordinary volume control */
	{ MODKEY,			XK_Down,	spawn,	"amixer set Master 5%-" },
	{ MODKEY,			XK_Up,		spawn,	"amixer set Master 5%+" },
	
	/* special keys on Fujitsu Green IT keyboard
	   XF86HomePage            0x1008ff18
	   XF86Mail                0x1008ff19
	   XF86AudioLowerVolume    0x1008ff11
	   XF86AudioMute           0x1008ff12
	   XF86AudioRaiseVolume    0x1008ff13
	   XF86Sleep               0x1008ff2f */
	{ 0,				0x1008ff11,	spawn,	"amixer set Master 5%-" },
	{ 0,				0x1008ff12,	spawn,	"pa-togglemute" },
	{ 0,				0x1008ff13,	spawn,	"amixer set Master 5%+" },
};
