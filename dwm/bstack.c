void
bstack(unsigned int s) {
    unsigned int i, n, nx, ny, nw, nh, mh, tw;
    Client *c, *mc;

    domwfact[s] = dozoom[s] = True;
    for(n = 0, c = nexttiled(clients, s); c; c = nexttiled(c->next, s))
	    n++;
    
    mh = (n == 1) ? wah[s] : mwfact[s][selws[s]-1] * wah[s];
    tw = (n > 1) ? waw[s] / (n - 1) : 0;
    
    nx = wax[s];
    ny = way[s];
    nh = 0;
    for(i = 0, c = mc = nexttiled(clients, s); c; c = nexttiled(c->next, s), i++) {
	    c->ismax = False;
	    if(i == 0) {
		    nh = mh - 2 * c->border;
		    nw = waw[s] - 2 * c->border;
	    }
	    else {
		    if(i == 1) {
			    nx = wax[s];
			    ny += mc->h + 2 * mc->border;
			    nh = (way[s] + wah[s]) - ny - 2 * c->border;
		    }
		    if(i + 1 == n)
			    nw = (wax[s] + waw[s]) - nx - 2 * c->border;
		    else
			    nw = tw - 2 * c->border;
	    }
	    resize(c, nx, ny, nw, nh, RESIZEHINTS);
	    if((RESIZEHINTS) && ((c->h < bh) || (c->h > nh) || (c->w < bh) || (c->w > nw)))
		    resize(c, nx, ny, nw, nh, False);
	    if(n > 1 && tw != waw[s])
		    nx = c->x + c->w + 2 * c->border;
    }
}
