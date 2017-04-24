#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/* BMP file loader loads a 24-bit bmp file only */

/*
 * getint and getshort are help functions to load the bitmap byte by byte
 */
static unsigned int getint(FILE *fp) {
	int c, c1, c2, c3;
	
	/*  get 4 bytes */
	c = getc(fp);
	c1 = getc(fp);
	c2 = getc(fp);
	c3 = getc(fp);
	
	return ((unsigned int) c) +
	(((unsigned int) c1) << 8) +
	(((unsigned int) c2) << 16) +
	(((unsigned int) c3) << 24);
}

static unsigned int getshort(FILE *fp){
	int c, c1;
	
	/* get 2 bytes*/
	c = getc(fp);
	c1 = getc(fp);
	
	return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

/*  quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  */

int Texture::ImageLoad(const char *filename, Image *image) {
	FILE *file;
	unsigned long size;                 /*  size of the image in bytes. */
	unsigned long i;                    /*  standard counter. */
	unsigned short int planes;          /*  number of planes in image (must be 1)  */
	unsigned short int bpp;             /*  number of bits per pixel (must be 24) */
	char temp;                          /*  used to convert bgr to rgb color. */
	
	/*  make sure the file is there. */
	if ((file = fopen(filename, "rb"))==NULL) {
		printf("File Not Found : %s\n",filename);
		return 0;
	}
	
	/*  seek through the bmp header, up to the width height: */
	fseek(file, 18, SEEK_CUR);
	
	/*  No 100% errorchecking anymore!!! */
	
	/*  read the width */    image->sizeX = getint (file);
	
	/*  read the height */
	image->sizeY = getint (file);
	
	/*  calculate the size (assuming 24 bits or 3 bytes per pixel). */
	size = image->sizeX * image->sizeY * 3;
	
	/*  read the planes */
	planes = getshort(file);
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}
	
	/*  read the bpp */
	bpp = getshort(file);
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	
	/*  seek past the rest of the bitmap header. */
	fseek(file, 24, SEEK_CUR);
	
	/*  read the data.  */
	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}
	
	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	
	for (i=0;i<size;i+=3) { /*  reverse all of the colors. (bgr -> rgb) */
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
	}
	
	fclose(file); /* Close the file and release the filedes */
	
	/*  we're done. */
	return 1;
}

Texture::Texture() :
	filename(""),
	name(""),
	tid(0),
	unit(0),
	handle(0)
{
	
}

Texture::~Texture()
{
	
}

void Texture::init()
{
	Texture::Image *image0 = (Texture::Image *)malloc(sizeof(Texture::Image));
	if(image0 == NULL) {
		printf("Error allocating space for image");
	}
	if(!Texture::ImageLoad(filename.c_str(), image0)) {
		printf("Error loading texture image\n");
	}
	
	// Set active texture to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	// Generate a texture buffer object
	glGenTextures(1, &tid);
	// Bind the current texture to be the newly generated texture object
	glBindTexture(GL_TEXTURE_2D, tid);
	// Load the actual texture data
	// Base level is 0, number of channels is 3, and border is 0.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image0->sizeX, image0->sizeY,
				 0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*) image0->data);

	// Generate image pyramid
	glGenerateMipmap(GL_TEXTURE_2D);
	// Set texture wrap modes for the S and T directions
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set filtering mode for magnification and minimification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	// Free image, since the data is now on the GPU
	free(image0);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tid);
	glUniform1i(handle, unit);
}

void Texture::unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}
