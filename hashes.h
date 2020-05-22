long hashconst( const char* data, long length )
    {
    return 1;
    }

long hashd( const char* data, long length )
    {
    return data[0];
    }

long hashlen( const char* data, long length )
    {
    return length;
    }

long hashsum( const char* data, long length )
    {
    long res = 0;
    for (long i = 0; i < length; i++)
        {
        res+=data[i];
        }

    return res;
    }

char rol(char a, char n)
    {
    n = n % (sizeof(a)*8);

    return (a << n) | ( a >> (sizeof(a)*8 - n) );
    }

long hashrol( const char* data, long length )
    {
    char res = 0;
    for ( long i = 1; i < length; i++ )
        {
        res = (rol( res, 1))^data[i] ;
        }

    return res;
    }

long hashgnu( const char* data, long length )
    {
    long res = 5381;
    for ( long i = 0; i < length; i++)
        {
        res = res*33+data[i];
        }

    return res;
    }

long hashmurmur2( const char* data, long length )
    {
    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ length;

    const unsigned char * byte = (const unsigned char *)data;
    unsigned int k;

    while (length >= 4)
        {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        byte += 4;
        length -= 4;
        }

    switch (length)
        {
            case 3:
                h ^= byte[2] << 16;
            case 2:
                h ^= byte[1] << 8;
            case 1:
                h ^= byte[0];
                h *= m;
        };

     h ^= h >> 13;
     h *= m;
     h ^= h >> 15;

     return h;
    }