#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct id3v1 {
    char title[31];
    char artist[31];
    char album[31];
    char year[5];
    char comment[31];
    int track;
    int genre;
};

int read_id3_tag(FILE *file, struct id3v1 *tag) {
    char tagbuf[129];
    memset(tag, 0x00, sizeof(struct id3v1));

    fseek(file, -128, SEEK_END); /* Go to last 128 bytes of the file where ID3v1 tag is stored */
    if (fread(tagbuf, 1, 128, file) != 128)
        return -1;

    if (tagbuf[0] != 'T' || tagbuf[1] != 'A' || tagbuf[2] != 'G')
        return -1;
    
    strncpy(tag->title, tagbuf + 3, 30);
    strncpy(tag->artist, tagbuf + 33, 30);
    strncpy(tag->album, tagbuf + 63, 30);
    strncpy(tag->year, tagbuf + 93, 4);

    /* Check if the track number is stored in the last two bytes of the comment field or not */
    if (!tagbuf[125]) {
        tag->track = tagbuf[126];
        strncpy(tag->comment, tagbuf + 97, 28);
    } else
        strncpy(tag->comment, tagbuf + 97, 30);

    tag->genre = tagbuf[127];

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <.mp3 file>\n", argv[0]);
        exit(0);
    }

    struct id3v1 tag;
    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(-1);
    }

    if (read_id3_tag(file, &tag) < 0) {
        fprintf(stderr, "Error reading id3v1 tag\n");
        exit(-1);
    }

    printf("Title: %s\nArist: %s\nAlbum: %s\nYear: %s\nComment: %s\nTrack: %d\nGenre: %d\n", tag.title, tag.artist, tag.album, tag.year, tag.comment, tag.track, tag.genre);

    fclose(file);
    exit(0);
}
