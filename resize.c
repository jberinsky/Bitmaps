// Jason Berinsky
// CS 50 Problem Set 3
// Resizes a BMP file by a factor

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize f infile outfile\n");
        return 1;
    }

    // remember filenames
    float factor = atof(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // Check if the factor is valid
    if (factor <= 0 || factor > 100)
    {
        fprintf(stderr, "Must enter a resize factor between 0 and 100.\n");
        return 1;
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 1;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 1;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for scanlines of input bmp
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int input_padding = padding;

    // Output file and info header
    BITMAPFILEHEADER output_bf = bf;
    BITMAPINFOHEADER output_bi = bi;

    // Sets width and height of output bitmap in info header
    float width = bi.biWidth * factor;
    float height = bi.biHeight * factor;
    int ratio;
    if (width - (int) width >= .5)
    {
        ratio = factor + 1;
        output_bi.biWidth = bi.biWidth * ratio;
    }
    else
    {
        output_bi.biWidth = (int) width;
        ratio = factor;
    }
    output_bi.biHeight = bi.biHeight * ratio;

    // Determine output padding;
    int output_padding = (4 - (output_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Sets image and file sizes
    output_bi.biSizeImage = ((sizeof(RGBTRIPLE) * output_bi.biWidth) + output_padding) * abs(output_bi.biHeight);
    output_bf.bfSize = output_bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&output_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&output_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        for (int n = 0; n < ratio; n++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                for (int k = 0; k < ratio; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // Add padding
            for (int k = 0; k < output_padding; k++)
            {
                fputc(0x00, outptr);
            }

            if (ratio - n >= 2)
            {
                int move = -1 * (bi.biWidth * sizeof(RGBTRIPLE));

                fseek(inptr, move, SEEK_CUR);
            }
            else
            {
                fseek(inptr, padding, SEEK_CUR);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}