/* pixels2pgf -- pixel-exact image to PGF converter
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>

void output_pgf(unsigned char* pixels, int width, int height, int pitch)
{
	int y, x;

	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			unsigned char pixel = pixels[y * pitch + x];

			if (!(pixel & 0xBF))
				printf("\\filldraw (%d, -%d) rectangle ++(1, -1);\n", x, y);
		}
	}
}

int process(SDL_Surface* input)
{
	SDL_PixelFormat nicepf = {
		NULL,
		8, 1,
		0, 2, 4, 6,
		6, 6, 6, 6,
		0x03, 0x0C, 0x30, 0xC0,
		0xC0, 0
	};

	SDL_Surface* image;

	if (!input)
	{
		fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
		return 1;
	}

	/* Convert to a nicer format. */
	image = SDL_ConvertSurface(input, &nicepf, SDL_SWSURFACE);
	SDL_FreeSurface(input);

	if (!image)
	{
		fprintf(stderr, "Unable to convert image: %s\n",
				SDL_GetError());
		return 1;
	}

	output_pgf(image->pixels, image->w, image->h, image->pitch);

	SDL_FreeSurface(image);
	return 0;
}

int main(int argc, char* argv[])
{
	int ret;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return 1;
	}

	if (SDL_Init(0))
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	IMG_Init(0);

	ret = process(IMG_Load(argv[1]));

	IMG_Quit();
	SDL_Quit();

	return ret;
}
