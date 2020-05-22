typedef hashelem_t elem_t;
typedef long index_t;

const size_t default_size_list = 127;
const int MAX_LENGTH_OF_NAME = 16;
const elem_t POISON = { {"3595322", 7 }, "3595322"};
const char directory_name[16] = "../LIST_dump_";
const double MULTIPLIER_FOR_MEMORY_INCREASE = 2.0;

#define listdef(name) list_t name = { #name };

#define for_listdump __FILE__,__PRETTY_FUNCTION__,__LINE__

struct list_t
{
    const char name[MAX_LENGTH_OF_NAME] = "";
    elem_t* data = nullptr;
    index_t * next = nullptr;
    index_t * prev = nullptr;
    size_t head = 0;
    size_t tail = 0;
    size_t free = 0;
    size_t size = 0;
    size_t max_size = 0;
};

bool str_comp( elem_t str1, elem_t str2 )
{
    if ( strcmp(str1.val.word, str2.val.word) )
    {
        return false;
    }
    return true;
}

void list_initial( list_t* list )
{
    assert( list );

    list->data = nullptr;
    list->next = nullptr;
    list->prev = nullptr;
    list->head = 0;
    list->tail = 0;
    list->free = 0;
    list->size = 0;
    list->max_size = 0;
}

bool list_verify( const list_t* list )
{
    assert( list );

    if ( (list->data == nullptr) || (list->next == nullptr) || (list->prev == nullptr) )
    {
        return false;
    }
    if ( (list->data == (elem_t*)list->next) || (list->next == list->prev) || ((elem_t*)list->prev == list->data) )
    {
        return false;
    }

    if ( (list->size > list->max_size) || ( list->size < 0 ) || ( list->max_size < 0 ) )
    {
        return false;
    }

    if (  (list->head == list->free) || (list->tail == list->free) )
    {
        return false;
    }

    size_t i = list->free;

    while ( i != 0  )
    {
        if ( list->prev[i] != -1 )
        {
            return false;
        }
        i = list->next[i];
    }

    i = list->head;

    while ( i != list->tail )
    {
        if ( i != list->prev[ list->next[i] ] )
        {
            return false;
        }
        i = list->next[i];
    }

    i = list->tail;

    while ( i != list->head )
    {
        if ( i != list->next[ list->prev[i] ] )
        {
            return false;
        }
        i = list->prev[i];
    }

    return true;
}

void list_dot_dump( const list_t* list, bool open = false )
{
    assert( list );

    char dot_name[2*MAX_LENGTH_OF_NAME] = "";
    strcat( strcat( dot_name, directory_name), "dot_" );
    strcat( dot_name, list->name );
    FILE* dot = fopen( dot_name, "w" );
    fprintf( dot, "digraph G\n{\nrankdir = LR;\nnode[ color = \"goldenrod1\",fontsize = 10, shape = \"rect\" ];\n" );
    fprintf( dot, "LIST[ shape = record, label = \" name:%s | { data:[%p] | next:[%p] | prev:[%p] } | \n"
                  "{ head:%lu | tail:%lu | free:%lu } | { size:%lu | max_size:%lu }\" ];\n",
             list->name, list->data, list->next, list->prev, list->head, list->tail, list->free, list->size, list->max_size );
    long phys = list->head;
    size_t elem = 1;
    while ( phys != 0)
    {
        fprintf( dot, "LIST%lu[ shape = record, label = \" %s | phys:%ld | { next:%ld | prev:%ld } \" ]; ", elem, (list->data+phys)->val.word,
                 phys, *(list->next+phys), *(list->prev+phys) );
        if ( elem != 1 )
        {
            fprintf( dot, "LIST%lu->LIST%lu[ color = \"blue\" ];", elem-1, elem );
            fprintf( dot, "LIST%lu->LIST%lu[ color = \"red\" ];", elem, elem-1 );
        }
        phys = list->next[phys];
        elem++;
    }
    fprintf( dot, "}" );
    fclose(dot);
    char dot_out[4*MAX_LENGTH_OF_NAME] = "dot -Tps ";
    strcat( strcat( dot_out, dot_name ), " -o ");
    strcat( strcat( dot_out, dot_name ), ".PDF");
    system(dot_out);
    if ( open )
    {
        strcpy(dot_out, "gio open ");
        strcat(strcat(dot_out, dot_name), ".PDF");
        system(dot_out);
    }
}

void list_dump ( const list_t* list, const char s1[] = "", const char s2[] = "", int line = 0 )
{
    assert( list );

    char file_name[strlen(directory_name)+MAX_LENGTH_OF_NAME+4] = "";
    strcat( file_name,directory_name );
    strcat( file_name, list->name );
    strcat( file_name, ".txt" );

    FILE* dump = fopen( file_name, "w");

    fprintf( dump, "NAME OF LIST: %s\n", list->name );
    fprintf( dump, "In file %s | In function %s | line %d\n", s1, s2, line);
    fprintf( dump, "current size %ld; max size %ld;\n", list->size, list->max_size );
    fprintf( dump, "data array [%p]; next array [%p]; prev array [%p];\n", list->data, list->next, list->prev );
    fprintf( dump, "head %ld; tail %ld; free %ld;\n", list->head, list->tail, list->free );
    fprintf( dump, "                 data       next       prev\n");
    for (size_t i = 0; i < list->max_size+1; ++i)
    {
        if ( list->prev[i] == -1 )
        {
            fprintf( dump, "free list[%-5ld] ", i);
        }
        else
        {
            fprintf( dump, "used list[%-5ld] ", i);
        }
        fprintf( dump, "%-10s %-10ld %-10ld\n", (list->data)->val.word, list->next[i], list->prev[i] );
    }

    fclose(dump);
}

bool list_create ( list_t* list, size_t size = default_size_list )
{
    assert( list );
    assert( size > 0 );

    list_initial(list);
    list->data = (elem_t*) calloc( size+1, sizeof( elem_t ) );
    list->next = (index_t*) calloc( size+1, sizeof(index_t) );
    list->prev = (index_t*) calloc( size+1, sizeof(index_t) );
    list->max_size = size;
    for (int i = 0; i < list->max_size+1; i++)
    {
        list->data[i] = POISON ;
    }
    list->next[0] = 0;
    for (int i = 1; i < list->max_size; i++)
    {
        list->next[i] = i+1;
    }
    list->next[list->max_size] = 0;
    list->prev[0] = 0;
    for (int i = 1; i < list->max_size+1; i++)
    {
        list->prev[i] = -1;
    }
    list->free = 1;

    assert( list_verify(list) );
    return true;
}

void list_delete ( list_t* list )
{
    assert( list );
    free( list->data );
    free( list->next );
    free( list->prev );
    list_initial( list );

}

bool list_memory( list_t* list )
{
    if ( list->size == list->max_size )
    {
        list->data = (elem_t *) realloc(list->data,(1+round(MULTIPLIER_FOR_MEMORY_INCREASE * list->max_size)) * sizeof(elem_t));
        list->next = (index_t *) realloc(list->next,(1+round(MULTIPLIER_FOR_MEMORY_INCREASE * list->max_size)) * sizeof(index_t *));
        list->prev = (index_t *) realloc(list->prev,(1+round(MULTIPLIER_FOR_MEMORY_INCREASE * list->max_size)) * sizeof(index_t *));
        if (!(list->data && list->next && list->prev))
        {
            return false;
        }
        list->free = list->max_size+1;
        list->next[list->max_size] = list->max_size+1;
        list->max_size = round(MULTIPLIER_FOR_MEMORY_INCREASE * list->max_size);
        for (size_t i = list->size+1; i <= list->max_size ; i++ )
        {
            list->data[i] = POISON;
            list->prev[i] = -1;
            list->next[i] = i+1;
        }
        list->next[list->max_size] = 0;
    }
    return true;
}

size_t list_first_elem( list_t* list, elem_t elem )
{
    assert( list_verify(list) );

    list->data[list->free] = elem;
    list->head = list->free;
    list->tail = list->free;
    list->free = list->next[list->free];
    list->size++;
    list->prev[list->head] = 0;
    list->next[list->head] = 0;

    assert( list_verify(list) );
    return list->head;
}

size_t list_head_insert( list_t* list, elem_t elem )
{
    assert( list_verify(list) );
    if ( list->size == 0)
    {
        return list_first_elem( list, elem );
    }
    size_t temp = list->head;
    list->data[list->free] = elem;
    list->prev[list->head] = list->free;
    list->prev[list->free] = 0;
    list->head = list->prev[list->head];
    list->free = list->next[list->free];
    list->next[ list->head ] = temp;
    list->size++;
    list_memory( list );

    assert( list_verify(list) );
    return list->head;
}

size_t list_append( list_t* list, elem_t elem )
{
    assert( list_verify(list) );
    if ( list->size == 0)
    {
        return list_first_elem( list, elem );
    }

    size_t temp = list->tail;
    list->tail = list->free;
    list->data[list->tail] = elem;
    list->free = list->next[list->free];
    list->next[list->tail] = 0;
    list->prev[list->tail] = temp;
    list->next[temp] = list->tail;
    list->size++;
    list_memory(list);

    assert( list_verify(list) );
    return  list->tail;
}

size_t list_logicaltophysical( const list_t* list, size_t number )
{
    assert( list_verify(list) );

    size_t phys = list->head;
    for (size_t i = 1; i < number; i++)
    {
        phys = list->next[phys];
    }
    return  phys;
}

size_t list_insert_before( list_t* list, size_t phys, elem_t elem )
{
    assert( list_verify(list) );
    if ( phys == list->head )
    {
        return list_head_insert( list, elem );
    }

    list->data[list->free] = elem;
    list->prev[list->free] = list->prev[phys];
    list->next[ list->prev[phys] ] = list->free;
    list->prev[phys] = list->free;
    list->free = list->next[list->free];
    list->next[ list->prev[phys] ] = phys;
    list->size++;
    list_memory(list);

    assert( list_verify(list) );
    return list->prev[phys];
}

size_t list_insert_after( list_t* list, size_t phys, elem_t elem )
{
    assert( list_verify(list) );
    if ( phys == list->tail )
    {
        return list_append( list, elem );
    }

    return list_insert_before( list, list->next[phys], elem );
}

void list_delete_only( list_t* list)
{
    assert( list_verify(list) );

    list->data[list->head] = POISON;
    list->prev[list->head] = -1;
    list->next[list->head] = list->free;
    list->free = list->head;
    list->head = 0;
    list->tail = 0;
    list->size--;

    assert( list_verify(list) );
}

void list_delete_head( list_t* list)
{
    assert( list_verify(list) );
    if ( list->size == 1 )
    {
        return list_delete_only(list);
    }

    list->data[list->head] = POISON;
    list->prev[list->head] = -1;
    size_t temp = list->head;
    list->head = list->next[list->head];
    list->next[temp] = list->free;
    list->prev[list->head] = 0;
    list->free = temp;
    list->size--;

    assert( list_verify(list)  );
}

void list_delete_tail ( list_t* list)
{
    assert( list_verify(list) );
    if ( list->size == 1 )
    {
        return list_delete_only(list);
    }

    list->data[list->tail] = POISON;
    size_t temp = list->tail;
    list->tail = list->prev[list->tail];
    list->prev[temp] = -1;
    list->next[temp] = list->free;
    list->next[list->tail] = 0;
    list->free = temp;
    list->size--;

    assert( list_verify(list) );
}

void list_delete_bypos( list_t* list, size_t phys )
{
    assert( list_verify(list) );

    if ( phys == list->head )
    {
        return list_delete_head( list );
    }
    if ( phys == list->tail )
    {
        return list_delete_tail(list);
    }
    list->next[ list->prev[phys] ] = list->next[phys];
    list->prev[ list->next[phys] ] = list->prev[phys];
    list->data[phys] = POISON;
    list->prev[phys] = -1;
    list->next[phys] = list->free;
    list->free = phys;
    list->size--;

    assert( list_verify(list) );
}

void list_delete_before( list_t* list, size_t phys )
{
    assert( list_verify(list) );
    if ( phys == list->head )
    {
        return ;
    }
    return list_delete_bypos( list, list->prev[phys] );
}

void list_delete_after( list_t* list, size_t phys )
{
    assert( list_verify(list) );
    if ( phys == list->tail )
    {
        return ;
    }
    return list_delete_bypos( list, list->next[phys] );
}

size_t list_find_posbyelem( const list_t* list, elem_t elem)
{
    assert( list_verify(list) );

    size_t ind = 0;
    size_t i = list->head ;
    while ( i != 0 )
    {
        if ( str_comp( elem,list->data[i] ) )
        {
            ind = i;
            break;
        }
        i = list->next[i];
    }

    return ind ;
}

size_t list_find_posbykey( const list_t* list, key_type key)
{
    assert( list_verify(list) );

    size_t ind = 0;
    size_t i = list->head ;
    while ( i != 0 )
    {
        if ( !strcmp( key.key_str,list->data[i].key.key_str ) )
        {
            ind = i;
            break;
        }
        i = list->next[i];
    }

    return ind ;
}

elem_t list_find_bykey( const list_t* list, key_type key )
{
    assert( list_verify(list) );

    size_t ind = 0;
    size_t i = list->head ;
    while ( i != 0 )
    {
        if ( !strcmp(key.key_str, list->data[i].key.key_str ) )
        {
            ind = i;
            break;
        }
        i = list->next[i];
    }

    return list->data[i];
}