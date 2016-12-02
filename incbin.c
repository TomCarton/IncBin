//
//  main.c
//  incbin
//
//  Created by Thomas Carton.
//

#include <stdio.h>
#include <string.h>


int main(int argc, const char *argv[])
{
    int ret = 0;

    char outfile[FILENAME_MAX] = "data.c";

    // read parameters
    for (unsigned int i = 0; i < argc; ++i)
    {
        // output
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
        {
            if (argc > i + 1)
            {
                strcpy(outfile, argv[i + 1]);

                memmove(argv + i + 1, argv + i + 2, (argc - i - 2) * sizeof *argv);
                --argc;
                continue;
            }
        }
        // help
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            goto usage;
        }
    }

    char *name = "test";

    // output
    FILE *out = NULL;

    if (!(out = fopen(outfile, "w")))
    {
        fprintf(stderr, "failed to open `%s' for output\n", outfile);
        return 1;
    }

    fprintf(out, "// Generated file\n// %s\n\n", name);
    fprintf(out, "#ifndef %s_h_\n", name);
    fprintf(out, "#define %s_h_\n\n", name);

    fprintf(out, "#endif // %s_h_\n\n", name);

    fclose(out);

    if (ret == 0)
    {
        printf("generated `%s'\n", outfile);
    }
    else
    {
        remove(outfile);
    }

    return ret;

usage:
    fprintf(stderr, "\n");
    fprintf(stderr, " %s path [-h/--help] | [-o/--output output]\n\n", argv[0]);

    fprintf(stderr, "   usage:\n");
    fprintf(stderr, "     -h, --help     - display this\n");
    fprintf(stderr, "     -o, --output   - output file [default is \"data.c\"]\n");

    fprintf(stderr, "   example:\n");
    fprintf(stderr, "     %s icon.png -o icon.c\n", argv[0]);

    fprintf(stderr, "\n");

    return 1;
}

