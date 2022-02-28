#include <common.h>
#include <exports.h>

int hello(int argc, char *const argv[])
    {
    int i;

    /* Print the ABI version */
    app_startup(argv);

    if(XF_VERSION != get_version())
        {
        printf ("Example expects ABI version %d\n", XF_VERSION);
        printf ("Actual U-Boot ABI version %d\n", (int)get_version());
        }

    printf ("hello, world!\n");

    printf ("argc = %d\n", argc);

    for (i=0; i<argc; ++i)
        {
        printf ("argv[%d] = \"%s\"\n", i, argv[i]);
        }

    printf ("Hit any key to exit ... ");
    while (!tstc());

    /* consume input */
    (void) getc();

    printf ("\n\n");
    return (0);
    }
