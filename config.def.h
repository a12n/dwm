/* See LICENSE file for copyright and license details. */

#include <X11/XKBlib.h>

/* custom functions declarations */
void bstack(Monitor *m);
void nextlayout(const Arg *arg);
void setxkbgroup(const Arg *arg);

/* appearance */
static const char *fonts[] = {
	"Terminus:size=14"
};
static const char dmenufont[]       = "Terminus:size=14";
#define SOLARIZED_BASE03    "#002b36"
#define SOLARIZED_BASE02    "#073642"
#define SOLARIZED_BASE01    "#586e75"
#define SOLARIZED_BASE00    "#657b83"
#define SOLARIZED_BASE0     "#839496"
#define SOLARIZED_BASE1     "#93a1a1"
#define SOLARIZED_BASE2     "#eee8d5"
#define SOLARIZED_BASE3     "#fdf6e3"
#define SOLARIZED_YELLOW    "#b58900"
#define SOLARIZED_ORANGE    "#cb4b16"
#define SOLARIZED_RED       "#dc322f"
#define SOLARIZED_MAGENTA   "#d33682"
#define SOLARIZED_VIOLET    "#6c71c4"
#define SOLARIZED_BLUE      "#268bd2"
#define SOLARIZED_CYAN      "#2aa198"
#define SOLARIZED_GREEN     "#859900"
#ifdef SOLARIZED_LIGHT
static const char normbordercolor[] = SOLARIZED_BASE2;
static const char normbgcolor[]     = SOLARIZED_BASE3;
static const char normfgcolor[]     = SOLARIZED_BASE00;
static const char selbordercolor[]  = SOLARIZED_BLUE;
static const char selbgcolor[]      = SOLARIZED_BLUE;
static const char selfgcolor[]      = SOLARIZED_BASE02;
#else /* SOLARIZED_LIGHT */
static const char normbordercolor[] = SOLARIZED_BASE02;
static const char normbgcolor[]     = SOLARIZED_BASE03;
static const char normfgcolor[]     = SOLARIZED_BASE0;
static const char selbordercolor[]  = SOLARIZED_BLUE;
static const char selbgcolor[]      = SOLARIZED_BLUE;
static const char selfgcolor[]      = SOLARIZED_BASE2;
#endif /* SOLARIZED_LIGHT */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "TTT",      bstack },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

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
static const char *termcmd[]  = { "urxvt", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
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
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
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

/* custom functions */

void
bstack(Monitor *m) {
	int w, h, mh, mx, tx, ty, tw;
	unsigned int i, n;
	Client *c;

	for(n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if(n == 0) {
		return;
	}
	if(n > m->nmaster) {
		mh = m->nmaster ? m->mfact * m->wh : 0;
		tw = m->ww / (n - m->nmaster);
		ty = m->wy + mh;
	} else {
		mh = m->wh;
		tw = m->ww;
		ty = m->wy;
	}
	for(i = mx = 0, tx = m->wx, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++) {
		if(i < m->nmaster) {
			w = (m->ww - mx) / (MIN(n, m->nmaster) - i);
			resize(c, m->wx + mx, m->wy, w - (2 * c->bw), mh - (2 * c->bw), False);
			mx += WIDTH(c);
		} else {
			h = m->wh - mh;
			resize(c, tx, ty, tw - (2 * c->bw), h - (2 * c->bw), False);
			if(tw != m->ww) {
				tx += WIDTH(c);
			}
		}
	}
}

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
