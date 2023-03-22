from ctypes import *
import platform
import array

def debug_print(arg):
    pass

#/* used in all UMFPACK_report_* routines: */
UMFPACK_PRL = 0                  # /* print level */

#/* used in UMFPACK_*symbolic only: */
UMFPACK_DENSE_ROW = 1            #/* dense row parameter */
UMFPACK_DENSE_COL = 2            #/* dense col parameter */
UMFPACK_BLOCK_SIZE = 4           #/* BLAS-3 block size */
UMFPACK_STRATEGY = 5             #/* auto, symmetric, unsym., or 2by2 */
UMFPACK_2BY2_TOLERANCE = 12      #/* 2-by-2 pivot tolerance */
UMFPACK_FIXQ = 13                #/* -1: no fixQ, 0: default, 1: fixQ */
UMFPACK_AMD_DENSE = 14           #/* for AMD ordering */
UMFPACK_AGGRESSIVE = 19          #/* whether or not to use aggressive

#/* default values of Control may change in future versions of UMFPACK. */

#/* -------------------------------------------------------------------------- */
#/* status codes */
#/* -------------------------------------------------------------------------- */
#
UMFPACK_OK = 0
#
#/* status > 0 means a warning, but the method was successful anyway. */
#/* A Symbolic or Numeric object was still created. */
##define UMFPACK_WARNING_singular_matrix (1)
#
#/* The following warnings were added in umfpack_*_get_determinant */
##define UMFPACK_WARNING_determinant_underflow (2)
##define UMFPACK_WARNING_determinant_overflow (3)
#
#/* status < 0 means an error, and the method was not successful. */
#/* No Symbolic of Numeric object was created. */
##define UMFPACK_ERROR_out_of_memory (-1)
##define UMFPACK_ERROR_invalid_Numeric_object (-3)
##define UMFPACK_ERROR_invalid_Symbolic_object (-4)
##define UMFPACK_ERROR_argument_missing (-5)
##define UMFPACK_ERROR_n_nonpositive (-6)
##define UMFPACK_ERROR_invalid_matrix (-8)
##define UMFPACK_ERROR_different_pattern (-11)
##define UMFPACK_ERROR_invalid_system (-13)
##define UMFPACK_ERROR_invalid_permutation (-15)
##define UMFPACK_ERROR_internal_error (-911) /* yes, call me if you get this! */
##define UMFPACK_ERROR_file_IO (-17)

#/* -------------------------------------------------------------------------- */
#/* solve codes */
#/* -------------------------------------------------------------------------- */
#
#/* Solve the system ( )x=b, where ( ) is defined below.  "t" refers to the */
#/* linear algebraic transpose (complex conjugate if A is complex), or the (') */
#/* operator in MATLAB.  "at" refers to the array transpose, or the (.') */
#/* operator in MATLAB. */
#
UMFPACK_A = (0)     # /* Ax=b    */
UMFPACK_At = (1) #    /* A'x=b   */
UMFPACK_Aat = (2) #     /* A.'x=b  */
#
UMFPACK_Pt_L = (3)  #   /* P'Lx=b  */
##define UMFPACK_L      (4)     /* Lx=b    */
##define UMFPACK_Lt_P   (5)     /* L'Px=b  */
##define UMFPACK_Lat_P  (6)     /* L.'Px=b */
##define UMFPACK_Lt     (7)     /* L'x=b   */
##define UMFPACK_Lat    (8)     /* L.'x=b  */
#
UMFPACK_U_Qt = (9) #    /* UQ'x=b  */
##define UMFPACK_U      (10)    /* Ux=b    */
##define UMFPACK_Q_Ut   (11)    /* QU'x=b  */
##define UMFPACK_Q_Uat  (12)    /* QU.'x=b */
##define UMFPACK_Ut     (13)    /* U'x=b   */
##define UMFPACK_Uat    (14)    /* U.'x=b  */
#
#/* -------------------------------------------------------------------------- */
#
#/* Integer constants are used for status and solve codes instead of enum */
#/* to make it easier for a Fortran code to call UMFPACK. */

# /* -------------------------------------------------------------------------- */
# /* size of Info and Control arrays */
# /* -------------------------------------------------------------------------- */
#
# /* These might be larger in future versions, since there are only 3 unused
#  * entries in Info, and no unused entries in Control. */

UMFPACK_INFO = 90
UMFPACK_CONTROL = 20

def get_transpose(x):
    if x:
        return UMFPACK_Aat
    else:
        return UMFPACK_A

def get_dll_naming():
    systems = {
      'Linux' : {"prefix" : "lib", "suffix" : ".so"},
      'Darwin' : {"prefix" : "lib", "suffix" : ".dylib"},
      'Windows' : {"prefix" : "", "suffix" : ".dll"},
    }
    return systems[platform.system()]

def get_umfpack_name():
    return "./%(prefix)sumfpack_lgpl%(suffix)s" % get_dll_naming()

def load_umfpack_dll(dllname):
    dll = cdll.LoadLibrary(dllname)
    if not dll:
        raise RuntimeError("Cannot find UMFPACK dll")
    return dll

def get_blas_name():
    if platform.system() == "Windows":
        return "mkl_rt.1.dll"
    else:
        return "%(prefix)sopenblas%(suffix)s" % get_dll_naming()

def load_blas_dll(gdata, blaslib = None, noexcept = False):
    #print(dll.blasw_load_dll)
    if blaslib:
        dllname = blaslib
    else:
        dllname = get_blas_name()
    libname = c_char_p(dllname.encode('utf-8'))
    #libname = c_char_p(b'mkl_rt.2.dll')
    msg = c_char_p()
    #dll.blasw_load_dll.argtypes = [c_char_p, c_void_p]
    #dll.blasw_load_dll.restype = c_void_p
    #h = dll.blasw_load_dll(libname, byref(msg))
    h = cdll.LoadLibrary(dllname)
    #if not h or msg:
    #    if msg:
    #        print(string_at(msg))
    #    if not noexcept:
    #        raise RuntimeError("NO BLAS DLL LOADED")
    if not h or msg:
        if not noexcept:
            raise RuntimeError("NO BLAS DLL LOADED")
    else:
        gdata.blaslibs.append((dllname, h))
    #print(h)
    return h

def load_blas_functions(gdata, h):
    gdata.dll.blasw_load_functions.restype = c_int
    gdata.dll.blasw_load_functions.argtypes = [c_void_p]
    i = gdata.dll.blasw_load_functions(h._handle)
    return i

def myprintcb(msg):
    pmsg = msg.decode("utf-8")
    pmsg = pmsg.replace("\n", "\nUMF: ")
    print(pmsg, end="")

def set_python_print_callback(gdata):
    CALLBACK = CFUNCTYPE(None, c_char_p)
    gdata.dll.blasw_set_printer_callback.argtypes = [CALLBACK]
    gdata.dll.blasw_set_printer_callback.restype = None
    dcb = CALLBACK(myprintcb)
    gdata.dll.blasw_set_printer_callback(dcb)
    gdata.printcb = dcb
    return dcb


#TODO: handle same symbolic
class di_symbolic:
    def __init__(self, uc):
        self.Symbolic = c_void_p()
        self.umf_control = uc

    def __del__(self):
        if self.umf_control.is_complex:
            if self.Symbolic:
                debug_print('umfpack_zi_free_symbolic 176')
            self.umf_control.gdata.dll.umfpack_zi_free_symbolic (byref(self.Symbolic))
        else:
            if self.Symbolic:
                debug_print('umfpack_di_free_symbolic 180')
            self.umf_control.gdata.dll.umfpack_di_free_symbolic (byref(self.Symbolic))
        self.umf_control = None

    def factor_symbolic(self, matrix):
        if self.umf_control.is_complex:
            if self.Symbolic:
                debug_print('umfpack_zi_free_symbolic 187')
            self.umf_control.gdata.dll.umfpack_zi_free_symbolic (byref(self.Symbolic))
            NULL = c_void_p()
            debug_print('umfpack_zi_symbolic 190/u')
            self.status = self.umf_control.gdata.dll.umfpack_zi_symbolic (matrix.n, matrix.n, matrix.AP, matrix.AI, matrix.AX, NULL, byref(self.Symbolic), self.umf_control.Control, self.umf_control.Info)
        else:
            if self.Symbolic:
                debug_print('umfpack_di_free_symbolic 194')
            self.umf_control.gdata.dll.umfpack_di_free_symbolic (byref(self.Symbolic))
            debug_print('umfpack_di_symbolic 196')
            self.status = self.umf_control.gdata.dll.umfpack_di_symbolic (matrix.n, matrix.n, matrix.AP, matrix.AI, matrix.AX, byref(self.Symbolic), self.umf_control.Control, self.umf_control.Info)
        return self.status

class di_numeric:
    def __init__(self, uc):
        self.Numeric = c_void_p()
        self.umf_control = uc

    def __del__(self):
        if self.umf_control.is_complex:
            if self.Numeric:
                debug_print('umfpack_zi_free_numeric 209')
            self.umf_control.gdata.dll.umfpack_zi_free_numeric (byref(self.Numeric))
        else:
            if self.Numeric:
                debug_print('umfpack_di_free_numeric 213')
            self.umf_control.gdata.dll.umfpack_di_free_numeric (byref(self.Numeric))

    def factor_numeric(self, matrix, Symbolic):
        if self.umf_control.is_complex:
            if self.Numeric:
                debug_print('umfpack_zi_free_numeric 219')
            self.umf_control.gdata.dll.umfpack_zi_free_numeric (byref(self.Numeric))
            NULL = c_void_p()
            debug_print('umfpack_zi_numeric 222')
            self.status = self.umf_control.gdata.dll.umfpack_zi_numeric (matrix.AP, matrix.AI, matrix.AX, NULL, Symbolic.Symbolic, byref(self.Numeric), self.umf_control.Control, self.umf_control.Info)
        else:
            if self.Numeric:
                debug_print('umfpack_di_free_numeric 226')
            self.umf_control.gdata.dll.umfpack_di_free_numeric (byref(self.Numeric))
            debug_print('umfpack_di_numeric 228')
            self.status = self.umf_control.gdata.dll.umfpack_di_numeric (matrix.AP, matrix.AI, matrix.AX, Symbolic.Symbolic, byref(self.Numeric), self.umf_control.Control, self.umf_control.Info)
        return self.status

    def solve(self, matrix, x, b, transpose):
        X = c_void_p(x.buffer_info()[0])
        B = c_void_p(b.buffer_info()[0])
        if self.umf_control.is_complex:
            NULL = c_void_p()
            self.status = self.umf_control.gdata.dll.umfpack_zi_solve (get_transpose(transpose), matrix.AP, matrix.AI, matrix.AX, NULL, X, NULL, B, NULL, self.Numeric, self.umf_control.Control, self.umf_control.Info)
        else:
            self.status = self.umf_control.gdata.dll.umfpack_di_solve (get_transpose(transpose), matrix.AP, matrix.AI, matrix.AX, X, B, self.Numeric, self.umf_control.Control, self.umf_control.Info)
        return self.status

    def determinant(self, x, r):
        X = c_void_p(x.buffer_info()[0])
        R = c_void_p(r.buffer_info()[0])
        if self.umf_control.is_complex:
            NULL = c_void_p()
            self.status = self.umf_control.gdata.dll.umfpack_zi_get_determinant (X, NULL, R, NULL, self.Numeric, self.umf_control.Info)
        else:
            self.status = self.umf_control.gdata.dll.umfpack_di_get_determinant (X, R, self.Numeric, self.umf_control.Info)
        return self.status

class di_triplet:
    def __init__(self, uc, Arow, Acol, Aval):
        self.umf_control = uc
        self.n = max(Arow) + 1
        self.nz = len(Arow)
        self.Arow = Arow
        self.Acol = Acol
        self.Aval = Aval
        self.Ar = c_void_p(Arow.buffer_info()[0])
        self.Ac = c_void_p(Acol.buffer_info()[0])
        self.Av = c_void_p(Aval.buffer_info()[0])

    def __del__(self):
        pass

class matrix:
    def __init__(self, uc, Ap, Ai, Ax):
        self.umf_control = uc
        self.n = len(Ap)-1
        self.Ap = Ap
        self.Ai = Ai
        self.Ax = Ax
        self.AP = c_void_p(Ap.buffer_info()[0])
        self.AI = c_void_p(Ai.buffer_info()[0])
        self.AX = c_void_p(Ax.buffer_info()[0])

    def __del__(self):
        pass

class global_data:
    def __init__(self):
        self.printcb = None
        self.blaslibs = []
        self.dll = None

    def __del__(self):
        self.printcb = None
        self.blaslibs = None
        self.dll = None

class umf_control:
    def __init__(self, gdata, matrix_type):
        self.timer = None
        self.gdata = gdata
        self.Control = None
        self.Info = None
        # expected attributes
        self.matrix_format = "csc"
        self.matrix_type = "real"
        if matrix_type == "real":
            self.is_complex = False
        elif matrix_type == "complex":
            self.is_complex = True
        else:
            raise RuntimeError("Unknown matrix_type real/complex")
        self.set_defaults()

    def __del__(self):
        # having a destructor is preventing segmentation fault
        self.gdata = None
        self.Info = None
        self.Control = None

    def tic(self):
        self.timer = (c_double * 2)()
        self.gdata.dll.umfpack_tic(byref(self.timer))

    def toc(self):
        self.gdata.dll.umfpack_toc (self.timer)
        print ("\numfpack complete.\nTotal time: %5.2f seconds (CPU time), %5.2f seconds (wallclock time)\n" % ( self.timer [1], self.timer [0]))

    def set_defaults(self):
        self.Control = (c_double * UMFPACK_CONTROL)()
        if self.is_complex:
            self.gdata.dll.umfpack_zi_defaults(self.Control)
        else:
            self.gdata.dll.umfpack_di_defaults(self.Control)
        self.Info = (c_double * UMFPACK_INFO)()

    def init_verbose(self):
        #    /* change the default print level for this demo */
        #    /* (otherwise, nothing will print) */
        self.Control [UMFPACK_PRL] = 6
        #    /* print the license agreement */
        if self.is_complex:
            self.gdata.dll.umfpack_zi_report_status (self.Control, UMFPACK_OK)
        else:
            self.gdata.dll.umfpack_di_report_status (self.Control, UMFPACK_OK)
        self.Control [UMFPACK_PRL] = 5
        #
        #    /* print the control parameters */
        if self.is_complex:
            self.gdata.dll.umfpack_zi_report_control(self.Control)
        else:
            self.gdata.dll.umfpack_di_report_control(self.Control)

    def print_vector(self, b, label):
        #    /* print the right-hand-side */
        print ("\n%s: " % (label,), flush=True, end="")
        if self.is_complex:
            NULL = (c_void_p)()
            self.gdata.dll.umfpack_zi_report_vector.argtypes = [c_int, c_void_p, c_void_p, c_void_p]
            self.gdata.dll.umfpack_zi_report_vector.restype = None
            self.gdata.dll.umfpack_zi_report_vector (len(b)//2, b.buffer_info()[0], NULL, self.Control)
        else:
            self.gdata.dll.umfpack_di_report_vector.argtypes = [c_int, c_void_p, c_void_p]
            self.gdata.dll.umfpack_di_report_vector.restype = None
            self.gdata.dll.umfpack_di_report_vector (len(b), b.buffer_info()[0], self.Control)

    def print_triplet(self, tm):
        print ("\nA: ")
        if self.is_complex:
            NULL = (c_void_p)()
            self.gdata.dll.umfpack_zi_report_triplet(tm.n, tm.n, tm.nz, tm.Ar, tm.Ac, tm.Av, NULL, self.Control)
        else:
            self.gdata.dll.umfpack_di_report_triplet(tm.n, tm.n, tm.nz, tm.Ar, tm.Ac, tm.Av, self.Control)

    def triplet_to_col(self, tm):
        n = tm.n
        nz = tm.nz
        NULL = (c_void_p)()
        nz1 = max(nz,1) #; /* ensure arrays are not of size zero. */
        Ap = array.array('i', [0] * (n+1))
        Ai = array.array('i', [0] * (nz1))
        if self.is_complex:
            Ax = array.array('d', [0] * (2*nz1))
            matrix = di_matrix(self, Ap, Ai, Ax)
            self.status = self.gdata.dll.umfpack_zi_triplet_to_col (n, n, nz, tm.Ar, tm.Ac, tm.Av, NULL, matrix.AP, matrix.AI, matrix.AX, NULL, NULL)
        else:
            Ax = array.array('d', [0] * (nz1))
            matrix = di_matrix(self, Ap, Ai, Ax)
            self.status = self.gdata.dll.umfpack_di_triplet_to_col (n, n, nz, tm.Ar, tm.Ac, tm.Av, matrix.AP, matrix.AI, matrix.AX, NULL)

        if self.status < 0:
            self.gdata.dll.umfpack_di_report_status (self.Control, self.status)
            raise RuntimeError("umfpack triplet_to_col failed")

        return matrix

    def print_matrix(self, matrix):
        print("\nA: ")
        if self.is_complex:
            NULL = (c_void_p)()
            self.gdata.dll.umfpack_zi_report_matrix (matrix.n, matrix.n, matrix.AP, matrix.AI, matrix.AX, NULL, 1, self.Control)
        else:
            self.gdata.dll.umfpack_di_report_matrix (matrix.n, matrix.n, matrix.AP, matrix.AI, matrix.AX, 1, self.Control)

    def print_info(self):
        if self.is_complex:
            self.gdata.dll.umfpack_zi_report_info (self.Control, self.Info)
        else:
            self.gdata.dll.umfpack_di_report_info (self.Control, self.Info)

    def print_status(self):
        if self.is_complex:
            self.gdata.dll.umfpack_zi_report_status (self.Control, self.status)
        else:
            self.gdata.dll.umfpack_di_report_status (self.Control, self.status)

    def error_on_result(self, msg):
        if self.status != 0:
            self.print_info()
            self.print_status()
            raise RuntimeError("%s failed" % (msg,))

    def symbolic(self, matrix):
        Symbolic = di_symbolic(self)
        self.status = Symbolic.factor_symbolic(matrix)
        self.error_on_result("umfpack symbolic")
        return Symbolic

    def print_symbolic(self, Symbolic):
        print("\nSymbolic factorization of A: ")
        if self.is_complex:
            self.gdata.dll.umfpack_zi_report_symbolic(Symbolic.Symbolic, self.Control)
        else:
            self.gdata.dll.umfpack_di_report_symbolic(Symbolic.Symbolic, self.Control)

    def numeric(self, matrix, Symbolic):
        Numeric = di_numeric(self)
        self.status = Numeric.factor_numeric(matrix, Symbolic)
        self.error_on_result("umfpack numeric")
        return Numeric

    def print_numeric(self, Numeric):
        print ("\nNumeric factorization of A: ")
        if self.is_complex:
            self.gdata.dll.umfpack_zi_report_numeric (Numeric.Numeric, self.Control)
        else:
            self.gdata.dll.umfpack_di_report_numeric (Numeric.Numeric, self.Control)

    def solve(self, matrix, x, b, Numeric, transpose):
        if self.is_complex:
            self.status = Numeric.solve(matrix, x, b, transpose)
        else:
            self.status = Numeric.solve(matrix, x, b, transpose)
        self.error_on_result("umfpack solve")

    def determinant(self, x, r, Numeric):
        self.status = Numeric.determinant(x, r)
        self.error_on_result("umfpack get_determinant")
        return self.status

    def print_determinant(self, x, r):
        print("determinant: (%g" % x [0], end="")
        print(") * 10^(%g)\n"% r [0])


#/* -------------------------------------------------------------------------- */
#/* resid: compute the residual, r = Ax-b or r = A'x=b and return maxnorm (r) */
#/* -------------------------------------------------------------------------- */
def real_resid ( transpose, matrix, x, r, b):
  Ap = matrix.Ap
  Ai = matrix.Ai
  Ax = matrix.Ax
  n = len(b)
  for i in range(n):
    r[i] = -b[i]
  if (transpose):
    for j in range(n):
      for p in range(Ap[j], Ap[j+1]):
        i = Ai [p]
        r [j] += Ax [p] * x [i]
  else:
    for j in range(n):
      for p in range(Ap[j], Ap[j+1]):
        i = Ai [p]
        r [i] += Ax [p] * x [j]
  norm = 0.0
  print(len(r))
  for i in range(n):
    if abs(r[i]) > norm:
      norm = abs(r[i])
  return norm

def complex_resid ( transpose, matrix, x, r, b):
  Ap = matrix.Ap
  Ai = matrix.Ai
  Ax = matrix.Ax
  f = 2
  n = len(b)//f
  for i in range(n):
    r[f*i] = -b[f*i]
  if (transpose):
    for j in range(n):
      for p in range(Ap[j], Ap[j+1]):
        i = Ai [p]
        r [f*j] += Ax [f*p] * x [f*i]
  else:
    for j in range(n):
      for p in range(Ap[j], Ap[j+1]):
        i = Ai [p]
        r [f*i] += Ax [f*p] * x [f*j]
  norm = 0.0
  print(len(r))
  for i in range(n):
    if abs(r[2*i]) > norm:
      norm = abs(r[2*i])
  return norm

def resid ( transpose, is_complex, matrix, x, r, b):
    if is_complex:
        return complex_resid(transpose, matrix, x, r, b)
    else:
        return real_resid(transpose, matrix, x, r, b)
