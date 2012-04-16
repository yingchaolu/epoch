#ifndef _SDF_COMMON_H_
#define _SDF_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef PARALLEL
#include <mpi.h>
#endif

#ifdef SDF_DEBUG_ALL
#define SDF_DEBUG
#endif

#define SDF_MAXDIMS 4
#define SDF_ID_LENGTH 32
#define SDF_HEADER_LENGTH (11 * 4 + 2 * 8 + 8 + 12 + SDF_ID_LENGTH)
#define SDF_ENDIANNESS 16911887

#define SDF_VERSION  1
#define SDF_REVISION 0

#define SDF_MAGIC "SDF1"

#ifdef __cplusplus
extern "C" {
#endif

enum sdf_blocktype {
    SDF_BLOCKTYPE_SCRUBBED = -1,
    SDF_BLOCKTYPE_NULL = 0,
    SDF_BLOCKTYPE_PLAIN_MESH,
    SDF_BLOCKTYPE_POINT_MESH,
    SDF_BLOCKTYPE_PLAIN_VARIABLE,
    SDF_BLOCKTYPE_POINT_VARIABLE,
    SDF_BLOCKTYPE_CONSTANT,
    SDF_BLOCKTYPE_ARRAY,
    SDF_BLOCKTYPE_RUN_INFO,
    SDF_BLOCKTYPE_SOURCE,
    SDF_BLOCKTYPE_STITCHED_TENSOR,
    SDF_BLOCKTYPE_STITCHED_MATERIAL,
    SDF_BLOCKTYPE_STITCHED_MATVAR,
    SDF_BLOCKTYPE_STITCHED_SPECIES,
    SDF_BLOCKTYPE_SPECIES,
    SDF_BLOCKTYPE_PLAIN_DERIVED,
    SDF_BLOCKTYPE_POINT_DERIVED,
};

enum sdf_geometry {
    SDF_GEOMETRY_NULL = 0,
    SDF_GEOMETRY_CARTESIAN,
    SDF_GEOMETRY_CYLINDRICAL,
    SDF_GEOMETRY_SPHERICAL,
};

enum sdf_stagger {
    SDF_STAGGER_CELL_CENTRE = 0,
    SDF_STAGGER_FACE_X,
    SDF_STAGGER_FACE_Y,
    SDF_STAGGER_EDGE_Z,
    SDF_STAGGER_FACE_Z,
    SDF_STAGGER_EDGE_Y,
    SDF_STAGGER_EDGE_X,
    SDF_STAGGER_VERTEX,
};

enum sdf_datatype {
    SDF_DATATYPE_NULL = 0,
    SDF_DATATYPE_INTEGER4,
    SDF_DATATYPE_INTEGER8,
    SDF_DATATYPE_REAL4,
    SDF_DATATYPE_REAL8,
    SDF_DATATYPE_REAL16,
    SDF_DATATYPE_CHARACTER,
    SDF_DATATYPE_LOGICAL,
    SDF_DATATYPE_OTHER,
};

enum sdf_dimension {
    SDF_DIMENSION_IRRELEVANT = 0,
    SDF_DIMENSION_1D,
    SDF_DIMENSION_2D,
    SDF_DIMENSION_3D,
};

static const char *sdf_blocktype_c[] = {
    "SDF_BLOCKTYPE_NULL",
    "SDF_BLOCKTYPE_PLAIN_MESH",
    "SDF_BLOCKTYPE_POINT_MESH",
    "SDF_BLOCKTYPE_PLAIN_VARIABLE",
    "SDF_BLOCKTYPE_POINT_VARIABLE",
    "SDF_BLOCKTYPE_CONSTANT",
    "SDF_BLOCKTYPE_ARRAY",
    "SDF_BLOCKTYPE_RUN_INFO",
    "SDF_BLOCKTYPE_SOURCE",
    "SDF_BLOCKTYPE_STITCHED_TENSOR",
    "SDF_BLOCKTYPE_STITCHED_MATERIAL",
    "SDF_BLOCKTYPE_STITCHED_MATVAR",
    "SDF_BLOCKTYPE_STITCHED_SPECIES",
    "SDF_BLOCKTYPE_SPECIES",
    "SDF_BLOCKTYPE_PLAIN_DERIVED",
    "SDF_BLOCKTYPE_POINT_DERIVED",
};

static const char *sdf_geometry_c[] = {
    "SDF_GEOMETRY_NULL",
    "SDF_GEOMETRY_CARTESIAN",
    "SDF_GEOMETRY_CYLINDRICAL",
    "SDF_GEOMETRY_SPHERICAL",
};

static const char *sdf_stagger_c[] = {
    "SDF_STAGGER_CELL_CENTRE",
    "SDF_STAGGER_FACE_X",
    "SDF_STAGGER_FACE_Y",
    "SDF_STAGGER_EDGE_Z",
    "SDF_STAGGER_FACE_Z",
    "SDF_STAGGER_EDGE_Y",
    "SDF_STAGGER_EDGE_X",
    "SDF_STAGGER_VERTEX",
};

static const char *sdf_datatype_c[] = {
    "SDF_DATATYPE_NULL",
    "SDF_DATATYPE_INTEGER4",
    "SDF_DATATYPE_INTEGER8",
    "SDF_DATATYPE_REAL4",
    "SDF_DATATYPE_REAL8",
    "SDF_DATATYPE_REAL16",
    "SDF_DATATYPE_CHARACTER",
    "SDF_DATATYPE_LOGICAL",
    "SDF_DATATYPE_OTHER",
};

#ifdef PARALLEL
    typedef MPI_Comm comm_t;
#else
    typedef int comm_t;
#endif

typedef struct sdf_block sdf_block_t;
typedef struct sdf_file sdf_file_t;

struct sdf_block {
    double *dim_mults, *extents, mult;
    uint64_t block_start;
    uint64_t next_block_location, data_location;
    uint64_t nelements, npoints, data_length, info_length;
    uint32_t ndims, geometry, datatype, blocktype;
    uint32_t type_size, stagger, datatype_out, type_size_out;
    uint32_t *dims_in;
    uint64_t dims[3];
    int local_dims[3], nm, nlocal;
    char const_value[16];
    char *id, *units, *mesh_id, *material_id;
    char *name, *material_name;
    char **dim_labels, **dim_units;
    char **variable_ids, **material_names;
    char done_header, done_info, done_data;
    void **grids, *data;
    sdf_block_t *next;
    sdf_block_t *(*populate_data)(sdf_file_t *, sdf_block_t *);
#ifdef PARALLEL
    MPI_Datatype mpitype, distribution, mpitype_out;
    int cart_rank, cpu_split[SDF_MAXDIMS];
    int coordinates[3], proc_min[3], proc_max[3];
    MPI_Comm cart_comm;
#endif
};

struct sdf_file {
    double time;
    uint64_t first_block_location, summary_location, start_location, soi, sof;
    uint64_t current_location;
    uint32_t jobid1, jobid2, endianness, summary_size;
    uint32_t block_header_length, string_length, nblocks;
    uint32_t file_version, file_revision, code_io_version, step;
    int rank, ncpus, ndomains, rank_master, indent, print;
    char *buffer;
    char done_header, restart_flag, other_domains, use_float;
    char *code_name;
    sdf_block_t *blocklist, *current_block;
    char *mmap;
#ifdef SDF_DEBUG
    char *dbg, *dbg_buf;
    size_t dbg_count;
#endif
#ifdef PARALLEL
    MPI_File filehandle;
#else
    FILE *filehandle;
#endif
    comm_t comm;
};

sdf_file_t *sdf_open(const char *filename, int rank, comm_t comm, int use_mmap);
int sdf_close(sdf_file_t *h);
int sdf_seek(sdf_file_t *h);
sdf_block_t *sdf_find_block_by_id(sdf_file_t *h, const char *id);
sdf_block_t *sdf_find_block_by_name(sdf_file_t *h, const char *name);
int sdf_read_header(sdf_file_t *h);
int sdf_read_blocklist(sdf_file_t *h);
int sdf_read_block_info(sdf_file_t *h);
int sdf_read_data(sdf_file_t *h);
int sdf_read_bytes(sdf_file_t *h, char *buf, int buflen);
int sdf_free_blocklist_data(sdf_file_t *h);
int sdf_set_ncpus(sdf_file_t *h, int ncpus);
int sdf_broadcast(sdf_file_t *h, void *buf, int size);
int sdf_get_domain_extents(sdf_file_t *h, int rank, int *start, int *local);

// internal routines

int sdf_factor(sdf_file_t *h, int *start);
int sdf_convert_array_to_float(sdf_file_t *h, void **var_in, int count);
int sdf_set_rank_master(sdf_file_t *h, int rank);
int sdf_read_nblocks(sdf_file_t *h);

int sdf_abort(sdf_file_t *h);
int sdf_read_next_block_header(sdf_file_t *h);
int sdf_read_stitched_tensor(sdf_file_t *h);
int sdf_read_stitched_material(sdf_file_t *h);
int sdf_read_stitched_matvar(sdf_file_t *h);
int sdf_read_stitched_species(sdf_file_t *h);
int sdf_read_constant(sdf_file_t *h);

int sdf_read_plain_mesh(sdf_file_t *h);
int sdf_read_plain_mesh_info(sdf_file_t *h);
int sdf_read_plain_variable(sdf_file_t *h);
int sdf_read_plain_variable_info(sdf_file_t *h);

int sdf_read_point_mesh(sdf_file_t *h);
int sdf_read_point_mesh_info(sdf_file_t *h);
int sdf_read_point_variable(sdf_file_t *h);
int sdf_read_point_variable_info(sdf_file_t *h);


#ifdef SDF_DEBUG
  #define DBG_CHUNK 256

  #define SDF_RANK0 if (h->rank == h->rank_master)

  #define SDF_PRNT(...) SDF_RANK0 do { \
    sprintf(h->dbg, __VA_ARGS__); \
    h->dbg += strlen(h->dbg); \
    if (h->dbg_count - (h->dbg - h->dbg_buf) < DBG_CHUNK) { \
        char *old = h->dbg_buf; \
        h->dbg_buf = malloc(2 * h->dbg_count); \
        memcpy(h->dbg_buf, old, h->dbg_count); \
        h->dbg_count *= 2; \
        h->dbg = h->dbg_buf + (h->dbg - old); \
        free(old); \
    }} while (0)

  #define SDF_DPRNT(...) do { \
        int _a; for (_a=0; _a<h->indent; _a++) SDF_PRNT(" "); \
        SDF_PRNT(__VA_ARGS__); \
    } while (0)

  #define SDF_DPRNTa(a,f,len) SDF_RANK0 { \
            int _b, _c; \
            for (_b=0; _b<len; _b++) { \
                for (_c=0; _c<h->indent; _c++) SDF_PRNT(" "); \
                SDF_PRNT(#a "[%i]: %" #f "\n", _b, a[_b]); \
            } \
        }

 #ifdef SDF_DEBUG_ALL
  #define SDF_DPRNTar(a,len) SDF_RANK0 { \
        int _d; \
        if (b->datatype_out == SDF_DATATYPE_REAL4) { \
            float *arr = (a); \
            SDF_PRNT("r4 "); \
            for (_d=0; _d<(len); _d++) SDF_PRNT(" %g", arr[_d]); \
        } else if (b->datatype_out == SDF_DATATYPE_REAL8) { \
            double *arr = (a); \
            SDF_PRNT("r8 "); \
            for (_d=0; _d<(len); _d++) SDF_PRNT(" %g", arr[_d]); \
        } else { \
            int *arr = (a); \
            SDF_PRNT("i4 "); \
            for (_d=0; _d<(len); _d++) SDF_PRNT(" %i", arr[_d]); \
        } \
        SDF_PRNT("\n"); \
    }
 #else
  #define SDF_DPRNTar(a,len) do {} while(0)
 #endif
#else
  #define SDF_DPRNT(...) do {} while(0)
  #define SDF_DPRNTa(a,f,len) do {} while(0)
  #define SDF_DPRNTar(a,len) do {} while(0)
#endif

#define SDF_READ_ENTRY_INT4(value) do { \
        (value) = *((uint32_t *) \
            (h->buffer + h->current_location - h->start_location)); \
        h->current_location += 4; \
        SDF_DPRNT(#value ": %i\n", (value)); \
    } while (0)

#define SDF_READ_ENTRY_INT8(value) do { \
        (value) = *((uint64_t *) \
            (h->buffer + h->current_location - h->start_location)); \
        h->current_location += 8; \
        SDF_DPRNT(#value ": %lli\n", (long long int)(value)); \
    } while (0)

#define SDF_READ_ENTRY_REAL4(value) do { \
        (value) = *((float *) \
            (h->buffer + h->current_location - h->start_location)); \
        h->current_location += 4; \
        SDF_DPRNT(#value ": %g\n", (float)(value)); \
    } while (0)

#define SDF_READ_ENTRY_REAL8(value) do { \
        (value) = *((double *) \
            (h->buffer + h->current_location - h->start_location)); \
        h->current_location += 8; \
        SDF_DPRNT(#value ": %g\n", (double)(value)); \
    } while (0)

#define SDF_READ_ENTRY_LOGICAL(value) do { \
        (value) = *((char *) \
            (h->buffer + h->current_location - h->start_location)); \
        h->current_location += 1; \
        if ((value)) { \
            SDF_DPRNT(#value ": true\n"); \
        } else { \
            SDF_DPRNT(#value ": false\n"); \
        } \
    } while (0)

#define SDF_READ_ENTRY_CONST(value) do { \
        memcpy((value), (h->buffer + h->current_location - h->start_location), \
            b->type_size); \
        h->current_location += b->type_size; \
        switch (b->datatype) { \
        case(SDF_DATATYPE_REAL4): \
          SDF_DPRNT(#value ": %g\n", *((float*)(value))); \
          break; \
        case(SDF_DATATYPE_REAL8): \
          SDF_DPRNT(#value ": %g\n", *((double*)(value))); \
          break; \
        case(SDF_DATATYPE_INTEGER4): \
          SDF_DPRNT(#value ": %i\n", *((int32_t*)(value))); \
          break; \
        case(SDF_DATATYPE_INTEGER8): \
          SDF_DPRNT(#value ": %lli\n", *((int64_t*)(value))); \
          break; \
        } \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_INT4(value, length) do { \
        if (!(value)) value = calloc((length), sizeof(int32_t)); \
        memcpy((value), (h->buffer + h->current_location - h->start_location), \
            4 * (length)); \
        h->current_location += 4 * (length); \
        SDF_DPRNTa(value, i, (length)); \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_INT8(value, length) do { \
        if (!(value)) value = calloc((length), sizeof(int64_t)); \
        memcpy((value), (h->buffer + h->current_location - h->start_location), \
            8 * (length)); \
        h->current_location += 8 * (length); \
        SDF_DPRNTa(value, lli, (length)); \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_REAL4(value, length) do { \
        if (!(value)) value = calloc((length), sizeof(float)); \
        memcpy((value), (h->buffer + h->current_location - h->start_location), \
            4 * (length)); \
        h->current_location += 4 * (length); \
        SDF_DPRNTa(value, g, (length)); \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_REAL8(value, length) do { \
        if (!(value)) value = calloc((length), sizeof(double)); \
        memcpy((value), (h->buffer + h->current_location - h->start_location), \
            8 * (length)); \
        h->current_location += 8 * (length); \
        SDF_DPRNTa(value, g, (length)); \
    } while (0)

#define SDF_READ_ENTRY_TYPE(value) do { \
        (b->value) = *((uint32_t *) \
            (h->buffer + h->current_location - h->start_location)); \
        h->current_location += 4; \
        SDF_DPRNT("b->" #value ": %s\n", sdf_ ## value ## _c[b->value]); \
    } while (0)

#define SDF_READ_ENTRY_STRINGLEN(value, length) do { \
        if (!(value)) value = malloc(h->string_length); \
        memcpy((value), (h->buffer + h->current_location - h->start_location), \
            (length)); \
        h->current_location += (length); \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_STRINGLEN(value, length, clen) do { \
        int _e; \
        if (!(value)) value = calloc((length), sizeof(char*)); \
        for (_e=0; _e<(length); _e++) { \
            SDF_READ_ENTRY_STRINGLEN(value[_e], clen); \
            SDF_DPRNT(#value "[%i]: %s\n", _e, (value[_e])); \
        } \
    } while (0)

#define SDF_READ_ENTRY_ID(value) do { \
        SDF_READ_ENTRY_STRINGLEN(value, SDF_ID_LENGTH); \
        SDF_DPRNT(#value ": %s\n", (value)); \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_ID(value, length) \
        SDF_READ_ENTRY_ARRAY_STRINGLEN(value, length, SDF_ID_LENGTH)

#define SDF_READ_ENTRY_STRING(value) do { \
        SDF_READ_ENTRY_STRINGLEN(value, h->string_length); \
        SDF_DPRNT(#value ": %s\n", (value)); \
    } while (0)

#define SDF_READ_ENTRY_ARRAY_STRING(value, length) \
        SDF_READ_ENTRY_ARRAY_STRINGLEN(value, length, h->string_length)

#define SDF_SET_ENTRY_STRINGLEN(value, strvalue, length) do { \
        if (!(value)) value = malloc(h->string_length); \
        strncpy((value), (strvalue), (length)); \
    } while (0)

#define SDF_SET_ENTRY_ID(value, strvalue) do { \
        SDF_SET_ENTRY_STRINGLEN(value, strvalue, SDF_ID_LENGTH); \
        SDF_DPRNT(#value ": %s\n", (value)); \
    } while (0)

#define SDF_SET_ENTRY_STRING(value, strvalue) do { \
        SDF_SET_ENTRY_STRINGLEN(value, strvalue, h->string_length); \
        SDF_DPRNT(#value ": %s\n", (value)); \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
