/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include <X11/XKBlib.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char dmenufont[]       = "JetBrains Mono:size=15";
static const char *fonts[]          = { dmenufont };
#define GRUVBOX_DARK0          "#282828"
#define GRUVBOX_DARK1          "#3c3836"
#define GRUVBOX_DARK2          "#504945"
#define GRUVBOX_DARK3          "#665C54"
#define GRUVBOX_DARK4          "#7C6F64"
#define GRUVBOX_GRAY           "#928374"
#define GRUVBOX_LIGHT0         "#FBF1C7"
#define GRUVBOX_LIGHT1         "#EBDBB2"
#define GRUVBOX_LIGHT2         "#D5C4A1"
#define GRUVBOX_LIGHT3         "#BDAE93"
#define GRUVBOX_LIGHT4         "#A89984"
#define GRUVBOX_BRIGHT_RED     "#FB4934"
#define GRUVBOX_BRIGHT_GREEN   "#B8BB26"
#define GRUVBOX_BRIGHT_YELLOW  "#FABD2F"
#define GRUVBOX_BRIGHT_BLUE    "#83A598"
#define GRUVBOX_BRIGHT_PURPLE  "#D3869B"
#define GRUVBOX_BRIGHT_AQUA    "#8EC07C"
#define GRUVBOX_BRIGHT_ORANGE  "#FE8019"
#define GRUVBOX_NEUTRAL_RED    "#CC241D"
#define GRUVBOX_NEUTRAL_GREEN  "#98971A"
#define GRUVBOX_NEUTRAL_YELLOW "#D79921"
#define GRUVBOX_NEUTRAL_BLUE   "#458588"
#define GRUVBOX_NEUTRAL_PURPLE "#B16286"
#define GRUVBOX_NEUTRAL_AQUA   "#689D6A"
#define GRUVBOX_NEUTRAL_ORANGE "#D65D0E"
#define GRUVBOX_FADED_RED      "#9D0006"
#define GRUVBOX_FADED_GREEN    "#79740E"
#define GRUVBOX_FADED_YELLOW   "#B57614"
#define GRUVBOX_FADED_BLUE     "#076678"
#define GRUVBOX_FADED_PURPLE   "#8F3F71"
#define GRUVBOX_FADED_AQUA     "#427B58"
#define GRUVBOX_FADED_ORANGE   "#AF3A03"
#if defined _LIGHT
static const char normbordercolor[] = GRUVBOX_LIGHT1;
static const char normbgcolor[]     = GRUVBOX_LIGHT0;
static const char normfgcolor[]     = GRUVBOX_DARK1;
static const char selbordercolor[]  = GRUVBOX_NEUTRAL_BLUE;
static const char selbgcolor[]      = GRUVBOX_NEUTRAL_BLUE;
static const char selfgcolor[]      = GRUVBOX_DARK1;
#else  /* _LIGHT */
static const char normbordercolor[] = GRUVBOX_DARK1;
static const char normbgcolor[]     = GRUVBOX_DARK0;
static const char normfgcolor[]     = GRUVBOX_LIGHT1;
static const char selbordercolor[]  = GRUVBOX_NEUTRAL_BLUE;
static const char selbgcolor[]      = GRUVBOX_NEUTRAL_BLUE;
static const char selfgcolor[]      = GRUVBOX_LIGHT1;
#endif /* _LIGHT */
static const char *colors[][3]      = {
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

static const char *barcolors[] = {
#if defined _LIGHT
	GRUVBOX_LIGHT0,
	GRUVBOX_NEUTRAL_RED,
	GRUVBOX_NEUTRAL_GREEN,
	GRUVBOX_NEUTRAL_YELLOW,
	GRUVBOX_NEUTRAL_BLUE,
	GRUVBOX_NEUTRAL_PURPLE,
	GRUVBOX_NEUTRAL_AQUA,
	GRUVBOX_DARK4,
	GRUVBOX_GRAY,
	GRUVBOX_FADED_RED,
	GRUVBOX_FADED_GREEN,
	GRUVBOX_FADED_YELLOW,
	GRUVBOX_FADED_BLUE,
	GRUVBOX_FADED_PURPLE,
	GRUVBOX_FADED_AQUA,
	GRUVBOX_DARK1,
#else  /* _LIGHT */
	GRUVBOX_DARK0,
	GRUVBOX_NEUTRAL_RED,
	GRUVBOX_NEUTRAL_GREEN,
	GRUVBOX_NEUTRAL_YELLOW,
	GRUVBOX_NEUTRAL_BLUE,
	GRUVBOX_NEUTRAL_PURPLE,
	GRUVBOX_NEUTRAL_AQUA,
	GRUVBOX_LIGHT4,
	GRUVBOX_GRAY,
	GRUVBOX_BRIGHT_RED,
	GRUVBOX_BRIGHT_GREEN,
	GRUVBOX_BRIGHT_YELLOW,
	GRUVBOX_BRIGHT_BLUE,
	GRUVBOX_BRIGHT_PURPLE,
	GRUVBOX_BRIGHT_AQUA,
	GRUVBOX_LIGHT1,
#endif /* _LIGHT */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 2,       0,           -1 },
	{ "Chromium", NULL,       NULL,       1 << 2,       0,           -1 },
	{ "Emacs",    NULL,       NULL,       1 << 1,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* custom functions declarations */
void nextlayout(const Arg *arg);
void setxkbgroup(const Arg *arg);

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, "-p", ">", NULL };
static const char *passmenucmd[] = { "passmenu", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, "-p", "*", NULL };
static const char *termcmd[]  = { "urxvtc", NULL };
static const char *xkillcmd[] = { "xkill", NULL };
/* fn commands */
static const char *audionextcmd[] = { "mpc", "-q", "next", NULL };
static const char *audioplaycmd[] = { "mpc", "-q", "toggle", NULL };
static const char *audioprevcmd[] = { "mpc", "-q", "prev", NULL };
static const char *audiostopcmd[] = { "mpc", "-q", "stop", NULL };
static const char *screensavercmd[] = { "xset", "s", "activate", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = passmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_space,  nextlayout,     {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_u,      setxkbgroup,    {.ui = 0 } },
	{ MODKEY,                       XK_r,      setxkbgroup,    {.ui = 1 } },
	{ MODKEY,                       XK_e,      setxkbgroup,    {.ui = 2 } },
	{ MODKEY,                       XK_g,      setxkbgroup,    {.ui = 3 } },
	{ MODKEY,                       XK_x,      spawn,          {.v = xkillcmd } },
	{ MODKEY|ShiftMask,             XK_Delete, spawn,          {.v = screensavercmd } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },

    { 0,    XF86XK_AudioNext,           spawn,  {.v = audionextcmd } },
    { 0,    XF86XK_AudioPlay,           spawn,  {.v = audioplaycmd } },
    { 0,    XF86XK_AudioPrev,           spawn,  {.v = audioprevcmd } },
    { 0,    XF86XK_AudioStop,           spawn,  {.v = audiostopcmd } },
    { 0,    XF86XK_ScreenSaver,         spawn,  {.v = screensavercmd } },

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

void
nextlayout(const Arg *arg) {
	unsigned int i = 0;
	unsigned int n = sizeof layouts / sizeof layouts[0];
	while (i < n && (layouts + i) != selmon->lt[selmon->sellt]) {
		++i;
	}
	setlayout(&((Arg){ .v = layouts + (i + 1) % n }));
}

void
setxkbgroup(const Arg *arg) {
	static int hasxkb = -1;
	if (hasxkb < 0) {
		int major = XkbMajorVersion;
		int minor = XkbMinorVersion;
		hasxkb = XkbQueryExtension(dpy, NULL, NULL, NULL, &major, &minor);
	}
	if (hasxkb > 0) {
		if (arg != NULL && arg->ui < 4) {
			XkbLockGroup(dpy, XkbUseCoreKbd, arg->ui);
		}
	}
}
