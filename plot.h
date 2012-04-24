#ifndef PLOT_H
#define PLOT_H

struct plot;
typedef struct plot plot_t;

plot_t *plot_new(void);
void plot_addlink(plot_t *plot, char *from, char *to, char *mods);
void plot_doplot(plot_t *plot);

#endif  /* PLOT_H */
