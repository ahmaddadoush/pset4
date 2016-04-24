/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 * Student: Ahmad Dadoush
 * Recovers JPEGs from a forensic image.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cs50.h>

#define BLOCK 512

bool checkJpg(uint8_t* buf);

int main(int argc, char* argv[])
{
    // open the hard disk to recover from and remember it.
    FILE* disk = fopen("card.raw" , "r");
    if (disk == NULL)
    {
        printf("Couldn't open the file\n");
        return 1;
    }
    
    // create space of 512Bytes
    uint8_t* buffer = malloc(sizeof(uint8_t) * BLOCK);

    FILE* curFile = NULL;
    
    // create sting for images numbering
    char* str = malloc(sizeof(char)*10);
    int lastName = 0;
    
    //Read the disk and recover images.
    while(fread(buffer,sizeof(uint8_t),BLOCK,disk))
    {
        //check if the current block is A jpg Start.
        if(checkJpg(buffer) == true)
        {
            // check if any file is opened
            if(curFile != NULL)
            {
                // close the previous file
                fclose(curFile);
                
                // open new file and  write the block.
                sprintf(str,"%03d.jpg",lastName);
                lastName++;
                curFile = fopen(str, "w+");
                fwrite(buffer, sizeof(uint8_t), BLOCK,curFile);
            }
            // if there is no file opened create new one.
            else if(curFile == NULL)
            {
                sprintf(str,"%03d.jpg",lastName);
                lastName++;
                curFile = fopen(str , "w+");
                fwrite(buffer, sizeof(uint8_t), BLOCK,curFile);
            }
        }
        else 
        {
            // check if any file is opened
            if (curFile != NULL)
            {
                // if yes continue writing on same file
                fwrite(buffer, sizeof(uint8_t), BLOCK,curFile);
            }
            // if no skip this block
        }
    }
    // free memory and close files.
    free(buffer);
    free(str);
    if (curFile) fclose(curFile);
    fclose(disk);
    //printf("%i Images was Recoverd\n",lastName);
}

// function checks if the block has a jpg Sign, Usage: checkJpg(pointer to the block , jpg signature).
bool checkJpg(uint8_t* buf)
{
    // jpg signature
    uint8_t jpgSign[3] = {0xff, 0xd8, 0xff};
    int i = 0;
    if(*(buf + 3) >= 0xe0 && *(buf + 3) <= 0xef)
    {
        while (i < 3)
        {
            if(*(buf + i) != jpgSign[i]) 
            {
                return false;
                break;
            }
            i++;
        }
        return true;
    }
    else return false;
}
