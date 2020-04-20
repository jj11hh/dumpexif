#include <stdio.h>
#include <netinet/in.h>

#define TAG_SOI 0xFFD8
#define TAG_APP0 0xFFE0
#define TAG_APP1 0xFFE1
#define TAG_SOS 0xFFDA
#define TAG_EOI 0xFFD9

int main (int argc, char * argv[]) {
    char seg[65535 + 2];
    uint16_t seg_size = 0;
    uint16_t tag = 0;
    if (argc != 3){
        fprintf(stderr, "usage: %s IMAGE.jpg STRIP.jpg\n", argv[0]);
        return -1;
    }

    FILE* jpegfile = fopen(argv[1], "r");
    if (! jpegfile){
        perror("Cannot open image");
        return -1;
    }

    if (fread((void*)&tag, sizeof(uint16_t), 1, jpegfile) == 0){
        perror("Cannot read tag");
        return -1;
    }
    tag = ntohs(tag);

    fprintf(stderr, "read tag = %4x\n", (int)tag);

    if (tag != TAG_SOI){
        fprintf(stderr, "SOI tag not found\n");
        return -1;
    } 

    FILE* stripfile = fopen(argv[2], "w");
    if (! stripfile){
        perror("Cannot open file");
        return -1;
    }
    
    uint16_t soi = htons(tag);
    fwrite(&soi, sizeof(uint16_t), 1, stripfile);
    
    while (! feof(jpegfile)) {
        if (fread((void*)&tag, sizeof(uint16_t), 1, jpegfile) == 0){
            perror("Cannot read tag");
            return -1;
        }
        tag = ntohs(tag);
        
        fprintf(stderr, "read tag = %4x\n", (int)tag);
        if (tag == TAG_SOS){
            fprintf(stderr, "read SOS\n");

            tag = htons(tag);
            fwrite(&tag, sizeof(uint16_t), 1, stripfile);

            char buffer[4096];
            size_t len = 0;
            while (! feof(jpegfile)){
                len = fread(buffer, sizeof(char), 4096, jpegfile);
                fwrite(buffer, sizeof(char), len, stripfile);
            }
            return 0;
        }
        if (fread((void*)&seg_size, sizeof(uint16_t), 1, jpegfile) == 0){
            perror("Cannot read seg size");
            return -1;
        }
        seg_size = ntohs(seg_size);
        fprintf(stderr, "seg length = %d\n", (int) seg_size);

        if ((tag & 0xFFF0) == 0xFFE0) { // is a APPx marker
            fprintf(stderr, "skip tag = %4x\n", (int)tag);
            fseek(jpegfile, seg_size - 2, SEEK_CUR);
        }
        else {
            fprintf(stderr, "pick tag = %4x\n", (int)tag);
            fseek(jpegfile, -4, SEEK_CUR); // seek back
            size_t len = fread((void *)seg, sizeof(char), seg_size + 2, jpegfile);
            fwrite((const void *)seg, sizeof(char), len, stripfile);
        }
    }

    fclose(jpegfile);
    fclose(stripfile);

    return 0;
}
