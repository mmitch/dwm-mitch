/* See LICENSE file for copyright and license details.
 *
 * dynamic window manager is designed like any other X client as well. It is
 * driven through handling X events. In contrast to other X clients, a window
 * manager selects for SubstructureRedirectMask on the root window, to receive
 * events about window (dis-)appearance.  Only one X connection at a time is
 * allowed to select for this event mask.
 *
 * Calls to fetch an X event from the event queue are blocking.  Due reading
 * status text from standard input, a select()-driven main loop has been
 * implemented which selects for reads on the X connection and STDIN_FILENO to
 * handle all data smoothly. The event handlers of dwm are organized in an
 * array which is accessed whenever a new event has been fetched. This allows
 * event dispatching in O(1) time.
 *
 * Each child of the root window is called a client, except windows which have
 * set the override_redirect flag.  Clients are organized in a global
 * doubly-linked client list, the focus history is remembered through a global
 * stack list. Each client contains an array of Bools of the same size as the
 * global tags array to indicate the tags of a client.  
 *
 * Keys and tagging rules are organized as arrays and defined in config.h.
 *
 * To understand everything else, start reading main().
 */
#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <regex.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>

/* macros */
#define BUTTONMASK		(ButtonPressMask | ButtonReleaseMask)
#define CLEANMASK(mask)		(mask & ~(numlockmask | LockMask))
#define LENGTH(x)		(sizeof x / sizeof x[0])
#define MOUSEMASK		(BUTTONMASK | PointerMotionMask)


/* enums */
enum { BarTop, BarBot, BarOff };			/* bar position */
enum { CurNormal, CurResize, CurMove, CurLast };	/* cursor */
enum { ColBorder, ColFG, ColBG, ColLast };		/* color */
enum { NetSupported, NetWMName, NetLast };		/* EWMH atoms */
enum { WMProtocols, WMDelete, WMName, WMState, WMLast };/* default atoms */

/* typedefs */
typedef struct Client Client;
struct Client {
	char name[256];
	int x, y, w, h;
	int rx, ry, rw, rh; /* revert geometry */
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
	int minax, maxax, minay, maxay;
	long flags;
	unsigned int border, oldborder, workspace;
	Bool isbanned, isfixed, ismax, isfloating, wasfloating;
	Client *next;
	Client *prev;
	Client *snext;
	Window win;
};

typedef struct {
	int x, y, w, h;
	unsigned long norm[ColLast];
	unsigned long sel[ColLast];
	Drawable drawable;
	GC gc;
	struct {
		int ascent;
		int descent;
		int height;
		XFontSet set;
		XFontStruct *xfont;
	} font;
} DC; /* draw context */

typedef struct {
	unsigned long mod;
	KeySym keysym;
	void (*func)(const char *arg);
	const char *arg;
} Key;

typedef struct {
	const char *symbol;
	void (*arrange)(void);
} Layout;

typedef struct {
	const char *prop;
	Bool isfloating;
	unsigned int workspace;
} Rule;

typedef struct {
	regex_t *propregex;
} Regs;

/* function declarations */
void *emallocz(unsigned int size);
void applyrules(Client *c);
void arrange(void);
void attach(Client *c);
void attachstack(Client *c);
void ban(Client *c);
void buttonpress(XEvent *e);
void checkotherwm(void);
void cleanup(void);
void compileregs(void);
void configure(Client *c);
void configurenotify(XEvent *e);
void configurerequest(XEvent *e);
void destroynotify(XEvent *e);
void detach(Client *c);
void detachstack(Client *c);
void doreload(void);
void drawbar(void);
void drawtext(const char *text, unsigned long col[ColLast]);
void enternotify(XEvent *e);
void eprint(const char *errstr, ...);
void expose(XEvent *e);
void floating(void); /* default floating layout */
void focus(Client *c);
void focusin(XEvent *e);
void focusnext(const char *arg);
void focusprev(const char *arg);
Client *getclient(Window w);
unsigned long getcolor(const char *colstr);
long getstate(Window w);
Bool gettextprop(Window w, Atom atom, char *text, unsigned int size);
void grabbuttons(Client *c, Bool focused);
void grabkeys(void);
void initfont(const char *fontstr);
Bool isprotodel(Client *c);
Bool isvisible(Client *c);
void keypress(XEvent *e);
void killclient(const char *arg);
void leavenotify(XEvent *e);
void manage(Window w, XWindowAttributes *wa);
void mappingnotify(XEvent *e);
void maprequest(XEvent *e);
Client *nexttiled(Client *c);
void movemouse(Client *c);
void moveto(const char *arg);
void moveto(const char *arg);
void popstack(const char *arg);
void processrules(Client *c);
void propertynotify(XEvent *e);
void pushstack(const char *arg);
void quit(const char *arg);
void resize(Client *c, int x, int y, int w, int h, Bool sizehints);
void resizemouse(Client *c);
void restack(void);
void run(void);
void scan(void);
void setclientstate(Client *c, long state);
void setlayout(const char *arg);
void setmwfact(const char *arg);
void setup(void);
void sigusr1(int notused);
void spawn(const char *arg);
unsigned int textnw(const char *text, unsigned int len);
unsigned int textw(const char *text);
void tile(void);
void togglebar(const char *arg);
void togglefloating(const char *arg);
void togglemax(const char *arg);
void unban(Client *c);
void unmanage(Client *c);
void unmapnotify(XEvent *e);
void updatebarpos(void);
void updatesizehints(Client *c);
void updatetitle(Client *c);
void updatewstext(void);
void view(const char *arg);
void viewrel(const char *arg);
void wscount(const char *arg);
int xerror(Display *dpy, XErrorEvent *ee);
int xerrordummy(Display *dsply, XErrorEvent *ee);
int xerrorstart(Display *dsply, XErrorEvent *ee);
void zoom(const char *arg);

/* variables */
char stext[256];
int screen, sx, sy, sw, sh, wax, way, waw, wah;
int (*xerrorxlib)(Display *, XErrorEvent *);
unsigned int bh, bpos;
unsigned int blw = 0;
int wstextwidth = 0;
unsigned int numlockmask = 0;
unsigned int selws = 1;
void (*handler[LASTEvent]) (XEvent *) = {
	[ButtonPress] = buttonpress,
	[ConfigureRequest] = configurerequest,
	[ConfigureNotify] = configurenotify,
	[DestroyNotify] = destroynotify,
	[EnterNotify] = enternotify,
	[Expose] = expose,
	[FocusIn] = focusin,
	[KeyPress] = keypress,
	[LeaveNotify] = leavenotify,
	[MappingNotify] = mappingnotify,
	[MapRequest] = maprequest,
	[PropertyNotify] = propertynotify,
	[UnmapNotify] = unmapnotify
};
Atom wmatom[WMLast], netatom[NetLast];
Bool domwfact = True;
Bool dozoom = True;
Bool otherwm, readin;
Bool running = True;
Bool reload = False;
Bool selscreen = True;
Client *clients = NULL;
Client *sel = NULL;
Client *stack = NULL;
Cursor cursor[CurLast];
Display *dpy;
DC dc = {0};
Window barwin, root;
Regs *regs = NULL;
char **cargv;

/* predefine variables depending on config.h */
extern double mwfact[];

/* configuration, allows nested code to access above variables */
#include "config.h"

/* variables depending on config.h */
Layout *layout[MAXWORKSPACES];
double mwfact[MAXWORKSPACES];
unsigned int workspaces = INITIALWORKSPACES;
char wstext[MAXWSTEXTWIDTH];

/* function implementations */
void
applyrules(Client *c) {
	static char buf[512];
	unsigned int i;
	regmatch_t tmp;
	XClassHint ch = { 0 };

	/* rule matching */
	XGetClassHint(dpy, c->win, &ch);
	snprintf(buf, sizeof buf, "%s:%s:%s",
			ch.res_class ? ch.res_class : "",
			ch.res_name ? ch.res_name : "", c->name);
	for(i = 0; i < LENGTH(rules); i++)
		if(regs[i].propregex && !regexec(regs[i].propregex, buf, 1, &tmp, 0)) {
			c->isfloating = rules[i].isfloating;
			if((rules[i].workspace > 0) && (rules[i].workspace <= workspaces))
				c->workspace = rules[i].workspace;
		}
	if(ch.res_class)
		XFree(ch.res_class);
	if(ch.res_name)
		XFree(ch.res_name);
}

void
arrange(void) {
	Client *c;

	for(c = clients; c; c = c->next)
		if(isvisible(c))
			unban(c);
		else
			ban(c);
	layout[selws-1]->arrange();
	focus(NULL);
	restack();
}

void
attach(Client *c) {
	if(clients)
		clients->prev = c;
	c->next = clients;
	clients = c;
}

void
attachstack(Client *c) {
	c->snext = stack;
	stack = c;
}

void
ban(Client *c) {
	if(c->isbanned)
		return;
	XMoveWindow(dpy, c->win, c->x + 2 * sw, c->y);
	c->isbanned = True;
}

void
buttonpress(XEvent *e) {
	unsigned int x;
	Client *c;
	XButtonPressedEvent *ev = &e->xbutton;

	if(ev->window == barwin) {
		x = textw(wstext);
		if(ev->x < x) {
			if(ev->button == Button1) {
				if(ev->state & MODKEY)
					viewrel("-1");
				else
					viewrel("1");
			}
			else if(ev->button == Button3) {
				if(ev->state & MODKEY)  
					wscount("-1");  
				else
					wscount("1");
			}
			return;
		}
                if((ev->x < x + blw) && ev->button == Button1) {
			setlayout(NULL);
			return;
		}
	}
	else if((c = getclient(ev->window))) {
		focus(c);
		if(CLEANMASK(ev->state) != MODKEY)
			return;
		if(ev->button == Button1) {
			if((layout[selws-1]->arrange == floating) || c->isfloating)
				restack();
			else
				togglefloating(NULL);
			movemouse(c);
		}
		else if(ev->button == Button2) {
			if((floating != layout[selws-1]->arrange) && c->isfloating)
				togglefloating(NULL);
			else
				zoom(NULL);
		}
		else if(ev->button == Button3 && !c->isfixed) {
			if((floating == layout[selws-1]->arrange) || c->isfloating)
				restack();
			else
				togglefloating(NULL);
			resizemouse(c);
		}
	}
}

void
checkotherwm(void) {
	otherwm = False;
	XSetErrorHandler(xerrorstart);

	/* this causes an error if some other window manager is running */
	XSelectInput(dpy, root, SubstructureRedirectMask);
	XSync(dpy, False);
	if(otherwm)
		eprint("dwm: another window manager is already running\n");
	XSync(dpy, False);
	XSetErrorHandler(NULL);
	xerrorxlib = XSetErrorHandler(xerror);
	XSync(dpy, False);
}

void
cleanup(void) {
	if (!reload)
		close(STDIN_FILENO);
	while(stack) {
		unban(stack);
		unmanage(stack);
	}
	if(dc.font.set)
		XFreeFontSet(dpy, dc.font.set);
	else
		XFreeFont(dpy, dc.font.xfont);
	XUngrabKey(dpy, AnyKey, AnyModifier, root);
	XFreePixmap(dpy, dc.drawable);
	XFreeGC(dpy, dc.gc);
	XDestroyWindow(dpy, barwin);
	XFreeCursor(dpy, cursor[CurNormal]);
	XFreeCursor(dpy, cursor[CurResize]);
	XFreeCursor(dpy, cursor[CurMove]);
	XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSync(dpy, False);
}

void
compileregs(void) {
	unsigned int i;
	regex_t *reg;

	if(regs)
		return;
	regs = emallocz(LENGTH(rules) * sizeof(Regs));
	for(i = 0; i < LENGTH(rules); i++) {
		if(rules[i].prop) {
			reg = emallocz(sizeof(regex_t));
			if(regcomp(reg, rules[i].prop, REG_EXTENDED))
				free(reg);
			else
				regs[i].propregex = reg;
		}
	}
}

void
configure(Client *c) {
	XConfigureEvent ce;

	ce.type = ConfigureNotify;
	ce.display = dpy;
	ce.event = c->win;
	ce.window = c->win;
	ce.x = c->x;
	ce.y = c->y;
	ce.width = c->w;
	ce.height = c->h;
	ce.border_width = c->border;
	ce.above = None;
	ce.override_redirect = False;
	XSendEvent(dpy, c->win, False, StructureNotifyMask, (XEvent *)&ce);
}

void
configurenotify(XEvent *e) {
	XConfigureEvent *ev = &e->xconfigure;

	if(ev->window == root && (ev->width != sw || ev->height != sh)) {
		sw = ev->width;
		sh = ev->height;
		XFreePixmap(dpy, dc.drawable);
		dc.drawable = XCreatePixmap(dpy, root, sw, bh, DefaultDepth(dpy, screen));
		XResizeWindow(dpy, barwin, sw, bh);
		updatebarpos();
		arrange();
	}
}

void
configurerequest(XEvent *e) {
	Client *c;
	XConfigureRequestEvent *ev = &e->xconfigurerequest;
	XWindowChanges wc;

	if((c = getclient(ev->window))) {
		c->ismax = False;
		if(ev->value_mask & CWBorderWidth)
			c->border = ev->border_width;
		if(c->isfixed || c->isfloating || (floating == layout[selws-1]->arrange)) {
			if(ev->value_mask & CWX)
				c->x = ev->x;
			if(ev->value_mask & CWY)
				c->y = ev->y;
			if(ev->value_mask & CWWidth)
				c->w = ev->width;
			if(ev->value_mask & CWHeight)
				c->h = ev->height;
			if((c->x + c->w) > sw && c->isfloating)
				c->x = sw / 2 - c->w / 2; /* center in x direction */
			if((c->y + c->h) > sh && c->isfloating)
				c->y = sh / 2 - c->h / 2; /* center in y direction */
			if((ev->value_mask & (CWX | CWY))
			&& !(ev->value_mask & (CWWidth | CWHeight)))
				configure(c);
			if(isvisible(c))
				XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
		}
		else
			configure(c);
	}
	else {
		wc.x = ev->x;
		wc.y = ev->y;
		wc.width = ev->width;
		wc.height = ev->height;
		wc.border_width = ev->border_width;
		wc.sibling = ev->above;
		wc.stack_mode = ev->detail;
		XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
	}
	XSync(dpy, False);
}

void
destroynotify(XEvent *e) {
	Client *c;
	XDestroyWindowEvent *ev = &e->xdestroywindow;

	if((c = getclient(ev->window)))
		unmanage(c);
}

void
detach(Client *c) {
	if(c->prev)
		c->prev->next = c->next;
	if(c->next)
		c->next->prev = c->prev;
	if(c == clients)
		clients = c->next;
	c->next = c->prev = NULL;
}

void
detachstack(Client *c) {
	Client **tc;

	for(tc=&stack; *tc && *tc != c; tc=&(*tc)->snext);
	*tc = c->snext;
}

void
doreload(void) {
	execvp(cargv[0], cargv);
	eprint("Can't exec: %s\n", strerror(errno));
}

void
drawbar(void) {
	int x;

	dc.x = dc.y = 0;
	dc.w = wstextwidth;
	drawtext(wstext, dc.norm);
	dc.x = dc.w;
	dc.w = blw;
	drawtext(layout[selws-1]->symbol, dc.norm);
	x = dc.x + dc.w;
	dc.w = textw(stext);
	dc.x = sw - dc.w;
	if(dc.x < x) {
		dc.x = x;
		dc.w = sw - x;
	}
	drawtext(stext, dc.norm);
	if((dc.w = dc.x - x) > bh) {
		dc.x = x;
		if(sel) {
			drawtext(sel->name, dc.sel);
		}
		else
			drawtext(NULL, dc.norm);
	}
	XCopyArea(dpy, dc.drawable, barwin, dc.gc, 0, 0, sw, bh, 0, 0);
	XSync(dpy, False);
}

void
drawtext(const char *text, unsigned long col[ColLast]) {
	int x, y, w, h;
	static char buf[256];
	unsigned int len, olen;
	XRectangle r = { dc.x, dc.y, dc.w, dc.h };

	XSetForeground(dpy, dc.gc, col[ColBG]);
	XFillRectangles(dpy, dc.drawable, dc.gc, &r, 1);
	if(!text)
		return;
	w = 0;
	olen = len = strlen(text);
	if(len >= sizeof buf)
		len = sizeof buf - 1;
	memcpy(buf, text, len);
	buf[len] = 0;
	h = dc.font.ascent + dc.font.descent;
	y = dc.y + (dc.h / 2) - (h / 2) + dc.font.ascent;
	x = dc.x + (h / 2);
	/* shorten text if necessary */
	while(len && (w = textnw(buf, len)) > dc.w - h)
		buf[--len] = 0;
	if(len < olen) {
		if(len > 1)
			buf[len - 1] = '.';
		if(len > 2)
			buf[len - 2] = '.';
		if(len > 3)
			buf[len - 3] = '.';
	}
	if(w > dc.w)
		return; /* too long */
	XSetForeground(dpy, dc.gc, col[ColFG]);
	if(dc.font.set)
		Xutf8DrawString(dpy, dc.drawable, dc.font.set, dc.gc, x, y, buf, len);
	else
		XDrawString(dpy, dc.drawable, dc.gc, x, y, buf, len);
}

void *
emallocz(unsigned int size) {
	void *res = calloc(1, size);

	if(!res)
		eprint("fatal: could not malloc() %u bytes\n", size);
	return res;
}

void
enternotify(XEvent *e) {
	Client *c;
	XCrossingEvent *ev = &e->xcrossing;

	if(ev->mode != NotifyNormal || ev->detail == NotifyInferior)
		return;
	if((c = getclient(ev->window)))
		focus(c);
	else if(ev->window == root) {
		selscreen = True;
		focus(NULL);
	}
}

void
eprint(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void
expose(XEvent *e) {
	XExposeEvent *ev = &e->xexpose;

	if(ev->count == 0) {
		if(ev->window == barwin)
			drawbar();
	}
}

void
floating(void) { /* default floating layout */
	Client *c;

	domwfact = dozoom = False;
	for(c = clients; c; c = c->next)
		if(isvisible(c))
			resize(c, c->x, c->y, c->w, c->h, True);
}

void
focus(Client *c) {
	if((!c && selscreen) || (c && !isvisible(c)))
		for(c = stack; c && !isvisible(c); c = c->snext);
	if(sel && sel != c) {
		grabbuttons(sel, False);
		XSetWindowBorder(dpy, sel->win, dc.norm[ColBorder]);
	}
	if(c) {
		detachstack(c);
		attachstack(c);
		grabbuttons(c, True);
	}
	sel = c;
	drawbar();
	if(!selscreen)
		return;
	if(c) {
		XSetWindowBorder(dpy, c->win, dc.sel[ColBorder]);
		XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
	}
	else
		XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
}

void
focusin(XEvent *e) { /* there are some broken focus acquiring clients */
	XFocusChangeEvent *ev = &e->xfocus;

	if(sel && ev->window != sel->win)
		XSetInputFocus(dpy, sel->win, RevertToPointerRoot, CurrentTime);
}

void
focusnext(const char *arg) {
	Client *c;

	if(!sel)
		return;
	for(c = sel->next; c && !isvisible(c); c = c->next);
	if(!c)
		for(c = clients; c && !isvisible(c); c = c->next);
	if(c) {
		focus(c);
		restack();
	}
}

void
focusprev(const char *arg) {
	Client *c;

	if(!sel)
		return;
	for(c = sel->prev; c && !isvisible(c); c = c->prev);
	if(!c) {
		for(c = clients; c && c->next; c = c->next);
		for(; c && !isvisible(c); c = c->prev);
	}
	if(c) {
		focus(c);
		restack();
	}
}

Client *
getclient(Window w) {
	Client *c;

	for(c = clients; c && c->win != w; c = c->next);
	return c;
}

unsigned long
getcolor(const char *colstr) {
	Colormap cmap = DefaultColormap(dpy, screen);
	XColor color;

	if(!XAllocNamedColor(dpy, cmap, colstr, &color, &color))
		eprint("error, cannot allocate color '%s'\n", colstr);
	return color.pixel;
}

long
getstate(Window w) {
	int format, status;
	long result = -1;
	unsigned char *p = NULL;
	unsigned long n, extra;
	Atom real;

	status = XGetWindowProperty(dpy, w, wmatom[WMState], 0L, 2L, False, wmatom[WMState],
			&real, &format, &n, &extra, (unsigned char **)&p);
	if(status != Success)
		return -1;
	if(n != 0)
		result = *p;
	XFree(p);
	return result;
}

Bool
gettextprop(Window w, Atom atom, char *text, unsigned int size) {
	char **list = NULL;
	int n;
	XTextProperty name;

	if(!text || size == 0)
		return False;
	text[0] = '\0';
	XGetTextProperty(dpy, w, &name, atom);
	if(!name.nitems)
		return False;
	if(name.encoding == XA_STRING)
		strncpy(text, (char *)name.value, size - 1);
	else {
		if(Xutf8TextPropertyToTextList(dpy, &name, &list, &n) >= Success
		&& n > 0 && *list) {
			strncpy(text, *list, size - 1);
			XFreeStringList(list);
		}
	}
	text[size - 1] = '\0';
	XFree(name.value);
	return True;
}

void
grabbuttons(Client *c, Bool focused) {
	XUngrabButton(dpy, AnyButton, AnyModifier, c->win);

	if(focused) {
		XGrabButton(dpy, Button1, MODKEY, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button1, MODKEY | LockMask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button1, MODKEY | numlockmask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button1, MODKEY | numlockmask | LockMask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);

		XGrabButton(dpy, Button2, MODKEY, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button2, MODKEY | LockMask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button2, MODKEY | numlockmask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button2, MODKEY | numlockmask | LockMask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);

		XGrabButton(dpy, Button3, MODKEY, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button3, MODKEY | LockMask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button3, MODKEY | numlockmask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
		XGrabButton(dpy, Button3, MODKEY | numlockmask | LockMask, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
	}
	else
		XGrabButton(dpy, AnyButton, AnyModifier, c->win, False, BUTTONMASK,
				GrabModeAsync, GrabModeSync, None, None);
}

void
grabkeys(void)  {
	unsigned int i;
	KeyCode code;

	XUngrabKey(dpy, AnyKey, AnyModifier, root);
	for(i = 0; i < LENGTH(keys); i++) {
		code = XKeysymToKeycode(dpy, keys[i].keysym);
		XGrabKey(dpy, code, keys[i].mod, root, True,
				GrabModeAsync, GrabModeAsync);
		XGrabKey(dpy, code, keys[i].mod | LockMask, root, True,
				GrabModeAsync, GrabModeAsync);
		XGrabKey(dpy, code, keys[i].mod | numlockmask, root, True,
				GrabModeAsync, GrabModeAsync);
		XGrabKey(dpy, code, keys[i].mod | numlockmask | LockMask, root, True,
				GrabModeAsync, GrabModeAsync);
	}
}

void
initfont(const char *fontstr) {
	char *def, **missing;
	int i, n;

	missing = NULL;
	if(dc.font.set)
		XFreeFontSet(dpy, dc.font.set);
	dc.font.set = XCreateFontSet(dpy, fontstr, &missing, &n, &def);
	if(missing) {
		while(n--)
			fprintf(stderr, "dwm: missing fontset: %s\n", missing[n]);
		XFreeStringList(missing);
	}
	if(dc.font.set) {
		XFontSetExtents *font_extents;
		XFontStruct **xfonts;
		char **font_names;
		dc.font.ascent = dc.font.descent = 0;
		font_extents = XExtentsOfFontSet(dc.font.set);
		n = XFontsOfFontSet(dc.font.set, &xfonts, &font_names);
		for(i = 0, dc.font.ascent = 0, dc.font.descent = 0; i < n; i++) {
			if(dc.font.ascent < (*xfonts)->ascent)
				dc.font.ascent = (*xfonts)->ascent;
			if(dc.font.descent < (*xfonts)->descent)
				dc.font.descent = (*xfonts)->descent;
			xfonts++;
		}
	}
	else {
		if(dc.font.xfont)
			XFreeFont(dpy, dc.font.xfont);
		dc.font.xfont = NULL;
		if(!(dc.font.xfont = XLoadQueryFont(dpy, fontstr))
		&& !(dc.font.xfont = XLoadQueryFont(dpy, "fixed")))
			eprint("error, cannot load font: '%s'\n", fontstr);
		dc.font.ascent = dc.font.xfont->ascent;
		dc.font.descent = dc.font.xfont->descent;
	}
	dc.font.height = dc.font.ascent + dc.font.descent;
}

Bool
isprotodel(Client *c) {
	int i, n;
	Atom *protocols;
	Bool ret = False;

	if(XGetWMProtocols(dpy, c->win, &protocols, &n)) {
		for(i = 0; !ret && i < n; i++)
			if(protocols[i] == wmatom[WMDelete])
				ret = True;
		XFree(protocols);
	}
	return ret;
}

Bool
isvisible(Client *c) {
	return (c->workspace == selws);
}

void
keypress(XEvent *e) {
	unsigned int i;
	KeySym keysym;
	XKeyEvent *ev;

	ev = &e->xkey;
	keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
	for(i = 0; i < LENGTH(keys); i++)
		if(keysym == keys[i].keysym
		&& CLEANMASK(keys[i].mod) == CLEANMASK(ev->state))
		{
			if(keys[i].func)
				keys[i].func(keys[i].arg);
		}
}

void
killclient(const char *arg) {
	XEvent ev;

	if(!sel)
		return;
	if(isprotodel(sel)) {
		ev.type = ClientMessage;
		ev.xclient.window = sel->win;
		ev.xclient.message_type = wmatom[WMProtocols];
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = wmatom[WMDelete];
		ev.xclient.data.l[1] = CurrentTime;
		XSendEvent(dpy, sel->win, False, NoEventMask, &ev);
	}
	else
		XKillClient(dpy, sel->win);
}

void
leavenotify(XEvent *e) {
	XCrossingEvent *ev = &e->xcrossing;

	if((ev->window == root) && !ev->same_screen) {
		selscreen = False;
		focus(NULL);
	}
}

void
manage(Window w, XWindowAttributes *wa) {
	Client *c, *t = NULL;
	Window trans;
	Status rettrans;
	XWindowChanges wc;

	c = emallocz(sizeof(Client));
	c->win = w;
	c->workspace = selws;
	c->x = wa->x;
	c->y = wa->y;
	c->w = wa->width;
	c->h = wa->height;
	c->oldborder = wa->border_width;
	if(c->w == sw && c->h == sh) {
		c->x = sx;
		c->y = sy;
		c->border = wa->border_width;
	}
	else {
		if(c->x + c->w + 2 * c->border > wax + waw)
			c->x = wax + waw - c->w - 2 * c->border;
		if(c->y + c->h + 2 * c->border > way + wah)
			c->y = way + wah - c->h - 2 * c->border;
		if(c->x < wax)
			c->x = wax;
		if(c->y < way)
			c->y = way;
		c->border = BORDERPX;
	}
	wc.border_width = c->border;
	XConfigureWindow(dpy, w, CWBorderWidth, &wc);
	XSetWindowBorder(dpy, w, dc.norm[ColBorder]);
	configure(c); /* propagates border_width, if size doesn't change */
	updatesizehints(c);
	XSelectInput(dpy, w, EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask);
	grabbuttons(c, False);
	updatetitle(c);
	if((rettrans = XGetTransientForHint(dpy, w, &trans) == Success))
		for(t = clients; t && t->win != trans; t = t->next);
	applyrules(c);
	if(!c->isfloating)
		c->isfloating = (rettrans == Success) || c->isfixed;
	attach(c);
	attachstack(c);
	XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h); /* some windows require this */
	ban(c);
	XMapWindow(dpy, c->win);
	setclientstate(c, NormalState);
	arrange();
}

void
mappingnotify(XEvent *e) {
	XMappingEvent *ev = &e->xmapping;

	XRefreshKeyboardMapping(ev);
	if(ev->request == MappingKeyboard)
		grabkeys();
}

void
maprequest(XEvent *e) {
	static XWindowAttributes wa;
	XMapRequestEvent *ev = &e->xmaprequest;

	if(!XGetWindowAttributes(dpy, ev->window, &wa))
		return;
	if(wa.override_redirect)
		return;
	if(!getclient(ev->window))
		manage(ev->window, &wa);
}

void
movemouse(Client *c) {
	int x1, y1, ocx, ocy, di, nx, ny;
	unsigned int dui;
	Window dummy;
	XEvent ev;

	ocx = nx = c->x;
	ocy = ny = c->y;
	if(XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
			None, cursor[CurMove], CurrentTime) != GrabSuccess)
		return;
	c->ismax = False;
	XQueryPointer(dpy, root, &dummy, &dummy, &x1, &y1, &di, &di, &dui);
	for(;;) {
		XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);
		switch (ev.type) {
		case ButtonRelease:
			XUngrabPointer(dpy, CurrentTime);
			return;
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			XSync(dpy, False);
			nx = ocx + (ev.xmotion.x - x1);
			ny = ocy + (ev.xmotion.y - y1);
			if(abs(wax + nx) < SNAP)
				nx = wax;
			else if(abs((wax + waw) - (nx + c->w + 2 * c->border)) < SNAP)
				nx = wax + waw - c->w - 2 * c->border;
			if(abs(way - ny) < SNAP)
				ny = way;
			else if(abs((way + wah) - (ny + c->h + 2 * c->border)) < SNAP)
				ny = way + wah - c->h - 2 * c->border;
			resize(c, nx, ny, c->w, c->h, False);
			break;
		}
	}
}

Client *
nexttiled(Client *c) {
	for(; c && (c->isfloating || !isvisible(c)); c = c->next);
	return c;
}

void
propertynotify(XEvent *e) {
	Client *c;
	Window trans;
	XPropertyEvent *ev = &e->xproperty;

	if(ev->state == PropertyDelete)
		return; /* ignore */
	if((c = getclient(ev->window))) {
		switch (ev->atom) {
			default: break;
			case XA_WM_TRANSIENT_FOR:
				XGetTransientForHint(dpy, c->win, &trans);
				if(!c->isfloating && (c->isfloating = (getclient(trans) != NULL)))
					arrange();
				break;
			case XA_WM_NORMAL_HINTS:
				updatesizehints(c);
				break;
		}
		if(ev->atom == XA_WM_NAME || ev->atom == netatom[NetWMName]) {
			updatetitle(c);
			if(c == sel)
				drawbar();
		}
	}
}

void
quit(const char *arg) {
	readin = running = False;
}


void
resize(Client *c, int x, int y, int w, int h, Bool sizehints) {
	XWindowChanges wc;

	if(sizehints) {
		/* set minimum possible */
		if (w < 1)
			w = 1;
		if (h < 1)
			h = 1;

		/* temporarily remove base dimensions */
		w -= c->basew;
		h -= c->baseh;

		/* adjust for aspect limits */
		if (c->minay > 0 && c->maxay > 0 && c->minax > 0 && c->maxax > 0) {
			if (w * c->maxay > h * c->maxax)
				w = h * c->maxax / c->maxay;
			else if (w * c->minay < h * c->minax)
				h = w * c->minay / c->minax;
		}

		/* adjust for increment value */
		if(c->incw)
			w -= w % c->incw;
		if(c->inch)
			h -= h % c->inch;

		/* restore base dimensions */
		w += c->basew;
		h += c->baseh;

		if(c->minw > 0 && w < c->minw)
			w = c->minw;
		if(c->minh > 0 && h < c->minh)
			h = c->minh;
		if(c->maxw > 0 && w > c->maxw)
			w = c->maxw;
		if(c->maxh > 0 && h > c->maxh)
			h = c->maxh;
	}
	if(w <= 0 || h <= 0)
		return;
	/* offscreen appearance fixes */
	if(x > sw)
		x = sw - w - 2 * c->border;
	if(y > sh)
		y = sh - h - 2 * c->border;
	if(x + w + 2 * c->border < sx)
		x = sx;
	if(y + h + 2 * c->border < sy)
		y = sy;
	if(c->x != x || c->y != y || c->w != w || c->h != h) {
		c->x = wc.x = x;
		c->y = wc.y = y;
		c->w = wc.width = w;
		c->h = wc.height = h;
		wc.border_width = c->border;
		XConfigureWindow(dpy, c->win, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &wc);
		configure(c);
		XSync(dpy, False);
	}
}

void
resizemouse(Client *c) {
	int ocx, ocy;
	int nw, nh;
	XEvent ev;

	ocx = c->x;
	ocy = c->y;
	if(XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
			None, cursor[CurResize], CurrentTime) != GrabSuccess)
		return;
	c->ismax = False;
	XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w + c->border - 1, c->h + c->border - 1);
	for(;;) {
		XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask , &ev);
		switch(ev.type) {
		case ButtonRelease:
			XWarpPointer(dpy, None, c->win, 0, 0, 0, 0,
					c->w + c->border - 1, c->h + c->border - 1);
			XUngrabPointer(dpy, CurrentTime);
			while(XCheckMaskEvent(dpy, EnterWindowMask, &ev));
			return;
		case ConfigureRequest:
		case Expose:
		case MapRequest:
			handler[ev.type](&ev);
			break;
		case MotionNotify:
			XSync(dpy, False);
			if((nw = ev.xmotion.x - ocx - 2 * c->border + 1) <= 0)
				nw = 1;
			if((nh = ev.xmotion.y - ocy - 2 * c->border + 1) <= 0)
				nh = 1;
			resize(c, c->x, c->y, nw, nh, True);
			break;
		}
	}
}

void
restack(void) {
	Client *c;
	XEvent ev;
	XWindowChanges wc;

	drawbar();
	if(!sel)
		return;
	if(sel->isfloating || (layout[selws-1]->arrange == floating))
		XRaiseWindow(dpy, sel->win);
	if(layout[selws-1]->arrange != floating) {
		wc.stack_mode = Below;
		wc.sibling = barwin;
		if(!sel->isfloating) {
			XConfigureWindow(dpy, sel->win, CWSibling | CWStackMode, &wc);
			wc.sibling = sel->win;
		}
		for(c = nexttiled(clients); c; c = nexttiled(c->next)) {
			if(c == sel)
				continue;
			XConfigureWindow(dpy, c->win, CWSibling | CWStackMode, &wc);
			wc.sibling = c->win;
		}
	}
	XSync(dpy, False);
	while(XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

void
run(void) {
	char *p;
	char buf[sizeof stext];
	fd_set rd;
	int r, xfd;
	unsigned int len, offset;
	XEvent ev;

	/* main event loop, also reads status text from stdin */
	XSync(dpy, False);
	xfd = ConnectionNumber(dpy);
	readin = True;
	offset = 0;
	len = sizeof stext - 1;
	buf[len] = stext[len] = '\0'; /* 0-terminator is never touched */
	while(running) {
		FD_ZERO(&rd);
		if(readin)
			FD_SET(STDIN_FILENO, &rd);
		FD_SET(xfd, &rd);
		if(select(xfd + 1, &rd, NULL, NULL, NULL) == -1) {
			if(errno == EINTR)
				continue;
			eprint("select failed\n");
		}
		if(FD_ISSET(STDIN_FILENO, &rd)) {
			switch((r = read(STDIN_FILENO, buf + offset, len - offset))) {
			case -1:
				strncpy(stext, strerror(errno), len);
				readin = False;
				break;
			case 0:
				strncpy(stext, "EOF", 4);
				readin = False;
				break;
			default:
				for(p = buf + offset; r > 0; p++, r--, offset++)
					if(*p == '\n' || *p == '\0') {
						*p = '\0';
						strncpy(stext, buf, len);
						p += r - 1; /* p is buf + offset + r - 1 */
						for(r = 0; *(p - r) && *(p - r) != '\n'; r++);
						offset = r;
						if(r)
							memmove(buf, p - r + 1, r);
						break;
					}
				break;
			}
			drawbar();
		}
		while(XPending(dpy)) {
			XNextEvent(dpy, &ev);
			if(handler[ev.type])
				(handler[ev.type])(&ev); /* call handler */
		}
	}
}

void
scan(void) {
	unsigned int i, num;
	Window *wins, d1, d2;
	XWindowAttributes wa;

	wins = NULL;
	if(XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
		for(i = 0; i < num; i++) {
			if(!XGetWindowAttributes(dpy, wins[i], &wa)
			|| wa.override_redirect || XGetTransientForHint(dpy, wins[i], &d1))
				continue;
			if(wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
				manage(wins[i], &wa);
		}
		for(i = 0; i < num; i++) { /* now the transients */
			if(!XGetWindowAttributes(dpy, wins[i], &wa))
				continue;
			if(XGetTransientForHint(dpy, wins[i], &d1)
			&& (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
				manage(wins[i], &wa);
		}
	}
	if(wins)
		XFree(wins);
}

void
setclientstate(Client *c, long state) {
	long data[] = {state, None};

	XChangeProperty(dpy, c->win, wmatom[WMState], wmatom[WMState], 32,
			PropModeReplace, (unsigned char *)data, 2);
}

void
setlayout(const char *arg) {
	unsigned int i;

	if(!arg) {
		if(++layout[selws-1] == &layouts[LENGTH(layouts)])
			layout[selws-1] = &layouts[0];
	}
	else {
		for(i = 0; i < LENGTH(layouts); i++)
			if(!strcmp(arg, layouts[i].symbol))
				break;
		if(i == LENGTH(layouts))
			return;
		layout[selws-1] = &layouts[i];
	}
	if(sel)
		arrange();
	else
		drawbar();
}

void
setmwfact(const char *arg) {
	double delta;

	if(!domwfact)
		return;
	/* arg handling, manipulate mwfact */
	if(arg == NULL)
		mwfact[selws-1] = MWFACT;
	else if(sscanf(arg, "%lf", &delta) == 1) {
		if(arg[0] == '+' || arg[0] == '-')
			mwfact[selws-1] += delta;
		else
			mwfact[selws-1] = delta;
		if(mwfact[selws-1] < 0.1)
			mwfact[selws-1] = 0.1;
		else if(mwfact[selws-1] > 0.9)
			mwfact[selws-1] = 0.9;
	}
	arrange();
}

void
setup(void) {
	int d;
	unsigned int i, j, mask;
	Window w;
	XModifierKeymap *modmap;
	XSetWindowAttributes wa;

	/* init atoms */
	wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False);
	wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	wmatom[WMName] = XInternAtom(dpy, "WM_NAME", False);
	wmatom[WMState] = XInternAtom(dpy, "WM_STATE", False);
	netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False);
	netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False);
	XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
			PropModeReplace, (unsigned char *) netatom, NetLast);

	/* init cursors */
	cursor[CurNormal] = XCreateFontCursor(dpy, XC_left_ptr);
	cursor[CurResize] = XCreateFontCursor(dpy, XC_sizing);
	cursor[CurMove] = XCreateFontCursor(dpy, XC_fleur);

	/* init geometry */
	sx = sy = 0;
	sw = DisplayWidth(dpy, screen);
	sh = DisplayHeight(dpy, screen);

	/* init modifier map */
	modmap = XGetModifierMapping(dpy);
	for(i = 0; i < 8; i++)
		for(j = 0; j < modmap->max_keypermod; j++) {
			if(modmap->modifiermap[i * modmap->max_keypermod + j]
			== XKeysymToKeycode(dpy, XK_Num_Lock))
				numlockmask = (1 << i);
		}
	XFreeModifiermap(modmap);

	/* select for events */
	wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask
		| EnterWindowMask | LeaveWindowMask | StructureNotifyMask;
	wa.cursor = cursor[CurNormal];
	XChangeWindowAttributes(dpy, root, CWEventMask | CWCursor, &wa);
	XSelectInput(dpy, root, wa.event_mask);

	/* grab keys */
	grabkeys();

	/* init workspaces */
	compileregs();

	/* init appearance */
	dc.norm[ColBorder] = getcolor(NORMBORDERCOLOR);
	dc.norm[ColBG] = getcolor(NORMBGCOLOR);
	dc.norm[ColFG] = getcolor(NORMFGCOLOR);
	dc.sel[ColBorder] = getcolor(SELBORDERCOLOR);
	dc.sel[ColBG] = getcolor(SELBGCOLOR);
	dc.sel[ColFG] = getcolor(SELFGCOLOR);
	initfont(FONT);
	dc.h = bh = dc.font.height + 2;

	/* init layouts */
	for(i = 0; i < MAXWORKSPACES; i++) {
		mwfact[i] = MWFACT;
		layout[i] = &layouts[0];
	}
	for(blw = i = 0; i < LENGTH(layouts); i++) {
		j = textw(layouts[i].symbol);
		if(j > blw)
			blw = j;
	}

	/* init bar */
	bpos = BARPOS;
	wa.override_redirect = 1;
	wa.background_pixmap = ParentRelative;
	wa.event_mask = ButtonPressMask | ExposureMask;
	barwin = XCreateWindow(dpy, root, sx, sy, sw, bh, 0,
			DefaultDepth(dpy, screen), CopyFromParent, DefaultVisual(dpy, screen),
			CWOverrideRedirect | CWBackPixmap | CWEventMask, &wa);
	XDefineCursor(dpy, barwin, cursor[CurNormal]);
	updatebarpos();
	XMapRaised(dpy, barwin);
	strcpy(stext, "dwm-"VERSION);
	dc.drawable = XCreatePixmap(dpy, root, sw, bh, DefaultDepth(dpy, screen));
	dc.gc = XCreateGC(dpy, root, 0, 0);
	XSetLineAttributes(dpy, dc.gc, 1, LineSolid, CapButt, JoinMiter);
	if(!dc.font.set)
		XSetFont(dpy, dc.gc, dc.font.xfont->fid);

	/* multihead support */
	selscreen = XQueryPointer(dpy, root, &w, &w, &d, &d, &d, &d, &mask);

	/* restart support */
	if (signal(SIGUSR1, sigusr1) == SIG_ERR) 
		eprint("Can't bind to signal USR1\n");

	/* check initial parameters */
	if (workspaces < 1)
		workspaces = 1;
	else if (workspaces > MAXWORKSPACES)
		workspaces = MAXWORKSPACES;
	
}

void
sigusr1(int notused) {
	quit(NULL);
	reload = True;
}

void
spawn(const char *arg) {
	static char *shell = NULL;

	if(!shell && !(shell = getenv("SHELL")))
		shell = "/bin/sh";
	if(!arg)
		return;
	/* The double-fork construct avoids zombie processes and keeps the code
	 * clean from stupid signal handlers. */
	if(fork() == 0) {
		if(fork() == 0) {
			if(dpy)
				close(ConnectionNumber(dpy));
			setsid();
			execl(shell, shell, "-c", arg, (char *)NULL);
			fprintf(stderr, "dwm: execl '%s -c %s'", shell, arg);
			perror(" failed");
		}
		exit(0);
	}
	wait(0);
}

void
updatewstext(void) {
	snprintf(wstext, MAXWSTEXTWIDTH, "%d/%d", selws, workspaces);
	wstextwidth = textw(wstext);
}

void
viewrel(const char *arg) {
	int i;
	
	if (workspaces == 1)
		return;
	i = arg ? atoi(arg) : 0;
	if (i == 0)
                return;
	selws += i;
	if (selws > workspaces)
		selws = 1;
	if (selws < 1)
		selws = workspaces;
	updatewstext();
        arrange();
}

unsigned int
textnw(const char *text, unsigned int len) {
	XRectangle r;

	if(dc.font.set) {
		Xutf8TextExtents(dc.font.set, text, len, NULL, &r);
		return r.width;
	}
	return XTextWidth(dc.font.xfont, text, len);
}

unsigned int
textw(const char *text) {
	return textnw(text, strlen(text)) + dc.font.height;
}

void
tile(void) {
	unsigned int i, n, nx, ny, nw, nh, mw, th;
	Client *c, *mc;

	domwfact = dozoom = True;
	for(n = 0, c = nexttiled(clients); c; c = nexttiled(c->next))
		n++;

	/* window geoms */
	mw = (n == 1) ? waw : mwfact[selws-1] * waw;
	th = (n > 1) ? wah / (n - 1) : 0;
	if(n > 1 && th < bh)
		th = wah;

	nx = wax;
	ny = way;
	nw = 0; /* gcc stupidity requires this */
	for(i = 0, c = mc = nexttiled(clients); c; c = nexttiled(c->next), i++) {
		c->ismax = False;
		if(i == 0) { /* master */
			nw = mw - 2 * c->border;
			nh = wah - 2 * c->border;
		}
		else {  /* tile window */
			if(i == 1) {
				ny = way;
				nx += mc->w + 2 * mc->border;
				nw = waw - nx - 2 * c->border;
			}
			if(i + 1 == n) /* remainder */
				nh = (way + wah) - ny - 2 * c->border;
			else
				nh = th - 2 * c->border;
		}
		resize(c, nx, ny, nw, nh, RESIZEHINTS);
		if((RESIZEHINTS) && ((c->h < bh) || (c->h > nh) || (c->w < bh) || (c->w > nw)))
			/* client doesn't accept size constraints */
			resize(c, nx, ny, nw, nh, False);
		if(n > 1 && th != wah)
			ny = c->y + c->h + 2 * c->border;
	}
}

void
togglebar(const char *arg) {
	if(bpos == BarOff)
		bpos = (BARPOS == BarOff) ? BarTop : BARPOS;
	else
		bpos = BarOff;
	updatebarpos();
	arrange();
}

void
togglefloating(const char *arg) {
	if(!sel)
		return;
	sel->isfloating = !sel->isfloating;
	if(sel->isfloating)
		resize(sel, sel->x, sel->y, sel->w, sel->h, True);
	arrange();
}

void
togglemax(const char *arg) {
	XEvent ev;

	if(!sel || sel->isfixed)
		return;
	if((sel->ismax = !sel->ismax)) {
		if((layout[selws-1]->arrange == floating) || sel->isfloating)
			sel->wasfloating = True;
		else {
			togglefloating(NULL);
			sel->wasfloating = False;
		}
		sel->rx = sel->x;
		sel->ry = sel->y;
		sel->rw = sel->w;
		sel->rh = sel->h;
		resize(sel, wax, way, waw - 2 * sel->border, wah - 2 * sel->border, True);
	}
	else {
		resize(sel, sel->rx, sel->ry, sel->rw, sel->rh, True);
		if(!sel->wasfloating)
			togglefloating(NULL);
	}
	drawbar();
	while(XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

void
view(const char *arg) {
	int i;
	
	i = arg ? atoi(arg) : 0;
	if ((i < 1) || (i > workspaces) || (i == selws))
		return;
	selws = i;
	updatewstext();
	arrange();  
}

void
pushstack(const char *arg) {
	if (!sel)
		return;
	sel->workspace = 0;
	focus(NULL);
	arrange();
}

void
popstack(const char *arg) {
	Client *c;
	for(c = stack; c && c->workspace; c = c->snext);
	if (c)
		c->workspace = selws;
	focus(c);
	arrange();
}

void
wscount(const char *arg) {
	Client *c;
	int i;
	unsigned int j;
	
	i = arg ? atoi(arg) : 0;
	if (i == 0 )
		return;   
	if (i > 0) {
		if (workspaces + i > MAXWORKSPACES) {
			i = MAXWORKSPACES - workspaces;
			if (i == 0)
				return;
		}
		for(c = clients; c; c = c->next)
			if (c->workspace > selws)
				c->workspace += i;
		workspaces += i;
		for(j = workspaces - 1; j >= selws + i; j--) {
			layout[j] = layout[j-i];
			mwfact[j] = mwfact[j-i];
		}
		for(j = 0; j < i; j++) {
			layout[selws + j] = layout[selws - 1];
			mwfact[selws + j] = mwfact[selws - 1];
		}
		selws++; 
	}
	else {
		i = workspaces + i;
		if (i < 1)
			i = 1;
		while (i < workspaces) {
			for(c = clients; c; c = c->next)
				if (c->workspace > selws)
					c->workspace--;
				else if (c->workspace == selws) {
					c->workspace = 0;
				}
			for(j = selws; j < workspaces; j++) {
				layout[j-1] = layout[j];
				mwfact[j-1] = mwfact[j];   
			}
			if (selws == workspaces)
				selws--;
			workspaces--;
		}
	}
	updatewstext();
	arrange();
}

void
unban(Client *c) {
	if(!c->isbanned)
		return;
	XMoveWindow(dpy, c->win, c->x, c->y);
	c->isbanned = False;
}

void
unmanage(Client *c) {
	XWindowChanges wc;

	wc.border_width = c->oldborder;
	/* The server grab construct avoids race conditions. */
	XGrabServer(dpy);
	XSetErrorHandler(xerrordummy);
	XConfigureWindow(dpy, c->win, CWBorderWidth, &wc); /* restore border */
	detach(c);
	detachstack(c);
	if(sel == c)
		focus(NULL);
	XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
	setclientstate(c, WithdrawnState);
	free(c);
	XSync(dpy, False);
	XSetErrorHandler(xerror);
	XUngrabServer(dpy);
	arrange();
}

void
unmapnotify(XEvent *e) {
	Client *c;
	XUnmapEvent *ev = &e->xunmap;

	if((c = getclient(ev->window)))
		unmanage(c);
}

void
updatebarpos(void) {
	XEvent ev;

	wax = sx;
	way = sy;
	wah = sh;
	waw = sw;
	switch(bpos) {
	default:
		wah -= bh;
		way += bh;
		XMoveWindow(dpy, barwin, sx, sy);
		break;
	case BarBot:
		wah -= bh;
		XMoveWindow(dpy, barwin, sx, sy + wah);
		break;
	case BarOff:
		XMoveWindow(dpy, barwin, sx, sy - bh);
		break;
	}
	XSync(dpy, False);
	while(XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

void
updatesizehints(Client *c) {
	long msize;
	XSizeHints size;

	if(!XGetWMNormalHints(dpy, c->win, &size, &msize) || !size.flags)
		size.flags = PSize;
	c->flags = size.flags;
	if(c->flags & PBaseSize) {
		c->basew = size.base_width;
		c->baseh = size.base_height;
	}
	else if(c->flags & PMinSize) {
		c->basew = size.min_width;
		c->baseh = size.min_height;
	}
	else
		c->basew = c->baseh = 0;
	if(c->flags & PResizeInc) {
		c->incw = size.width_inc;
		c->inch = size.height_inc;
	}
	else
		c->incw = c->inch = 0;
	if(c->flags & PMaxSize) {
		c->maxw = size.max_width;
		c->maxh = size.max_height;
	}
	else
		c->maxw = c->maxh = 0;
	if(c->flags & PMinSize) {
		c->minw = size.min_width;
		c->minh = size.min_height;
	}
	else if(c->flags & PBaseSize) {
		c->minw = size.base_width;
		c->minh = size.base_height;
	}
	else
		c->minw = c->minh = 0;
	if(c->flags & PAspect) {
		c->minax = size.min_aspect.x;
		c->maxax = size.max_aspect.x;
		c->minay = size.min_aspect.y;
		c->maxay = size.max_aspect.y;
	}
	else
		c->minax = c->maxax = c->minay = c->maxay = 0;
	c->isfixed = (c->maxw && c->minw && c->maxh && c->minh
			&& c->maxw == c->minw && c->maxh == c->minh);
}

void
updatetitle(Client *c) {
	if(!gettextprop(c->win, netatom[NetWMName], c->name, sizeof c->name))
		gettextprop(c->win, wmatom[WMName], c->name, sizeof c->name);
}

/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's).  Other types of errors call Xlibs
 * default error handler, which may call exit.  */
int
xerror(Display *dpy, XErrorEvent *ee) {
	if(ee->error_code == BadWindow
	|| (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
	|| (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
	|| (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
	|| (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
	|| (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
	|| (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
		return 0;
	fprintf(stderr, "dwm: fatal error: request code=%d, error code=%d\n",
		ee->request_code, ee->error_code);
	return xerrorxlib(dpy, ee); /* may call exit */
}

int
xerrordummy(Display *dsply, XErrorEvent *ee) {
	return 0;
}

/* Startup Error handler to check if another window manager
 * is already running. */
int
xerrorstart(Display *dsply, XErrorEvent *ee) {
	otherwm = True;
	return -1;
}

void
moveto(const char *arg) {
	unsigned int i;
	
	if (!sel)
		return;
	i = arg ? atoi(arg) : 0;   
	if ((i < 1) || (i > workspaces))
		return;
	sel->workspace = i;
        arrange();
}

void
zoom(const char *arg) {
	Client *c;

	if(!sel || !dozoom || sel->isfloating)
		return;
	if((c = sel) == nexttiled(clients))
		if(!(c = nexttiled(c->next)))
			return;
	detach(c);
	attach(c);
	focus(c);
	arrange();
}

int
main(int argc, char *argv[]) {
	if(argc == 2 && !strcmp("-v", argv[1]))
		eprint("dwm-"VERSION", © 2006-2007 Anselm R. Garbe, Sander van Dijk, "
		       "Jukka Salmi, Premysl Hruby, Szabolcs Nagy\n");
	else if(argc != 1)
		eprint("usage: dwm [-v]\n");
	cargv = argv;

	setlocale(LC_CTYPE, "");
	if(!(dpy = XOpenDisplay(0)))
		eprint("dwm: cannot open display\n");
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	checkotherwm();
	setup();
	updatewstext();
	drawbar();
	scan();
	run();
	cleanup();

	XCloseDisplay(dpy);
	if (reload)
		doreload();
	return 0;
}
