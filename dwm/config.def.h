/* See LICENSE file for copyright and license details. */

/* status bar flags for normal/float/sticky/sticky+float */
char clientstat[] = {' ', '<', '>', 'x'};

/* status bar flag for locked mode */
const char* const lockedstat = "L";

/* appearance */
#define BARPOS			BarTop /* BarBot, BarOff */
#define BORDERPX		1
#define FLOATBORDERPX		1
#define FONT			"-*-terminus-medium-*-*-*-*-*-*-*-*-*-*-*"
#define NORMBORDERCOLOR		"#cccccc"
#define NORMBGCOLOR		"#cccccc"
#define NORMFGCOLOR		"#000000"
#define SELBORDERCOLOR		"#0066ff"
#define SELBGCOLOR		"#0066ff"
#define SELFGCOLOR		"#ffffff"
#define ERRBORDERCOLOR		"#cc6666" /* error colors are relevant for status bar timeouts */
#define ERRBGCOLOR		"#ff6666"
#define ERRFGCOLOR		"#ffffff"
#define EDGECOLOR		"#000000" /* rounded corner pixels in screen edges */

/* workspaces */
/* Query class:instance:title for regex matching info with following command:
 * xprop | awk -F '"' '/^WM_CLASS/ { printf("%s:%s:",$4,$2) }; /^WM_NAME/ { printf("%s\n",$2) }' */
Rule rules[] = {
	/* class:instance:title regex	isfloating	workspace (0=current)*/
	{ "Firefox",			False,		0 },
	{ "Gimp",			True,		0 },
	{ "MPlayer",			True,		0 },
	{ "Acroread",			True,		0 },
};

#define INITIALWORKSPACES	   1
#define MAXWORKSPACES		  99
#define MAXWSTEXTWIDTH		   6	/* must be 2*(strlen(MAXWORKSPACES)+1)  */
#define MAXXINERAMASCREENS	   4
#define HORIZONTALAUTOSPLIT	1700	/* split screen into multiple workspaces when wider than this */
#define REFRESH_HZ		  60	/* refresh frequency for movemouse() and resizemouse() */

/* show stack size in status bar - undefine SHOWSTACKSIZE to disable */
#define SHOWSTACKSIZE

/* rotate layouts on swapscreen() - undefine SWAPSCREEN_LAYOUT to disable */
#define SWAPSCREEN_LAYOUT

/* snap clients to local screen borders on mousemove() - undefine to snap to global borders */
/* only relevant with multiple screens */
#define SNAPLOCALBORDERS

/* status bar timeout in seconds
 * if no update is received for this duration, the status bar is colored in the ERR* colors defined above
 * set to 0 to deactivate */
#define STATUSBARTIMEOUT	60

/* ugly: depending on constants above but needed by layouts below */
double mwfact[MAXXINERAMASCREENS][MAXWORKSPACES];
Bool domwfact[MAXXINERAMASCREENS] = {True};
Bool dozoom[MAXXINERAMASCREENS] = {True};

/* layout(s) */
#define MWFACT			0.6	/* default master width factor [0.1 .. 0.9] */
#define RESIZEHINTS		True	/* False - respect size hints in tiled resizals */
#define SNAP			32	/* snap pixel */
#include "bstack.c"
#include "maximize.c"
#include "widescreen.c"
Layout layouts[] = {
	/* symbol		function */
	{ "[]=",		tile }, /* first entry is default */
	{ "=[]",		tileleft },
	{ "><>",		floating },
	{ "TTT",		bstack },
	{ "[ ]",		maximize },
	{ "= =",		widescreen },
};

/* key definitions */
#define MODKEY			Mod1Mask
Key keys[] = {
	/* modifier			key		function	argument */
	{ MODKEY,			XK_p,		spawn,		"exec dwm-choose" },
	{ MODKEY|ShiftMask,		XK_Return,	spawn, "exec uxterm" },
	{ MODKEY,			XK_space,	setlayout,	NULL },
	{ MODKEY,			XK_b,		togglebar,	NULL },
	{ MODKEY,			XK_j,		focusnext,	NULL },
	{ MODKEY,			XK_k,		focusprev,	NULL },
	{ MODKEY,			XK_g,		setmwfact,	"-0.05" },
	{ MODKEY,			XK_s,		setmwfact,	"+0.05" },
	{ MODKEY,			XK_a,		popstack,	NULL },
	{ MODKEY,			XK_d,		pushstack,	NULL },
	{ MODKEY|ControlMask,		XK_y,		wscount,	"1" },
	{ MODKEY|ControlMask,		XK_r,		wscount,	"-1" },
	{ MODKEY,			XK_m,		togglemax,	NULL },
	{ MODKEY,			XK_Return,	zoom,		NULL },
	{ MODKEY,			XK_Tab,		focusnext,	NULL },
	{ MODKEY|ShiftMask,		XK_Tab,		focusprev,	NULL },
	{ MODKEY|ShiftMask,		XK_space,	togglefloating,	NULL },
	{ MODKEY|ControlMask,		XK_s,		togglesticky,	NULL },
	{ MODKEY|ShiftMask,		XK_c,		killclient,	NULL },
	{ 0,				XK_Scroll_Lock,	togglelocked,	NULL },

	{ MODKEY|ShiftMask,		XK_s,		swapscreen,	"1" },
	/* uncomment next line if the mouse pointer should be swapped too */
	/* { MODKEY|ShiftMask,		XK_s,		warpmouserel,	"1" }, */

	{ MODKEY,			XK_1,		view,		"1" },
	{ MODKEY,			XK_2,		view,		"2" },
	{ MODKEY,			XK_3,		view,		"3" },
	{ MODKEY,			XK_4,		view,		"4" },
	{ MODKEY,			XK_5,		view,		"5" },
	{ MODKEY,			XK_6,		view,		"6" },
	{ MODKEY,			XK_7,		view,		"7" },
	{ MODKEY,			XK_8,		view,		"8" },
	{ MODKEY,			XK_9,		view,		"9" },
	{ MODKEY,			XK_0,		view,		"10" },
	{ MODKEY|ShiftMask,		XK_1,		moveto,		"1" },
	{ MODKEY|ShiftMask,		XK_2,		moveto,		"2" },
	{ MODKEY|ShiftMask,		XK_3,		moveto,		"3" },
	{ MODKEY|ShiftMask,		XK_4,		moveto,		"4" },
	{ MODKEY|ShiftMask,		XK_5,		moveto,		"5" },
	{ MODKEY|ShiftMask,		XK_6,		moveto,		"6" },
	{ MODKEY|ShiftMask,		XK_7,		moveto,		"7" },
	{ MODKEY|ShiftMask,		XK_8,		moveto,		"8" },
	{ MODKEY|ShiftMask,		XK_9,		moveto,		"9" },
	{ MODKEY|ShiftMask,		XK_0,		moveto,		"10" },
	{ MODKEY,			XK_l,		viewrel,	"1" },
	{ MODKEY,			XK_h,		viewrel,	"-1" },
	{ MODKEY|ShiftMask,		XK_l,		pushstack,	NULL },
	{ MODKEY|ShiftMask,		XK_l,		viewrel,	"1" },
	{ MODKEY|ShiftMask,		XK_l,		popstack,	NULL },
	{ MODKEY|ShiftMask,		XK_h,		pushstack,	NULL },
	{ MODKEY|ShiftMask,		XK_h,		viewrel,	"-1" },
	{ MODKEY|ShiftMask,		XK_h,		popstack,	NULL },
	{ MODKEY|ShiftMask,		XK_q,		quit,		NULL },
	{ MODKEY|ControlMask,		XK_l,		warpmouserel,	"1" },
	{ MODKEY|ControlMask,		XK_h,		warpmouserel,	"-1" },
};

/* key definitions for locked mode */
Key locked_keys[] = {
	/* modifier			key		function	argument */
	{ 0,				XK_Scroll_Lock,	togglelocked,	NULL },
};

/* button definitions */
/* click can be ClkWsNumber, ClkLtSymbol, ClkWinTitle, ClkStatusText, ClkClientWin or ClkRootWin */
Button buttons[] = {
	/* click		modifier	button		function	argument */
	{ ClkWsNumber,		0,		Button1,	viewrel,	"1" },
	{ ClkWsNumber,		0,		Button3,	viewrel,	"-1" },
	{ ClkWsNumber,		MODKEY,		Button1,	wscount,	"1" },
	{ ClkWsNumber,		MODKEY,		Button3,	wscount,	"-1" },
	{ ClkWsNumber,		0,		Button4,	viewrel,	"1" },
	{ ClkWsNumber,		0,		Button5,	viewrel,	"-1" },
	{ ClkLtSymbol,		0,		Button1,	setlayout,	NULL },
	{ ClkLtSymbol,		0,		Button4,	setlayout,	NULL },
	{ ClkLtSymbol,		0,		Button3,	setlayout,	NULL2 },
	{ ClkLtSymbol,		0,		Button5,	setlayout,	NULL2 },
	{ ClkWinTitle,		0,		Button1,	focusnext,	NULL },
	{ ClkWinTitle,		0,		Button4,	focusnext,	NULL },
	{ ClkWinTitle,		0,		Button3,	focusprev,	NULL },
	{ ClkWinTitle,		0,		Button5,	focusprev,	NULL },
	{ ClkWinTitle,		0,		Button2,	zoom,		NULL },
	{ ClkStatusText,	0,		Button1,	spawn,		"amixer set Master 5%+" },
	{ ClkStatusText,	0,		Button4,	spawn,		"amixer set Master 5%+" },
	{ ClkStatusText,	0,		Button3,	spawn,		"amixer set Master 5%-" },
	{ ClkStatusText,	0,		Button5,	spawn,		"amixer set Master 5%-" },
	{ ClkClientWin,		MODKEY,		Button1,	movemouse,	NULL },
	{ ClkClientWin,		MODKEY,		Button2,	zoom,		NULL },
	{ ClkClientWin,		MODKEY,		Button2,	togglemax,	NULL },
	{ ClkClientWin,		MODKEY,		Button3,	resizemouse,	NULL },
	{ ClkRootWin,		0,		Button1,	spawn,		"exec uxterm" },
};
