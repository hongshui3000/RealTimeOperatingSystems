/*
** $Id: liolib.c 6576 2017-01-10 10:04:42Z u_bonnes $
** Standard I/O (and system) library
** See Copyright Notice in lua.h
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define liolib_c
#define LUA_LIB

#include <lua/lua.h>

#include <lua/lauxlib.h>
#include <lua/lualib.h>
#include <lua/lrotable.h>

#ifdef NUTLUA_IOLIB_TCP
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define IO_INPUT    1
#define IO_OUTPUT   2
#define IO_STDERR     0

#if NUTLUA_OPTIMIZE_MEMORY != 2
#define LUA_IO_GETFIELD(f)  lua_rawgeti(L, LUA_ENVIRONINDEX, f)
#define LUA_IO_SETFIELD(f)  lua_rawseti(L, LUA_ENVIRONINDEX, f)
#else
#define LUA_IO_GETFIELD(f)  lua_rawgeti(L, LUA_REGISTRYINDEX, liolib_keys[f]);
#define LUA_IO_SETFIELD(f)  lua_rawseti(L, LUA_REGISTRYINDEX, liolib_keys[f])
static const int liolib_keys[] = {(int)&stderr, (int)&stdin, (int)&stdout};
#endif

static const char *const fnames[] = {"input", "output"};

static int pushresult (lua_State *L, int i, const char *filename) {
  int en = errno;  /* calls to Lua API may change this value */
  if (i) {
    lua_pushboolean(L, 1);
    return 1;
  }
  else {
    lua_pushnil(L);
    if (filename)
      lua_pushfstring(L, "%s: %s", filename, strerror(en));
    else
      lua_pushfstring(L, "%s", strerror(en));
    lua_pushinteger(L, en);
    return 3;
  }
}


static void fileerror (lua_State *L, int arg, const char *filename) {
  lua_pushfstring(L, "%s: %s", filename, strerror(errno));
  luaL_argerror(L, arg, lua_tostring(L, -1));
}


#define tofilep(L)  ((FILE **)luaL_checkudata(L, 1, LUA_FILEHANDLE))


static int io_type (lua_State *L) {
  void *ud;
  luaL_checkany(L, 1);
  ud = lua_touserdata(L, 1);
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_FILEHANDLE);
  if (ud == NULL || !lua_getmetatable(L, 1) || !lua_rawequal(L, -2, -1))
    lua_pushnil(L);  /* not a file */
  else if (*((FILE **)ud) == NULL)
    lua_pushliteral(L, "closed file");
  else
    lua_pushliteral(L, "file");
  return 1;
}


static FILE *tofile (lua_State *L) {
  FILE **f = tofilep(L);
  if (*f == NULL)
    luaL_error(L, "attempt to use a closed file");
  return *f;
}



/*
** When creating file handles, always creates a `closed' file handle
** before opening the actual file; so, if there is a memory error, the
** file is not left opened.
*/
static FILE **newfile (lua_State *L) {
  FILE **pf = (FILE **)lua_newuserdata(L, sizeof(FILE *));
  *pf = NULL;  /* file handle is currently `closed' */
  luaL_getmetatable(L, LUA_FILEHANDLE);
  lua_setmetatable(L, -2);
  return pf;
}

#if NUTLUA_OPTIMIZE_MEMORY != 2
/*
** function to (not) close the standard files stdin, stdout, and stderr
*/
static int io_noclose (lua_State *L) {
  lua_pushnil(L);
  lua_pushliteral(L, "cannot close standard file");
  return 2;
}
#endif


#if NUTLUA_OPTIMIZE_MEMORY != 2
/*
** function to close 'popen' files
*/
static int io_pclose (lua_State *L) {
  FILE **p = tofilep(L);
  int ok = lua_pclose(L, *p);
  *p = NULL;
  return pushresult(L, ok, NULL);
}
#endif


#if NUTLUA_OPTIMIZE_MEMORY != 2
/*
** function to close regular files and tcp sockets
*/
static int io_fclose (lua_State *L) {
  FILE **p = tofilep(L);
#ifdef NUTLUA_IOLIB_TCP
  NUTFILE *nf = (NUTFILE *) (uintptr_t) _fileno(*p);
#endif
  int ok = (fclose(*p) == 0);
#ifdef NUTLUA_IOLIB_TCP
  if (nf->nf_dev == NULL) {
    NutTcpCloseSocket((TCPSOCKET *) nf);
  }
#endif
  *p = NULL;
  return pushresult(L, ok, NULL);
}
#endif


static int aux_close (lua_State *L) {
#if NUTLUA_OPTIMIZE_MEMORY != 2
  lua_getfenv(L, 1);
  lua_getfield(L, -1, "__close");
  return (lua_tocfunction(L, -1))(L);
#else
  FILE **p = tofilep(L);
  if(*p == stdin || *p == stdout || *p == stderr)
  {
    lua_pushnil(L);
    lua_pushliteral(L, "cannot close standard file");
    return 2;
  }
  int ok = (fclose(*p) == 0);
  *p = NULL;
  return pushresult(L, ok, NULL);
#endif
}


static int io_close (lua_State *L) {
  if (lua_isnone(L, 1))
    LUA_IO_GETFIELD(IO_OUTPUT);
  tofile(L);  /* make sure argument is a file */
  return aux_close(L);
}


static int io_gc (lua_State *L) {
  FILE *f = *tofilep(L);
  /* ignore closed files */
  if (f != NULL)
    aux_close(L);
  return 0;
}


static int io_tostring (lua_State *L) {
  FILE *f = *tofilep(L);
  if (f == NULL)
    lua_pushliteral(L, "file (closed)");
  else
    lua_pushfstring(L, "file (%p)", f);
  return 1;
}


static int io_open (lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  const char *mode = luaL_optstring(L, 2, "r");
  FILE **pf = newfile(L);
  *pf = fopen(filename, mode);
  return (*pf == NULL) ? pushresult(L, 0, filename) : 1;
}

#ifdef NUTLUA_IOLIB_TCP

static int io_connect (lua_State *L) {
  char addr[22];
  char *port;
  const char *remote = luaL_checkstring(L, 1);
  const char *mode = luaL_optstring(L, 2, "r");
  FILE **pf = newfile(L);
  TCPSOCKET *sock;

  strncpy(addr, remote, sizeof(addr) - 1);
  if ((port = strchr(addr, ':')) != NULL) {
      *port++ = '\0';
  }
  if (port && (sock = NutTcpCreateSocket()) != NULL) {
    if (NutTcpConnect(sock, inet_addr(addr), (uint16_t)atoi(port)) == 0) {
      *pf = _fdopen((int) sock, mode);
    }
  }
  return (*pf == NULL) ? pushresult(L, 0, remote) : 1;
}


static int io_accept (lua_State *L) {
  const char *port = luaL_checkstring(L, 1);
  const char *mode = luaL_optstring(L, 2, "r");
  FILE **pf = newfile(L);
  TCPSOCKET *sock;

  if ((sock = NutTcpCreateSocket()) != NULL) {
    if (NutTcpAccept(sock, (uint16_t)atoi(port)) == 0) {
      *pf = _fdopen((int) sock, mode);
    }
  }
  return (*pf == NULL) ? pushresult(L, 0, port) : 1;
}

#endif /* NUTLUA_IOLIB_TCP */

/*
** this function has a separated environment, which defines the
** correct __close for 'popen' files
*/
static int io_popen (lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  const char *mode = luaL_optstring(L, 2, "r");
  FILE **pf = newfile(L);
  *pf = lua_popen(L, filename, mode);
  return (*pf == NULL) ? pushresult(L, 0, filename) : 1;
}

#ifndef NUTLUA_IOLIB_TMPFILE_NOT_IMPLEMENTED
static int io_tmpfile (lua_State *L) {
  FILE **pf = newfile(L);
  *pf = tmpfile();
  return (*pf == NULL) ? pushresult(L, 0, NULL) : 1;
}
#endif

static FILE *getiofile (lua_State *L, int findex) {
  FILE *f;
  LUA_IO_GETFIELD(findex);
  f = *(FILE **)lua_touserdata(L, -1);
  if (f == NULL)
    luaL_error(L, "standard %s file is closed", fnames[findex - 1]);
  return f;
}


static int g_iofile (lua_State *L, int f, const char *mode) {
  if (!lua_isnoneornil(L, 1)) {
    const char *filename = lua_tostring(L, 1);
    if (filename) {
      FILE **pf = newfile(L);
      *pf = fopen(filename, mode);
      if (*pf == NULL)
        fileerror(L, 1, filename);
    }
    else {
      tofile(L);  /* check that it's a valid file handle */
      lua_pushvalue(L, 1);
    }
    LUA_IO_SETFIELD(f);
  }
  /* return current value */
  LUA_IO_GETFIELD(f);
  return 1;
}


static int io_input (lua_State *L) {
  return g_iofile(L, IO_INPUT, "r");
}


static int io_output (lua_State *L) {
  return g_iofile(L, IO_OUTPUT, "w");
}


static int io_readline (lua_State *L);


static void aux_lines (lua_State *L, int idx, int toclose) {
  lua_pushvalue(L, idx);
  lua_pushboolean(L, toclose);  /* close/not close file when finished */
  lua_pushcclosure(L, io_readline, 2);
}


static int f_lines (lua_State *L) {
  tofile(L);  /* check that it's a valid file handle */
  aux_lines(L, 1, 0);
  return 1;
}


static int io_lines (lua_State *L) {
  if (lua_isnoneornil(L, 1)) {  /* no arguments? */
    /* will iterate over default input */
    LUA_IO_GETFIELD(IO_INPUT);
    return f_lines(L);
  }
  else {
    const char *filename = luaL_checkstring(L, 1);
    FILE **pf = newfile(L);
    *pf = fopen(filename, "r");
    if (*pf == NULL)
      fileerror(L, 1, filename);
    aux_lines(L, lua_gettop(L), 1);
    return 1;
  }
}


/*
** {======================================================
** READ
** =======================================================
*/


static int read_number (lua_State *L, FILE *f) {
  lua_Number d;
  if (fscanf(f, LUA_NUMBER_SCAN, &d) == 1) {
    lua_pushnumber(L, d);
    return 1;
  }
  else return 0;  /* read fails */
}


static int test_eof (lua_State *L, FILE *f) {
  int c = getc(f);
  ungetc(c, f);
  lua_pushlstring(L, NULL, 0);
  return (c != EOF);
}


static int read_line (lua_State *L, FILE *f) {
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  for (;;) {
    size_t l;
    char *p = luaL_prepbuffer(&b);
    if (fgets(p, LUAL_BUFFERSIZE, f) == NULL) {  /* eof? */
      luaL_pushresult(&b);  /* close buffer */
      return (lua_objlen(L, -1) > 0);  /* check whether read something */
    }
    l = strlen(p);
    if (l == 0 || p[l-1] != '\n')
      luaL_addsize(&b, l);
    else {
      luaL_addsize(&b, l - 1);  /* do not include `eol' */
      luaL_pushresult(&b);  /* close buffer */
      return 1;  /* read at least an `eol' */
    }
  }
}


static int read_chars (lua_State *L, FILE *f, size_t n) {
  size_t rlen;  /* how much to read */
  size_t nr;  /* number of chars actually read */
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  rlen = LUAL_BUFFERSIZE;  /* try to read that much each time */
  do {
    char *p = luaL_prepbuffer(&b);
    if (rlen > n) rlen = n;  /* cannot read more than asked */
    nr = fread(p, sizeof(char), rlen, f);
    luaL_addsize(&b, nr);
    n -= nr;  /* still have to read `n' chars */
  } while (n > 0 && nr == rlen);  /* until end of count or eof */
  luaL_pushresult(&b);  /* close buffer */
  return (n == 0 || lua_objlen(L, -1) > 0);
}


static int g_read (lua_State *L, FILE *f, int first) {
  int nargs = lua_gettop(L) - 1;
  int success;
  int n;
  clearerr(f);
  if (nargs == 0) {  /* no arguments? */
    success = read_line(L, f);
    n = first+1;  /* to return 1 result */
  }
  else {  /* ensure stack space for all results and for auxlib's buffer */
    luaL_checkstack(L, nargs+LUA_MINSTACK, "too many arguments");
    success = 1;
    for (n = first; nargs-- && success; n++) {
      if (lua_type(L, n) == LUA_TNUMBER) {
        size_t l = (size_t)lua_tointeger(L, n);
        success = (l == 0) ? test_eof(L, f) : read_chars(L, f, l);
      }
      else {
        const char *p = lua_tostring(L, n);
        luaL_argcheck(L, p && p[0] == '*', n, "invalid option");
        switch (p[1]) {
          case 'n':  /* number */
            success = read_number(L, f);
            break;
          case 'l':  /* line */
            success = read_line(L, f);
            break;
          case 'a':  /* file */
            read_chars(L, f, ~((size_t)0));  /* read MAX_SIZE_T chars */
            success = 1; /* always success */
            break;
          default:
            return luaL_argerror(L, n, "invalid format");
        }
      }
    }
  }
  if (ferror(f))
    return pushresult(L, 0, NULL);
  if (!success) {
    lua_pop(L, 1);  /* remove last result */
    lua_pushnil(L);  /* push nil instead */
  }
  return n - first;
}


static int io_read (lua_State *L) {
  return g_read(L, getiofile(L, IO_INPUT), 1);
}


static int f_read (lua_State *L) {
  return g_read(L, tofile(L), 2);
}


static int io_readline (lua_State *L) {
  FILE *f = *(FILE **)lua_touserdata(L, lua_upvalueindex(1));
  int sucess;
  if (f == NULL)  /* file is already closed? */
    luaL_error(L, "file is already closed");
  sucess = read_line(L, f);
  if (ferror(f))
    return luaL_error(L, "%s", strerror(errno));
  if (sucess) return 1;
  else {  /* EOF */
    if (lua_toboolean(L, lua_upvalueindex(2))) {  /* generator created file? */
      lua_settop(L, 0);
      lua_pushvalue(L, lua_upvalueindex(1));
      aux_close(L);  /* close it */
    }
    return 0;
  }
}

/* }====================================================== */


static int g_write (lua_State *L, FILE *f, int arg) {
  int nargs = lua_gettop(L) - 1;
  int status = 1;
  for (; nargs--; arg++) {
    if (lua_type(L, arg) == LUA_TNUMBER) {
      /* optimization: could be done exactly as for strings */
      status = status &&
          fprintf(f, LUA_NUMBER_FMT, lua_tonumber(L, arg)) > 0;
    }
    else {
      size_t l;
      const char *s = luaL_checklstring(L, arg, &l);
      status = status && (fwrite(s, sizeof(char), l, f) == l);
    }
  }
  return pushresult(L, status, NULL);
}


static int io_write (lua_State *L) {
  return g_write(L, getiofile(L, IO_OUTPUT), 1);
}


static int f_write (lua_State *L) {
  return g_write(L, tofile(L), 2);
}


static int f_seek (lua_State *L) {
  static const int mode[] = {SEEK_SET, SEEK_CUR, SEEK_END};
  static const char *const modenames[] = {"set", "cur", "end", NULL};
  FILE *f = tofile(L);
  int op = luaL_checkoption(L, 2, "cur", modenames);
  long offset = luaL_optlong(L, 3, 0);
  op = fseek(f, offset, mode[op]);
  if (op)
    return pushresult(L, 0, NULL);  /* error */
  else {
    lua_pushinteger(L, ftell(f));
    return 1;
  }
}

#ifndef NUTLUA_IOLIB_SETVBUF_NOT_IMPLEMENTED
static int f_setvbuf (lua_State *L) {
  static const int mode[] = {_IONBF, _IOFBF, _IOLBF};
  static const char *const modenames[] = {"no", "full", "line", NULL};
  FILE *f = tofile(L);
  int op = luaL_checkoption(L, 2, NULL, modenames);
  lua_Integer sz = luaL_optinteger(L, 3, LUAL_BUFFERSIZE);
  int res = setvbuf(f, NULL, mode[op], sz);
  return pushresult(L, res == 0, NULL);
}
#endif


static int io_flush (lua_State *L) {
  return pushresult(L, fflush(getiofile(L, IO_OUTPUT)) == 0, NULL);
}


static int f_flush (lua_State *L) {
  return pushresult(L, fflush(tofile(L)) == 0, NULL);
}

#ifdef NUTLUA_IOLIB_TCP
#define NUTLUA_IOLIB_TCP_ACCEPT_FUNC  {"accept", io_accept},
#define NUTLUA_IOLIB_TCP_CONNECT_FUNC {"connect", io_connect},
#else
#define NUTLUA_IOLIB_TCP_ACCEPT_FUNC
#define NUTLUA_IOLIB_TCP_CONNECT_FUNC
#endif
#ifdef NUTLUA_IOLIB_TMPFILE_NOT_IMPLEMENTED
#define NUTLUA_IOLIB_TMPFILE_FUNC
#else
#define NUTLUA_IOLIB_TMPFILE_FUNC  {"tmpfile", io_tmpfile},
#endif

#define LUA_IO_FUNCS\
  NUTLUA_IOLIB_TCP_ACCEPT_FUNC\
  {"close", io_close},\
  NUTLUA_IOLIB_TCP_CONNECT_FUNC\
  {"flush", io_flush},\
  {"input", io_input},\
  {"lines", io_lines},\
  {"open", io_open},\
  {"output", io_output},\
  {"popen", io_popen},\
  {"read", io_read},\
  NUTLUA_IOLIB_TMPFILE_FUNC\
  {"type", io_type},\
  {"write", io_write}

#if NUTLUA_OPTIMIZE_MEMORY != 2
static const luaL_Reg iolib[] = {
  LUA_IO_FUNCS,
  {NULL, NULL}
};
#else
static const luaL_Reg iolib_funcs[] = {
  LUA_IO_FUNCS,
  {NULL, NULL}
};

static int iolib_index(lua_State *L)
{
  return luaR_findfunction(L, iolib_funcs);
}

static const luaL_Reg iolib[] = {
  {"__index", iolib_index},
  {NULL, NULL}
};
#endif

#ifdef NUTLUA_IOLIB_SETVBUF_NOT_IMPLEMENTED
#define NUTLUA_IOLIB_SETVBUF_FUNC
#else
#define NUTLUA_IOLIB_SETVBUF_FUNC {"setvbuf", f_setvbuf},
#endif

#define LUA_IO_FLIB_FUNCS\
  {"close", io_close},\
  {"flush", f_flush},\
  {"lines", f_lines},\
  {"read", f_read},\
  {"seek", f_seek},\
  NUTLUA_IOLIB_SETVBUF_FUNC\
  {"write", f_write}

#if NUTLUA_OPTIMIZE_MEMORY != 2
static const luaL_Reg flib[] = {
  LUA_IO_FLIB_FUNCS,
  {"__gc", io_gc},
  {"__tostring", io_tostring},
  {NULL, NULL}
};
#else
static const luaL_Reg flib_funcs[] = {
  LUA_IO_FLIB_FUNCS,
  {NULL, NULL}
};

static int flib_index(lua_State *L)
{
  return luaR_findfunction(L, flib_funcs);
}

static const luaL_Reg flib[] = {
  {"__index", flib_index},
  {"__gc", io_gc},
  {"__tostring", io_tostring},
  {NULL, NULL}
};
#endif

static void createmeta (lua_State *L) {
  luaL_newmetatable(L, LUA_FILEHANDLE);  /* create metatable for file handles */
#if NUTLUA_OPTIMIZE_MEMORY != 2
  lua_pushvalue(L, -1);  /* push metatable */
  lua_setfield(L, -2, "__index");  /* metatable.__index = metatable */
  luaL_register(L, NULL, flib);  /* file methods */
#else
  luaL_register_light(L, NULL, flib);
#endif
}


static void createstdfile (lua_State *L, FILE *f, int k, const char *fname) {
  *newfile(L) = f;
#if NUTLUA_OPTIMIZE_MEMORY != 2
  if (k > 0) {
    lua_pushvalue(L, -1);
    lua_rawseti(L, LUA_ENVIRONINDEX, k);
  }
  lua_pushvalue(L, -2);  /* copy environment */
  lua_setfenv(L, -2);  /* set it */
  lua_setfield(L, -3, fname);
#else
  lua_pushvalue(L, -1);
  lua_rawseti(L, LUA_REGISTRYINDEX, liolib_keys[k]);
  lua_setfield(L, -2, fname);
#endif
}

#if NUTLUA_OPTIMIZE_MEMORY != 2
static void newfenv (lua_State *L, lua_CFunction cls) {
  lua_createtable(L, 0, 1);
  lua_pushcfunction(L, cls);
  lua_setfield(L, -2, "__close");
}
#endif


LUALIB_API int luaopen_io (lua_State *L) {
  createmeta(L);
#if NUTLUA_OPTIMIZE_MEMORY != 2
  /* create (private) environment (with fields IO_INPUT, IO_OUTPUT, __close) */
  newfenv(L, io_fclose);
  lua_replace(L, LUA_ENVIRONINDEX);
  /* open library */
  luaL_register(L, LUA_IOLIBNAME, iolib);
  /* create (and set) default files */
  newfenv(L, io_noclose);  /* close function for default files */
#else
  luaL_register_light(L, LUA_IOLIBNAME, iolib);
  lua_pushvalue(L, -1);
  lua_setmetatable(L, -2);
#endif
  createstdfile(L, stdin, IO_INPUT, "stdin");
  createstdfile(L, stdout, IO_OUTPUT, "stdout");
  createstdfile(L, stderr, IO_STDERR, "stderr");
#if NUTLUA_OPTIMIZE_MEMORY != 2
  lua_pop(L, 1);  /* pop environment for default files */
  lua_getfield(L, -1, "popen");
  newfenv(L, io_pclose);  /* create environment for 'popen' */
  lua_setfenv(L, -2);  /* set fenv for 'popen' */
  lua_pop(L, 1);  /* pop 'popen' */
#endif
  return 1;
}
