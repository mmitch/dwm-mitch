void
widescreen(unsigned int s) {
  unsigned int i, n, lx, ly, rx, ry, nx, ny, nw, nh, mw, th, tw;
        Client *c, *mc;

        domwfact[s] = dozoom[s] = True;
        for(n = 0, c = nexttiled(clients, s); c; c = nexttiled(c->next, s))
                n++;

        /* window geoms */
        mw = (n == 1) ? waw[s] : mwfact[s][selws[s]-1] * waw[s];
        th = (n > 1) ? wah[s] / (n / 2) : 0;
        if(n > 1 && th < bh)
                th = wah[s];

	tw = (n > 1) ? (waw[s] - mw) / 2 : 0;

	lx = wax[s];
        nx = lx + tw;
	rx = nx + mw;
        ny = ly = ry = way[s];
        nw = 0; /* gcc stupidity requires this */
                
        for(i = 0, c = mc = nexttiled(clients, s); c; c = nexttiled(c->next, s), i++) {
                c->ismax = False;
                if(i == 0) { /* master */
                        nw = mw - 2 * c->border;
                        nh = wah[s] - 2 * c->border;
			if (n == 0)
				nx = 0;
                }
                else {  /* tile window */
		  if(i % 2) {
		    if(i > 1)
		      ly += th;
		    ny = ly;
		    nx = lx;
		    nw = tw - 2 * c->border;
		    nh = th - 2 * c->border;
		  } else {
		    if(i > 2)
		      ry += th;
		    ny = ry;
		    nx = rx;
		    nw = tw - 2 * c->border;
		    nh = th - 2 * c->border;
		  }
                }
                resize(c, nx, ny, nw, nh, RESIZEHINTS);
                if((RESIZEHINTS) && ((c->h < bh) || (c->h > nh) || (c->w < bh) || (c->w > nw)))
                        /* client doesn't accept size constraints */
                        resize(c, nx, ny, nw, nh, False);
        }
}

