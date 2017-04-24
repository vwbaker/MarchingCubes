#ifndef __Texture__
#define __Texture__

#include <GL/glew.h>
#include <string>

class Texture
{
public:
	
	/*data structure for the image used for  texture mapping */
	typedef struct Image {
		int sizeX;
		int sizeY;
		char *data;
	} Image;
	
	typedef struct RGB {
		GLubyte r;
		GLubyte g;
		GLubyte b;
	} RGB;
	
	static int ImageLoad(const char *filename, Image *image);
	
	Texture();
	virtual ~Texture();
	void setFilename(const std::string &f) { filename = f; }
	void setName(const std::string &n) { name = n; }
	const std::string &getName() const { return name; }
	void setUnit(int u) { unit = u; }
	int getUnit() const { return unit; }
	void setHandle(GLint h) { handle = h; }
	GLint getHandle() const { return handle; }
	void init();
	void bind();
	void unbind();
	
private:
	std::string filename;
	std::string name;
	GLuint tid;
	int unit;
	GLint handle;
	
};

#endif
