#pragma once
#include <EGL/egl.h>
void menuStyle();

// HOOKINPUT(void, Input, void *thiz, void *ex_ab, void *ex_ac)
// {
//     origInput(thiz, ex_ab, ex_ac);
//     ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
//     return;
// }

// This menu_addr is used to allow for multiple game support in the future
void *initModMenu(void *menu_addr, void *on_init_addr = nullptr);

void setupMenu();

void internalDrawMenu(int width, int height);

EGLBoolean swapbuffers_hook(EGLDisplay dpy, EGLSurface surf);

int getGlWidth();
int getGlHeight();
