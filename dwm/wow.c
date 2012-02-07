void
wow(unsigned int s) {
	Client *c;
	
	domwfact[s] = True;
	dozoom[s] = False;        
        for(c = clients; c; c = c->next)
		if (c->screen == s)
			if (isvisible(c)) {
				unban(c);
				if(! c->isfloating)
					resize(c, wax[s] + ( 1 - mwfact[s][selws[s]-1] ) * waw[s], way[s], waw[s] - 2 * BORDERPX - ( 1 - mwfact[s][selws[s]-1] ) * waw[s], wah[s] - 2 * BORDERPX, True);
			} else {
				ban(c);
			}
	
        focus(NULL);
        restack();
}

