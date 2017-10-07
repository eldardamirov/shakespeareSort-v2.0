//
//  main.cpp
//  shakespeareSort
//
//  Created by Эльдар Дамиров on 04.10.2017.
//  Copyright © 2017 Эльдар Дамиров. All rights reserved.
//



const int quantityOfElementsToCompareFor = 64; // accurancy ~ quantityOfElementsToCompareFor; speed ~ 1 / quantityOfElementsToCompareFor;

#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <bitset>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <time.h>


#include <iostream>





struct fileMapping
    {
    int fileDescriptor;
    size_t fileSize;
    unsigned char* dataPointer;
    };


class readFromFile
    {
    public:

        readFromFile ( char* inputFileName )
            {
            inputFileName = "input.txt";

            init();
            }

        ~readFromFile()
            {
            munmap ( dataPointer, fileSize );
            close ( mapping->fileDescriptor );
            free ( mapping );
            }



        char getNextChar()
            {
            char tempBuffer =  *( ( mapping->dataPointer ) + currentChar );
            currentChar++;

            return tempBuffer;
            }


        size_t getFileSize()
            {
            return fileSize;
            }

        int getLinesQuantity()
            {
            return linesQuantity;
            }


        char* meGetDataPointer()
            {
            return ( char* ) ( mapping->dataPointer );
            }


        bool isEnd()
            {
            return ( currentChar + 1 ) >= fileSize;
            }

    private:

        char* inputFileName = {};
        struct stat st;
        size_t fileSize = 0;
        int fileDescriptor = 0;
        unsigned char* dataPointer = {};
        fileMapping* mapping;
        int currentChar = 0;
        int linesQuantity = 0; // OPTIONAL

        void init()
            {
           // fileDescriptor = open ( "input.txt", O_RDONLY , 0 );
            fileDescriptor = open ( "input.txt", O_RDWR , 0 );
            if ( fileDescriptor < 0 )
                {
                printf ( "File descriptor error.\n" );
                abort();
                }


            if ( fstat ( fileDescriptor, &st ) < 0 )
                {
                printf ( "Fstat failed.\n" );
                close ( fileDescriptor );
                abort();
                }
            fileSize = ( size_t ) st.st_size;

            dataPointer = getDataPointer ( fileSize, fileDescriptor );
            mapping = createFileMapping ( fileDescriptor, dataPointer, fileSize );


            linesQuantity = calculateLinesQuantity();
            }

        unsigned char* getDataPointer ( size_t fileSize, int fileDescriptor )
            {
            unsigned char* tempDataPointer = ( unsigned char* ) mmap ( nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fileDescriptor, 0 );

            if ( tempDataPointer == MAP_FAILED )
                {
                printf ( "mmap failed\n" );
                close ( fileDescriptor );
                abort();
                }

            return tempDataPointer;
            }

        fileMapping* createFileMapping ( int fileDescriptor, unsigned char* dataPointer, size_t fileSize )
            {
            fileMapping* tempMapping = ( fileMapping* ) malloc ( sizeof ( fileMapping ) );

            if ( tempMapping == nullptr )
                {
                printf ( "mmap failed\n" );
                munmap ( dataPointer, fileSize );
                close ( fileDescriptor );
                abort();
                }

            tempMapping->fileDescriptor = fileDescriptor;
            tempMapping->fileSize = fileSize;
            tempMapping->dataPointer = dataPointer;

            return tempMapping;
            }


        int calculateLinesQuantity()  // returns EXACT number of lines, including empty lines; OPTIONAL0
            {
            int linesQuantity = 0;
            char* index = strchr ( ( ( char* ) ( mapping->dataPointer ) ), '\n' );

            char* filePointerEnd = strchr ( ( ( char* ) ( mapping->dataPointer ) ), '\0' ) - 1;
            while ( ( index ) < filePointerEnd )
                {
                linesQuantity++;
                index = ( strchr ( ( index + 1 ), '\n' ) );
                }

            linesQuantity = linesQuantity + 1;

            return linesQuantity;
            }

    };



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



class writeToFile
    {
    public:

        writeToFile ( char* fileName, size_t fileSize )
            {
            outputFileName = fileName;
            fileSizeMe = fileSize + 1;

            init();
            }


        void writeNextChar ( char inputChar )
            {
            map [ currentPosition ] = inputChar;
            currentPosition++;
            }

        ~writeToFile()
            {
            msync ( map, fileSizeMe, MS_SYNC );
            munmap ( map, fileSizeMe );
            close ( fileDescriptor );
//            msync ( map, fileSizeMe, MS_SYNC );
            }


    private:

        char* outputFileName = {};
        struct stat st;
        size_t fileSizeMe = 0;
        int fileDescriptor = 0;
        int fileDescription = 0;
        char* map;
        int currentPosition = 0;



        void init()
            {
            fileDescriptor = open ( "output.txt", O_RDWR | O_CREAT , ( mode_t ) 0600 );
            if ( fileDescriptor < 0 )
                {
                printf ( "File descriptor error.\n" );
                abort();
                }

            fileDescription = makeFileDescription ( fileDescriptor, fileSizeMe );
            updateFileSize ( fileDescriptor, fileDescription );
            map = mmapFile ( fileDescriptor, fileSizeMe );

            }


        int makeFileDescription ( int fileDescriptor, size_t fileSize )
            {
            //int tempFileDescription = lseek ( fileDescriptor, ( fileSize - 1 ), SEEK_SET );
            int tempFileDescription = lseek ( fileDescriptor, fileSizeMe, SEEK_SET );
            if ( tempFileDescription < 0 )
                {
                close ( fileDescriptor );
                printf ( "lseek error." );
                abort();
                }

            return tempFileDescription;
            }

        void updateFileSize ( int fileDescriptor, int fileDescription )
            {
            if ( write ( fileDescriptor, "", 1 ) < 0 )
                {
                close ( fileDescriptor );
                printf ( "Writing null string to last file's byte failed." );
                abort();
                }
            }



        char* mmapFile ( int fileDescriptor, size_t fileSize )
            {
            char* tempMap = ( char* ) ( mmap ( 0, fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, fileDescriptor, 0 ) );
            if ( tempMap == MAP_FAILED )
                {
                close ( fileDescriptor );
                printf ( "File mapping failed." );
                abort();
                }

            return tempMap;
            }




    };


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



int comparator ( const void* first, const void* second );




int main()
    {
    clock_t beginClock = clock();
    readFromFile inputFile ( "input.txt" );
    writeToFile outputFile ( "output.txt", inputFile.getFileSize() );


    //clock_t beginClock = clock();
    int linesQuantity = inputFile.getLinesQuantity();

    //double finalTime = clock() - beginClock;
    //printf ( "%f", finalTime / 1000000 );

    char** lineBeginnings = new char* [ linesQuantity ] {};  // lines beginnings pointers storage;

    char* index = inputFile.meGetDataPointer();
    lineBeginnings [ 0 ] = index;
    for ( int i = 1; i < linesQuantity; i++ )
        {
        index = strchr ( ( index + 1 ) , '\n' );
        lineBeginnings [ i ] = ( index + 1 );
        }


//    clock_t beginClock = clock();
    qsort ( lineBeginnings, linesQuantity, sizeof ( char** ), comparator );

//    double finalTime = clock() - beginClock;
//    printf ( "%f", finalTime / 1000000 );




    int addingToIndex = 0;

    int fileBorder = linesQuantity - 1;
    for ( int i = 0; i < fileBorder; i++ )
        {
        addingToIndex = 0;
        //while ( ( ( lineBeginnings [ i ] + addingToIndex ) != NULL ) && ( * ( lineBeginnings [ i ] + addingToIndex ) != '\n' ) && ( * ( lineBeginnings [ i ] + addingToIndex ) != '\0' ) )
        while ( ( ( lineBeginnings [ i ] + addingToIndex ) != NULL ) && ( * ( lineBeginnings [ i ] + addingToIndex ) != '\n' ) )
            {
            outputFile.writeNextChar ( * ( lineBeginnings [ i ] + addingToIndex ) );
            addingToIndex++;
            }
        outputFile.writeNextChar ( '\n' );
        i++;
        }


    double finalTime = clock() - beginClock;
    printf ( "%f", finalTime / 1000000 );

    return 0;
    }



int comparator ( const void* first, const void* second )
    {
    return strncmp ( *( const char** ) first, * ( const char** ) second, quantityOfElementsToCompareFor );
    }




