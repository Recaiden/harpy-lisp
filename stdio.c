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
  LASSERT_TYPE("fopen", a, 1, LVAL_INT);
  
  lval* file = lval_pop(a, 0);
  lval* strMode = lval_pop(a, 0);

  // TODO create STREAM-type lval(!?) containing result.
  fopen (file->str, strMode->str);
  lval_del(file);
  lval_del(strMode);

}


lval* builtin_fread(lenv* e, lval* a)
{
}

lval* builtin_fwrite(lenv* e, lval* a)
{
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
