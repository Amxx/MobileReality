#ifndef 	__APICAM_UVC_RELEASE_H__
#define 	__APICAM_UVC_RELEASE_H__


#include "avilib.h"

#include <linux/videodev2.h>
#include <stdio.h>

#define NB_BUFFER 4

struct vdIn {
    int fd;
    char *videodevice;
    char *status;
    char *pictName;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_buffer buf;
    struct v4l2_requestbuffers rb;
    void *mem[NB_BUFFER];
    unsigned char *tmpbuffer;
    unsigned char *framebuffer;
    int isstreaming;
    int grabmethod;
    int width;
    int height;
    int fps;
    int formatIn;
    int formatOut;
    int framesizeIn;
    int signalquit;
    int toggleAvi;
    int getPict;
    int rawFrameCapture;
    /* raw frame capture */
    unsigned int fileCounter;
    /* raw frame stream capture */
    unsigned int rfsFramesWritten;
    unsigned int rfsBytesWritten;
    /* raw stream capture */
    FILE *captureFile;
    unsigned int framesWritten;
    unsigned int bytesWritten;
    avi_t *avifile;
    char *avifilename;
    int framecount;
    int recordstart;
    int recordtime;
};

// fonctions de la libairie libuvc-release.so
// ecrite et compilee en C

extern "C" {
	int close_v4l2(struct vdIn *vd);
	int uvcGrab(struct vdIn *vd);
	int init_videoIn(struct vdIn *, char *, int, int, int, int, int, char *);
	void initLut(void);
	unsigned int Pyuv422tobgr24(unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height);
	unsigned int Pyuv422torgb24(const unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height);
	unsigned int Pyuv422torgb444DownSample(const unsigned char *input_ptr, unsigned char * output_ptr, unsigned int input_image_width, unsigned int input_image_height);
	unsigned int Pyuv422tobgr444DownSample(const unsigned char *input_ptr, unsigned char * output_ptr, unsigned int input_image_width, unsigned int input_image_height);
	int v4l2SetControl(struct vdIn *vd, int control, int value);
	int v4l2GetControl(struct vdIn *vd, int control);
	int v4l2ResetControl(struct vdIn *vd, int control);
	int enum_controls(int vd);
	int enum_controls_bak(int vd);
	int enum_frame_formats(int dev, unsigned int *supported_formats, unsigned int max_formats);
}

#endif		// 	__APICAM_UVC_RELEASE_H__
