int load_map(char *filename, char dest[100][100], int size_x, int size_y) {
    FILE *f = fopen(filename, "rt");
    if(f == NULL) {
        printf("\n\nCOULDNT OPEN\n\n");
        return 1;
    }

    int read = 0;

    for(int j = 0; j < size_y; ++j) {
        read = fread(dest[j], sizeof(char), size_x, f);
        if(read != size_x) {
            fclose(f);
            return 0;

        }
        dest[j][size_x + 1] = '\0';
    }

    fclose(f);
    return 0;
}   