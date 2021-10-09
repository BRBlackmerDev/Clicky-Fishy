
struct FILE_DATA {
    uint8 * contents;
    uint32  size;
};

typedef FILE_DATA ( READ_FILE  )( MEMORY * memory, const char * saveDir, const char * filename );
typedef boo32     ( WRITE_FILE )( const char * saveDir, const char * filename, void * data, uint32 size );