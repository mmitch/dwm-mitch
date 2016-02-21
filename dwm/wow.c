void
wow(unsigned int s) {
	Client *c;
	int wempty;
	
	domwfact[s] = True;
	dozoom[s] = False;
	wempty = (0.5 - (mwfact[s][selws[s]-1] / 2)) * waw[s];
        for(c = clients; c; c = c->next)
		if (c->screen == s) {
			if (isvisible(c)) {
				unban(c);
				if(! c->isfloating)
					resize(c, wax[s] + wempty * (sx[s] != 0), way[s], waw[s] - 2 * BORDERPX - wempty, wah[s] - 2 * BORDERPX, True);
			} else {
				ban(c);
			}
		}
	
        focus(NULL);
        restack();
}

