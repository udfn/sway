#ifndef _SWAYBAR_RENDER_H
#define _SWAYBAR_RENDER_H

struct swaybar_output;
struct swaybar;

void destroy_popup(struct swaybar *bar);
void render_popup(struct swaybar_output *output);

void render_frame(struct swaybar_output *output);

#endif
