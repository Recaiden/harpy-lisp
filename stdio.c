#include "stdio.h"

#define FILE_MODE_WRITE 2
#define FILE_MODE_READ 3
#define FILE_TAG_APPEND 5
#define FILE_TAG_OVERWRITE 7
#define FILE_TAG_PRESERVE 11
#define FILE_TAG_NO_DISCARD 13

/* fclose filename(str) */
lval* builtin_fclose(lenv* e, lval* a)
{
  LASSERT_NUM("fclose", a, 1);
  LASSERT_TYPE("fclose", a, 0, LVAL_STR);

}

/* fopen filename(str) mode(str) */
lval* builtin_fopen(lenv* e, lval* a)
{
  LASSERT_NUM("fopen", a, 2);
  LASSERT_TYPE("fopen", a, 0, LVAL_STR);
  LASSERT_TYPE("fopen", a, 1, LVAL_STR);
  
  lval* filename = lval_pop(a, 0);
  lval* strMode = lval_pop(a, 0);

  lval* x = lval_obj("FILE", sizeof(FILE*));
  x->mem = (void*)fopen(filename->str, strMode->str);
  if(x->mem == NULL)
  {
    lval_del(x);
    x = lval_err(strerror(errno));
    perror("The following error occurred opening");
  }

  lval_del(filename);
  lval_del(strMode);
  return x;
}


/* Paramters are file handle, size */
lval* builtin_fread(lenv* e, lval* a)
{
  LASSERT_TYPE("fread", a, 0, LVAL_OBJ);
  LASSERT_TYPE("fread", a, 1, LVAL_INT);

  //TODO assert that obj-type is "FILE"

  lval* valFile = lval_pop(a, 0);
  lval* valSize = lval_pop(a, 0);
  int size = valSize->integer;
  int cRead;

  char* buffer = malloc(size);

  cRead = fread(buffer, size, 1, (FILE*)valFile->mem);

  lval* x;
  if(cRead)
    x = lval_str(buffer);
  else
    x = lval_err("fread failed");
  free(buffer);

  lval_del(valFile);
  lval_del(valSize);

  return x;
}

lval* builtin_fwrite(lenv* e, lval* a)
{
  LASSERT_TYPE("fwrite", a, 0, LVAL_OBJ);
  LASSERT_TYPE("fwrite", a, 1, LVAL_STR);

  //TODO assert that obj-type is "FILE"

  lval* valFile = lval_pop(a, 0);
  lval* valText = lval_pop(a, 0);
  int size = strlen(valText->str);
  int cRead;
  
  cWrote = fread(valText->str, size, 1, (FILE*)valFile->mem);

  lval* x;
  if(cWrote == size)
    x = lval_int(cWrote);
  else
    x = lval_err("fwrite failed");

  lval_del(valFile);
  lval_del(valSize);

  return x;
}

lval* builtin_fflush(lenv* e, lval* a)
{
}

/* 
 getpos
 seek
 setpos
 rewind

 end-of-file

 character manipulation
 string manipulation

 tempfiles
*/

/* Implemented at a higher level */
/*
  freopen ; close then open, or set permissions
  printf ; print to a specific stream

scan/print
get/put

 */


void lenv_add_stdio(lenv* e)
{
  lenv_add_builtin(e, "fopen", builtin_fopen);
  lenv_add_builtin(e, "fread", builtin_fread);

}
