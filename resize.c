// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char *argv[])
{

    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // number to multiply by
    //eprintf("argv[1] = %s\n", argv[1]);

    //these two lines (for some reason) prevent me from getting 2073760490 for (int)argv[1] when 4 is given as an argument
    string mult = argv[1];
    int multiplier = (int)mult[0]-48;

    //eprintf("multiplier = %i\n", multiplier);
    //eprintf("multiplier = %i\n", multiplier);
    //eprintf("integer 4 = %i\n", (int)4);

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }
    /*small.bmp
        BITMAPINFOHEADER size = 14
            biSizeImage =
                sizeof(RBGTRIPLE) =
                biWidth =
                padding =
                biHeight =
        BITMAPFILEHEADER size = 40
            bfSize = 58
                padding
                pixels
                headers = 54?
    */


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

    //make BITMAPFILEHEADER for outfile
    BITMAPFILEHEADER out_bf;


    //make BITMAPINFOHEADER for outfile
    BITMAPINFOHEADER out_bi;

    //make them identical so that it isnt necessary to rewrite all the untouched attributes of the new files
    out_bf = bf;
    out_bi = bi;

    //change out_bi specs
    out_bi.biWidth = bi.biWidth * multiplier;
    eprintf("out_bi.biWidth = %u\n", out_bi.biWidth);

    //eprintf("bi.biHeight = %i\n", bi.biHeight);

    //change out_bi.biHeight
    out_bi.biHeight = abs(bi.biHeight*multiplier);
    eprintf("out_bi.biHeight = %i\n", out_bi.biHeight);

    //calculate padding
    int outfile_padding =  ((out_bi.biWidth*3) % 4) % 4;
    eprintf("outfile_padding = %i\n", outfile_padding);

    // change biSizeImage
    out_bi.biSizeImage = ((out_bi.biHeight*out_bi.biWidth)* 3) + (outfile_padding*out_bi.biHeight);

    eprintf("bi.biSizeImage = %i\n", bi.biSizeImage);
    eprintf("out_bi.biSizeImage = %i\n", out_bi.biSizeImage);

    //change out_bf.bfSize
    out_bf.bfSize = 54 + out_bi.biSizeImage;
    eprintf("out_bf.bfSize = %u\n", out_bf.bfSize);

    // write outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER) , 1, outptr);

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    int counter = 0;
    // iterate over infile's scanlines (for each row)
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        //for n-1 times
        //printing the first 3 lines 4 times then printing the rest???
        //printing first (multiplier-1) lines (multiplier) times
        for (int m = 0; m < multiplier -1; m++)
        {
            //write pixels, padding to outfile
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;


                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                //printf("reading ");

                //expand the image horizontally
                for (int k = 0; k < multiplier; k++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    counter += 1;


                }
                //fseek(inptr, -sizeof(RGBTRIPLE), SEEK_CUR);
            }

        }
        //send infile cursor back
        fseek(inptr, -(bi.biWidth*sizeof(RGBTRIPLE)), SEEK_CUR);

}
    //##############################      write pixels, padding to outfile one last time      #############################################//
    // temporary storage
    RGBTRIPLE triple;


    // read RGB triple from infile
    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

    //expand the image horizontally
    for (int k = 0; k < multiplier; k++)
    {
        // write RGB triple to outfile
        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        counter += 1;
    }

    eprintf("counter: %i\n", counter);

    // skip over padding, if any
    fseek(inptr, outfile_padding, SEEK_CUR);

    // then add it back (to demonstrate how)
    for (int k = 0; k < outfile_padding; k++)
    {
        fputc(0x00, outptr);
    }



    //check infile specs to make sense of this mess
    /*
    eprintf("BITMAPFILEHEADER SIZE: %lu\n", sizeof(BITMAPFILEHEADER));
    eprintf("BITMAPINFOHEADER SIZE: %lu\n", sizeof(BITMAPINFOHEADER));
    eprintf("bf.bfSize = %u\n", bf.bfSize);
    eprintf("bi.biHeight = %i\n", bi.biHeight);
    eprintf("bi.biWidth = %u\n", bi.biWidth);
    eprintf("bi.biSizeImage = %i\n", bi.biSizeImage);
    */


    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
