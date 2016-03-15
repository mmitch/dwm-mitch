dwm-mitch
=========

This is my version for the dynamic window manager (http://dwm.suckless.org).
Get the most current version from https://github.com/mmitch/dwm-mitch
Older versions were hosted at http://www.cgarbs.de/dwm-mitch.en.html

dwm-mitch is based von dwm 4.7 from December 2007, so patches from
current dwm versions will not apply cleanly, although it is possible
to cherry-pick a thing or two without too much hassle.

installation
------------

Use ``make`` or ``make install`` to install everything.
Use ``make uninstall`` to remove installed files.
Installation probably needs root privileges.

running
-------

After installation, dwm-mitch should show up as an X session
type in your login screen.

See the ``CHEATSHEET`` for default keybindings.

A running dwm can be restarted by ``kill -s USR1 $pid_of_dwm``.
Use this if you have changed your configuration and did a
``make install`` to activate your changes without losing your
current X session.

configuration
-------------

The included file ``dwm-mitch`` is a start script that initializes a
dmenu-cache, sources your startup script, starts a loop for the status
line (showing current time and system load) and runs dwm.

``dwm-mitch`` looks for ``$HOME/.dwm-startup`` and, if found, sources
and executes it just before starting dwm.  This allows you to
automatically start some programs after your X session is completely
intialized (ssh-agent, session manager etc.) and is useful if your
login manager starts dwm-mitch via the ``dwm-mitch.desktop`` entry.

If ``dwm-mitch`` finds an executable file in ``$HOME/.dwm-status``, it
will be run for status line updates instead of the built-in default.
To update the status line, change the title of the X root window,
e.g. call ``xsetroot -name $content`` regularly.  See the
``dwm-mitch`` script for an example.

The environment variable ``DMENU_COLORS`` is used for changing the
colors of dmenu on the dwm-choose script.  Set it like this:
``DMENU_COLORS="-fn fixed -nb #336 -nf #ccc -sb #669 -sf #eee"``

dmenu uses ``$HOME/.dmenu-history`` to save its history.
dmenu uses ``$HOME/.dmenu-cache`` to cache the list of executables.
The list is regenerated with every new dwm session.  If you want it
to be updated sooner, just delete the cache file.  The next dmenu
call will regenerate it.

Your personal configuration changes should be kept in a separate
branch in your personal dwm-mitch git repository.  Edit
``dwm/config.def.h`` to your needs and check it into your local
branch.  Changes to the master branch will might result in merge
conflics that you have to clean up, but at least you'll notice that
things have changed.  My setup does NOT use the ``dwm/config.h``
intermediate file like the original dwm.

history
-------

I was a big fan of wmii-2.  I liked the dynamic workspaces, but I did
not like the tags-approach of wmii-3.  Then I came upon dwm, which had
all the core functionality of wmii - unluckily of wmii-3 and not of
wmii-2.  But because dwm is easy to extend, I was able to make dwm
behave like my beloved wmii-2.

included dwm patches
--------------------

 *  add bottom stack layout,
    written by: Ross Mohn <rpmohn@waxandwane.org>

 *  add fullscreen layout

 *  add widescreen layout

 *  remove tagging and add dynamic workspaces

 *  allow a different settings (layout, mwidth) on every workspace

 *  remove square boxes in status line

 *  fix display of wide characters

 *  add dwm-mitch patchlevel to dwm version

 *  add dwm reload via SIGUSR1,
    written by: anydot <dfenze@gmail.com>,
    also applied suggestions by: Jukka Salmi <j+dwm@2007.salmi.ch>

 *  allow different border sizes for floating and non-floating clients

 *  togglemax only works for floating clients,
    (this allows the same key for zoom and togglemax and it does the
    right thing[tm] depending on whether the client is floating or not)

 *  add tile layout with stack on the left side

 *  add Xinerama support

 *  read status line from X root window instead of stdin,
    use ``xsetroot -name $foo`` to update

 *  skip keybindings to unknown symbols

 *  move mouse pointer via keyboard across Xinerama screens

 *  switch workspace, layout and window via mousewheel on statusbar

 *  volume control via statusbar text

included dmenu patches
----------------------

 *  fix display of wide characters

 *  add dmenu-mitch patchlevel to dmenu version

