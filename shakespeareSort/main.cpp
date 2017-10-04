//
//  main.cpp
//  shakespeareSort
//
//  Created by Эльдар Дамиров on 04.10.2017.
//  Copyright © 2017 Эльдар Дамиров. All rights reserved.
//

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

       int getLinesQuantity()
            {
            int linesQuantity = 0;
            char* index = strchr ( ( ( char* ) ( mapping->dataPointer ) ), '\n' );
            //*index =  ( '\0' );
            /*
            __asm
                {
                mov eax, index;    // EAX = address of name
                mov bl, '\0';
                mov byte[eax], bl;
                }
            */
           // printf ( "INDEX: %d\n", index );
           // std::cout << "INDEX:" << index << std::endl;
           // std::cout << "END" << std::endl;
            char* filePointerEnd = strchr ( ( ( char* ) ( mapping->dataPointer ) ), '\0' ) - 1;
            while ( ( index ) < filePointerEnd )
                {
                linesQuantity++;
                index = ( strchr ( ( index + 1 ), '\n' ) );
                }

            linesQuantity = linesQuantity + 2;

            return linesQuantity;
            }


        size_t getFileSize()
            {
            return fileSize;
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

    };


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

            /*
            if ( fstat ( fileDescriptor, &st ) < 0 )
                {
                printf ( "Fstat failed.\n" );
                close ( fileDescriptor );
                abort();
                }
            */
            //fileSize = ( size_t ) st.st_size;
            //fileSize = fileSize + 1;

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
            //fileDescription = write ( fileDescriptor, "", 1 );
            //write ( fileDescriptor, "", 1 );
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



bool comparator ( char* first, char* second );




int main()
    {
//    clock_t beginClock = clock();
    //printf ( "%d, %d", 'a', 'Z' );
    readFromFile inputFile ( "input.txt" );
    writeToFile outputFile ( "output.txt", inputFile.getFileSize() );

    char** lineBeginnings = new char* [ inputFile.getLinesQuantity() + 2 ] {};

    int i = 0;
    lineBeginnings [ i ] = inputFile.meGetDataPointer();
    i++;
    char* index = strchr ( ( ( char* ) ( inputFile.meGetDataPointer() ) ), '\n' );
    char* filePointerEnd = strchr ( ( ( char* ) ( inputFile.meGetDataPointer() ) ), '\0' ) - 1;
    while ( ( index ) < filePointerEnd )
        {
        lineBeginnings [ i ] = index + 1;
        i++;
        index = ( strchr ( ( index + 1 ), '\n' ) );
        }

    clock_t beginClock = clock();

    std::sort ( lineBeginnings, ( lineBeginnings + inputFile.getLinesQuantity() ), comparator );

    int addingToIndex = 0;

    int fileBorder = inputFile.getLinesQuantity() - 1;
    for ( int i = 0; i < fileBorder; i++ )
        {
        addingToIndex = 0;
        while ( ( ( lineBeginnings [ i ] + addingToIndex ) != NULL ) && ( * ( lineBeginnings [ i ] + addingToIndex ) != '\n' ) && ( * ( lineBeginnings [ i ] + addingToIndex ) != '\0' ) )
            {
            outputFile.writeNextChar ( * ( lineBeginnings [ i ] + addingToIndex ) );
            addingToIndex++;
            }
        outputFile.writeNextChar ( '\n' );
        }



    double finalTime = clock() - beginClock;
    printf ( "%f", finalTime / 1000000 );

    return 0;
    }


bool comparator ( char* first, char* second )
    {
    int addingToIndex = 0;
    //int oddSpaces = 0;

    //while ( ( ( second + addingToIndex ) != NULL ) && ( ( first + addingToIndex ) != NULL ) && ( * ( first + addingToIndex ) != '\n' ) && ( * ( first + addingToIndex ) != '\0' ) && ( * ( second + addingToIndex ) != '\n' ) && ( * ( second + addingToIndex ) != '\0' ) )
    while ( ( ( first + addingToIndex ) != NULL ) && ( ( second + addingToIndex ) != NULL ) && ( * ( first + addingToIndex ) != '\n' ) && ( * ( second + addingToIndex ) != '\n' ) )
        {
        //if ( ( ( ( ( * ( first + addingToIndex ) >= 'a' ) ) && ( ( * ( first + addingToIndex ) <= 'z' ) ) ) || ( ( ( * ( first + addingToIndex ) >= 'A' ) ) && ( ( * ( first + addingToIndex ) <= 'Z' ) ) ) ) && ( ( ( ( * ( second + addingToIndex ) >= 'a' ) ) && ( ( * ( second + addingToIndex ) <= 'z' ) ) ) || ( ( ( * ( second + addingToIndex ) >= 'A' ) ) && ( ( * ( second + addingToIndex ) <= 'Z' ) ) ) ) )
        if ( ( * ( first + addingToIndex ) + * ( second + addingToIndex ) >= 2 * 'Z' ) && ( * ( first + addingToIndex ) + * ( second + addingToIndex ) <= 2 * 'a' ) )
            {
            /*
            if ( ( * ( first + addingToIndex ) == ' ' ) || ( * ( first + addingToIndex ) == ' ' ) )
                {
                oddSpaces++;
                }
            if ( oddSpaces > 1 )
                {
                return true;
                }
            */
            if (  *( first + addingToIndex ) < *( second + addingToIndex ) )
                {
                return true;
                }
            }
        addingToIndex++;
        }


    return false;
    }












