#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <libpmemobj.h>

#define MAX_CONTENT_LEN 50

POBJ_LAYOUT_BEGIN(array); 
    POBJ_LAYOUT_TOID(array, struct content);
POBJ_LAYOUT_END(array);

static PMEMobjpool *pop;

enum Filetype{
    Directory,
    Regular
};

// struct metadata{
//     int ino;
//     size_t size;
//     int blocks;
//     time_t mtime;
//     time_t ctime;
//     enum Filetype ft;
// };

struct content{
    int ino; //inode id
    size_t size;
    time_t mtime;
    time_t ctime;
    enum Filetype ft;
};

//find the content struct by the inode number
static TOID(struct content) find_metadata(const int id){
    TOID(struct content) ret;
    POBJ_FOREACH_TYPE(pop, ret){
        if(D_RO(ret)->ino == id){
            return ret;
        }
    }
    return TOID_NULL(struct content);
}

static int create_metadata(
    const int id,
    size_t size,
    enum Filetype ft
    ){
    TOID(struct content) cnt = find_metadata(id);
    if(!TOID_IS_NULL(cnt)){
        POBJ_FREE(&cnt);
    }
    POBJ_ZNEW(pop, &cnt, struct content);
    struct content* cnt_dir = D_RW(cnt);
    cnt_dir->ino = id;
    cnt_dir->size = size;
    cnt_dir->ft = ft;
    cnt_dir->mtime = time(NULL);
    cnt_dir->ctime = time(NULL);
    pmemobj_persist(pop, cnt_dir, sizeof(*cnt_dir));
    return 0;
}

static void print_metadata(const int id){
    TOID(struct content) ret = find_metadata(id);
    if(TOID_IS_NULL(ret)){
        printf("not found\n");
        return;
    }
    printf("ino: %d\nsize: %ld\nmtime: %ld\nctime: %ld\n", D_RW(ret)->ino, D_RW(ret)->size, D_RW(ret)->ctime, D_RW(ret)->ctime);
}

static void print_content_all(){
    TOID(struct content) cnt;
    POBJ_FOREACH_TYPE(pop, cnt){
        printf("ino: %d\nsize: %ld\nmtime: %ld\nctime: %ld\n", D_RW(cnt)->ino, D_RW(cnt)->size, D_RW(cnt)->ctime, D_RW(cnt)->ctime);
    }
}

static void my_free(const int id){
    TOID(struct content) ret = find_metadata(id);
    if(TOID_IS_NULL(ret)){
        printf("not found\n");
        return ;
    }
    POBJ_FREE(&ret);
}

static void my_free_all(){
    TOID(struct content) cnt;
    POBJ_FOREACH_TYPE(pop, cnt){
        my_free(D_RW(cnt)->ino);
    }
}

int main(int argc, char* argv[]){
    const char* path = argv[1];
    pop = NULL;
    if((pop = pmemobj_create(path, POBJ_LAYOUT_NAME(array), PMEMOBJ_MIN_POOL,0666))==NULL){
        if((pop = pmemobj_open(path, POBJ_LAYOUT_NAME(array)))==NULL){
            printf("fail to open\n");
            return 1;
        }
    }
    const char* option = argv[2];
    //if create: ./a.out <file_name> create <id> <size> <ft>
    if(strcmp(option, "create")==0){
        const int id = atoi(argv[3]);
        size_t size = atoi(argv[4]);
        enum Filetype ft = atoi(argv[5]);
        create_metadata(id, size, ft);
    }
    //if print: ./a.out <file_name> print <id>
    else if(strcmp(option, "print")==0){
        const int id = atoi(argv[3]);
        print_metadata(id);
    }
    //if print_all: ./a.out <file_name> print_all
    else if(strcmp(option, "print_all")==0){
        print_content_all();
    }
    else if(strcmp(option, "free")==0){
        const int id = atoi(argv[3]);
        my_free(id);
    }
    else if(strcmp(option, "free_all")==0){
        my_free_all();
    }
    pmemobj_close(pop);
    return 0;
}



