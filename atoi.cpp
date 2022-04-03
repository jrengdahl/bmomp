extern "C"
int atoi(const char *p)
    {
    int value = 0;

    while('0'<=*p && *p<='9')
        {
        value = value*10 + *p++ - '0';
        }

    return value;
    }
