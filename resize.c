/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember ntor and check proper value
    int n = atoi(argv[1]);
    
    if(n < 1 || n > 100)
    {
        printf("Not Valid ntor (n) was given\n");
        return 2;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
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
        return 5;
    }
    // update size and save the infile size
    int inWidth = bi.biWidth;
    int inHeight = bi.biHeight;
    bi.biWidth *= n;
    bi.biHeight *= n;
    
    
    // determine padding for scanlines in the in file.
    int padding =  (4 - (inWidth * (bi.biBitCount/8)) % 4) % 4;
    // determine the new file padding
    int padding_out = (4 - (bi.biWidth * (bi.biBitCount/8)) % 4) % 4;

    // Update BITMAPINFORHEADER
    bi.biSizeImage = abs(bi.biHeight) * ((bi.biWidth * (bi.biBitCount/8)) + padding_out) ;

    // Update BITMAPFILERHEADER
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // create array to hold  pixeles to fill new width
    RGBTRIPLE pixels[bi.biWidth];

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(inHeight); i < biHeight; i++)
    {
        int shifter = 0;
        // iterate over pixels in scanline
        for (int j = 0; j < inWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
            // remember the pixel and repeat it n times
            for(int r = 0; r < n; r++)
            {
                pixels[r+shifter] = triple;
                
            }
            shifter += n;
        }
        
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
        
        // write the pixels array n times
        
            for (int c = 0 ; c < n; c++)
            {
                for(int w = 0 ; w < bi.biWidth; w++)
                {
                    fwrite(&pixels[w], sizeof(RGBTRIPLE),1, outptr);
                }
                for (int k = 0; k < padding_out; k++)
                {
                    fputc(0x00, outptr);
                }
            }
            
            
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    
    // that's all folks
    return 0;
}
