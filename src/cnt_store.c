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

// static PMEMobjpool *pop;

struct content{
    int ino; //inode id
    char cont[MAX_CONTENT_LEN];
    size_t size; //length of the content
};

//find the content struct by the inode number
TOID(struct content) find_content(PMEMobjpool* pop, const int id){
    TOID(struct content) ret;
    POBJ_FOREACH_TYPE(pop, ret){
        if(D_RO(ret)->ino == id){
            return ret;
        }
    }
    return TOID_NULL(struct content);
}

int create_content(PMEMobjpool*pop, size_t size, char* buf, const int id){
    TOID(struct content) cnt = find_content(pop, id);
    if(!TOID_IS_NULL(cnt)){
        POBJ_FREE(&cnt);
    }
    POBJ_ZNEW(pop, &cnt, struct content);
    struct content* cnt_dir = D_RW(cnt);
    cnt_dir->ino = id;
    cnt_dir->size = size;
    pmemobj_memcpy_persist(pop, cnt_dir->cont, buf, size);
    pmemobj_persist(pop, cnt_dir, sizeof(*cnt_dir));
    return 0;
}

int write_at_content(PMEMobjpool* pop, const int id, const int offset, char* buf, const int size){
    TOID(struct content) cnt = find_content(pop, id);
    if(TOID_IS_NULL(cnt)){
        return create_content(pop, size, buf, id);
    }
    int new_size = offset+size > (int)D_RO(cnt)->size? offset+size: (int)D_RO(cnt)->size;
    pmemobj_persist(pop, &D_RW(cnt)->size, new_size);
    pmemobj_memcpy_persist(pop, D_RW(cnt)->cont+offset, buf, size);
    pmemobj_persist(pop, D_RW(cnt), sizeof(*D_RW(cnt)));
    return 0;
}

int write_content(PMEMobjpool * pop, const int id, char* buf, const int size){
    return write_at_content(pop, id, 0, buf, size);
}

int read_at_content(PMEMobjpool *pop, const int id, const int offset, char* buf){
    TOID(struct content) cnt = find_content(pop, id);
    if(TOID_IS_NULL(cnt)){
        printf("not found\n");
        return -2;
    }
    if(offset> (int)D_RW(cnt)->size){
        return -3;
    }
    sprintf(buf, "%s", D_RW(cnt)->cont+offset);
    // memcpy(buf, D_RW(cnt)->cont+offset, D_RW(cnt)->size-offset);
    return 0;
}

int read_content(PMEMobjpool* pop, const int id, char* buf){
    return read_at_content(pop, id, 0, buf);
}


void print_content(PMEMobjpool* pop, const int id){
    TOID(struct content) ret = find_content(pop, id);
    if(TOID_IS_NULL(ret)){
        printf("not found\n");
        return;
    }
    printf("ino: %d\ncontent: %s\n", D_RW(ret)->ino, D_RW(ret)->cont);
}

void print_content_all(PMEMobjpool* pop){
    TOID(struct content) cnt;
    POBJ_FOREACH_TYPE(pop, cnt){
        struct content* cnt_dir = D_RW(cnt);
        printf("ino: %d\ncontent: %s\n", cnt_dir->ino, cnt_dir->cont);
    }
}

void my_free_fn(PMEMobjpool* pop, const int id){
    TOID(struct content) ret = find_content(pop, id);
    if(TOID_IS_NULL(ret)){
        printf("not found\n");
        return ;
    }
    POBJ_FREE(&ret);
}

void my_free_all_fn(PMEMobjpool* pop){
    TOID(struct content) cnt;
    POBJ_FOREACH_TYPE(pop, cnt){
        my_free_fn(pop, D_RW(cnt)->ino);
    }
}

PMEMobjpool* init(const char *path){
    POBJ_LAYOUT_BEGIN(array); 
        POBJ_LAYOUT_TOID(array, struct content);
    POBJ_LAYOUT_END(array);
    static PMEMobjpool* pop = NULL;
    if((pop = pmemobj_create(path, POBJ_LAYOUT_NAME(array), PMEMOBJ_MIN_POOL,0666))==NULL){
        if((pop = pmemobj_open(path, POBJ_LAYOUT_NAME(array)))==NULL){
            printf("fail to open\n");
            return NULL;
        }
    }
    return pop;    
}

void fin(PMEMobjpool* pop){
    pmemobj_close(pop);
}

// int main(int argc, char* argv[]){
//     const char* path = argv[1];
//     static PMEMobjpool* pop = NULL;
//     if((pop = pmemobj_create(path, POBJ_LAYOUT_NAME(array), PMEMOBJ_MIN_POOL,0666))==NULL){
//         if((pop = pmemobj_open(path, POBJ_LAYOUT_NAME(array)))==NULL){
//             printf("fail to open\n");
//             return 1;
//         }
//     }
//     const char* option = argv[2];
//     //if create: ./a.out <file_name> create <size> <content> <id>
//     if(strcmp(option, "create")==0){
//         size_t size = atoi(argv[3]);
//         char* buf;
//         buf = argv[4];
//         const int id = atoi(argv[5]);
//         create_content(pop, size, buf, id);
//     }
//     //if print: ./a.out <file_name> print <id>
//     else if(strcmp(option, "print")==0){
//         const int id = atoi(argv[3]);
//         print_content(pop, id);
//     }
//     //if print_all: ./a.out <file_name> print_all
//     else if(strcmp(option, "print_all")==0){
//         print_content_all(pop);
//     }
//     else if(strcmp(option, "free")==0){
//         const int id = atoi(argv[3]);
//         my_free(pop, id);
//     }
//     else if(strcmp(option, "free_all")==0){
//         my_free_all(pop);
//     }
//     //if read: ./a.out <file_name> read <id>
//     else if(strcmp(option, "read")==0){
//         const int id = atoi(argv[3]);
//         char buf[MAX_CONTENT_LEN];
//         if(read_content(pop, id, buf)<0) return -1;
//         printf("%s\n", buf);
//     }
//     //if read_at: ./a.out <file_name> read_at <id> <offset>
//     else if (strcmp(option, "read_at")==0){
//         const int id = atoi(argv[3]);
//         const int offset = atoi(argv[4]);
//         char buf[MAX_CONTENT_LEN];
//         read_at_content(pop, id, offset, buf);
//         printf("%s\n", buf);
//     }
//     //if write: ./a.out <file_name> write <id> <content> <size>
//     else if (strcmp(option, "write")==0){
//         const int id = atoi(argv[3]);
//         char *buf = argv[4];
//         size_t size = atoi(argv[5]);
//         write_content(pop, id, buf, size);
//     }
//     //if write_at: ./a.out <file_name> write_at <id> <offset> <content> <size>
//     else if(strcmp(option, "write_at")==0){
//         const int id = atoi(argv[3]);
//         const int offset = atoi(argv[4]);
//         char* buf = argv[5];
//         size_t size = atoi(argv[6]);
//         write_at_content(pop, id, offset, buf, size);
//     }
//     pmemobj_close(pop);
//     return 0;
// }



