#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "files.h"
#include "hashes.h"

struct value_t
    {
    char word[32] = "";
    };

struct key_type
    {
    char key_str[32] = "";
    long length = 0;
    };

struct hashelem_t
    {
    key_type key;
    value_t val;
    };

#include "list.h"

const size_t default_size_hashtbl = 31;
const char source_name[] = "../words.txt";
const int hash_count = 7;

struct hashtbl_t
    {
    long (*hfunc)( const char*, long ) = nullptr;
    size_t size = 0;
    list_t* list = nullptr;
    };

bool hashtbl_create( hashtbl_t* hashtbl, long (*hashfunc)( const char *, long), size_t size = default_size_hashtbl )
    {
    assert( hashtbl && hashfunc );

    hashtbl->hfunc = hashfunc;
    hashtbl->size = size;
    hashtbl->list = (list_t*)calloc( size, sizeof(list_t) );
    for (size_t i = 0; i < size; i++)
        {
        list_create( hashtbl->list+i );
        }
    return true;
    }

bool hashtbl_delete( hashtbl_t* hashtbl )
    {
    assert( hashtbl );

    for (size_t i = 0; i < hashtbl->size ; i++)
        {
        list_delete(hashtbl->list+i);
        }
    free( hashtbl->list );
    hashtbl->list = nullptr;
    hashtbl->size = 0;
    hashtbl->hfunc = nullptr;
    return true;
    }

void hashtbl_add( hashtbl_t* tabel, key_type* key, value_t* value )
    {
    assert( tabel && key && value );

    hashelem_t elem = { *key, *value };
    list_append( tabel->list+( tabel->hfunc( key->key_str, key->length  ) % tabel->size ), elem );
    }

value_t hashtbl_findbykey( const hashtbl_t* tabel, key_type key )
    {
    assert( tabel );

    return list_find_bykey( tabel->list+( tabel->hfunc( key.key_str, key.length  ) % tabel->size ), key ).val;
    }

void hashtbl_delete_bykey(  hashtbl_t* tabel, key_type key )
    {
    assert( tabel );

    list_t* list = tabel->list+( tabel->hfunc( key.key_str, key.length  ) % tabel->size );
    list_delete_bypos( list, list_find_posbykey(list, key) );
    }

void hashtbl_arrayinit( hashtbl_t* array )
    {
    assert(array);

    hashtbl_create( array+0 , hashconst );
    hashtbl_create( array+1 , hashd );
    hashtbl_create( array+2 , hashlen );
    hashtbl_create( array+3 , hashsum );
    hashtbl_create( array+4 , hashrol );
    hashtbl_create( array+5 , hashgnu );
    hashtbl_create( array+6 , hashmurmur2 );
    }

void csvcreate( const hashtbl_t* array )
    {
    assert( array );

    FILE* out = fopen( "../out.csv" , "w");
    value_t values[7] = { "const", "hashd", "length", "sum", "rol", "gnu", "murmur2" };
    for ( int i = 0; i < 7; i++)
        {
        fprintf( out, "%s; ", values[i].word );
        for (size_t j = 0; j < (array+i)->size; j++ )
            {
            fprintf( out, "%lu; ", ((array+i)->list+j)->size );
            }
        fprintf( out, "\n" );
        }

    fclose(out);
    }

int main()
    {
    char* buffer = nullptr;
    long count_sym = -1;
    buffer_read( (char**) &buffer, &count_sym, source_name, 0, "rt" );
    long line_count = lines( buffer, count_sym);
    line* strings = nullptr;
    strings = (line*) calloc(line_count, sizeof(line));
    change(buffer,&count_sym, '\n','\0');
    fragmentation( strings, buffer, count_sym );

    value_t val;
    key_type key;
    hashtbl_t hasharray[7];
    hashtbl_arrayinit( hasharray );

    for (long i = 0; i < line_count; i++ )
        {
        strcpy(val.word, strings[i].begin_ptr);
        strcpy(key.key_str, strings[i].begin_ptr);
        key.length = strings[i].length-1;
        for ( int j = 0; j < 7; j++)
            {
            hashtbl_add( hasharray+j, &key, &val );
            }
        }

    csvcreate( hasharray );
    printf("%ld", line_count);

    free(buffer);
    free(strings);
    for ( int i = 0; i < 7; i++)
        {
        hashtbl_delete(hasharray+i);
        }
    return 0;
    }