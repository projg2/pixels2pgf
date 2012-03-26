/* pixels2pgf -- pixel-exact image to PGF converter
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <stdio.h>
#include <getopt.h>

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
				printf("\\fill (%d, -%d) rectangle ++(1, -1);\n", x, y);
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

const struct option long_opts[] = {
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'V' },

	{ 0, 0, 0, 0 }
};

void print_help(FILE* f, const char* self)
{
	fprintf(f, ""
"Usage: %s [options] <input-file>\n"
"\n"
"Convert image in <input-file> to pixel-perfect rectangles for PGF/Tikz.\n"
"Outputs to stdout.\n"
"\n"
"Options:\n"
"	--help		output this help message\n"
"	--version	output program version\n",
			self);
}

int main(int argc, char* argv[])
{
	int opt, ret;

	while ((opt = getopt_long(argc, argv, "hV", long_opts, NULL)) != -1)
	{
		switch (opt)
		{
			case 'V':
				printf("%s-%s\n", PACKAGE_NAME, PACKAGE_VERSION);
				return 0;
			case 'h':
				print_help(stdout, argv[0]);
				return 0;
			default:
				print_help(stderr, argv[0]);
				return 1;
		}
	}

	if (argc - optind != 1)
	{
		print_help(stderr, argv[0]);
		return 1;
	}

	if (SDL_Init(0))
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	IMG_Init(0);

	ret = process(IMG_Load(argv[optind]));

	IMG_Quit();
	SDL_Quit();

	return ret;
}
