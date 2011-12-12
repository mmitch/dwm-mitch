void
bstack(void) {
    unsigned int i, n, nx, ny, nw, nh, mh, tw;
    Client *c, *mc;

    domwfact = dozoom = True;
    for(n = 0, c = nexttiled(clients); c; c = nexttiled(c->next))
        n++;

    mh = (n == 1) ? wah : mwfact * wah;
    tw = (n > 1) ? waw / (n - 1) : 0;

    nx = wax;
    ny = way;
    nh = 0;
    for(i = 0, c = mc = nexttiled(clients); c; c = nexttiled(c->next), i++) {
        c->ismax = False;
        if(i == 0) {
            nh = mh - 2 * c->border;
            nw = waw - 2 * c->border;
        }
        else {
            if(i == 1) {
                nx = wax;
                ny += mc->h + 2 * mc->border;
                nh = (way + wah) - ny - 2 * c->border;
            }
            if(i + 1 == n)
                nw = (wax + waw) - nx - 2 * c->border;
            else
                nw = tw - 2 * c->border;
        }
        resize(c, nx, ny, nw, nh, RESIZEHINTS);
        if((RESIZEHINTS) && ((c->h < bh) || (c->h > nh) || (c->w < bh) || (c->w > nw)))
            resize(c, nx, ny, nw, nh, False);
        if(n > 1 && tw != waw)
            nx = c->x + c->w + 2 * c->border;
    }
}
