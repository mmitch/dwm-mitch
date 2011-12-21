/* See LICENSE file for copyright and license details. */

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
#define INITIALWORKSPACES	 1
#define MAXWORKSPACES		99
#define MAXWSTEXTWIDTH		 6	/* must be 2*(strlen(MAXWORKSPACES)+1)  */
#define MAXXINERAMASCREENS	 2

/* ugly: depending on constants above but needed by layouts below */
double mwfact[MAXXINERAMASCREENS][MAXWORKSPACES];
Bool domwfact[MAXXINERAMASCREENS] = {True};
Bool dozoom[MAXXINERAMASCREENS] = {True};

/* layout(s) */
#define MWFACT			0.6	/* master width factor [0.1 .. 0.9] */
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
	{ MODKEY,			XK_p,		spawn,
		"exe=`dmenu_path | dmenu -fn '"FONT"' -nb '"NORMBGCOLOR"' -nf '"NORMFGCOLOR"'"
		" -sb '"SELBGCOLOR"' -sf '"SELFGCOLOR"'` && exec $exe" },
	{ MODKEY|ShiftMask,		XK_Return,	spawn, "exec uxterm" },
	{ MODKEY,			XK_space,	setlayout,	NULL },
	{ MODKEY,			XK_b,		togglebar,	NULL },
	{ MODKEY,			XK_j,		focusnext,	NULL },
	{ MODKEY,			XK_k,		focusprev,	NULL },
	{ MODKEY,			XK_g,		setmwfact,	"-0.05" },
	{ MODKEY,			XK_s,		setmwfact,	"+0.05" },
/*	{ MODKEY|ShiftMask,		XK_g,		incnmaster,	"1" }, */
/*	{ MODKEY|ShiftMask,		XK_s,		incnmaster,	"-1" }, */
	{ MODKEY,			XK_a,		popstack,	NULL },
	{ MODKEY,			XK_d,		pushstack,	NULL },
	{ MODKEY|ControlMask,		XK_y,		wscount,	"1" },
	{ MODKEY|ControlMask,		XK_r,		wscount,	"-1" },
	{ MODKEY,			XK_m,		togglemax,	NULL },
	{ MODKEY,			XK_Return,	zoom,		NULL },
	{ MODKEY,			XK_Tab,		focusnext,	NULL },
	{ MODKEY|ShiftMask,		XK_Tab,		focusprev,	NULL },
	{ MODKEY|ShiftMask,		XK_space,	togglefloating,	NULL },
	{ MODKEY|ShiftMask,		XK_c,		killclient,	NULL },
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
