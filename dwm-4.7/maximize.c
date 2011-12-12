void
maximize(void) {
	Client *c;
	
	domwfact = dozoom = False;        
        for(c = clients; c; c = c->next)
                if (isvisible(c)) {
                        unban(c);
                        if(! c->isfloating)
                                resize(c, wax, way, waw - 2 * BORDERPX, wah - 2 * BORDERPX, True);
                } else {
                        ban(c);
                }
	
        focus(NULL);
        restack();
}

