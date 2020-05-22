//header for work with files

struct line
    {
    char* begin_ptr = nullptr;
    int length = -1;
    };

void buffer_read(char** buffer , long* count_sym, const char file_name[], const int shift, const char open_type[])
    {
    FILE* fileptr = fopen( file_name, open_type);
    fseek( fileptr, 0, SEEK_END);
    *count_sym = ftell(fileptr) + 1 - shift;
    rewind( fileptr );
    fseek( fileptr, shift, SEEK_SET);

    *buffer = (char*) calloc( *count_sym, sizeof( char ));
    fread(*buffer, sizeof(char), *count_sym, fileptr);
    fclose( fileptr );

    assert( *buffer );
    assert( *count_sym >= 0 );
    }

long lines(char buffer[], long count)
    {
   // assert( buffer );
    assert(count >= 0);

    int ans = 0;
    for ( int i = 0; i < count ; i++ )
        {
        if ( (buffer[i])  == '\n')
            {
            ans++;
            }
        }

    assert( ans >= 0 );
    return ans+1;
    }

void change(char* main_buffer, long* length,  char s1, char s2)
    {
    assert( main_buffer);
    assert( length );
    for (int i = 0 ; i < *length; i++)
        {
        if (main_buffer[i] == s1 )
            {
            main_buffer[i] = s2;
            }
        }
    }

void fragmentation (line* array, char* buffer, long count)
    {
    assert( buffer );
    assert( array );

    long i = 0;
    long array_index = 0;
    long previous_slash0 = -1;
    (array+array_index)->begin_ptr = buffer ;

    while (i < count)
        {
        switch ( *(buffer+i) )
                {

                case ';' :
                    {
                        buffer[i] = '\0';
                    (array+array_index)->length = i - previous_slash0;
                    i++;
                    while ( *(buffer+i) != '\0' )
                        {
                        i++;
                        }
                    previous_slash0 = i;
                    if ( i != count-1)
                    {
                        array_index++;
                        (array+array_index)->begin_ptr = (buffer+i+1);
                    }
                    i++;
                    }
                    break;

                case '\0':
                    {
                    (array + array_index)->length = i - previous_slash0;
                    previous_slash0 = i;
                    if ( i != count-1)
                        {

                        array_index++;
                        (array+array_index)->begin_ptr = (buffer+i+1);
                        }
                    i++;
                    }
                    break;

                default :
                    {
                    i++;
                    }
                }

        }

    }