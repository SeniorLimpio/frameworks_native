/*
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/

#include <stdlib.h>
#include <stdio.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

int main(int argc, char** argv)
{
    EGLint s_configAttribs[] = {
         EGL_RED_SIZE,       5,
         EGL_GREEN_SIZE,     6,
         EGL_BLUE_SIZE,      5,
         EGL_NONE
     };
     
     EGLint numConfigs = -1, n=0;
     EGLint majorVersion;
     EGLint minorVersion;
     EGLConfig config;
     EGLContext context;
     EGLSurface surface;
     EGLint w, h;
     
     EGLDisplay dpy;

     dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
     eglInitialize(dpy, &majorVersion, &minorVersion);
     
     // Get all the "potential match" configs...
     eglGetConfigs(dpy, NULL, 0, &numConfigs);
     EGLConfig* const configs = malloc(sizeof(EGLConfig)*numConfigs);
     eglChooseConfig(dpy, s_configAttribs, configs, numConfigs, &n);
     config = configs[0];
     if (n > 1) {
         // if there is more than one candidate, go through the list
         // and pick one that matches our framebuffer format
         int fbSzA = 0; // should not hardcode
         int fbSzR = 5; // should not hardcode
         int fbSzG = 6; // should not hardcode
         int fbSzB = 5; // should not hardcode
         int i;
         for (i=0 ; i<n ; i++) {
             EGLint r,g,b,a;
             eglGetConfigAttrib(dpy, configs[i], EGL_RED_SIZE,   &r);
             eglGetConfigAttrib(dpy, configs[i], EGL_GREEN_SIZE, &g);
             eglGetConfigAttrib(dpy, configs[i], EGL_BLUE_SIZE,  &b);
             eglGetConfigAttrib(dpy, configs[i], EGL_ALPHA_SIZE, &a);
             if (fbSzA == a && fbSzR == r && fbSzG == g && fbSzB  == b) {
                 config = configs[i];
                 break;
             }
         }
     }
     free(configs);
     
     
     
     surface = eglCreateWindowSurface(dpy, config,
             android_createDisplaySurface(), NULL);
     context = eglCreateContext(dpy, config, NULL, NULL);
     eglMakeCurrent(dpy, surface, surface, context);   
     eglQuerySurface(dpy, surface, EGL_WIDTH, &w);
     eglQuerySurface(dpy, surface, EGL_HEIGHT, &h);
     GLint dim = w<h ? w : h;


     GLint crop[4] = { 0, 4, 4, -4 };
     glBindTexture(GL_TEXTURE_2D, 0);
     glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
     glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     glEnable(GL_TEXTURE_2D);
     glColor4f(1,1,1,1);

     // packing is always 4
     uint8_t t8[]  = { 
             0x00, 0x55, 0x00, 0x55, 
             0xAA, 0xFF, 0xAA, 0xFF,
             0x00, 0x55, 0x00, 0x55, 
             0xAA, 0xFF, 0xAA, 0xFF  };

     uint16_t t16[]  = { 
             0x0000, 0x5555, 0x0000, 0x5555, 
             0xAAAA, 0xFFFF, 0xAAAA, 0xFFFF,
             0x0000, 0x5555, 0x0000, 0x5555, 
             0xAAAA, 0xFFFF, 0xAAAA, 0xFFFF  };

     uint16_t t5551[]  = { 
             0x0000, 0xFFFF, 0x0000, 0xFFFF, 
             0xFFFF, 0x0000, 0xFFFF, 0x0000,
             0x0000, 0xFFFF, 0x0000, 0xFFFF, 
             0xFFFF, 0x0000, 0xFFFF, 0x0000  };

     uint32_t t32[]  = { 
             0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 
             0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF, 
             0xFF00FFFF, 0xFF00FF00, 0x00FF00FF, 0xFFFFFF00, 
             0xFF000000, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF
     };


     glClear(GL_COLOR_BUFFER_BIT);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 4, 4, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, t8);
     glDrawTexiOES(0, 0, 0, dim/2, dim/2);

     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, t16);
     glDrawTexiOES(dim/2, 0, 0, dim/2, dim/2);

     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, t16);
     glDrawTexiOES(0, dim/2, 0, dim/2, dim/2);

     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, t32);
     glDrawTexiOES(dim/2, dim/2, 0, dim/2, dim/2);

     eglSwapBuffers(dpy, surface);
     return 0;
}
